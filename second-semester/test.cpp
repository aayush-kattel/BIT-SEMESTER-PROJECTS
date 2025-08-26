#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>

// Audio globals
Mix_Music *bgMusic = nullptr;
Mix_Chunk *shootSound = nullptr;
Mix_Chunk *zombieHitSound = nullptr;
Mix_Chunk *playerHitSound = nullptr;
Mix_Chunk *gameOverSound = nullptr;
bool audioInitialized = false;

// Game constants
const int MAP_WIDTH = 50;
const int MAP_HEIGHT = 30;
const int MAX_ZOMBIES = 15;
const int MAX_BULLETS = 20;
const int PLAYER_LIVES = 3;
const int ZOMBIE_DAMAGE = 1;
const int BULLET_SPEED = 2;
const int PLAYER_SPEED = 1;
const int ZOMBIE_SPEED_MIN = 1;
const int ZOMBIE_SPEED_MAX = 2;
const int SPAWN_RATE = 50;

// Game objects
struct GameObject {
    int x, y;
    bool active;
};

struct Zombie : public GameObject {
    int speed;
};

struct Bullet : public GameObject {
    int dx, dy; // direction vector
};

struct Player {
    int x, y;
    int lives;
    int score;
    int direction; // 0=up, 1=right, 2=down, 3=left
};

// Game state
Player player;
std::vector<Zombie> zombies(MAX_ZOMBIES);
std::vector<Bullet> bullets(MAX_BULLETS);
bool gameRunning = true;
time_t startTime, endTime;

// Terminal control
void disableBuffering() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

void enableBuffering() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag |= ICANON | ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

int kbhit() {
    struct timeval tv = {0L, 0L};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}

// Audio functions
bool initAudio() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! Mix_OpenAudio: " << Mix_GetError() << std::endl;
        return false;
    }

    // Load sounds (replace with your own paths)
    bgMusic = Mix_LoadMUS("bg_music.mp3");
    shootSound = Mix_LoadWAV("shoot.wav");
    zombieHitSound = Mix_LoadWAV("zombie_hit.wav");
    playerHitSound = Mix_LoadWAV("player_hit.wav");
    gameOverSound = Mix_LoadWAV("game_over.wav");

    if (!bgMusic || !shootSound || !zombieHitSound || !playerHitSound || !gameOverSound) {
        std::cerr << "Failed to load sounds! Mix_Error: " << Mix_GetError() << std::endl;
        return false;
    }

    audioInitialized = true;
    return true;
}

void playBackgroundMusic() {
    if (audioInitialized) {
        Mix_PlayMusic(bgMusic, -1);
    }
}

void cleanupAudio() {
    if (audioInitialized) {
        Mix_HaltMusic();
        Mix_FreeMusic(bgMusic);
        Mix_FreeChunk(shootSound);
        Mix_FreeChunk(zombieHitSound);
        Mix_FreeChunk(playerHitSound);
        Mix_FreeChunk(gameOverSound);
        Mix_CloseAudio();
        SDL_Quit();
    }
}

// Game initialization
void initGame() {
    // Initialize player
    player.x = MAP_WIDTH / 2;
    player.y = MAP_HEIGHT / 2;
    player.lives = PLAYER_LIVES;
    player.score = 0;
    player.direction = 0; // Facing up

    // Initialize zombies
    for (auto& zombie : zombies) {
        zombie.active = false;
    }

    // Initialize bullets
    for (auto& bullet : bullets) {
        bullet.active = false;
    }

    time(&startTime);
}

// Game logic
void spawnZombie() {
    for (auto& zombie : zombies) {
        if (!zombie.active) {
            // Spawn at edge of map
            if (rand() % 2) {
                zombie.x = rand() % MAP_WIDTH;
                zombie.y = (rand() % 2) ? 0 : MAP_HEIGHT - 1;
            } else {
                zombie.x = (rand() % 2) ? 0 : MAP_WIDTH - 1;
                zombie.y = rand() % MAP_HEIGHT;
            }
            
            zombie.speed = ZOMBIE_SPEED_MIN + rand() % (ZOMBIE_SPEED_MAX - ZOMBIE_SPEED_MIN + 1);
            zombie.active = true;
            break;
        }
    }
}

void moveZombies() {
    for (auto& zombie : zombies) {
        if (zombie.active) {
            // Move toward player
            if (zombie.x < player.x) zombie.x += zombie.speed;
            else if (zombie.x > player.x) zombie.x -= zombie.speed;
            
            if (zombie.y < player.y) zombie.y += zombie.speed;
            else if (zombie.y > player.y) zombie.y -= zombie.speed;

            // Check collision with player
            if (abs(zombie.x - player.x) < 2 && abs(zombie.y - player.y) < 2) {
                player.lives -= ZOMBIE_DAMAGE;
                zombie.active = false;
                if (audioInitialized) Mix_PlayChannel(-1, playerHitSound, 0);
                
                if (player.lives <= 0) {
                    gameRunning = false;
                    if (audioInitialized) Mix_PlayChannel(-1, gameOverSound, 0);
                }
            }
        }
    }
}

void shootBullet() {
    for (auto& bullet : bullets) {
        if (!bullet.active) {
            bullet.x = player.x;
            bullet.y = player.y;
            
            // Set direction based on player direction
            switch(player.direction) {
                case 0: bullet.dx = 0; bullet.dy = -BULLET_SPEED; break; // Up
                case 1: bullet.dx = BULLET_SPEED; bullet.dy = 0; break;  // Right
                case 2: bullet.dx = 0; bullet.dy = BULLET_SPEED; break;  // Down
                case 3: bullet.dx = -BULLET_SPEED; bullet.dy = 0; break; // Left
            }
            
            bullet.active = true;
            if (audioInitialized) Mix_PlayChannel(-1, shootSound, 0);
            break;
        }
    }
}

void moveBullets() {
    for (auto& bullet : bullets) {
        if (bullet.active) {
            bullet.x += bullet.dx;
            bullet.y += bullet.dy;
            
            // Check if out of bounds
            if (bullet.x < 0 || bullet.x >= MAP_WIDTH || bullet.y < 0 || bullet.y >= MAP_HEIGHT) {
                bullet.active = false;
                continue;
            }
            
            // Check collision with zombies
            for (auto& zombie : zombies) {
                if (zombie.active && abs(bullet.x - zombie.x) < 2 && abs(bullet.y - zombie.y) < 2) {
                    zombie.active = false;
                    bullet.active = false;
                    player.score += 10;
                    if (audioInitialized) Mix_PlayChannel(-1, zombieHitSound, 0);
                    break;
                }
            }
        }
    }
}

void movePlayer(char input) {
    switch(input) {
        case 'w': // Up
            player.direction = 0;
            if (player.y > 1) player.y -= PLAYER_SPEED;
            break;
        case 'd': // Right
            player.direction = 1;
            if (player.x < MAP_WIDTH - 2) player.x += PLAYER_SPEED;
            break;
        case 's': // Down
            player.direction = 2;
            if (player.y < MAP_HEIGHT - 2) player.y += PLAYER_SPEED;
            break;
        case 'a': // Left
            player.direction = 3;
            if (player.x > 1) player.x -= PLAYER_SPEED;
            break;
    }
}

// Rendering
void clearScreen() {
    std::cout << "\033[H\033[J";
}

void drawGame() {
    clearScreen();
    
    // Draw border and map
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            bool drawn = false;
            
            // Draw player (block character)
            if (x == player.x && y == player.y) {
                std::cout << "\033[1;32m█\033[0m"; // Green block
                drawn = true;
            }
            
            // Draw bullets
            if (!drawn) {
                for (const auto& bullet : bullets) {
                    if (bullet.active && x == bullet.x && y == bullet.y) {
                        std::cout << "\033[1;33m*\033[0m"; // Yellow *
                        drawn = true;
                        break;
                    }
                }
            }
            
            // Draw zombies (| character)
            if (!drawn) {
                for (const auto& zombie : zombies) {
                    if (zombie.active && x == zombie.x && y == zombie.y) {
                        std::cout << "\033[1;31m|\033[0m"; // Red |
                        drawn = true;
                        break;
                    }
                }
            }
            
            if (!drawn) {
                if (x == 0 || x == MAP_WIDTH - 1 || y == 0 || y == MAP_HEIGHT - 1) {
                    std::cout << "#"; // Border
                } else {
                    std::cout << " ";
                }
            }
        }
        std::cout << std::endl;
    }
    
    // Draw HUD
    time(&endTime);
    int elapsedTime = static_cast<int>(difftime(endTime, startTime));
    
    std::cout << "Lives: ";
    for (int i = 0; i < PLAYER_LIVES; i++) {
        if (i < player.lives) std::cout << "\033[1;31m♥ \033[0m";
        else std::cout << "  ";
    }
    
    std::cout << "  Score: " << player.score;
    std::cout << "  Time: " << elapsedTime << "s";
    
    std::cout << std::endl;
    std::cout << "Controls: WASD (Move), SPACE (Shoot), Q (Quit)" << std::endl;
}

// Game loop
void gameLoop() {
    initGame();
    if (audioInitialized) playBackgroundMusic();
    
    while (gameRunning) {
        // Spawn zombies randomly
        if (rand() % SPAWN_RATE == 0) spawnZombie();
        
        // Handle input
        if (kbhit()) {
            char input;
            read(STDIN_FILENO, &input, 1);
            
            switch (input) {
                case 'w':
                case 'a':
                case 's':
                case 'd':
                    movePlayer(input);
                    break;
                case ' ':
                    shootBullet();
                    break;
                case 'q':
                    gameRunning = false;
                    break;
            }
        }
        
        // Update game state
        moveZombies();
        moveBullets();
        
        // Draw
        drawGame();
        
        // Small delay
        usleep(50000);
    }
    
    // Game over screen
    clearScreen();
    std::cout << "=== GAME OVER ===" << std::endl;
    std::cout << "Final Score: " << player.score << std::endl;
    std::cout << "Survival Time: " << static_cast<int>(difftime(endTime, startTime)) << " seconds" << std::endl;
    std::cout << "Press Enter to continue..." << std::endl;
    getchar();
}

// Main menu
void showMenu() {
    clearScreen();
    std::cout << "=== ZOMBIE SURVIVAL ===" << std::endl;
    std::cout << "1. Start Game" << std::endl;
    std::cout << "2. Instructions" << std::endl;
    std::cout << "3. Exit" << std::endl;
    std::cout << "Select option: ";
    
    char choice;
    std::cin >> choice;
    
    switch (choice) {
        case '1': 
            disableBuffering();
            gameLoop();
            enableBuffering();
            showMenu(); // Return to menu after game
            break;
        case '2':
            clearScreen();
            std::cout << "=== INSTRUCTIONS ===" << std::endl;
            std::cout << "Survive as long as possible against zombie hordes!" << std::endl;
            std::cout << "- WASD to move (green █)" << std::endl;
            std::cout << "- SPACE to shoot (yellow *)" << std::endl;
            std::cout << "- Avoid red | zombies" << std::endl;
            std::cout << "- Each hit reduces your lives (♥)" << std::endl;
            std::cout << "- Kill zombies to score points" << std::endl;
            std::cout << "Press Enter to continue..." << std::endl;
            getchar(); // Clear newline
            getchar(); // Wait for Enter
            showMenu();
            break;
        case '3':
            return;
        default:
            std::cout << "Invalid choice!" << std::endl;
            sleep(1);
            showMenu();
    }
}

int main() {
    srand(static_cast<unsigned>(time(nullptr)));
    
    if (!initAudio()) {
        std::cerr << "Audio initialization failed - continuing without sound" << std::endl;
    }
    
    showMenu();
    cleanupAudio();
    
    return 0;
}
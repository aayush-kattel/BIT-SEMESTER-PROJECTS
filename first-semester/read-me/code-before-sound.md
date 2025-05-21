#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>
#include <sys/ioctl.h>
#include <string.h>

#define WIDTH 50
#define HEIGHT 30
#define HIGH_SCORE_FILE "highscore.txt"
#define LEVEL_SCORES_FILE "level_scores.dat"
#define MAX_LEVEL_SCORES 10

typedef struct {
    int score;
    int time;
} LevelScore;

typedef struct {
    LevelScore easy[MAX_LEVEL_SCORES];
    LevelScore medium[MAX_LEVEL_SCORES];
    LevelScore hard[MAX_LEVEL_SCORES];
    int easy_high;
    int medium_high;
    int hard_high;
} LevelHighScores;

// Global Variables
int carLane = 1;
int carRow = HEIGHT - 3;
int obstacleLane[3];
int obstacleRow[3];
int numObstacles = 3;
int score = 0;
int highScore = 0;
int gameRunning = 1;
time_t startTime, endTime;
int carShape = 0;
int obstacleShape = 0;
const char *carColor = "\033[0;34m";
const char *obstacleColor = "\033[0;31m";
int lives = 3;
int heartLane[3];
int heartRow[3];
int numHearts = 0;
int lifeCount = 3;
int gameSpeed = 100000;
int lastHeartTime = 0;
int heartActive[3] = {0};
LevelHighScores levelHighScores;
int currentLevel = 1;

int kbhit() {
    struct timeval tv = {0L, 0L};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}

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

int getTerminalWidth() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;
}

void printCentered(const char *text) {
    int termWidth = getTerminalWidth();
    int padding = (termWidth - strlen(text)) / 2;
    if (padding < 0) padding = 0;
    printf("%*s%s\n", padding, "", text);
}

void loadHighScore() {
    FILE *file = fopen(HIGH_SCORE_FILE, "r");
    if (file == NULL) {
        highScore = 0;
        return;
    }
    fscanf(file, "%d", &highScore);
    fclose(file);
}

void saveHighScore() {
    FILE *file = fopen(HIGH_SCORE_FILE, "w");
    if (file != NULL) {
        fprintf(file, "%d", highScore);
        fclose(file);
    }
}

void loadLevelHighScores() {
    FILE *file = fopen(LEVEL_SCORES_FILE, "rb");
    if (file != NULL) {
        fread(&levelHighScores, sizeof(LevelHighScores), 1, file);
        fclose(file);
    } else {
        memset(&levelHighScores, 0, sizeof(LevelHighScores));
    }
}

void saveLevelHighScores() {
    FILE *file = fopen(LEVEL_SCORES_FILE, "wb");
    if (file != NULL) {
        fwrite(&levelHighScores, sizeof(LevelHighScores), 1, file);
        fclose(file);
    }
}

int getCurrentLevelHighScore() {
    switch(currentLevel) {
        case 1: return levelHighScores.easy_high;
        case 2: return levelHighScores.medium_high;
        case 3: return levelHighScores.hard_high;
        default: return 0;
    }
}

void updateLevelHighScore(int score) {
    switch(currentLevel) {
        case 1:
            if (score > levelHighScores.easy_high) {
                levelHighScores.easy_high = score;
            }
            break;
        case 2:
            if (score > levelHighScores.medium_high) {
                levelHighScores.medium_high = score;
            }
            break;
        case 3:
            if (score > levelHighScores.hard_high) {
                levelHighScores.hard_high = score;
            }
            break;
    }
}

void addLevelScore(int level, int score, int time) {
    LevelScore *scores;
    switch(level) {
        case 1: scores = levelHighScores.easy; break;
        case 2: scores = levelHighScores.medium; break;
        case 3: scores = levelHighScores.hard; break;
        default: return;
    }

    int pos = MAX_LEVEL_SCORES - 1;
    while (pos >= 0 && (scores[pos].score < score || scores[pos].score == 0)) {
        pos--;
    }
    pos++;

    if (pos < MAX_LEVEL_SCORES) {
        for (int i = MAX_LEVEL_SCORES - 1; i > pos; i--) {
            scores[i] = scores[i-1];
        }
        scores[pos].score = score;
        scores[pos].time = time;
    }
}

int showLeaderboard() {
    system("clear");
    time(&endTime);
    int playTime = (int)difftime(endTime, startTime);
    
    addLevelScore(currentLevel, score, playTime);
    saveLevelHighScores();
    
    printCentered("======== GAME OVER ========");
    printf("\n%*sYour Score: %d\n", getTerminalWidth()/2 - 8, "", score);
    printf("%*sTime: %d seconds\n", getTerminalWidth()/2 - 8, "", playTime);
    printCentered("==========================");
    
    LevelScore *currentScores;
    char *levelName;
    
    switch(currentLevel) {
        case 1: 
            currentScores = levelHighScores.easy;
            levelName = "EASY";
            break;
        case 2:
            currentScores = levelHighScores.medium;
            levelName = "MEDIUM";
            break;
        case 3:
            currentScores = levelHighScores.hard;
            levelName = "HARD";
            break;
    }
    
    printf("\n%*s%s LEVEL HIGH SCORES\n", getTerminalWidth()/2 - 10, "", levelName);
    printCentered("-------------------------------");
    printCentered("Rank  Score  Time");
    printCentered("-------------------------------");
    
    for (int i = 0; i < MAX_LEVEL_SCORES && currentScores[i].score > 0; i++) {
        printf("%*s%2d.  %5d  %4d\n", 
              getTerminalWidth()/2 - 10, "", i+1, 
              currentScores[i].score, currentScores[i].time);
    }
    
    printCentered("==========================");
    printf("\n%*sPlay again? (1=Yes, 0=No): ", getTerminalWidth()/2 - 10, "");
    
    int choice;
    scanf("%d", &choice);
    getchar();
    
    return choice;
}

void showCarAnimation() {
    system("clear");
    char *car[] = {
        "         ______",
        "   ____/      \\____",
        "  |    _        _  -.",
        "  '-(_)------(_)--'"
    };
    int termWidth = getTerminalWidth();
    const char *resetColor = "\033[0m";

    for (int pos = 1; pos < termWidth - 20; pos++) {
        system("clear");
        printCentered("WELCOME TO CAR DODGING GAME! 🚗💨\n");
        for (int i = 0; i < 4; i++) {
            printf("%*s%s%s\n", pos, "", carColor, car[i]);
        }
        printf("%s", resetColor);
        usleep(10000);
    }
    usleep(200000);
}

int showMenu() {
    showCarAnimation();
    int choice;
    do {
        system("clear");
        printCentered("=====================================");
        printCentered("       🚗 CAR DODGING GAME 🚗       ");
        printCentered("=====================================");
        int columnWidth = getTerminalWidth() / 2 - 10;  
        printf("%*s1. Play Game\n", columnWidth, "");
        printf("%*s2. Instructions\n", columnWidth, "");
        printf("%*s3. Exit\n", columnWidth, "");
        printCentered("=====================================");
        printf("\n%*sEnter your choice: ", columnWidth, "");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1: return 1;
            case 2:
                system("clear");
                printCentered("======= 📜 INSTRUCTIONS 📜 =======");
                printf("\n");
                
                int instructionColumnWidth = getTerminalWidth() / 2 - 15;  
                
                // Basic Controls
                printf("%*s🚗 CAR CONTROLS:\n", instructionColumnWidth, "");
                printf("%*s- A: Move Left    D: Move Right\n", instructionColumnWidth, "");
                printf("%*s- W: Move Up      S: Move Down\n", instructionColumnWidth, "");
                printf("%*s- Q: Quit Game (anytime)\n\n", instructionColumnWidth, "");
                
                // Game Elements
                printf("%*s🎮 GAME ELEMENTS:\n", instructionColumnWidth, "");
                printf("%*s- Avoid the red obstacles (▲▲▲ or █▒█)\n", instructionColumnWidth, "");
                printf("%*s- Collect pink hearts (♥) to regain lives\n", instructionColumnWidth, "");
                printf("%*s- You start with 3 lives\n\n", instructionColumnWidth, "");
                
                // Scoring System
                printf("%*s🏆 SCORING:\n", instructionColumnWidth, "");
                printf("%*s- Score increases when obstacles pass\n", instructionColumnWidth, "");
                printf("%*s- Game tracks TWO high scores:\n", instructionColumnWidth, "");
                printf("%*s  1. Overall Highest (all levels)\n", instructionColumnWidth, "");
                printf("%*s  2. Level-specific Highest\n\n", instructionColumnWidth, "");
                
                // Levels
                printf("%*s📈 LEVELS:\n", instructionColumnWidth, "");
                printf("%*s- Easy (Slow): Great for beginners\n", instructionColumnWidth, "");
                printf("%*s- Medium (Normal): Balanced challenge\n", instructionColumnWidth, "");
                printf("%*s- Hard (Fast): For expert players\n\n", instructionColumnWidth, "");
                
                // Customization
                printf("%*s🎨 CUSTOMIZATION:\n", instructionColumnWidth, "");
                printf("%*s- Choose car & obstacle shapes\n", instructionColumnWidth, "");
                printf("%*s- Select colors for both\n\n", instructionColumnWidth, "");
                
                // Leaderboard
                printf("%*s🏅 LEADERBOARD:\n", instructionColumnWidth, "");
                printf("%*s- Shows top 10 scores per level\n", instructionColumnWidth, "");
                printf("%*s- Displays your play time\n", instructionColumnWidth, "");
                printf("%*s- Option to play again after game over\n", instructionColumnWidth, "");
                
                printf("\n%*sPress Enter to go back to menu...", instructionColumnWidth, "");
                getchar();
                break;
            case 3:
                printCentered("Exiting the game. Goodbye!");
                exit(0);
            default:
                printCentered("Invalid choice! Try again.");
                sleep(1);
        }
    } while (1);
}

void showCustomizationMenu() {
    int carShapeChoice, carColorChoice, obstacleShapeChoice, obstacleColorChoice;
    char *shapes[] = {"Block", "Triangle", "Circle"};
    char *colors[] = {"Blue", "Green", "Yellow", "Black", "Purple", "Orange", "White"};

    do {
        system("clear");
        printCentered("=====================================");
        printCentered("       CAR SHAPE CUSTOMIZATION       ");
        printCentered("=====================================");
        for (int i = 0; i < 3; i++) {
            printf("%*s%d. %s\n", getTerminalWidth() / 2 - 10, "", i + 1, shapes[i]);
        }
        printCentered("=====================================");
        printf("\n%*sEnter your car shape choice (1-3): ", getTerminalWidth() / 2 - 10, "");
        scanf("%d", &carShapeChoice);
        if (carShapeChoice >= 1 && carShapeChoice <= 3) break;
        printCentered("Invalid choice. Please try again.");
        sleep(1);
    } while (1);

    do {
        system("clear");
        printCentered("=====================================");
        printCentered("       CAR COLOR CUSTOMIZATION       ");
        printCentered("=====================================");
        for (int i = 0; i < 7; i++) {
            printf("%*s%d. %s\n", getTerminalWidth() / 2 - 10, "", i + 1, colors[i]);
        }
        printCentered("=====================================");
        printf("\n%*sEnter your car color choice (1-7): ", getTerminalWidth() / 2 - 10, "");
        scanf("%d", &carColorChoice);
        if (carColorChoice >= 1 && carColorChoice <= 7) break;
        printCentered("Invalid choice. Please try again.");
        sleep(1);
    } while (1);

    do {
        system("clear");
        printCentered("=====================================");
        printCentered("   OBSTACLE SHAPE CUSTOMIZATION     ");
        printCentered("=====================================");
        for (int i = 0; i < 3; i++) {
            printf("%*s%d. %s\n", getTerminalWidth() / 2 - 10, "", i + 1, shapes[i]);
        }
        printCentered("=====================================");
        printf("\n%*sEnter your obstacle shape choice (1-3): ", getTerminalWidth() / 2 - 10, "");
        scanf("%d", &obstacleShapeChoice);
        if (obstacleShapeChoice >= 1 && obstacleShapeChoice <= 3) break;
        printCentered("Invalid choice. Please try again.");
        sleep(1);
    } while (1);

    do {
        system("clear");
        printCentered("=====================================");
        printCentered("  OBSTACLE COLOR CUSTOMIZATION      ");
        printCentered("=====================================");
        for (int i = 0; i < 7; i++) {
            printf("%*s%d. %s\n", getTerminalWidth() / 2 - 10, "", i + 1, colors[i]);
        }
        printCentered("=====================================");
        printf("\n%*sEnter your obstacle color choice (1-7): ", getTerminalWidth() / 2 - 10, "");
        scanf("%d", &obstacleColorChoice);
        if (obstacleColorChoice >= 1 && obstacleColorChoice <= 7) break;
        printCentered("Invalid choice. Please try again.");
        sleep(1);
    } while (1);

    carShapeChoice -= 1;
    carColorChoice -= 1;
    obstacleShapeChoice -= 1;
    obstacleColorChoice -= 1;

    carShape = carShapeChoice;
    const char *carColorsArray[] = {
        "\033[0;34m", "\033[0;32m", "\033[0;33m", "\033[0;30m",
        "\033[0;35m", "\033[0;38m", "\033[0;37m"
    };
    carColor = carColorsArray[carColorChoice];

    obstacleShape = obstacleShapeChoice;
    const char *obstacleColorsArray[] = {
        "\033[0;31m", "\033[0;32m", "\033[0;33m", "\033[0;30m",
        "\033[0;35m", "\033[0;38m", "\033[0;37m"
    };
    obstacleColor = obstacleColorsArray[obstacleColorChoice];

    system("clear");
    printCentered("Customization Completed!");
    printCentered("Press Enter to Continue...");
    getchar();
    getchar();
}

void showLevelMenu() {
    int choice;
    do {
        system("clear");
        printCentered("=====================================");
        printCentered("          SELECT GAME LEVEL          ");
        printCentered("=====================================");
        int columnWidth = getTerminalWidth() / 2 - 10;
        printf("%*s1. Easy (Slow)\n", columnWidth, "");
        printf("%*s2. Medium (Normal)\n", columnWidth, "");
        printf("%*s3. Hard (Fast)\n", columnWidth, "");
        printCentered("=====================================");
        printf("\n%*sEnter your level choice (1-3): ", columnWidth, "");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                gameSpeed = 150000;
                currentLevel = 1;
                return;
            case 2:
                gameSpeed = 100000;
                currentLevel = 2;
                return;
            case 3:
                gameSpeed = 70000;
                currentLevel = 3;
                return;
            default:
                printCentered("Invalid choice! Try again.");
                sleep(1);
        }
    } while (1);
}

void initializeObstacles() {
    for (int i = 0; i < numObstacles; i++) {
        obstacleLane[i] = rand() % 3;
        obstacleRow[i] = -(rand() % HEIGHT);
    }
}

void initializeHearts() {
    numHearts = rand() % 3;
    for (int i = 0; i < numHearts; i++) {
        heartLane[i] = rand() % 3;
        heartRow[i] = -(rand() % HEIGHT);
        heartActive[i] = 1;
    }
}

void updateHearts() {
    time_t currentTime;
    time(&currentTime);
    
    if (difftime(currentTime, lastHeartTime) >= 5) {
        for (int i = 0; i < numHearts; i++) {
            if (!heartActive[i] && rand() % 100 < 2) {
                heartLane[i] = rand() % 3;
                heartRow[i] = -(rand() % HEIGHT);
                heartActive[i] = 1;
            }
        }
    }

    for (int i = 0; i < numHearts; i++) {
        if (heartActive[i]) {
            heartRow[i]++;
            if (heartRow[i] >= HEIGHT) {
                heartActive[i] = 0;
            }
        }
    }
}

void checkHeartCollision() {
    time_t currentTime;
    time(&currentTime);
    
    for (int i = 0; i < numHearts; i++) {
        if (heartActive[i] && carRow == heartRow[i] && carLane == heartLane[i]) {
            if (lives < 3) {
                lives++;
                lifeCount = lives;
                lastHeartTime = currentTime;
            }
            heartActive[i] = 0;
            return;
        }
    }
}

void drawScreen() {
    printf("\033[H\033[J");
    int termWidth = getTerminalWidth();
    int padding = (termWidth - WIDTH) / 2;
    const char *resetColor = "\033[0m";

    printf("%*sLives: ", padding, "");
    for (int i = 0; i < lifeCount; i++) {
        printf("\033[0;35m♥ ");
    }
    printf("%s\n", resetColor);

    for (int i = 0; i < HEIGHT; i++) {
        printf("%*s", padding, "");

        for (int j = 0; j < WIDTH; j++) {
            if (j == 0 || j == WIDTH - 1) {
                printf("|");
            } else if (i == 0 || i == HEIGHT - 1) {
                printf("-");
            } else if (j == WIDTH / 3 || j == 2 * WIDTH / 3) {
                printf(".");
            }
            else if ((i == carRow - 1 || i == carRow + 1) && j == carLane * (WIDTH / 3) + WIDTH / 6) {
                if (carShape == 0) printf("%s██", carColor);
                else if (carShape == 1) printf("%s▲", carColor);
                else printf("%s○", carColor);
                j++;
                printf("%s", resetColor);
            }
            else if (i == carRow && j == carLane * (WIDTH / 3) + WIDTH / 6 - 1) {
                if (carShape == 0) printf("%s████", carColor);
                else if (carShape == 1) printf("%s▲▲▲", carColor);
                else printf("%s○○○", carColor);
                j += 3;
                printf("%s", resetColor);
            } 
            else {
                int drawn = 0;
                for (int k = 0; k < numObstacles; k++) {
                    if (i == obstacleRow[k] && j == obstacleLane[k] * (WIDTH / 3) + WIDTH / 6) {
                        if (obstacleShape == 0) printf("%s█▒█", obstacleColor);
                        else if (obstacleShape == 1) printf("%s△△", obstacleColor);
                        else printf("%s○○", obstacleColor);
                        j += 2;
                        drawn = 1;
                        break;
                    }
                }

                for (int k = 0; k < numHearts; k++) {
                    if (heartActive[k] && i == heartRow[k] && j == heartLane[k] * (WIDTH / 3) + WIDTH / 6) {
                        printf("%s♥", "\033[0;35m");
                        drawn = 1;
                        j++;
                        break;
                    }
                }

                if (!drawn) printf(" ");
            }
        }
        printf("\n");
    }

    char buffer[150];
    time(&endTime);
    int elapsedTime = (int)difftime(endTime, startTime);
    int levelHigh = getCurrentLevelHighScore();
    
    sprintf(buffer, "Score: %d | Overall High: %d | %s High: %d | Time: %d sec", 
           score, highScore, 
           currentLevel == 1 ? "Easy" : (currentLevel == 2 ? "Medium" : "Hard"),
           levelHigh, 
           elapsedTime);
    
    printCentered(buffer);
    printf("\n");
    printCentered("Controls: W (Up), S (Down), A (Left), D (Right), Q (Quit)");
}

void moveCar(char input) {
    if (input == 'a' && carLane > 0) carLane--;
    if (input == 'd' && carLane < 2) carLane++;
    if (input == 'w' && carRow > 1) carRow--;
    if (input == 's' && carRow < HEIGHT - 3) carRow++;
}

void updateObstacles() {
    for (int i = 0; i < numObstacles; i++) {
        obstacleRow[i]++;
        if (obstacleRow[i] >= HEIGHT) {
            obstacleRow[i] = 0;
            obstacleLane[i] = rand() % 3;
            score++;
        }
    }
}

void checkCollision() {
    static time_t lastCollisionTime = 0;
    time_t currentTime;
    time(&currentTime);
    
    if (difftime(currentTime, lastCollisionTime) < 0.5) {
        return;
    }

    for (int i = 0; i < numObstacles; i++) {
        if (carRow == obstacleRow[i] && carLane == obstacleLane[i]) {
            lives--;
            lifeCount = lives;
            lastCollisionTime = currentTime;
            
            obstacleRow[i] = -rand() % HEIGHT;
            obstacleLane[i] = rand() % 3;

            if (lives <= 0) {
                gameRunning = 0;
                if (score > highScore) {
                    highScore = score;
                    saveHighScore();
                }
                
                updateLevelHighScore(score);
                saveLevelHighScores();
                
                addLevelScore(currentLevel, score, (int)difftime(currentTime, startTime));
                saveLevelHighScores();
                
                if (score > highScore) {
                    printCentered("🏆 New High Score! Congratulations! 🏆");
                } else {
                    printCentered("💥 Game Over! 💥");
                }
                sleep(2);
            }
            return;
        }
    }
}

int main() {
    loadHighScore();
    loadLevelHighScores();

    while (1) {
        if (showMenu() != 1) break;

        showCustomizationMenu();
        showLevelMenu();

        disableBuffering();
        srand(time(NULL));
        initializeHearts();
        initializeObstacles();
        score = 0;
        lives = 3;
        lifeCount = 3;
        gameRunning = 1;
        time(&startTime);

        while (gameRunning) {
            drawScreen();
            updateObstacles();
            updateHearts();
            checkCollision();
            checkHeartCollision();

            if (kbhit()) {
                char input;
                read(STDIN_FILENO, &input, 1);
                if (input == 'q') {
                    gameRunning = 0;
                }
                moveCar(input);
            }

            usleep(gameSpeed);

            if (score % 10 == 0 && gameSpeed > 50000) {
                gameSpeed -= 1000;
            }
        }

        enableBuffering();

        if (!showLeaderboard()) {
            break;
        }
    }

    printCentered("Thanks for playing!");
    return 0;
}
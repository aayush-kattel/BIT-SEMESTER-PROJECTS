#include <GL/glut.h> // GLUT header file

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <math.h>
#include <iostream>
#include <string.h>
#include <sstream>
#include <png.h>
#include <stdlib.h>
#include <vector>
#include <fstream>
using namespace std;

// header files
#include "png_load.h"
#include "load_and_bind_texture.h"
#include "textures.h"
#include "maze.h"
#include "Pacman.h"
#include "Ghost.h"
#include "Interface.h"

// SDL Audio variables
Mix_Chunk *eatPillSound = nullptr;
Mix_Chunk *eatGhostSound = nullptr;
Mix_Chunk *deathSound = nullptr;
Mix_Chunk *powerPillSound = nullptr;
Mix_Chunk *fruitSound = nullptr;
Mix_Music *introMusic = nullptr;
bool audioInitialized = false;

// Game globals variables
int gameTick = 0;
int deathTick = 0;
int frightenTick = 0;
int timestamp;
int score = 0;
int pills = 244;
int lives = 3;
int eatenCount = 0;
bool frighten;
float eatenX;
float eatenY;
int eatStamp;
Pacman pacman;
bool paused = false;
bool dead = false;

int fruitCounter = 0;
int tempFruitCounter = 0;
int eatFruitStamp = 0;
int eatenFruitX;
int eatenFruitY;
int fruitSpawned = 0;

// POWER-UP GLOBAL VARIABLES
bool ghostsFrozen = false;
int freezeTimer = 0;
bool doublePointsActive = false;
bool speedBoostActive = false;
int powerUpTimer = 0;

// mini-map variables
bool showMiniMap = true;             // Toggle mini-map on/off
int miniMapX = 300;                  // Position on screen (right side)
int miniMapY = 30;                   // Position from bottom
int miniMapWidth = 70;               // 28 * 3 (maze width * scale)
int miniMapHeight = 70;              // 31 * 3 (maze height * scale)
int miniMapViewRadius = 5;           // Show 5 tiles around Pacman

// Intro animation variables
float introPacmanX = -5.0f;
float introGhostsX[4] = {-8.0f, -10.0f, -12.0f, -14.0f};
int introTick = 0;
bool introComplete = false;

// Loading screen variables
int loadingProgress = 0;
int loadingTimer = 0;
const int LOADING_DURATION = 180; // 3 seconds at 60 FPS
string loadingMessages[5] = {
    "Initializing game world...",
    "Loading characters...",
    "Setting up AI...",
    "Preparing audio...",
    "Ready to play!"
};

// Declaration of ghosts in array with their starting positions and colours
Ghost ghosts[4] = {
    Ghost(13.5f, 19.0f, RED),
    Ghost(11.5f, 16.0f, BLUE),
    Ghost(13.5f, 16.0f, PINK),
    Ghost(15.5f, 16.0f, YELLOW),
};

// define game modes
typedef enum
{
    INTRO,
    LOADING,    
    BEGIN,
    PLAY,
    DIE,
    OVER
} gameState;
gameState stateGame = INTRO; // Start with INTRO

// Initialize SDL audio
bool initAudio()
{
    if (SDL_Init(SDL_INIT_AUDIO) < 0)
    {
        cout << "SDL audio could not initialize! SDL_Error: " << SDL_GetError() << endl;
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        cout << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << endl;
        return false;
    }

    return true;
}

// Load sound effects
bool loadSounds()
{
    cout << "Loading sound effects..." << endl;

    eatPillSound = Mix_LoadWAV("sounds/eat_pill.wav");
    cout << "eat_pill.wav: " << (eatPillSound ? "LOADED" : "MISSING") << endl;

    eatGhostSound = Mix_LoadWAV("sounds/eat_ghost.wav");
    cout << "eat_ghost.wav: " << (eatGhostSound ? "LOADED" : "MISSING") << endl;

    deathSound = Mix_LoadWAV("sounds/death.wav");
    cout << "death.wav: " << (deathSound ? "LOADED" : "MISSING") << endl;

    powerPillSound = Mix_LoadWAV("sounds/power_pill.wav");
    cout << "power_pill.wav: " << (powerPillSound ? "LOADED" : "MISSING") << endl;

    fruitSound = Mix_LoadWAV("sounds/fruit.wav");
    cout << "fruit.wav: " << (fruitSound ? "LOADED" : "MISSING") << endl;

    introMusic = Mix_LoadMUS("sounds/intro_music.mp3");
    cout << "intro_music.mp3: " << (introMusic ? "LOADED" : "MISSING") << endl;

    if (!eatPillSound)
    {
        cout << "CRITICAL: Missing eat_pill.wav - this sound is essential!" << endl;
        return false;
    }

    if (eatPillSound || eatGhostSound || deathSound || powerPillSound || fruitSound)
    {
        cout << "Audio loaded with available sounds" << endl;
        return true;
    }
    else
    {
        cout << "No sound files could be loaded" << endl;
        return false;
    }
}

// Play sound effect
void playSound(Mix_Chunk *sound)
{
    if (audioInitialized && sound)
    {
        Mix_PlayChannel(-1, sound, 0);
    }
}

// Play intro music
void playIntroMusic()
{
    if (audioInitialized && introMusic)
    {
        Mix_PlayMusic(introMusic, 0); // Play once (0 means don't loop)
    }
}

// Stop intro music
void stopIntroMusic()
{
    if (audioInitialized)
    {
        Mix_HaltMusic();
    }
}

// Cleanup audio resources
void cleanupAudio()
{
    if (audioInitialized)
    {
        Mix_FreeChunk(eatPillSound);
        Mix_FreeChunk(eatGhostSound);
        Mix_FreeChunk(deathSound);
        Mix_FreeChunk(powerPillSound);
        Mix_FreeChunk(fruitSound);
        Mix_CloseAudio();
        SDL_Quit();
        Mix_FreeMusic(introMusic);
    }
}

// Detect if pacman has eaten a pill or power pill
void detectPill()
{
    if (pacman.getTile(pacman.pacmanX, pacman.pacmanY) == O)
    {
        frighten = true;
        frightenTick = 0;
        playSound(powerPillSound);
        for (int i = 0; i < 4; i++)
        {
            if (ghosts[i].moveType == CHASE || ghosts[i].moveType == SCATTER)
            {
                ghosts[i].eaten = false;
                ghosts[i].moveType = FRIGHTEN;
                if (ghosts[i].currentDir == LEFT)
                { // Reverse their direction
                    ghosts[i].currentDir = RIGHT;
                }
                else if (ghosts[i].currentDir == UP)
                {
                    ghosts[i].currentDir = DOWN;
                }
                else if (ghosts[i].currentDir == RIGHT)
                {
                    ghosts[i].currentDir = LEFT;
                }
                else
                {
                    ghosts[i].currentDir = UP;
                }
            }
        }
    }
    else if (pacman.getTile(pacman.pacmanX, pacman.pacmanY) == F)
    {
        eatenFruitX = pacman.pacmanX;
        eatenFruitY = pacman.pacmanY;
        playSound(fruitSound);
    }
}

// check pacman eat ghost or ghost eat pacman
void detectGhost()
{
    for (int i = 0; i < 4; i++)
    {
        if (pacman.getPacX() == ghosts[i].getGhostX() && pacman.getPacY() == ghosts[i].getGhostY())
        { // check positions of both
            if (ghosts[i].moveType != FRIGHTEN && ghosts[i].moveType != DEATH)
            { // eat pacman
                stateGame = DIE;
                timestamp = gameTick;
                playSound(deathSound);
            }
            else
            {
                if (!ghosts[i].eaten)
                { // if ghost is dead already
                    eatenCount++;
                    eatStamp = gameTick;
                    playSound(eatGhostSound);
                    switch (eatenCount)
                    {
                    case 1:
                        score += 200;
                        break;
                    case 2:
                        score += 400;
                        break;
                    case 3:
                        score += 800;
                        break;
                    case 4:
                        score += 1600;
                        break;
                    }
                    eatenX = ghosts[i].ghostX; // Set ghost position
                    eatenY = ghosts[i].ghostY;
                    ghosts[i].eaten = true; // Mark ghost as eaten
                }
                ghosts[i].moveType = DEATH; // ghost death
            }
        }
    }
    if (stateGame == DIE && !dead)
    {
        lives--;
        dead = true;
    }
}

// set mode of ghosts
void setMode()
{
    if (gameTick >= 4450)
    {
        for (int i = 0; i < 4; i++)
        {
            if (ghosts[i].moveType != PEN && ghosts[i].moveType != LEAVE && !frighten && ghosts[i].moveType != DEATH)
            {
                ghosts[i].moveType = CHASE;
            }
        }
    }
    else if (gameTick >= 4200)
    {
        for (int i = 0; i < 4; i++)
        {
            if (ghosts[i].moveType != PEN && ghosts[i].moveType != LEAVE && !frighten && ghosts[i].moveType != DEATH)
            {
                ghosts[i].moveType = SCATTER;
            }
        }
    }
    else if (gameTick >= 3200)
    {
        for (int i = 0; i < 4; i++)
        {
            if (ghosts[i].moveType != PEN && ghosts[i].moveType != LEAVE && !frighten && ghosts[i].moveType != DEATH)
            {
                ghosts[i].moveType = CHASE;
            }
        }
    }
    else if (gameTick >= 2950)
    {
        for (int i = 0; i < 4; i++)
        {
            if (ghosts[i].moveType != PEN && ghosts[i].moveType != LEAVE && !frighten && ghosts[i].moveType != DEATH)
            {
                ghosts[i].moveType = SCATTER;
            }
        }
    }
    else if (gameTick >= 1950)
    {
        for (int i = 0; i < 4; i++)
        {
            if (ghosts[i].moveType != PEN && ghosts[i].moveType != LEAVE && !frighten && ghosts[i].moveType != DEATH)
            {
                ghosts[i].moveType = CHASE;
            }
        }
    }
    else if (gameTick >= 1600)
    {
        for (int i = 0; i < 4; i++)
        {
            if (ghosts[i].moveType != PEN && ghosts[i].moveType != LEAVE && !frighten && ghosts[i].moveType != DEATH)
            {
                ghosts[i].moveType = SCATTER;
            }
        }
    }
    else if (gameTick >= 600)
    {
        for (int i = 0; i < 4; i++)
        {
            if (ghosts[i].moveType != PEN && ghosts[i].moveType != LEAVE && !frighten && ghosts[i].moveType != DEATH)
            {
                ghosts[i].moveType = CHASE;
            }
        }
    }
}

// power-up timers
void updatePowerUps()
{
    // Update freeze timer
    if (freezeTimer > 0)
    {
        freezeTimer--;
        if (freezeTimer == 0)
        {
            ghostsFrozen = false;
        }
    }

    // Update other power-up timers
    if (powerUpTimer > 0)
    {
        powerUpTimer--;
        if (powerUpTimer == 0)
        {
            speedBoostActive = false;
            doublePointsActive = false;
        }
    }
}

// Handle game logic during idle time
void idle()
{
    if (!paused)
    {
        switch (stateGame)
        {
        case INTRO:
    // Play intro music when animation starts
    if (introTick == 0)
    {
        playIntroMusic();
    }

    // Update intro animation
    introTick++;

    // Move Pacman from left to right at constant speed
    introPacmanX += 0.4f;

    // Move ghosts following Pacman with proper distance gaps
    introGhostsX[0] = introPacmanX - 50.0f;
    introGhostsX[1] = introPacmanX - 100.0f;
    introGhostsX[2] = introPacmanX - 150.0f;
    introGhostsX[3] = introPacmanX - 200.0f;

    // Check if animation is complete
    if (introPacmanX > 15.0f && !introComplete)
    {
        introComplete = true;
        if (introTick > 150)
        {
            stopIntroMusic(); // STOP MUSIC when intro ends
            
            // IMPORTANT: Force screen clear before switching to loading
            glClear(GL_COLOR_BUFFER_BIT);
            glutSwapBuffers();
            
            // Small delay to ensure screen is cleared
            for(int i = 0; i < 5; i++) {
                glClear(GL_COLOR_BUFFER_BIT);
                glutSwapBuffers();
            }
            
            // Now switch to loading screen
            stateGame = LOADING;
            loadingProgress = 0;
            loadingTimer = 0;
            
            // Force immediate redraw to loading screen
            glutPostRedisplay();
        }
    }
    break;

        case LOADING:
            // Update loading progress
            loadingTimer++;
            loadingProgress = (loadingTimer * 100) / LOADING_DURATION;
            if (loadingProgress > 100) loadingProgress = 100;
            
            // Simulate loading tasks
            if (loadingTimer == 30) {
                // Task 1
            }
            else if (loadingTimer == 60) {
                // Task 2
            }
            else if (loadingTimer == 90) {
                // Task 3
            }
            else if (loadingTimer == 120) {
                // Task 4
            }
            else if (loadingTimer == 150) {
                // Task 5
            }
            
            // When loading is complete
            if (loadingTimer >= LOADING_DURATION) {
                stateGame = BEGIN;
                gameTick = 0;
            }
            break;

        case BEGIN:
            if (gameTick >= 250)
            {
                stateGame = PLAY;
            }
            break;

        case PLAY:
            setMode();
            detectPill();
            pacman.checkTile();
            detectGhost();
            pacman.move();
            detectGhost();
            for (int i = 0; i < 4; i++)
            {
                ghosts[i].move(ghosts[0]);
            }
            detectGhost();

            // Power-up updates
            spawnPowerUp();
            updatePowerUps();

            if (frightenTick <= 450 && frighten)
            {
                frightenTick++;
            }
            else if (frighten)
            {
                frighten = false;
                eatenCount = 0;
                frightenTick = 0;
                for (int i = 0; i < 4; i++)
                {
                    if (ghosts[i].moveType == FRIGHTEN)
                    {
                        ghosts[i].eaten = false;
                    }
                }
                setMode();
            }
            if (pills <= 0)
            {
                fruitReset();
                resetMaze();
                pacman.reset();
                pills = 244;
                fruitSpawned = 0;
                fruitCounter = 0;
                tempFruitCounter = 0;
                ghostsFrozen = false;
                doublePointsActive = false;
                speedBoostActive = false;
                freezeTimer = 0;
                powerUpTimer = 0;
                for (int i = 0; i < 4; i++)
                {
                    ghosts[i].reset();
                }
                gameTick = 0;
                stateGame = BEGIN;
            }
            break;

        case DIE:
            if (gameTick - timestamp >= 150)
            {
                deathTick++;
            }
            break;
        }
        gameTick++;
        glutPostRedisplay();
    }
}

// hanling input keys
void special(int key, int, int)
{
    switch (key)
    {
    case GLUT_KEY_LEFT:
        pacman.setDirStore(LEFT);
        break;
    case GLUT_KEY_UP:
        pacman.setDirStore(UP);
        break;
    case GLUT_KEY_RIGHT:
        pacman.setDirStore(RIGHT);
        break;
    case GLUT_KEY_DOWN:
        pacman.setDirStore(DOWN);
        break;
    }
}

// initialize openGl settings
void init()
{
    glMatrixMode(GL_PROJECTION); // set up projection matrix
    glLoadIdentity();            // clear old matrix
    gluOrtho2D(0, 420, 0, 300);  // create wider 2D world for mini-map
    loadAndBindTextures();       // load all items
}

// Display all elements
void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    switch (stateGame)
    {
    case INTRO:
        drawIntro();
        break;
    case LOADING:
        drawLoadingScreen();
        break;
    case BEGIN:
        drawMaze();
        pacman.draw();
        for (int i = 0; i < 4; i++)
        {
            ghosts[i].draw();
        }
        drawInterface();
        drawReady();
        if (showMiniMap) drawMiniMap();
        break;
    case PLAY:
        drawMaze();
        pacman.draw();
        if (pills <= 174 && fruitSpawned == 0)
        {
            locationFruit();
        }
        if (pills <= 74 && fruitSpawned == 1)
        {
            locationFruit();
        }
        if (fruitCounter > tempFruitCounter)
        {
            eatFruitStamp = gameTick;
        }
        if (gameTick - eatFruitStamp <= 200)
        {
            eatFruitScore(eatenFruitX, eatenFruitY);
        }
        for (int i = 0; i < 4; i++)
        {
            ghosts[i].draw();
            if (frighten && gameTick - eatStamp <= 200)
            {
                ghosts[i].drawEatScore(eatenX, eatenY);
            }
        }
        drawInterface();
        if (showMiniMap) drawMiniMap();
        tempFruitCounter = fruitCounter;
        break;
    case DIE:
        fruitReset();
        drawMaze();
        if (gameTick - timestamp < 150)//Draw collision frame
        {
            pacman.draw();
            for (int i = 0; i < 4; i++)
            {
                ghosts[i].draw();
            }
        }
        if (gameTick - timestamp >= 150 && deathTick < 110)//animation
        {
            pacman.death();
        }
        if (gameTick - timestamp >= 310)//reset
        {
            pacman.reset();
            for (int i = 0; i < 4; i++)
            {
                ghosts[i].reset();
            }
            gameTick = 0;
            eatFruitStamp = -999999999;//no longer show fruit score
            deathTick = 0;
            if (lives > 0)
            {
                stateGame = BEGIN;
            }
            else
            {
                setHighScore(score);
                stateGame = OVER;
            }
        }
        drawInterface();
        if (showMiniMap) drawMiniMap();
        break;
    case OVER:
        drawMaze();
        drawInterface();
        drawGameOver();
        if (showMiniMap) drawMiniMap();
    }

    glutSwapBuffers();
}

// Reset the game
void resetGame()
{
    gameTick = 0;
    deathTick = 0;
    score = 0;
    lives = 3;
    pills = 244;
    fruitCounter = 0;
    fruitSpawned = 0;
    tempFruitCounter = 0;

    // RESET POWER-UP VARIABLES
    ghostsFrozen = false;
    doublePointsActive = false;
    speedBoostActive = false;
    freezeTimer = 0;
    powerUpTimer = 0;

    //intro animation reset
    introPacmanX = -5.0f;
    for (int i = 0; i < 4; i++)
    {
        introGhostsX[i] = -8.0f - (i * 2.0f);
    }
    introTick = 0;
    introComplete = false;

    // RESET LOADING VARIABLES
    loadingProgress = 0;
    loadingTimer = 0;

    stateGame = INTRO; // Start with intro
}

// Handle keyboard input
void keyboard(unsigned char key, int, int)
{
    switch (key)
    {
    case 'q':
        cleanupAudio();
        exit(1);
    case 'p':
        if (paused)
        {
            paused = false;
        }
        else
        {
            paused = true;
        }
        break;
    case 'm':  
    case 'M':
        showMiniMap = !showMiniMap;
        cout << "Mini-map: " << (showMiniMap ? "ON" : "OFF") << endl;
        break;
    case 'r':
        if (stateGame == OVER)
        {
            resetGame();
            pacman.reset();
            fruitReset();
            resetMaze();
            for (int i = 0; i < 4; i++)
            {
                ghosts[i].reset();
            }
        }
        break;
    case ' ':  // Space bar to skip
        if (stateGame == INTRO)
        {
            stopIntroMusic();
            stateGame = LOADING;
            loadingProgress = 0;
            loadingTimer = 0;
        }
        else if (stateGame == LOADING)
        {
            stateGame = BEGIN;
            gameTick = 0;
        }
        break;
    }

    glutPostRedisplay();
}

int main(int argc, char *argv[])
{
    if (initAudio())
    {
        if (loadSounds())
        {
            audioInitialized = true;
            cout << "Audio system initialized successfully!" << endl;
        }
        else
        {
            cout << "Failed to load sounds, continuing without audio..." << endl;
        }
    }
    else
    {
        cout << "Failed to initialize audio, continuing without sound..." << endl;
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(700, 550);
    glutCreateWindow("Pacman");
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutIdleFunc(idle);
    init();
    atexit(cleanupAudio);
    glutMainLoop();
    return 0;
}
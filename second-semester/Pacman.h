#ifndef COURSEWORK_PACMAN_H
#define COURSEWORK_PACMAN_H

#include <SDL2/SDL_mixer.h>  

// Add extern declaration for playSound function and sound variables
extern void playSound(Mix_Chunk* sound);
extern Mix_Chunk* eatPillSound; 

// Add extern declarations for power-up variables
extern bool ghostsFrozen;
extern int freezeTimer;
extern bool doublePointsActive;
extern bool speedBoostActive;
extern int powerUpTimer;

int pacTick = 0;

extern int score;
extern int pills;
extern int deathTick;
extern bool dead;
extern int eatenCount;
extern int fruitCounter;


typedef enum {LEFT, UP, RIGHT, DOWN, NONE} direction;

class Pacman
{
public:
    float pacmanX;
    float pacmanY;
    direction currentDir;
    direction dirStore;
    float angle;
    bool begin;  

    Pacman()
    {
        pacmanX = 13.5f;
        pacmanY =  7.0f;
        currentDir = NONE;
        dirStore = NONE;
        begin = true;  
    }

    //Draw pacman animation
    void draw()
    {
        glPushMatrix();//saves the current transformation matrix

        unsigned int pac_tex;
        int tickDiv;

        if(!dead) {
            tickDiv = pacTick % 30;//mouth open and close animation after every 30 ticks
        }
        if(currentDir == NONE) {
            tickDiv = 0;
        }
        if(gameTick < 250) { // If the game is not in BEGIN mode
            pac_tex = pac_2_tex;
        } else {
            if (tickDiv % 30 < 10) {
                pac_tex = pac_0_tex;
            } else if (tickDiv % 30 < 20) {
                pac_tex = pac_1_tex;
            } else {
                pac_tex = pac_2_tex;
            }
        }

        switch(currentDir){ // Rotates pacman to face in his current travel direction
            case LEFT: angle = 0.0f;
                break;
            case UP: angle = 270.0f;
                break;
            case RIGHT: angle = 180.0f;
                break;
            case DOWN: angle = 90.0f;
                break;
            default: currentDir = NONE;
        }

        translateBottomLeft(); // Translate to bottom left of screen which is (0,0)
        translateToMazeCoords(pacmanX, pacmanY);

        glTranslatef(-3.0f, -3.0f, 0.0f); // Translate to take into account pacmans size

        drawTex(pac_tex, 14, 14, angle);

        glPopMatrix();//restores the previous transformation matrix

        pacTick++;

    }

    //Reset pacman 
    void reset()
    {
        pacmanX = 13.5f;
        pacmanY =  7.0f;
        currentDir = NONE;
        dirStore = NONE;
        angle = 0.0f;
        begin = true;  // RESET begin flag
        dead = false;
        eatenCount = 0;
    }

    // Applying death animation
    void death()
    {
        glPushMatrix();

        unsigned int pac_tex;

        if (deathTick < 10) {                  
            pac_tex = dead_0_tex;
        } else if (deathTick % 110 < 20) {
            pac_tex = dead_1_tex;
        } else if (deathTick % 110 < 30) {
            pac_tex = dead_2_tex;
        } else if (deathTick % 110 < 40) {
            pac_tex = dead_3_tex;
        } else if (deathTick % 110 < 50) {
            pac_tex = dead_4_tex;
        } else if (deathTick % 110 < 60) {
            pac_tex = dead_5_tex;
        } else if (deathTick % 110 < 70) {
            pac_tex = dead_6_tex;
        } else if (deathTick % 110 < 80) {
            pac_tex = dead_7_tex;
        } else if (deathTick % 110 < 90) {
            pac_tex = dead_8_tex;
        } else if (deathTick % 110 < 100) {
            pac_tex = dead_9_tex;
        } else {
            pac_tex = dead_10_tex;
        }

        translateBottomLeft();  
        translateToMazeCoords(pacmanX, pacmanY);

        glTranslatef(-3.0f, -3.0f, 0.0f);

        drawTex(pac_tex, 14, 14, 0);

        glPopMatrix();
    }

    //Eating pill and portal detection 
    void checkTile()
    {
        switch(getTile(pacmanX, pacmanY)) {
            case o:                                                     
                maze[(int) floor(pacmanX)][(int) floor(pacmanY)] = e;
                if(doublePointsActive) {
                    score += 20; // Double points if active
                } else {
                    score += 10;
                }
                pills--;
                playSound(eatPillSound); 
                break;
            case O:                                                     
                maze[(int) floor(pacmanX)][(int) floor(pacmanY)] = E;
                if(doublePointsActive) {
                    score += 100; // Double points if active
                } else {
                    score += 50;
                }
                pills--;
                break;
            case P:
                if (getPacX() >= 26.5) {                             
                    pacmanX = 1.0f;
                } else {
                    pacmanX = 26.5f;
                }
                break;
            case F:                                     
                maze[(int) floor(pacmanX)][(int) floor(pacmanY)] = e;
                fruitCounter++; 
                if(fruitCounter == 1) { 
                    if(doublePointsActive) {
                        score += 200; // Double points if active
                    } else {
                        score += 100;
                    }
                } else {
                    if(doublePointsActive) {
                        score += 600; // Double points if active
                    } else {
                        score += 300;
                    }
                }
                break;
            
            case ICE:
                maze[(int) floor(pacmanX)][(int) floor(pacmanY)] = e;
                ghostsFrozen = true;
                freezeTimer = 300; // 5 seconds at 60 FPS
                if(doublePointsActive) {
                    score += 400; // Double points if active
                } else {
                    score += 200;
                }
                break;
            case SPEED:
                maze[(int) floor(pacmanX)][(int) floor(pacmanY)] = e;
                speedBoostActive = true;
                powerUpTimer = 300; // 5 seconds
                if(doublePointsActive) {
                    score += 400; // Double points if active
                } else {
                    score += 200;
                }
                break;
            case DOUBLE_SCORE:
                maze[(int) floor(pacmanX)][(int) floor(pacmanY)] = e;
                doublePointsActive = true;
                powerUpTimer = 300; // 5 seconds
                if(doublePointsActive) {
                    score += 400; // Double points if active (applies immediately)
                } else {
                    score += 200;
                }
                break;
        }
    }

    int getPacX()
    {
        return (int)floor(pacmanX);
    }

    int getPacY()
    {
        return (int)floor(pacmanY);
    }

    tile getTile(float x, float y)
    {
        return maze[(int)floor(x)][(int)floor(y)];
    }
    
    //Helps prevent Pac-Man from moving into walls.
    tile getFollowingTile(direction dir)
    {
        switch(dir) {
            case LEFT: return getTile(getPacX() - 1, getPacY());
            case UP: return getTile(getPacX(), getPacY() + 1);
            case RIGHT: return getTile(getPacX() +1, getPacY());
            case DOWN: return getTile(getPacX(), getPacY() - 1);
            default: return getTile(getPacX(), getPacY());
        }
    }

    //check pacman is at center of tile
    bool isAtCenter()
    {
        return (int)(pacmanY * 10.0f) % 10 == 0 && (int)(pacmanX * 10.0f) % 10 == 0;
    }

    // Used to pass the user pressed direction to the dirStore variable
    void setDirStore(direction pressedDir)
    {
        dirStore = pressedDir;
    }

    //Checks if the tile in the specified direction is a wall
    bool isWall(direction dir)
    {
        if (getFollowingTile(dir) == W || getFollowingTile(dir) == G) {
            return true;
        }

        return false;
    }

    //move pacman 
    void move()
    {
        float currentSpeed = 0.1f; // Base speed
        
        // Apply speed boost if active
        if(speedBoostActive) {
            currentSpeed = 0.15f; // 50% speed increase
        }
        
        if(isAtCenter()) {
            if (!isWall(dirStore)) { 
                currentDir = dirStore;
            } else if (isWall(currentDir)) {
                currentDir = NONE; 
            }
        }
        if(!isAtCenter() && dirStore != NONE && begin && !isWall(dirStore)){ // Check to not cause pacman to jump at the very beginning of the game
            currentDir = dirStore;
            begin = false;
        }

        switch (currentDir) {
            case LEFT:
                pacmanX -= currentSpeed;
                pacmanY = round(pacmanY);
                break;
            case UP:
                pacmanY += currentSpeed;
                pacmanX = round(pacmanX);
                break;
            case RIGHT:
                pacmanX += currentSpeed;
                pacmanY = round(pacmanY);
                break;
            case DOWN:
                pacmanY -= currentSpeed;
                pacmanX = round(pacmanX);
                break;
            default:
                if(!begin) { 
                    pacmanX = round(pacmanX);
                    pacmanY = round(pacmanY);
                }
        }
    }
};

#endif
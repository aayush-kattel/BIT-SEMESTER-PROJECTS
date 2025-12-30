#ifndef COURSEWORK_MAZE_H
#define COURSEWORK_MAZE_H

extern unsigned int maze_tex;
extern unsigned int pill_tex;
extern unsigned int bigPill_tex;
extern int gameTick;
extern int fruitSpawned;
extern int fruitCounter;

// Add new extern variables for power-ups
extern bool ghostsFrozen;
extern int freezeTimer;
extern bool doublePointsActive;
extern bool speedBoostActive;
extern int powerUpTimer;

typedef enum {W, G, P, u, o, e, O, E, F, ICE, SPEED, DOUBLE_SCORE} tile;

tile maze[28][31] =
        {
                {W,W,W,W,W,W,W,W,W,W,W,W,u,u,u,W,P,W,u,u,u,W,W,W,W,W,W,W,W,W,W},
                {W,o,o,o,o,W,W,O,o,o,o,W,u,u,u,W,u,W,u,u,u,W,o,o,o,o,O,o,o,o,W},
                {W,o,W,W,o,W,W,o,W,W,o,W,u,u,u,W,u,W,u,u,u,W,o,W,W,o,W,W,W,o,W},
                {W,o,W,W,o,o,o,o,W,W,o,W,u,u,u,W,u,W,u,u,u,W,o,W,W,o,W,W,W,o,W},
                {W,o,W,W,o,W,W,W,W,W,o,W,u,u,u,W,u,W,u,u,u,W,o,W,W,o,W,W,W,o,W},
                {W,o,W,W,o,W,W,W,W,W,o,W,W,W,W,W,u,W,W,W,W,W,o,W,W,o,W,W,W,o,W},
                {W,o,W,W,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,W},
                {W,o,W,W,W,W,W,o,W,W,o,W,W,W,W,W,u,W,W,W,W,W,W,W,W,o,W,W,W,o,W},
                {W,o,W,W,W,W,W,o,W,W,o,W,W,W,W,W,u,W,W,W,W,W,W,W,W,o,W,W,W,o,W},
                {W,o,W,W,o,o,o,o,W,W,o,u,u,u,u,u,u,u,u,u,W,W,o,o,o,o,W,W,W,o,W},
                {W,o,W,W,o,W,W,o,W,W,o,W,W,u,W,W,W,W,W,u,W,W,o,W,W,o,W,W,W,o,W},
                {W,o,W,W,o,W,W,o,W,W,o,W,W,u,W,u,u,u,W,u,W,W,o,W,W,o,W,W,W,o,W},
                {W,o,o,o,o,W,W,o,o,o,o,W,W,u,W,u,u,u,W,u,u,u,o,W,W,o,o,o,o,o,W},
                {W,o,W,W,W,W,W,u,W,W,W,W,W,u,W,u,u,u,G,u,W,W,W,W,W,o,W,W,W,W,W},
                {W,o,W,W,W,W,W,u,W,W,W,W,W,u,W,u,u,u,G,u,W,W,W,W,W,o,W,W,W,W,W},
                {W,o,o,o,o,W,W,o,o,o,o,W,W,u,W,u,u,u,W,u,u,u,o,W,W,o,o,o,o,o,W},
                {W,o,W,W,o,W,W,o,W,W,o,W,W,u,W,u,u,u,W,u,W,W,o,W,W,o,W,W,W,o,W},
                {W,o,W,W,o,W,W,o,W,W,o,W,W,u,W,W,W,W,W,u,W,W,o,W,W,o,W,W,W,o,W},
                {W,o,W,W,o,o,o,o,W,W,o,u,u,u,u,u,u,u,u,u,W,W,o,o,o,o,W,W,W,o,W},
                {W,o,W,W,W,W,W,o,W,W,o,W,W,W,W,W,u,W,W,W,W,W,W,W,W,o,W,W,W,o,W},
                {W,o,W,W,W,W,W,o,W,W,o,W,W,W,W,W,u,W,W,W,W,W,W,W,W,o,W,W,W,o,W},
                {W,o,W,W,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,o,W},
                {W,o,W,W,o,W,W,W,W,W,o,W,W,W,W,W,u,W,W,W,W,W,o,W,W,o,W,W,W,o,W},
                {W,o,W,W,o,W,W,W,W,W,o,W,u,u,u,W,u,W,u,u,u,W,o,W,W,o,W,W,W,o,W},
                {W,o,W,W,o,o,o,o,W,W,o,W,u,u,u,W,u,W,u,u,u,W,o,W,W,o,W,W,W,o,W},
                {W,o,W,W,o,W,W,o,W,W,o,W,u,u,u,W,u,W,u,u,u,W,o,W,W,o,W,W,W,o,W},
                {W,o,o,o,o,W,W,O,o,o,o,W,u,u,u,W,u,W,u,u,u,W,o,o,o,o,O,o,o,o,W},
                {W,W,W,W,W,W,W,W,W,W,W,W,u,u,u,W,P,W,u,u,u,W,W,W,W,W,W,W,W,W,W}
        };

// move to maze coordinates
void translateToMazeCoords(float x, float y) {
    glTranslatef(x * 8, y * 8, 0.0f);
}

//start from bottom left
void translateBottomLeft() {
    glTranslatef(38.0f, 26.0f, 0.0f);
}

//Return tile postion
tile getTile(float x, float y) {
    return maze[(int) floor(x)][(int) floor(y)];
}

//Draw fruit eat score
void eatFruitScore(int x, int y) {
    glPushMatrix();

    unsigned int eat_score_tex;

    switch(fruitCounter) { 
        case 1: eat_score_tex = score_100_tex;
            break;
        case 2: eat_score_tex = score_300_tex;
            break;
    }

    translateBottomLeft();
    translateToMazeCoords(x, y); 
    glTranslatef(-2.0f, 0.0f, 0.0f);

    if(fruitCounter != 0) {
        drawTex(eat_score_tex, 15, 8, 0);
    }

    glPopMatrix();
}

//Reset fruits
void fruitReset() {
    for(int x=0;x<28;x++) {           
        for(int y=0;y<31;y++) {
            switch(maze[x][y]) {
                case F:
                    maze[x][y] = e;
                    break;
                case ICE:       
                case SPEED:
                case DOUBLE_SCORE:
                    maze[x][y] = e;
                    break;
            }
        }
    }
}

//Randomly locate fruits in maze
void locationFruit() {
    int x, y;
    do {
        x = rand() % 27;//0-26
        y = rand() % 12;//0-12
    } while(getTile(x, y) != e);

    maze[(int) floor(x)][(int) floor(y)] = F;
    fruitSpawned++; 
}

void spawnPowerUp() {
    if(rand() % 450 == 0) { 
        int x, y;
        int attempts = 0;
        
        do {
            x = rand() % 27;
            y = rand() % 31;
            attempts++;
        } while(getTile(x, y) != e && attempts < 50);
        
        if(attempts < 50) {
            int powerUpType = rand() % 3;
            switch(powerUpType) {
                case 0: maze[x][y] = ICE; break;
                case 1: maze[x][y] = SPEED; break;
                case 2: maze[x][y] = DOUBLE_SCORE; break;
            }
        }
    }
}

void drawFruit() {
    glTranslatef(-2.0f, -2.0f, 0.0f);
    if(fruitCounter == 0) { 
        drawTex(cherry_tex, 14, 14, 0);
    } else {
        drawTex(strawberry_tex, 14, 14, 0);
    }
    glTranslatef(2.0f, 2.0f, 0.0f);
}

void drawPowerUp(tile powerUpType) {
    glPushMatrix();
    glTranslatef(-3.0f, -3.0f, 0.0f); // Center the power-up on tile
    
    switch(powerUpType) {
        case ICE:
            drawTex(ice_tex, 10, 10, 0); // Scaled to fit maze
            break;
        case SPEED:
            drawTex(speed_tex, 10, 10, 0); // Scaled to fit maze
            break;
        case DOUBLE_SCORE:
            drawTex(double_tex, 10, 10, 0); // Scaled to fit maze
            break;
    }
    
    glPopMatrix();
}

void drawMaze() {
    glPushMatrix();

    translateBottomLeft();             
    drawTex(maze_tex, 224, 248, 0);     
    for(int x=0;x<28;x++) {            
        glPushMatrix();

        for(int y=0;y<31;y++) {
            switch(maze[x][y]) {
                case o:                 
                    drawTex(pill_tex, 8, 8, 0);
                    break;
                case O:               
                    if(gameTick % 30 <= 15) {
                        drawTex(bigPill_tex, 8, 8, 0);
                    }
                    break;
                case F:
                    drawFruit();
                    break;
                case ICE:           // ADD CASES FOR NEW ITEMS
                case SPEED:
                case DOUBLE_SCORE:
                    drawPowerUp(maze[x][y]);
                    break;
            }

            glTranslatef(0.0f, 8.0f, 0.0f);
        }
        glPopMatrix();

        glTranslatef(8.0f, 0.0f, 0.0f);
    }
    glPopMatrix();
}

void resetMaze() {
    for(int x=0;x<28;x++) {             
        for (int y = 0; y < 31; y++) {
            switch (maze[x][y]) {
                case e:
                    maze[x][y] = o;
                    break;
                case E:
                    maze[x][y] = O;
                    break;
            }
        }
    }
}

#endif
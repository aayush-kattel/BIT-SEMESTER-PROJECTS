#ifndef COURSEWORK_INTERFACE_H
#define COURSEWORK_INTERFACE_H

using namespace std;

// External declarations
extern int score;
extern int lives;
extern float introPacmanX;
extern float introGhostsX[4];
extern int introTick;

// Mini-map external declarations
extern Pacman pacman;
extern Ghost ghosts[4];
extern int gameTick;

// Mini-map variables (defined in pacman.cpp)
extern bool showMiniMap;
extern int miniMapX, miniMapY, miniMapWidth, miniMapHeight;
extern int miniMapViewRadius;

// Loading screen variables - ADDED
extern int loadingProgress;
extern int loadingTimer;
extern const int LOADING_DURATION;
extern string loadingMessages[5];

//Draw score
void drawScore(int scorePrint)
{
    glPushMatrix();
    translateBottomLeft();
    translateToMazeCoords(15.0, 32.5);
    drawTex(score_tex, 40, 8, 0); 
    translateToMazeCoords(4, -1.0); // move cursor below "SCORE" text
    string str = to_string(scorePrint); // Convert the current score to a string to enable iteration through number

    for(int i = str.length() -1; i>=0 ; i--) { 
        switch(str[i]) {
            case '0':
                drawTex(num_0_tex, 8, 8, 0);
                break;
            case '1':
                drawTex(num_1_tex, 8, 8, 0);
                break;
            case '2':
                drawTex(num_2_tex, 8, 8, 0);
                break;
            case '3':
                drawTex(num_3_tex, 8, 8, 0);
                break;
            case '4':
                drawTex(num_4_tex, 8, 8, 0);
                break;
            case '5':
                drawTex(num_5_tex, 8, 8, 0);
                break;
            case '6':
                drawTex(num_6_tex, 8, 8, 0);
                break;
            case '7':
                drawTex(num_7_tex, 8, 8, 0);
                break;
            case '8':
                drawTex(num_8_tex, 8, 8, 0);
                break;
            case '9':
                drawTex(num_9_tex, 8, 8, 0);
                break;
        }
        translateToMazeCoords(-1, 0); // Translate when the ^10 index is increased by 1 to ensure number reads correctly
    }
    glPopMatrix();
}

//Draw lives
void drawLives(int liveCount)
{
    glPushMatrix();
    translateBottomLeft();
    translateToMazeCoords(2, -2); 
    for(int i = 0; i < liveCount - 1; i++) { 
        drawTex(pac_1_tex, 14, 14, 0);
        translateToMazeCoords(2, 0); //move to right for next life
    }
    glPopMatrix();
}

//return highscore
int getHighScore()
{
    int highScore;
    fstream scoreFile("high_score.txt"); 
    if(scoreFile.good()) { 
        scoreFile >> highScore;
        scoreFile.close(); 
        return highScore; 
    } else {
        ofstream highScoreFile("high_score.txt"); 
        highScoreFile << 0; 
        scoreFile.close();
        highScoreFile.close();
    }
    return 0; 
}

//set highscore
void setHighScore(int score)
{
    int currentHigh = getHighScore(); 
    if(currentHigh < score) { 
        ofstream highFile("high_score.txt"); 
        highFile.clear();
        highFile << score; 
        highFile.close(); 
    }
}

//Draw high score
void drawHigh()
{
    glPushMatrix();
    translateBottomLeft();
    translateToMazeCoords(8.0, 32.5); 
    drawTex(high_tex, 32, 7, 0); 
    translateToMazeCoords(3, -1.0); 
    string str = to_string(getHighScore()); 

    for(int i = str.length() -1; i>=0 ; i--) { 
        switch(str[i]) {
            case '0':
                drawTex(num_0_tex, 8, 8, 0);
                break;
            case '1':
                drawTex(num_1_tex, 8, 8, 0);
                break;
            case '2':
                drawTex(num_2_tex, 8, 8, 0);
                break;
            case '3':
                drawTex(num_3_tex, 8, 8, 0);
                break;
            case '4':
                drawTex(num_4_tex, 8, 8, 0);
                break;
            case '5':
                drawTex(num_5_tex, 8, 8, 0);
                break;
            case '6':
                drawTex(num_6_tex, 8, 8, 0);
                break;
            case '7':
                drawTex(num_7_tex, 8, 8, 0);
                break;
            case '8':
                drawTex(num_8_tex, 8, 8, 0);
                break;
            case '9':
                drawTex(num_9_tex, 8, 8, 0);
                break;
        }
        translateToMazeCoords(-1, 0); // Translate when the ^10 index is increased by 1 to ensure number reads correctly
    }
    glPopMatrix();
}

//Draw "Ready" text
void drawReady()
{
    glPushMatrix();
    translateBottomLeft();
    translateToMazeCoords(11.5f, 13.0f); 
    drawTex(ready_tex, 40, 8, 0);
    glPopMatrix();
}

void drawGameOver()
{
    glPushMatrix();
    translateBottomLeft();
    translateToMazeCoords(10.0f, 12.75f); 
    drawTex(gameover_tex, 65, 10, 0);
    glPopMatrix();
}

void drawInterface()
{
    drawHigh();
    drawScore(score);
    drawLives(lives);
}

//Draw intro
void drawIntro() {
    glPushMatrix();
    
    // Draw black background
    glColor3f(0.0f, 0.0f, 0.0f); // Black background
    glBegin(GL_QUADS);//Draws a 300x300 pixel square
        glVertex2i(0, 0);
        glVertex2i(300, 0);
        glVertex2i(300, 300);
        glVertex2i(0, 300);
    glEnd();
    
    // Draw "PACMAN" title
    glColor3f(1.0f, 1.0f, 0.0f); // Yellow color
    glRasterPos2f(110, 250);
    string title = "PACMAN";
    for (char c : title) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
    
    glRasterPos2f(200, 55); 
    string Created = "CREATED BY"; 
    for (char c : Created) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }

    glRasterPos2f(220, 40); 
    string yourName = "Group 11"; 
    for (char c : yourName) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }

    // Draw animated characters in the middle of screen
    glTranslatef(150, 150, 0); // Center of screen
    
    // Draw ACTUAL Pacman with mouth animation
    glPushMatrix();
    glTranslatef(introPacmanX, 0, 0);
    
    // Animate Pacman's mouth (open/close like in game)
    unsigned int pac_tex;
    int animFrame = (introTick / 10) % 3; // Cycle through 3 frames
    switch(animFrame) {
        case 0: pac_tex = pac_0_tex; break; // Open
        case 1: pac_tex = pac_1_tex; break; // Half-open  
        case 2: pac_tex = pac_2_tex; break; // Closed
    }
    
    drawTex(pac_tex, 20, 20, 180.0f); // Actual Pacman size
    glPopMatrix();
    
    // Draw ghosts following Pacman with proper distance
    for(int i = 0; i < 4; i++) {
        glPushMatrix();
        glTranslatef(introGhostsX[i], 0, 0);
        
        // Use different ghost colors with animation
        unsigned int ghost_tex;
        int ghostFrame = (introTick / 15) % 2; // Ghost animation
        switch(i) {
            case 0: // Red ghost
                ghost_tex = (ghostFrame == 0) ? ghost_r_0_tex : ghost_r_1_tex;
                break;
            case 1: // Pink ghost  
                ghost_tex = (ghostFrame == 0) ? ghost_p_0_tex : ghost_p_1_tex;
                break;
            case 2: // Blue ghost
                ghost_tex = (ghostFrame == 0) ? ghost_b_0_tex : ghost_b_1_tex;
                break;
            case 3: // Yellow ghost
                ghost_tex = (ghostFrame == 0) ? ghost_y_0_tex : ghost_y_1_tex;
                break;
        }
        
        drawTex(ghost_tex, 20, 20, 0); // Actual ghost size
        glPopMatrix();
    }
    
    glPopMatrix();
}

// Draw loading screen 
void drawLoadingScreen() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Draw full black background
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
        glVertex2i(0, 0);
        glVertex2i(700, 0);
        glVertex2i(700, 550);
        glVertex2i(0, 550);
    glEnd();
    
    // Screen center coordinates
    int screenCenterX = 200;  // 700 / 2
    int screenCenterY = 250;  // 550 / 2
    
    // Draw "LOADING..." text at top center
    glColor3f(1.0f, 1.0f, 0.0f); // Yellow color
    glRasterPos2f(screenCenterX - 50, 450); // Centered horizontally
    string loadingText = "LOADING...";
    for (char c : loadingText) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
    
    // Draw animated Pacman in center
    glPushMatrix();
    glTranslatef(screenCenterX, screenCenterY, 0); // Center of screen
    
    // Animate Pacman's mouth
    unsigned int pac_tex;
    int animFrame = (loadingTimer / 5) % 3;
    switch(animFrame) {
        case 0: pac_tex = pac_0_tex; break;
        case 1: pac_tex = pac_1_tex; break;
        case 2: pac_tex = pac_2_tex; break;
    }
    
    // Rotate Pacman slowly
    float rotation = loadingTimer * 2.0f;
    glRotatef(rotation, 0.0f, 0.0f, 1.0f);
    
    drawTex(pac_tex, 60, 60, 0.0f); // Large Pacman
    glPopMatrix();
    
    // Progress bar dimensions (centered)
    int barWidth = 250;
    int barHeight = 15;
    int barX = screenCenterX - (barWidth / 2);
    int barY = screenCenterY - 100;
    
    // Draw progress bar background
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_QUADS);
        glVertex2i(barX, barY);
        glVertex2i(barX + barWidth, barY);
        glVertex2i(barX + barWidth, barY + barHeight);
        glVertex2i(barX, barY + barHeight);
    glEnd();
    
    // Draw progress bar fill
    int filledWidth = (loadingProgress * barWidth) / 100;
    glColor3f(0.0f, 1.0f, 0.0f); // Green fill
    glBegin(GL_QUADS);
        glVertex2i(barX, barY);
        glVertex2i(barX + filledWidth, barY);
        glVertex2i(barX + filledWidth, barY + barHeight);
        glVertex2i(barX, barY + barHeight);
    glEnd();
    
    // Draw progress percentage (centered below bar)
    glColor3f(1.0f, 1.0f, 1.0f);
    string percentText = to_string(loadingProgress) + "%";
    int textWidth = percentText.length() * 8; // Approximate width
    glRasterPos2f(screenCenterX - (textWidth / 2), barY - 30);
    for (char c : percentText) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }
    
    // Draw loading message (centered below percentage)
    int messageIndex = loadingProgress / 20;
    if (messageIndex > 4) messageIndex = 4;
    string message = loadingMessages[messageIndex];
    int msgWidth = message.length() * 8; // Approximate width
    glRasterPos2f(screenCenterX - (msgWidth / 2), barY - 50);
    for (char c : message) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }
    
    // Draw animated dots below (centered)
    glColor3f(0.0f, 1.0f, 0.0f);
    glPointSize(8.0f);
    glBegin(GL_POINTS);
        for(int i = 0; i < 3; i++) {
            int offset = (loadingTimer / 5 + i * 10) % 30;
            if(offset < 10) {
                glVertex2i(screenCenterX - 20 + (i * 20), barY - 80);
            }
        }
    glEnd();
}

//mini-map drawing
void drawMiniMap() {
    if (!showMiniMap) return;
    
    glPushMatrix();
    
    // Draw mini-map background frame
    glColor3f(0.0f, 0.0f, 0.0f); // Black frame
    glBegin(GL_QUADS);
        glVertex2i(miniMapX - 3, miniMapY - 3);
        glVertex2i(miniMapX + miniMapWidth + 3, miniMapY - 3);
        glVertex2i(miniMapX + miniMapWidth + 3, miniMapY + miniMapHeight + 3);
        glVertex2i(miniMapX - 3, miniMapY + miniMapHeight + 3);
    glEnd();
    
    // Enable texture for drawing the actual maze image
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, maze_tex);
    
    // Calculate which portion of the map to show (centered on Pacman)
    int pacTileX = pacman.getPacX();
    int pacTileY = pacman.getPacY();
    
    // Calculate viewport bounds (clamped to map edges)
    int viewStartX = max(0, pacTileX - miniMapViewRadius);
    int viewEndX = min(27, pacTileX + miniMapViewRadius);
    int viewStartY = max(0, pacTileY - miniMapViewRadius);
    int viewEndY = min(30, pacTileY + miniMapViewRadius);
    
    // Calculate texture coordinates for the visible portion
    float texLeft = viewStartX / 28.0f;
    float texRight = (viewEndX + 1) / 28.0f;
    float texBottom = viewStartY / 31.0f;
    float texTop = (viewEndY + 1) / 31.0f;
    
    // Draw the visible portion of the ACTUAL MAZE TEXTURE
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
        glTexCoord2f(texLeft, texBottom);
        glVertex2i(miniMapX, miniMapY);
        
        glTexCoord2f(texRight, texBottom);
        glVertex2i(miniMapX + miniMapWidth, miniMapY);
        
        glTexCoord2f(texRight, texTop);
        glVertex2i(miniMapX + miniMapWidth, miniMapY + miniMapHeight);
        
        glTexCoord2f(texLeft, texTop);
        glVertex2i(miniMapX, miniMapY + miniMapHeight);
    glEnd();
    
    glDisable(GL_TEXTURE_2D);
    
    // Draw all items on the mini-map
    for(int x = viewStartX; x <= viewEndX; x++) {
        for(int y = viewStartY; y <= viewEndY; y++) {
            // Calculate position on mini-map
            float relX = (x - viewStartX) / (float)(viewEndX - viewStartX + 1);
            float relY = (y - viewStartY) / (float)(viewEndY - viewStartY + 1);
            
            int screenX = miniMapX + (int)(relX * miniMapWidth);
            int screenY = miniMapY + (int)(relY * miniMapHeight);
            
            // Draw items based on tile type
            switch(maze[x][y]) {
                case o: // Small pill - white dot
                    glColor3f(1.0f, 1.0f, 1.0f);
                    glPointSize(2.0f);
                    glBegin(GL_POINTS);
                        glVertex2i(screenX, screenY);
                    glEnd();
                    break;
                    
                case O: // Power pill - blinking white dot
                    if((gameTick / 15) % 2 == 0) { // Blink effect
                        glColor3f(1.0f, 1.0f, 1.0f);
                        glPointSize(3.0f);
                        glBegin(GL_POINTS);
                            glVertex2i(screenX, screenY);
                        glEnd();
                    }
                    break;
                    
                case F: // Fruit - draw actual fruit texture if visible
                    if(screenX >= miniMapX && screenX <= miniMapX + miniMapWidth &&
                       screenY >= miniMapY && screenY <= miniMapY + miniMapHeight) {
                        glPushMatrix();
                        glTranslatef(screenX, screenY, 0);
                        drawTex(cherry_tex, 6, 6, 0);
                        glPopMatrix();
                    }
                    break;
                    
                case ICE: // Ice power-up - blue square
                    glColor3f(0.0f, 0.7f, 1.0f);
                    glPointSize(3.0f);
                    glBegin(GL_POINTS);
                        glVertex2i(screenX, screenY);
                    glEnd();
                    break;
                    
                case SPEED: // Speed power-up - green square
                    glColor3f(0.0f, 1.0f, 0.0f);
                    glPointSize(3.0f);
                    glBegin(GL_POINTS);
                        glVertex2i(screenX, screenY);
                    glEnd();
                    break;
                    
                case DOUBLE_SCORE: // Double score power-up - gold square
                    glColor3f(1.0f, 0.8f, 0.0f);
                    glPointSize(3.0f);
                    glBegin(GL_POINTS);
                        glVertex2i(screenX, screenY);
                    glEnd();
                    break;
            }
        }
    }
    
    // Draw viewport border (green border around visible area)
    glColor4f(0.0f, 1.0f, 0.0f, 0.3f); // Semi-transparent green
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
        glVertex2i(miniMapX, miniMapY);
        glVertex2i(miniMapX + miniMapWidth, miniMapY);
        glVertex2i(miniMapX + miniMapWidth, miniMapY + miniMapHeight);
        glVertex2i(miniMapX, miniMapY + miniMapHeight);
    glEnd();
    
    // Draw Pacman on mini-map with ACTUAL ANIMATION
    float pacRelativeX = (pacman.pacmanX - viewStartX) / (float)(viewEndX - viewStartX + 1);
    float pacRelativeY = (pacman.pacmanY - viewStartY) / (float)(viewEndY - viewStartY + 1);
    
    int pacScreenX = miniMapX + (int)(pacRelativeX * miniMapWidth);
    int pacScreenY = miniMapY + (int)(pacRelativeY * miniMapHeight);
    
    // Draw Pacman with actual mouth animation
    glPushMatrix();
    glTranslatef(pacScreenX, pacScreenY, 0);
    
    // Use Pacman's actual animation frames
    unsigned int pac_tex;
    int tickDiv = gameTick % 30;
    if(pacman.currentDir == NONE) {
        pac_tex = pac_2_tex; // Closed mouth when not moving
    } else {
        if (tickDiv % 30 < 10) {
            pac_tex = pac_0_tex;
        } else if (tickDiv % 30 < 20) {
            pac_tex = pac_1_tex;
        } else {
            pac_tex = pac_2_tex;
        }
    }
    
    // Rotate Pacman based on direction
    float pacAngle = 0.0f;
    switch(pacman.currentDir) {
        case LEFT: pacAngle = 0.0f; break;
        case UP: pacAngle = 270.0f; break;
        case RIGHT: pacAngle = 180.0f; break;
        case DOWN: pacAngle = 90.0f; break;
    }
    
    // Draw actual Pacman texture (scaled down)
    drawTex(pac_tex, 8, 8, pacAngle);
    glPopMatrix();
    
    // Draw ghosts on mini-map with ACTUAL TEXTURES
    for(int i = 0; i < 4; i++) {
        int ghostX = ghosts[i].getGhostX();
        int ghostY = ghosts[i].getGhostY();
        
        // Only draw ghosts that are in the current viewport
        if(ghostX >= viewStartX && ghostX <= viewEndX && 
           ghostY >= viewStartY && ghostY <= viewEndY) {
            
            float ghostRelativeX = (ghostX - viewStartX) / (float)(viewEndX - viewStartX + 1);
            float ghostRelativeY = (ghostY - viewStartY) / (float)(viewEndY - viewStartY + 1);
            
            int ghostScreenX = miniMapX + (int)(ghostRelativeX * miniMapWidth);
            int ghostScreenY = miniMapY + (int)(ghostRelativeY * miniMapHeight);
            
            glPushMatrix();
            glTranslatef(ghostScreenX, ghostScreenY, 0);
            
            // Choose ghost texture based on state
            unsigned int ghost_tex;
            
            if(ghosts[i].moveType == FRIGHTEN) {
                // Frightened ghosts - blinking animation
                int frightFrame = (gameTick / 10) % 4;
                switch(frightFrame) {
                    case 0: ghost_tex = ghost_scared_0_tex; break;
                    case 1: ghost_tex = ghost_scared_1_tex; break;
                    case 2: ghost_tex = ghost_scared_2_tex; break;
                    case 3: ghost_tex = ghost_scared_3_tex; break;
                }
            } 
            else if(ghosts[i].moveType == DEATH) {
                // Dead ghosts - just eyes
                switch(ghosts[i].currentDir) {
                    case LEFT: ghost_tex = eye_left_tex; break;
                    case UP: ghost_tex = eye_up_tex; break;
                    case RIGHT: ghost_tex = eye_right_tex; break;
                    case DOWN: ghost_tex = eye_down_tex; break;
                    default: ghost_tex = eye_left_tex;
                }
            } 
            else {
                // Normal ghosts with animation
                int ghostFrame = (gameTick / 20) % 2;
                switch(ghosts[i].ghostColour) {
                    case RED:
                        ghost_tex = (ghostFrame == 0) ? ghost_r_0_tex : ghost_r_1_tex;
                        break;
                    case PINK:
                        ghost_tex = (ghostFrame == 0) ? ghost_p_0_tex : ghost_p_1_tex;
                        break;
                    case BLUE:
                        ghost_tex = (ghostFrame == 0) ? ghost_b_0_tex : ghost_b_1_tex;
                        break;
                    case YELLOW:
                        ghost_tex = (ghostFrame == 0) ? ghost_y_0_tex : ghost_y_1_tex;
                        break;
                }
            }
            
            // Draw actual ghost texture (scaled down)
            drawTex(ghost_tex, 8, 8, 0);
            glPopMatrix();
        }
    }
    glPopMatrix();
}

#endif // COURSEWORK_INTERFACE_H
✔ GLUT_DOUBLE

Double buffering enabled → smooth animation.
This is why glutSwapBuffers() works.

✔ GLUT_RGBA

Color mode = Red, Green, Blue, Alpha.

pac_tex → will hold the OpenGL texture ID for Pac-Man’s current frame.

tickDiv → a counter to help animate the mouth opening/closing.

/**
 * Tile defined as an enum where each type is defined as:
 *      W: Wall
 *      G: Ghost Pen Gate
 *      P: Portal
 *      u: Empty path
 *      o: Pill
 *      e: Eaten Pill
 *      O: Big Pill
 *      E: Eaten Big Pill
 */

 libpng header file

 str.length() -1=== because of 0 based index

 for(int i = str.length() -1; i>=0 ; i--)=== Because when you draw numbers in a score HUD, you often want the rightmost digit first and then move left.

 liveCount - 1===Often in Pacman-style games, the current active life (the one Pacman is playing) is not drawn in the HUD, only the remaining lives.

So if you have 3 lives left, it draws 2 extra icons.'

IDHR=Header information

1 tile=8*8 pixel

Mode	    Meaning	                  Behavior
SCATTER	  Run to corners	        Safe for Pac-Man
CHASE	  Kill Pac-Man	            Very dangerous
FRIGHTEN  Big pill active	        Ghost turns blue & runs
DEATH	  Eaten ghost	            Eyes return home
LEAVE	  Coming out of house	    Slow exit
PEN	      In house, waiting	        Up-down movement

## What GLUT does: Once you enter the main loop:

GLUT starts a continuous loop:

Checks for input/events (keyboard, mouse, etc.)

Calls display() when needed

Calls idle() repeatedly whenever there’s nothing else to do

So you never call idle() yourself — GLUT does it automatically.

glTranslatef(-3.0f, -3.0f, 0.0f);/// allign properly in the tile

### pacman move 
# left
Pac-Man moves left (X decreases).
Y is snapped to the nearest integer tile.

# up
Pac-Man moves up (Y increases).
X is snapped to nearest tile.


## vector
- like an array but array has a fix size but vector has dynamic size

## Pacman Move
# Red = Directly follow pacman
# Pink = 4 step ahead of pacman:
If Pacman moves LEFT → Pinky targets 4 tiles to the LEFT (ahead of Pacman)
If Pacman moves UP → Pinky targets 4 tiles UP (ahead of Pacman)
Tries to ambush/cut off Pacman's path
# Blue = 2 tiles ahead of Pacman:
Then calculates: Vector from this target to Red ghost
Doubles that vector from Red ghost's position
Result: Unpredictable interception point
# Yellow = Chases from far, runs away when close
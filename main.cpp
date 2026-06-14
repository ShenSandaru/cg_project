/*=============================================================================
 * main.cpp - Space Odyssey: Rogue Starship
 *
 * Entry point that interconnects all five Computer Graphics topic modules.
 *
 * The five CG topics demonstrated in this project:
 *
 *   Topic 1 - Viewing & Clipping (ViewingClipping.h/.cpp):
 *     gluOrtho2D viewport configuration, boundary clamping, AABB collision
 *     detection, off-screen object clipping, and input handling.
 *
 *   Topic 2 - Polygon Filling & Primitive Construction (PolygonFilling.h/.cpp):
 *     Filled rectangles (GL_QUADS), filled triangles (GL_TRIANGLES),
 *     Bresenham line algorithm, Midpoint circle algorithm, health bar
 *     rendering, and menu UI polygon construction.
 *
 *   Topic 3 - Transformations (Transformations.h/.cpp):
 *     Object translation (entity movement), dynamic scaling (particle
 *     sizes shrinking), composite transformation matrices (glPushMatrix +
 *     glTranslatef for screen shake, glRotatef for scrap rotation).
 *
 *   Topic 4 - Rotation Vectors (RotationVectors.h/.cpp):
 *     Trigonometric calculations (cos/sin) for radial explosion debris,
 *     hexagon/pentagon/diamond vertex generation using angular iteration,
 *     boss turret rotation, and Euclidean distance calculations.
 *
 *   Topic 5 - Animation Principles (AnimationPrinciples.h/.cpp):
 *     Framerate management (glutTimerFunc at ~60fps), scrolling star
 *     parallax staging, multi-layer nebula parallax, enemy spawn timing,
 *     and follow-through/reaction feedback (shield flash, screen shake).
 *
 * The glutTimerFunc(16, timer, 0) call below demonstrates Topic 5
 * (Animation Principles - Timing) by maintaining a consistent ~60fps
 * update rate for smooth animation.
 *=============================================================================*/

#include "ViewingClipping.h"
#include "PolygonFilling.h"
#include "Transformations.h"
#include "RotationVectors.h"
#include "AnimationPrinciples.h"
#include <GL/freeglut.h>

Game game;

/* display() - GLUT display callback
 * Calls Game::draw() which is implemented in Transformations.cpp.
 * The draw function demonstrates composite transformation matrices
 * (screen shake via glTranslatef) and orchestrates rendering across
 * all topic modules. */
void display() {
    game.draw();
}

/* timer() - GLUT timer callback (~60fps)
 * This demonstrates Animation Principles (Topic 5) - Timing:
 * glutTimerFunc(16, ...) schedules this function every 16 milliseconds,
 * maintaining approximately 60 frames per second for smooth animation.
 * The consistent frame timing ensures all movement, scaling, and
 * animation effects play at the correct speed. */
void timer(int) {
    game.update();
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void keyDown(unsigned char key, int, int) {
    game.handleInput(key, true);
}

void keyUp(unsigned char key, int, int) {
    game.handleInput(key, false);
}

void specialDown(int key, int, int) {
    game.handleSpecialInput(key, true);
}

void specialUp(int key, int, int) {
    game.handleSpecialInput(key, false);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Space Odyssey - Rogue Starship");

    game.init();

    glutDisplayFunc(display);
    glutTimerFunc(16, timer, 0);
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    glutSpecialFunc(specialDown);
    glutSpecialUpFunc(specialUp);

    glutMainLoop();
    return 0;
}

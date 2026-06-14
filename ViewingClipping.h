/*=============================================================================
 * ViewingClipping.h - Topic 1: Viewing & Clipping (Master Header)
 *
 * This is the master header file for Space Odyssey: Rogue Starship.
 * It contains all type declarations, struct/class definitions, constants,
 * and function prototypes shared across all five CG topic modules.
 *
 * CG Concepts demonstrated:
 *   - gluOrtho2D viewport configuration (2D orthographic projection)
 *   - Boundary clamping (keeping entities within visible viewport)
 *   - Off-screen object clipping (removing bullets/enemies outside viewport)
 *   - AABB (Axis-Aligned Bounding Box) collision detection
 *
 * Header Dependency Chain:
 *   PolygonFilling.h  (standalone drawing primitives)
 *       ^
 *       |
 *   ViewingClipping.h (this file - all type declarations)
 *       ^
 *       |
 *   Transformations.h, RotationVectors.h, AnimationPrinciples.h
 *
 * Implementation files:
 *   ViewingClipping.cpp   - Viewport init, clipping, collisions, input
 *   PolygonFilling.cpp    - Primitive rendering, entity drawing, menus/HUD
 *   Transformations.cpp   - Movement, scaling, screen shake, game loop
 *   RotationVectors.cpp   - Radial explosions, trig vertex gen, enemy rendering
 *   AnimationPrinciples.cpp - Parallax staging, timing, UI screens
 *=============================================================================*/

#ifndef VIEWING_CLIPPING_H
#define VIEWING_CLIPPING_H

#include "PolygonFilling.h"
#include <vector>
#include <string>

/*-----------------------------------------------------------------------------
 * Viewport Constants
 * Define the 2D orthographic projection boundaries used by gluOrtho2D.
 * All game coordinates are mapped to this [0, WIN_W] x [0, WIN_H] space.
 *---------------------------------------------------------------------------*/
const int WIN_W = 800;
const int WIN_H = 600;

/*-----------------------------------------------------------------------------
 * Game State Enumeration
 * Controls which screen/mode the game is currently displaying.
 *---------------------------------------------------------------------------*/
enum GameState { MAIN_MENU, GUIDELINES, SHIP_SELECT, PLAYING, UPGRADE_SHOP, GAME_OVER, GAME_WON, LEVEL_TRANSITION, PAUSED };

/*-----------------------------------------------------------------------------
 * AABB Collision Detection
 * Tests for overlap between two axis-aligned bounding boxes.
 * Both boxes are defined by center (x,y) and dimensions (w,h).
 * Returns true if the two rectangles overlap.
 * (Implemented in ViewingClipping.cpp)
 *---------------------------------------------------------------------------*/
bool aabb(float ax, float ay, float aw, float ah,
          float bx, float by, float bw, float bh);

/*=============================================================================
 * Entity Struct Declarations
 * All game entity types are declared here so they can be used across
 * all five topic implementation files.
 *=============================================================================*/

/* Particle - Visual effect particle for explosions, muzzle flashes, trails.
 * update() in Transformations.cpp - demonstrates translation (x += vx, y += vy)
 * draw() in Transformations.cpp - demonstrates dynamic scaling (size shrinks with lifetime) */
struct Particle {
    float x, y;
    float vx, vy;
    float r, g, b;
    float lifetime;
    void update();
    void draw() const;
};

/* Star - Single background star for parallax scrolling */
struct Star {
    float x, y, speed;
};

/* StarField - Scrolling star background for Level 1 (staging/parallax)
 * All methods in AnimationPrinciples.cpp - demonstrates parallax staging */
class StarField {
public:
    void init();
    void update();
    void draw() const;
private:
    std::vector<Star> stars;
};

/* NebulaPuff - Single nebula cloud element for Level 2 background */
struct NebulaPuff {
    float x, y, radius, speed;
    float r, g, b, alpha;
};

/* NebulaField - Multi-layer nebula background for Level 2 (staging/parallax)
 * All methods in AnimationPrinciples.cpp - demonstrates multi-layer parallax */
class NebulaField {
public:
    void init();
    void update();
    void draw() const;
private:
    std::vector<NebulaPuff> puffs;
    std::vector<Star> stars;
};

/* Bullet - Player projectile
 * update() in ViewingClipping.cpp - demonstrates off-screen clipping
 * draw() in PolygonFilling.cpp - demonstrates Bresenham line rendering */
struct Bullet {
    float x, y;
    float vx, vy;
    bool  alive;

    void update();
    void draw() const;
};

/* EnemyBullet - Enemy projectile
 * update() in ViewingClipping.cpp - demonstrates off-screen clipping
 * draw() in PolygonFilling.cpp - demonstrates Bresenham line rendering */
struct EnemyBullet {
    float x, y;
    float vy;
    bool  alive;

    void update();
    void draw() const;
};

/* Scrap - Collectible currency item dropped by enemies
 * update() in ViewingClipping.cpp - demonstrates boundary clipping
 * draw() in Transformations.cpp - demonstrates glTranslatef + glRotatef composite */
struct Scrap {
    float x, y;
    float speed;
    float angle;
    bool  alive;

    void update();
    void draw() const;
};

/* Enemy - Hostile entity with multiple types and behaviors
 * update() in Transformations.cpp - demonstrates translation patterns per type
 * draw() in RotationVectors.cpp - demonstrates cos/sin vertex generation for
 *   hexagons, pentagons, diamonds, and boss turret rotation */
struct Enemy {
    float x, y;
    float w, h;
    float speed;
    bool  alive;
    int   enemyType;
    int   hp;
    int   maxHp;
    int   shootCooldown;
    float moveTimer;  // Used for boss oscillation

    void update(float playerX);
    void draw() const;
};

/* Player - The player-controlled starship
 * update() in ViewingClipping.cpp - demonstrates viewport boundary clamping
 * draw() in PolygonFilling.cpp - demonstrates polygon filling with primitives */
struct Player {
    float x, y;
    float w, h;
    float speed;
    int   shipType;
    int   hull;
    int   maxHull;
    int   shields;
    int   maxShields;
    int   weaponLevel;
    float hitFlashTimer;

    void update(bool left, bool right, bool up, bool down);
    void draw() const;
};

/* FloatingText - Animated text popup for damage numbers and notifications
 * update() in RotationVectors.cpp - position offset animation
 * draw() in RotationVectors.cpp - alpha-blended text rendering */
struct FloatingText {
    float x, y;
    std::string text;
    float vx, vy;
    float r, g, b;
    float alpha;
    bool alive;

    void update();
    void draw() const;
};

/*=============================================================================
 * Game Class Declaration
 * Central game controller managing all entities, states, and rendering.
 * Member functions are implemented across all five topic files.
 *=============================================================================*/
class Game {
public:
    Game();
    void init();        // ViewingClipping.cpp  - gluOrtho2D viewport setup
    void reset();       // ViewingClipping.cpp  - state reset
    void update();      // Transformations.cpp  - master update loop
    void draw();        // Transformations.cpp  - master draw with screen shake
    void handleInput(unsigned char key, bool pressed);   // ViewingClipping.cpp
    void handleSpecialInput(int key, bool pressed);      // ViewingClipping.cpp

    GameState getState() const { return state; }

private:
    GameState state;
    int score;
    int credits;
    int wave;
    int enemySpawnTimer;
    int enemiesSpawned;
    int maxEnemies;
    int baseShields;
    int maxBaseShields;
    int level;

    Player player;
    std::vector<Bullet> bullets;
    std::vector<EnemyBullet> enemyBullets;
    std::vector<Enemy> enemies;
    std::vector<Scrap> scraps;
    std::vector<Particle> particles;
    std::vector<FloatingText> floatingTexts;
    StarField starField;
    NebulaField nebulaField;

    bool keyLeft, keyRight, keyUp, keyDown;
    float screenShakeTimer;
    float baseFlashTimer;
    float superPowerCooldownTimer;
    float superPowerPulseTimer;
    float superPowerCenterX;
    float superPowerCenterY;
    bool superPowerReadyNotified;
    static constexpr float SUPER_POWER_COOLDOWN_MAX = 360.0f;
    static constexpr float SUPER_POWER_PULSE_MAX = 0.45f;

    // --- Functions implemented in AnimationPrinciples.cpp (Topic 5) ---
    void spawnEnemy();                          // Enemy spawn timing

    // --- Functions implemented in Transformations.cpp (Topic 3) ---
    void spawnEnemyBullet(const Enemy& e);      // Positional offsets
    void startNextWave();                       // Wave management

    // --- Functions implemented in RotationVectors.cpp (Topic 4) ---
    void spawnExplosion(float x, float y, float r, float g, float b);
    void spawnMuzzleFlash(float x, float y, float r, float g, float b);
    void spawnFloatingText(float x, float y, const std::string& text, float r, float g, float b, float vy = 1.2f);
    void activateSuperPower();

    // --- Functions implemented in ViewingClipping.cpp (Topic 1) ---
    void checkCollisions();                     // AABB collision system

    // --- Functions implemented in PolygonFilling.cpp (Topic 2) ---
    void drawMainMenu();
    void drawGuidelines();
    void drawVictoryScreen();
    void drawHUD();
    void drawText(float x, float y, const std::string& s, void* font);

    // --- Functions implemented in AnimationPrinciples.cpp (Topic 5) ---
    void drawShipSelect();
    void drawUpgradeShop();
    void drawLevelTransition();
};

#endif

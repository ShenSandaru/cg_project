/*=============================================================================
 * ViewingClipping.cpp - Topic 1: Viewing & Clipping
 *
 * This file demonstrates fundamental 2D viewing and clipping concepts:
 *
 *   1. VIEWPORT CONFIGURATION (gluOrtho2D):
 *      Game::init() sets up a 2D orthographic projection mapping world
 *      coordinates to the [0, 800] x [0, 600] pixel window using
 *      gluOrtho2D(0, WIN_W, 0, WIN_H). This defines the visible area.
 *
 *   2. BOUNDARY CLAMPING:
 *      Player::update() clamps the player ship position so it cannot
 *      move outside the viewport boundaries. This is a form of geometric
 *      clipping where objects are restricted to the viewing region.
 *
 *   3. OFF-SCREEN CLIPPING:
 *      Bullet::update() and EnemyBullet::update() mark projectiles as
 *      dead when they exit the viewport, effectively clipping them from
 *      the scene. Scrap::update() similarly clips collectibles.
 *
 *   4. AABB COLLISION DETECTION:
 *      The aabb() function tests for overlap between two axis-aligned
 *      bounding boxes. Game::checkCollisions() uses this extensively
 *      to detect bullet-enemy, bullet-player, and player-scrap collisions
 *      within the viewport.
 *
 *   5. INPUT HANDLING:
 *      Maps keyboard events to game actions within the viewport space.
 *=============================================================================*/

#include "ViewingClipping.h"
#include <GL/freeglut.h>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <sstream>
#include <algorithm>

/*=============================================================================
 * GAME CONSTRUCTOR
 * Initializes all game state variables to their default values.
 *=============================================================================*/
Game::Game() 
    : state(MAIN_MENU), score(0), credits(0), wave(1), enemySpawnTimer(0),
      enemiesSpawned(0), maxEnemies(5), baseShields(100), maxBaseShields(100),
      level(1), keyLeft(false), keyRight(false), keyUp(false), keyDown(false),
      screenShakeTimer(0), baseFlashTimer(0), superPowerCooldownTimer(0),
      superPowerPulseTimer(0), superPowerCenterX(WIN_W / 2.0f), superPowerCenterY(60.0f),
      superPowerReadyNotified(true) {
    // Restore player default state
    player = { WIN_W / 2.0f, 60.0f, 40.0f, 30.0f, 6.0f };
}

/*=============================================================================
 * VIEWPORT INITIALIZATION (gluOrtho2D)
 *
 * Sets up the 2D orthographic projection using gluOrtho2D.
 * This defines the coordinate system: x ranges from 0 to WIN_W (800),
 * y ranges from 0 to WIN_H (600). The origin (0,0) is at the bottom-left.
 *
 * glMatrixMode(GL_PROJECTION) selects the projection matrix stack.
 * glLoadIdentity() resets it to avoid compounding previous transforms.
 * gluOrtho2D(0, WIN_W, 0, WIN_H) creates the orthographic projection.
 *=============================================================================*/
void Game::init() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WIN_W, 0, WIN_H);
    glMatrixMode(GL_MODELVIEW);
    srand((unsigned)time(0));
    starField.init();
    nebulaField.init();
    reset();
    state = MAIN_MENU;
}

/*=============================================================================
 * GAME STATE RESET
 * Resets all game variables to their initial values for a new game.
 * Clears all entity vectors and repositions the player to center-bottom
 * of the viewport.
 *=============================================================================*/
void Game::reset() {
    score = 0;
    credits = 0;
    wave = 1;
    enemySpawnTimer = 0;
    enemiesSpawned = 0;
    maxEnemies = 5;
    baseShields = 100;
    maxBaseShields = 100;
    level = 1;
    superPowerCooldownTimer = 0.0f;
    superPowerPulseTimer = 0.0f;
    superPowerCenterX = WIN_W / 2.0f;
    superPowerCenterY = 60.0f;
    superPowerReadyNotified = true;

    bullets.clear();
    enemyBullets.clear();
    enemies.clear();
    scraps.clear();
    particles.clear();
    floatingTexts.clear();
    
    player.x = WIN_W / 2.0f;
    player.y = 60.0f;
    player.shipType = 0;
    player.hull = 3;
    player.maxHull = 3;
    player.shields = 2;
    player.maxShields = 2;
    player.speed = 6.5f;
    player.weaponLevel = 1;
    player.hitFlashTimer = 0.0f;
}

/*=============================================================================
 * AABB COLLISION DETECTION
 *
 * Tests for overlap between two Axis-Aligned Bounding Boxes.
 * Each box is defined by its center position (x, y) and dimensions (w, h).
 *
 * The test checks if the two rectangles overlap by comparing their
 * half-extents in both x and y axes. If all four overlap conditions
 * are true, the boxes are colliding.
 *
 * This is the most efficient 2D collision test since it only requires
 * 4 comparisons and no trigonometry or distance calculations.
 *=============================================================================*/
bool aabb(float ax, float ay, float aw, float ah,
          float bx, float by, float bw, float bh) {
    return ax - aw/2 < bx + bw/2 &&
           ax + aw/2 > bx - bw/2 &&
           ay - ah/2 < by + bh/2 &&
           ay + ah/2 > by - bh/2;
}

/*=============================================================================
 * PLAYER BOUNDARY CLAMPING (Viewport Clipping)
 *
 * After applying movement translation, the player position is clamped
 * to stay within the viewport boundaries:
 *   x is clamped to [w/2, WIN_W - w/2]
 *   y is clamped to [h/2, WIN_H - h/2]
 *
 * This prevents the player from moving outside the visible area,
 * which is a practical application of clipping to the view volume.
 *
 * The hitFlashTimer countdown provides visual feedback timing
 * (see AnimationPrinciples for more on timing).
 *=============================================================================*/
void Player::update(bool left, bool right, bool up, bool down) {
    if (left)  x -= speed;
    if (right) x += speed;
    if (up)    y += speed;
    if (down)  y -= speed;

    // Clamp player position to viewport boundaries
    if (x < w/2) x = w/2;
    if (x > WIN_W - w/2) x = WIN_W - w/2;
    if (y < h/2) y = h/2;
    if (y > WIN_H - h/2) y = WIN_H - h/2;

    if (hitFlashTimer > 0) {
        hitFlashTimer -= 0.016f;
    }
}

/*=============================================================================
 * BULLET OFF-SCREEN CLIPPING
 *
 * Marks bullets as dead when they exit the viewport boundaries.
 * This is off-screen clipping: objects that leave the visible area
 * are flagged for removal, preventing unnecessary processing of
 * invisible entities.
 *
 * Clipping conditions:
 *   y > WIN_H  (bullet went above the top of the viewport)
 *   x < 0      (bullet went past the left edge)
 *   x > WIN_W  (bullet went past the right edge)
 *=============================================================================*/
void Bullet::update() {
    x += vx;
    y += vy;
    if (y > WIN_H || x < 0 || x > WIN_W) {
        alive = false;
    }
}

/*=============================================================================
 * ENEMY BULLET OFF-SCREEN CLIPPING
 *
 * Similar to Bullet::update(), marks enemy bullets as dead when they
 * exit the viewport. Uses a small margin (-10, WIN_H + 10) to allow
 * bullets to travel slightly off-screen before being clipped.
 *=============================================================================*/
void EnemyBullet::update() {
    y -= vy;
    if (y < -10 || y > WIN_H + 10) {
        alive = false;
    }
}

/*=============================================================================
 * SCRAP OFF-SCREEN CLIPPING
 *
 * Collectible scrap items drift downward and are clipped when they
 * exit below the viewport (y < -10). The angle rotation is updated
 * here for visual spinning (rendered in Transformations.cpp).
 *=============================================================================*/
void Scrap::update() {
    y -= speed;
    angle += 4.0f;
    if (angle >= 360.0f) angle -= 360.0f;
    if (y < -10) {
        alive = false;
    }
}

/*=============================================================================
 * COMPREHENSIVE COLLISION DETECTION SYSTEM
 *
 * Uses AABB collision detection to check all entity pair interactions:
 *
 * 1. Player bullets vs Enemies:
 *    When a bullet hits an enemy, damage is applied. If the enemy dies,
 *    score is awarded, explosion particles spawn, and scrap may drop.
 *
 * 2. Reflected enemy bullets vs Enemies:
 *    Enemy bullets that have been reflected (vy >= 0) by the super power
 *    can damage enemies they originated from.
 *
 * 3. Enemies vs Player:
 *    Contact between enemies and the player deals damage to the player's
 *    shields first, then hull. Triggers hit flash and screen shake.
 *
 * 4. Enemy bullets vs Player:
 *    Direct projectile hits on the player follow the same damage model.
 *
 * 5. Scraps vs Player:
 *    Collecting scrap items awards credits for the upgrade shop.
 *
 * All collision tests use the aabb() function with center-based coordinates.
 *=============================================================================*/
void Game::checkCollisions() {
    // --- Player bullets hitting enemies ---
    for (auto& b : bullets) {
        if (!b.alive) continue;
        for (auto& e : enemies) {
            if (!e.alive) continue;
            if (aabb(b.x, b.y, 4, 12, e.x, e.y, e.w, e.h)) {
                b.alive = false;
                e.hp--;
                spawnExplosion(b.x, b.y, 0.2f, 0.9f, 1.0f);

                if (e.hp <= 0) {
                    e.alive = false;
                    int pts = (e.enemyType == 3 || e.enemyType == 7) ? 100 : 10;
                    score += pts;
                    spawnExplosion(e.x, e.y, 1.0f, 0.4f, 0.1f);
                    spawnFloatingText(e.x, e.y, "+" + std::to_string(pts), 1.0f, 1.0f, 1.0f, 1.0f);

                    if (e.enemyType == 3 || e.enemyType == 7) {
                        for (int i = 0; i < 8; ++i) {
                            Scrap s = { e.x + (rand() % 60 - 30), e.y + (rand() % 60 - 30), 1.2f, 0.0f, true };
                            scraps.push_back(s);
                        }
                    } else if (rand() % 100 < 55) {
                        Scrap s = { e.x, e.y, 1.5f, 0.0f, true };
                        scraps.push_back(s);
                    }
                }
            }
        }
    }

    // --- Reflected enemy bullets hitting enemies ---
    for (auto& eb : enemyBullets) {
        if (!eb.alive || eb.vy >= 0.0f) continue;
        for (auto& e : enemies) {
            if (!e.alive) continue;
            if (aabb(eb.x, eb.y, 5, 12, e.x, e.y, e.w, e.h)) {
                eb.alive = false;
                e.hp--;
                spawnExplosion(eb.x, eb.y, 0.2f, 0.9f, 1.0f);

                if (e.hp <= 0) {
                    e.alive = false;
                    int pts = (e.enemyType == 3 || e.enemyType == 7) ? 100 : 10;
                    score += pts;
                    spawnExplosion(e.x, e.y, 1.0f, 0.4f, 0.1f);
                    spawnFloatingText(e.x, e.y, "+" + std::to_string(pts), 1.0f, 1.0f, 1.0f, 1.0f);

                    if (e.enemyType == 3 || e.enemyType == 7) {
                        for (int i = 0; i < 8; ++i) {
                            Scrap s = { e.x + (rand() % 60 - 30), e.y + (rand() % 60 - 30), 1.2f, 0.0f, true };
                            scraps.push_back(s);
                        }
                    } else if (rand() % 100 < 55) {
                        Scrap s = { e.x, e.y, 1.5f, 0.0f, true };
                        scraps.push_back(s);
                    }
                }
                break;
            }
        }
    }

    // --- Enemies colliding with player (contact damage) ---
    for (auto& e : enemies) {
        if (!e.alive) continue;
        if (aabb(player.x, player.y, player.w, player.h, e.x, e.y, e.w, e.h)) {
            e.alive = false;
            spawnExplosion(e.x, e.y, 1.0f, 0.2f, 0.2f);
            
            player.hitFlashTimer = 0.2f;
            screenShakeTimer = 0.28f;
            int dmg = (e.enemyType == 3 || e.enemyType == 7) ? 3 : 1;
            int prevShields = player.shields;
            int prevHull = player.hull;

            if (player.shields > 0) {
                player.shields -= dmg;
                if (player.shields < 0) {
                    player.hull += player.shields;
                    player.shields = 0;
                }
            } else {
                player.hull -= dmg;
            }

            // Spawn damage popups
            if (prevShields > 0) {
                int shieldDmg = prevShields - player.shields;
                if (shieldDmg > 0) {
                    spawnFloatingText(player.x, player.y + 15.0f, "SHIELD HIT! -" + std::to_string(shieldDmg), 0.0f, 0.8f, 1.0f, 1.4f);
                }
            }
            if (prevHull > player.hull) {
                int hullDmg = prevHull - player.hull;
                spawnFloatingText(player.x, player.y + 15.0f, "HULL HIT! -" + std::to_string(hullDmg), 1.0f, 0.2f, 0.2f, 1.4f);
            }

            if (player.hull <= 0) {
                player.hull = 0;
                state = GAME_OVER;
            }
        }
    }

    // --- Enemy bullets hitting player ---
    for (auto& eb : enemyBullets) {
        if (!eb.alive) continue;
        if (aabb(eb.x, eb.y, 5, 12, player.x, player.y, player.w, player.h)) {
            eb.alive = false;
            spawnExplosion(eb.x, eb.y, 1.0f, 0.5f, 0.0f);

            player.hitFlashTimer = 0.18f;
            screenShakeTimer = 0.18f;

            if (player.shields > 0) {
                player.shields--;
                spawnFloatingText(player.x + 10.0f, player.y + 20.0f, "-1 SHIELD", 0.0f, 0.8f, 1.0f, 1.3f);
            } else {
                player.hull--;
                spawnFloatingText(player.x + 10.0f, player.y + 20.0f, "-1 HULL", 1.0f, 0.2f, 0.2f, 1.3f);
            }

            if (player.hull <= 0) {
                player.hull = 0;
                state = GAME_OVER;
            }
        }
    }

    // --- Player collecting scrap items ---
    for (auto& s : scraps) {
        if (!s.alive) continue;
        if (aabb(s.x, s.y, 8, 8, player.x, player.y, player.w, player.h)) {
            s.alive = false;
            credits += 10;
            spawnFloatingText(s.x, s.y, "+10 SCRAP", 1.0f, 0.85f, 0.0f, 1.2f);
            for (int i = 0; i < 5; ++i) {
                Particle p = { s.x, s.y, (float)(rand()%20-10)/5.0f, (float)(rand()%20-10)/5.0f, 1.0f, 0.85f, 0.0f, 0.5f };
                particles.push_back(p);
            }
        }
    }
}

/*=============================================================================
 * INPUT HANDLING
 *
 * Maps keyboard events to game actions and state transitions.
 * Handles different input contexts based on the current GameState:
 *   - MAIN_MENU: menu navigation (1/2/3 keys)
 *   - PLAYING: movement (WASD), shooting (Space), super power (F), pause (P)
 *   - GUIDELINES, SHIP_SELECT: navigation and selection
 *   - UPGRADE_SHOP: purchase upgrades and launch next wave
 *   - GAME_OVER, GAME_WON: restart/return to menu
 *   - LEVEL_TRANSITION: proceed to Level 2
 *=============================================================================*/
void Game::handleInput(unsigned char key, bool pressed) {
    if (pressed) {
        if (key == 27) exit(0);

        if (state == MAIN_MENU) {
            if (key == '1') {
                state = SHIP_SELECT;
            } else if (key == '2') {
                state = GUIDELINES;
            } else if (key == '3') {
                exit(0);
            }
            return;
        }
        else if (state == PLAYING) {
            if (key == 'f' || key == 'F') {
                activateSuperPower();
            }
        }
        else if (state == GUIDELINES) {
            if (key == 'b' || key == 'B') {
                state = MAIN_MENU;
            }
            return;
        }
        else if (state == SHIP_SELECT) {
            if (key == '1') {
                reset();
                player.shipType = 0;
                player.hull = 3; player.maxHull = 3;
                player.shields = 2; player.maxShields = 2;
                player.speed = 6.5f;
                state = PLAYING;
                spawnFloatingText(WIN_W / 2.0f - 50.0f, WIN_H / 2.0f, "WAVE 1", 0.2f, 0.9f, 1.0f, 0.6f);
            }
            else if (key == '2') {
                reset();
                player.shipType = 1;
                player.hull = 5; player.maxHull = 5;
                player.shields = 4; player.maxShields = 4;
                player.speed = 4.5f;
                state = PLAYING;
                spawnFloatingText(WIN_W / 2.0f - 50.0f, WIN_H / 2.0f, "WAVE 1", 0.2f, 0.9f, 1.0f, 0.6f);
            }
            else if (key == '3') {
                reset();
                player.shipType = 2;
                player.hull = 3; player.maxHull = 3;
                player.shields = 1; player.maxShields = 1;
                player.speed = 5.8f;
                state = PLAYING;
                spawnFloatingText(WIN_W / 2.0f - 50.0f, WIN_H / 2.0f, "WAVE 1", 0.2f, 0.9f, 1.0f, 0.6f);
            }
            else if (key == 'b' || key == 'B') {
                state = MAIN_MENU;
            }
            return;
        }
        else if (state == UPGRADE_SHOP) {
            if (key == '1') {
                if (credits >= 100 && player.weaponLevel < 3) {
                    credits -= 100;
                    player.weaponLevel++;
                }
            }
            else if (key == '2') {
                if (credits >= 30 && player.hull < player.maxHull) {
                    credits -= 30;
                    player.hull++;
                }
            }
            else if (key == '3') {
                if (credits >= 50) {
                    credits -= 50;
                    player.maxShields++;
                    player.shields = player.maxShields;
                }
            }
            else if (key == 13) {
                startNextWave();
            }
            return;
        }
        else if (state == GAME_OVER) {
            if (key == 'r' || key == 'R') {
                if (level == 2) {
                    // Retry Level 2 — keep ship type, restart at wave 6
                    int savedShipType = player.shipType;
                    float savedSpeed = player.speed;
                    int savedMaxHull = player.maxHull;
                    int savedMaxShields = player.maxShields;

                    wave = 5;
                    enemiesSpawned = 0;
                    enemySpawnTimer = 0;
                    baseShields = maxBaseShields;
                    bullets.clear();
                    enemyBullets.clear();
                    enemies.clear();
                    scraps.clear();
                    particles.clear();
                    floatingTexts.clear();

                    player.x = WIN_W / 2.0f;
                    player.y = 60.0f;
                    player.shipType = savedShipType;
                    player.speed = savedSpeed;
                    player.hull = savedMaxHull;
                    player.maxHull = savedMaxHull;
                    player.shields = savedMaxShields;
                    player.maxShields = savedMaxShields;
                    player.weaponLevel = 1;
                    player.hitFlashTimer = 0.0f;
                    credits = 0;
                    score = 0;

                    startNextWave();
                } else {
                    reset();
                    state = MAIN_MENU;
                }
            }
            return;
        }
        else if (state == GAME_WON) {
            if (key == 'r' || key == 'R') {
                reset();
                state = MAIN_MENU;
            }
            return;
        }
        else if (state == LEVEL_TRANSITION) {
            if (key == 13) { // Enter key
                // Transition to Level 2 — keep player stats, reset wave
                level = 2;
                wave = 5; // startNextWave will increment to 6 (first Level 2 wave)
                enemiesSpawned = 0;
                bullets.clear();
                enemyBullets.clear();
                enemies.clear();
                scraps.clear();
                floatingTexts.clear();
                player.shields = player.maxShields;
                baseShields = maxBaseShields; // Refill base shields
                startNextWave();
            }
            return;
        }

        if (state == PLAYING) {
            if (key == 'p' || key == 'P') {
                state = PAUSED;
            }
            else if (key == ' ') {
                float spawnY = player.y + player.h/2.0f;
                if (player.weaponLevel == 1) {
                    Bullet b = { player.x, spawnY, 0.0f, 9.0f, true };
                    bullets.push_back(b);
                    spawnMuzzleFlash(player.x, spawnY, 0.2f, 0.9f, 1.0f);
                }
                else if (player.weaponLevel == 2) {
                    Bullet b1 = { player.x - 8.0f, spawnY, 0.0f, 9.0f, true };
                    Bullet b2 = { player.x + 8.0f, spawnY, 0.0f, 9.0f, true };
                    bullets.push_back(b1);
                    bullets.push_back(b2);
                    spawnMuzzleFlash(player.x - 8.0f, spawnY, 0.2f, 0.9f, 1.0f);
                    spawnMuzzleFlash(player.x + 8.0f, spawnY, 0.2f, 0.9f, 1.0f);
                }
                else {
                    Bullet b1 = { player.x, spawnY, 0.0f, 9.0f, true };
                    Bullet b2 = { player.x - 8.0f, spawnY, -2.0f, 8.5f, true };
                    Bullet b3 = { player.x + 8.0f, spawnY, 2.0f, 8.5f, true };
                    bullets.push_back(b1);
                    bullets.push_back(b2);
                    bullets.push_back(b3);
                    spawnMuzzleFlash(player.x, spawnY, 0.2f, 0.9f, 1.0f);
                    spawnMuzzleFlash(player.x - 8.0f, spawnY, 0.2f, 0.9f, 1.0f);
                    spawnMuzzleFlash(player.x + 8.0f, spawnY, 0.2f, 0.9f, 1.0f);
                }
            }
            if (key == 'a' || key == 'A') keyLeft = true;
            if (key == 'd' || key == 'D') keyRight = true;
            if (key == 'w' || key == 'W') keyUp = true;
            if (key == 's' || key == 'S') keyDown = true;
        }
        else if (state == PAUSED) {
            if (key == 'p' || key == 'P') {
                state = PLAYING;
            }
        }
    } else {
        if (state == PLAYING) {
            if (key == 'a' || key == 'A') keyLeft = false;
            if (key == 'd' || key == 'D') keyRight = false;
            if (key == 'w' || key == 'W') keyUp = false;
            if (key == 's' || key == 'S') keyDown = false;
        }
    }
}

/*=============================================================================
 * SPECIAL KEY INPUT HANDLING
 *
 * Handles arrow key inputs for player movement during PLAYING state.
 * Arrow keys map to the same movement directions as WASD.
 *=============================================================================*/
void Game::handleSpecialInput(int key, bool pressed) {
    if (state == PLAYING) {
        if (pressed) {
            if (key == GLUT_KEY_LEFT)  keyLeft  = true;
            if (key == GLUT_KEY_RIGHT) keyRight = true;
            if (key == GLUT_KEY_UP)    keyUp    = true;
            if (key == GLUT_KEY_DOWN)  keyDown  = true;
        } else {
            if (key == GLUT_KEY_LEFT)  keyLeft  = false;
            if (key == GLUT_KEY_RIGHT) keyRight = false;
            if (key == GLUT_KEY_UP)    keyUp    = false;
            if (key == GLUT_KEY_DOWN)  keyDown  = false;
        }
    }
}

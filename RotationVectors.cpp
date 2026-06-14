/*=============================================================================
 * RotationVectors.cpp - Topic 4: Rotation Vectors
 *
 * This file demonstrates trigonometric rotation vector concepts:
 *
 *   1. RADIAL EXPLOSION DEBRIS:
 *      spawnExplosion() disperses particles in all directions using
 *      rotation vectors. For each particle, a random angle is chosen
 *      and the velocity vector is computed as:
 *        vx = cos(angle) * speed
 *        vy = sin(angle) * speed
 *      This is a polar-to-Cartesian conversion that creates a circular
 *      explosion pattern.
 *
 *   2. TRIGONOMETRIC VERTEX GENERATION:
 *      Enemy::draw() uses cos(theta) and sin(theta) to generate vertices
 *      for regular polygons (hexagons, pentagons, diamonds):
 *        vertex_x = center_x + cos(i * 2*PI/n) * radius
 *        vertex_y = center_y + sin(i * 2*PI/n) * radius
 *      This parameterizes circle points at equal angular intervals.
 *
 *   3. BOSS TURRET ROTATION:
 *      Enemy::draw() for the Nebula Overlord uses glRotatef to spin
 *      turret triangles around the Z axis at a rate proportional
 *      to elapsed time.
 *
 *   4. SHIELD RING GENERATION:
 *      The boss pulsing shield ring and super power beams use
 *      cos(theta)/sin(theta) to distribute points evenly around
 *      a circle.
 *
 *   5. DISTANCE CALCULATIONS:
 *      activateSuperPower() uses sqrt(dx*dx + dy*dy) to compute
 *      Euclidean distance between the player and enemies for
 *      area-of-effect damage.
 *=============================================================================*/

#include "ViewingClipping.h"
#include <GL/freeglut.h>
#include <cstdlib>
#include <cmath>
#include <sstream>
#include <algorithm>

/*=============================================================================
 * RADIAL EXPLOSION DEBRIS (Polar-to-Cartesian Conversion)
 *
 * Creates an explosion effect by spawning 20 particles dispersed radially.
 * For each particle:
 *   1. A random angle (0 to 360 degrees) is converted to radians
 *   2. A random speed magnitude is chosen
 *   3. The velocity ROTATION VECTOR is computed:
 *        vx = cos(angle) * speed  — horizontal component
 *        vy = sin(angle) * speed  — vertical component
 *
 * This polar-to-Cartesian conversion ensures particles radiate outward
 * uniformly in all directions from the explosion center, creating a
 * convincing circular blast pattern.
 *=============================================================================*/
void Game::spawnExplosion(float x, float y, float r, float g, float b) {
    for (int i = 0; i < 20; ++i) {
        Particle p;
        p.x = x; p.y = y;
        float angle = (rand() % 360) * 3.14159f / 180.0f;  // Random angle in radians
        float spd = 1.0f + (rand() % 100) / 100.0f * 3.5f; // Random speed magnitude
        p.vx = cos(angle) * spd;  // Rotation vector X component
        p.vy = sin(angle) * spd;  // Rotation vector Y component
        p.r = r; p.g = g; p.b = b;
        p.lifetime = 0.7f + (rand() % 30) / 100.0f;
        particles.push_back(p);
    }
}

/*=============================================================================
 * DIRECTIONAL MUZZLE FLASH
 *
 * Creates a small burst of particles when the player fires.
 * Particles are given slightly randomized velocity vectors with
 * an upward bias (vy > 0) to simulate muzzle flash direction.
 *=============================================================================*/
void Game::spawnMuzzleFlash(float x, float y, float r, float g, float b) {
    for (int i = 0; i < 4; ++i) {
        Particle p;
        p.x = x; p.y = y;
        p.vx = (rand() % 100 - 50) / 30.0f;
        p.vy = 2.0f + (rand() % 100) / 25.0f;
        p.r = r; p.g = g; p.b = b;
        p.lifetime = 0.15f + (rand() % 100) / 1000.0f;
        particles.push_back(p);
    }
}

/*=============================================================================
 * FLOATING TEXT SPAWN HELPER
 *
 * Creates a floating text notification at the specified position.
 * The text drifts upward at the given velocity (vy) and fades out
 * over time (alpha decreasing in FloatingText::update).
 *=============================================================================*/
void Game::spawnFloatingText(float x, float y, const std::string& text, float r, float g, float b, float vy) {
    FloatingText ft;
    ft.x = x;
    ft.y = y;
    ft.text = text;
    ft.vx = 0.0f;
    ft.vy = vy;
    ft.r = r;
    ft.g = g;
    ft.b = b;
    ft.alpha = 1.0f;
    ft.alive = true;
    floatingTexts.push_back(ft);
}

/*=============================================================================
 * SUPER POWER ACTIVATION (Radial Burst + Distance Calculation)
 *
 * The super power ("Refract") demonstrates several rotation vector concepts:
 *
 *   1. BULLET REFLECTION:
 *      All enemy bullets have their vy negated, reversing their direction.
 *      This is equivalent to reflecting the velocity vector across the X axis.
 *
 *   2. EUCLIDEAN DISTANCE CALCULATION:
 *      For each enemy, the distance from the player is computed:
 *        dist = sqrt(dx*dx + dy*dy)
 *      where dx = enemy.x - player.x, dy = enemy.y - player.y.
 *      Enemies within 170 units receive area-of-effect damage.
 *
 *   3. RADIAL EXPLOSION SPAWN:
 *      Damaged/destroyed enemies trigger spawnExplosion() which uses
 *      the cos/sin rotation vectors described above.
 *=============================================================================*/
void Game::activateSuperPower() {
    if (state != PLAYING || superPowerCooldownTimer > 0.0f) {
        return;
    }

    superPowerCooldownTimer = SUPER_POWER_COOLDOWN_MAX;
    superPowerPulseTimer = SUPER_POWER_PULSE_MAX;
    superPowerCenterX = player.x;
    superPowerCenterY = player.y;
    superPowerReadyNotified = false;
    screenShakeTimer = std::max(screenShakeTimer, 0.22f);

    spawnFloatingText(player.x - 70.0f, player.y + 30.0f, "SUPER POWER: REFRACT", 0.2f, 0.9f, 1.0f, 1.1f);

    // Reflect all enemy bullets (negate vy — vector reflection across X axis)
    for (auto& eb : enemyBullets) {
        if (!eb.alive) continue;
        eb.vy = -std::abs(eb.vy);
        eb.y += 8.0f;
    }

    // Area-of-effect damage using Euclidean distance
    for (auto& e : enemies) {
        if (!e.alive) continue;
        float dx = e.x - player.x;
        float dy = e.y - player.y;
        float dist = std::sqrt(dx * dx + dy * dy);  // Distance calculation
        if (dist <= 170.0f) {
            int damage = (e.enemyType == 3 || e.enemyType == 7) ? 4 : 2;
            e.hp -= damage;
            spawnExplosion(e.x, e.y, 0.3f, 0.9f, 1.0f);
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

/*=============================================================================
 * FLOATING TEXT UPDATE
 *
 * Translates the floating text upward and fades it out over time.
 * When alpha reaches zero, the text is marked as dead.
 *=============================================================================*/
void FloatingText::update() {
    x += vx;
    y += vy;
    alpha -= 0.02f; // Fade out slowly
    if (alpha <= 0.0f) {
        alive = false;
    }
}

/*=============================================================================
 * FLOATING TEXT RENDERING
 *
 * Renders alpha-blended text at the current position.
 * Uses glRasterPos2f for positioning and glutBitmapCharacter for rendering.
 *=============================================================================*/
void FloatingText::draw() const {
    if (!alive || alpha <= 0.0f) return;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(r, g, b, alpha);
    
    // Draw the text
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
    glDisable(GL_BLEND);
}

/*=============================================================================
 * ENEMY RENDERING WITH TRIGONOMETRIC VERTEX GENERATION
 *
 * This is the most comprehensive demonstration of rotation vectors:
 *
 * ENGINE THRUSTERS (all types):
 *   Uses sin(elapsed * frequency) for flame flicker animation.
 *   The flicker value modulates thruster flame height.
 *
 * TYPE 0 (Drone): Basic polygon filling with drawRect/drawTriangle.
 *
 * TYPE 1 (Shooter): drawTriangle hull + drawRect cockpit.
 *
 * TYPE 2 (Seeker): drawTriangle hull (inverted orange).
 *
 * TYPE 3 (L1 Boss): Large multi-part body with health bar.
 *
 * TYPE 4 (Phantom Diamond):
 *   Uses GL_POLYGON with 4 vertices forming a diamond shape.
 *   Inner diamond outline uses GL_LINE_LOOP.
 *
 * TYPE 5 (Sentinel Hexagon) - ROTATION VECTOR VERTEX GENERATION:
 *   Generates 6 hexagon vertices using:
 *     x = center_x + cos(i * 2*PI/6 - PI/6) * radius
 *     y = center_y + sin(i * 2*PI/6 - PI/6) * radius
 *   The -PI/6 offset rotates the hexagon by 30 degrees.
 *   Both filled (GL_POLYGON) and outlined (GL_LINE_LOOP) versions.
 *
 * TYPE 6 (Wraith Pentagon) - ROTATION VECTOR VERTEX GENERATION:
 *   Generates 5 pentagon vertices using:
 *     x = center_x + cos(i * 2*PI/5 + PI/2) * radius
 *     y = center_y + sin(i * 2*PI/5 + PI/2) * radius
 *   The +PI/2 offset points the pentagon upward.
 *   Also generates a star pattern using non-adjacent vertex connections.
 *
 * TYPE 7 (Nebula Overlord) - MULTIPLE ROTATION VECTOR TECHNIQUES:
 *   a) Hexagonal body using cos/sin vertex generation (6 vertices)
 *   b) Boss turret rotation using glPushMatrix + glTranslatef + glRotatef
 *      The turret spins at 60 degrees/second: turretAngle = elapsed * 60
 *   c) Pulsing shield ring using GL_LINE_LOOP with 24 cos/sin vertices
 *      at radius (bodyR + 20), with sin(elapsed*3) alpha pulsing
 *   d) HP bar using filled rectangles (polygon filling)
 *
 * STANDARD ENEMY HEALTH BAR:
 *   For multi-HP enemies that have taken damage, a small health bar
 *   is drawn using layered filled rectangles.
 *=============================================================================*/
void Enemy::draw() const {
    // --- Engine thruster flame (drawn behind the enemy, pointing upward) ---
    float elapsed = glutGet(GLUT_ELAPSED_TIME) * 0.001f;
    float flicker = 1.0f + 0.25f * sin(elapsed * 15.0f + x * 0.1f);

    if (enemyType == 0) {
        // Small red thruster on top
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        float fh = 10.0f * flicker;
        glColor4f(1.0f, 0.4f, 0.0f, 0.7f);
        glBegin(GL_TRIANGLES);
            glVertex2f(x - 5.0f, y + h/2);
            glVertex2f(x + 5.0f, y + h/2);
            glVertex2f(x, y + h/2 + fh);
        glEnd();
        glColor4f(1.0f, 0.8f, 0.2f, 0.5f);
        glBegin(GL_TRIANGLES);
            glVertex2f(x - 3.0f, y + h/2);
            glVertex2f(x + 3.0f, y + h/2);
            glVertex2f(x, y + h/2 + fh * 0.6f);
        glEnd();
        glDisable(GL_BLEND);

        glColor3f(1.0f, 0.3f, 0.3f);
        drawRect(x, y, w, h);
        glColor3f(0.6f, 0.0f, 0.0f);
        drawRect(x, y - 8, w - 8, 6);
        glColor3f(1.0f, 1.0f, 0.0f);
        drawRect(x, y, 6, 6);
    }
    else if (enemyType == 1) {
        // Purple thruster
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        float fh = 12.0f * flicker;
        glColor4f(0.8f, 0.2f, 1.0f, 0.6f);
        glBegin(GL_TRIANGLES);
            glVertex2f(x - 4.0f, y + h/2);
            glVertex2f(x + 4.0f, y + h/2);
            glVertex2f(x, y + h/2 + fh);
        glEnd();
        glDisable(GL_BLEND);

        glColor3f(0.7f, 0.1f, 0.8f);
        drawTriangle(x, y, w, h);
        glColor3f(0.2f, 0.9f, 1.0f);
        drawRect(x, y - 2, 8, 8);
    }
    else if (enemyType == 2) {
        // Orange thruster
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        float fh = 10.0f * flicker;
        glColor4f(1.0f, 0.6f, 0.0f, 0.7f);
        glBegin(GL_TRIANGLES);
            glVertex2f(x - 4.0f, y + h/2);
            glVertex2f(x + 4.0f, y + h/2);
            glVertex2f(x, y + h/2 + fh);
        glEnd();
        glDisable(GL_BLEND);

        glColor3f(1.0f, 0.5f, 0.0f);
        drawTriangle(x, y, w, h);
        glColor3f(1.0f, 1.0f, 0.0f);
        drawRect(x, y + 10, 4, 8);
    }
    else if (enemyType == 3) {
        // Boss: dual large thrusters
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        float fh = 18.0f * flicker;
        float flicker2 = 1.0f + 0.2f * sin(elapsed * 18.0f + 2.0f);
        float fh2 = 18.0f * flicker2;
        // Left thruster
        glColor4f(0.3f, 0.5f, 1.0f, 0.6f);
        glBegin(GL_TRIANGLES);
            glVertex2f(x - w/2 - 10 - 6, y + h/2 - 5);
            glVertex2f(x - w/2 - 10 + 6, y + h/2 - 5);
            glVertex2f(x - w/2 - 10, y + h/2 - 5 + fh);
        glEnd();
        // Right thruster
        glBegin(GL_TRIANGLES);
            glVertex2f(x + w/2 + 10 - 6, y + h/2 - 5);
            glVertex2f(x + w/2 + 10 + 6, y + h/2 - 5);
            glVertex2f(x + w/2 + 10, y + h/2 - 5 + fh2);
        glEnd();
        glDisable(GL_BLEND);

        glColor3f(0.4f, 0.4f, 0.45f);
        drawRect(x, y, w, h);
        glColor3f(0.25f, 0.25f, 0.3f);
        drawRect(x - w/2 - 10, y, 20, h - 10);
        drawRect(x + w/2 + 10, y, 20, h - 10);
        
        // Boss health bar (polygon filling)
        float pct = (float)hp / maxHp;
        glColor3f(1.0f, 0.0f, 0.0f);
        drawRect(x, y + h/2 + 15, w * pct, 6);
    }
    // --- Level 2 Enemies ---
    // Type 4: Phantom — Green/Teal Diamond (GL_POLYGON vertex placement)
    else if (enemyType == 4) {
        // Teal thruster
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        float fh = 10.0f * flicker;
        glColor4f(0.0f, 1.0f, 0.6f, 0.6f);
        glBegin(GL_TRIANGLES);
            glVertex2f(x - 4.0f, y + h/2);
            glVertex2f(x + 4.0f, y + h/2);
            glVertex2f(x, y + h/2 + fh);
        glEnd();
        glDisable(GL_BLEND);

        // Diamond shape using GL_POLYGON (rotated square)
        glBegin(GL_POLYGON);
            glColor3f(0.0f, 0.8f, 0.6f); // Teal top
            glVertex2f(x, y + h/2);
            glColor3f(0.1f, 0.5f, 0.4f); // Darker sides
            glVertex2f(x + w/2, y);
            glColor3f(0.0f, 0.7f, 0.5f);
            glVertex2f(x, y - h/2);
            glVertex2f(x - w/2, y);
        glEnd();
        // Inner diamond outline
        glColor3f(0.2f, 1.0f, 0.8f);
        glLineWidth(1.5f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(x, y + h/4);
            glVertex2f(x + w/4, y);
            glVertex2f(x, y - h/4);
            glVertex2f(x - w/4, y);
        glEnd();
    }
    // Type 5: Sentinel — Magenta/Pink Hexagon
    // ROTATION VECTOR VERTEX GENERATION:
    // Hexagon vertices are generated by iterating angle theta from 0 to 2*PI
    // in 6 equal steps, computing x = cos(theta)*r, y = sin(theta)*r
    else if (enemyType == 5) {
        // Magenta thruster
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        float fh = 12.0f * flicker;
        glColor4f(0.8f, 0.2f, 1.0f, 0.6f);
        glBegin(GL_TRIANGLES);
            glVertex2f(x - 5.0f, y + h/2);
            glVertex2f(x + 5.0f, y + h/2);
            glVertex2f(x, y + h/2 + fh);
        glEnd();
        glDisable(GL_BLEND);

        // Hexagon body — vertices generated using cos(theta)/sin(theta)
        float r = w / 2.0f;
        glBegin(GL_POLYGON);
        for (int i = 0; i < 6; ++i) {
            float theta = i * 2.0f * 3.14159f / 6.0f - 3.14159f / 6.0f;
            if (i < 3) glColor3f(0.8f, 0.15f, 0.6f); // Hot pink top
            else glColor3f(0.5f, 0.1f, 0.4f); // Darker bottom
            glVertex2f(x + cos(theta) * r, y + sin(theta) * r);
        }
        glEnd();
        // Hexagon outline using same cos/sin vertex generation
        glColor3f(1.0f, 0.4f, 0.8f);
        glLineWidth(1.5f);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 6; ++i) {
            float theta = i * 2.0f * 3.14159f / 6.0f - 3.14159f / 6.0f;
            glVertex2f(x + cos(theta) * r, y + sin(theta) * r);
        }
        glEnd();
        // Center dot
        glColor3f(1.0f, 1.0f, 1.0f);
        drawRect(x, y, 4, 4);
    }
    // Type 6: Wraith — Fading dark purple pentagon
    // ROTATION VECTOR VERTEX GENERATION:
    // Pentagon vertices at angles i * 2*PI/5 + PI/2 (offset to point upward)
    else if (enemyType == 6) {
        float fadeAlpha = 0.3f + 0.7f * (0.5f + 0.5f * sin(moveTimer * 2.0f)); // Pulse in/out
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Pentagon body — vertices generated using cos(theta)/sin(theta)
        float r = w / 2.0f;
        glBegin(GL_POLYGON);
        for (int i = 0; i < 5; ++i) {
            float theta = i * 2.0f * 3.14159f / 5.0f + 3.14159f / 2.0f;
            glColor4f(0.4f, 0.0f, 0.5f, fadeAlpha);
            glVertex2f(x + cos(theta) * r, y + sin(theta) * r);
        }
        glEnd();
        // Inner star pattern — connects non-adjacent vertices
        glColor4f(0.8f, 0.3f, 1.0f, fadeAlpha);
        glLineWidth(1.0f);
        glBegin(GL_LINES);
        for (int i = 0; i < 5; ++i) {
            float theta1 = i * 2.0f * 3.14159f / 5.0f + 3.14159f / 2.0f;
            float theta2 = ((i + 2) % 5) * 2.0f * 3.14159f / 5.0f + 3.14159f / 2.0f;
            glVertex2f(x + cos(theta1) * r * 0.8f, y + sin(theta1) * r * 0.8f);
            glVertex2f(x + cos(theta2) * r * 0.8f, y + sin(theta2) * r * 0.8f);
        }
        glEnd();
        glDisable(GL_BLEND);
    }
    // Type 7: Nebula Overlord Boss — Multiple rotation vector techniques
    else if (enemyType == 7) {
        float hpPct = (float)hp / maxHp;

        // Dual thrusters
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        float fh2L = 20.0f * flicker;
        float flicker2 = 1.0f + 0.2f * sin(elapsed * 18.0f + 2.0f);
        float fh2R = 20.0f * flicker2;
        // Left thruster — purple
        glColor4f(0.6f, 0.1f, 0.8f, 0.7f);
        glBegin(GL_TRIANGLES);
            glVertex2f(x - 25, y + h/2);
            glVertex2f(x - 15, y + h/2);
            glVertex2f(x - 20, y + h/2 + fh2L);
        glEnd();
        // Right thruster
        glBegin(GL_TRIANGLES);
            glVertex2f(x + 15, y + h/2);
            glVertex2f(x + 25, y + h/2);
            glVertex2f(x + 20, y + h/2 + fh2R);
        glEnd();
        glDisable(GL_BLEND);

        // Hexagonal body — cos/sin vertex generation with phase-based colors
        float bodyR = w / 2.0f;
        float bodyR2, bodyG2, bodyB2;
        if (hpPct > 0.6f) { bodyR2 = 0.3f; bodyG2 = 0.1f; bodyB2 = 0.5f; }      // Purple
        else if (hpPct > 0.3f) { bodyR2 = 0.6f; bodyG2 = 0.1f; bodyB2 = 0.5f; }  // Magenta
        else { bodyR2 = 0.8f; bodyG2 = 0.1f; bodyB2 = 0.2f; }                     // Red

        glBegin(GL_POLYGON);
        for (int i = 0; i < 6; ++i) {
            float theta = i * 2.0f * 3.14159f / 6.0f;
            if (i < 3) glColor3f(bodyR2 + 0.1f, bodyG2 + 0.1f, bodyB2 + 0.1f);
            else glColor3f(bodyR2 - 0.1f, bodyG2, bodyB2 - 0.1f);
            glVertex2f(x + cos(theta) * bodyR, y + sin(theta) * bodyR);
        }
        glEnd();

        // Wing panels
        glColor3f(bodyR2 - 0.15f, bodyG2, bodyB2 - 0.1f);
        drawRect(x - w/2 - 15, y, 25, h - 15);
        drawRect(x + w/2 + 15, y, 25, h - 15);

        // Rotating turrets using glPushMatrix/glRotatef (composite transformation)
        float turretAngle = elapsed * 60.0f; // Spin rate: 60 degrees per second
        // Left turret — composite: Translate to position, then Rotate
        glPushMatrix();
            glTranslatef(x - w/2 - 15, y, 0.0f);
            glRotatef(turretAngle, 0.0f, 0.0f, 1.0f);
            glColor3f(1.0f, 0.4f, 0.8f);
            glBegin(GL_TRIANGLES);
                glVertex2f(0, 8);
                glVertex2f(-5, -5);
                glVertex2f(5, -5);
            glEnd();
        glPopMatrix();
        // Right turret — rotates in opposite direction
        glPushMatrix();
            glTranslatef(x + w/2 + 15, y, 0.0f);
            glRotatef(-turretAngle, 0.0f, 0.0f, 1.0f);
            glColor3f(1.0f, 0.4f, 0.8f);
            glBegin(GL_TRIANGLES);
                glVertex2f(0, 8);
                glVertex2f(-5, -5);
                glVertex2f(5, -5);
            glEnd();
        glPopMatrix();

        // Pulsing shield ring — cos/sin circle vertex generation
        if (hpPct > 0.3f) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            float pulseAlpha = 0.2f + 0.2f * sin(elapsed * 3.0f);
            glColor4f(0.8f, 0.2f, 1.0f, pulseAlpha);
            glLineWidth(2.0f);
            glBegin(GL_LINE_LOOP);
            for (int i = 0; i < 24; ++i) {
                float theta = i * 2.0f * 3.14159f / 24.0f;
                glVertex2f(x + cos(theta) * (bodyR + 20), y + sin(theta) * (bodyR + 20));
            }
            glEnd();
            glDisable(GL_BLEND);
        }

        // HP bar (polygon filling)
        glColor3f(0.3f, 0.3f, 0.3f);
        drawRect(x, y + h/2 + 18, w + 20, 8);
        if (hpPct > 0.6f) glColor3f(0.5f, 0.1f, 0.8f);
        else if (hpPct > 0.3f) glColor3f(0.8f, 0.4f, 0.1f);
        else glColor3f(1.0f, 0.1f, 0.1f);
        drawRect(x - (w + 20)/2 + (w + 20) * hpPct / 2, y + h/2 + 18, (w + 20) * hpPct, 6);
    }

    // Draw a small health bar for standard multi-HP enemies that have taken damage
    if (enemyType != 3 && enemyType != 7 && maxHp > 1 && hp < maxHp) {
        float barW = w;
        float barH = 4.0f;
        float barY = y + h/2 + 8.0f;
        
        // Background (gray) — filled rectangle
        glColor3f(0.2f, 0.2f, 0.2f);
        drawRect(x, barY, barW, barH);
        
        // Foreground (green if > 50%, red otherwise) — filled rectangle
        float pct = (float)hp / maxHp;
        if (pct > 0.5f) {
            glColor3f(0.2f, 0.9f, 0.2f); // Green
        } else {
            glColor3f(1.0f, 0.3f, 0.3f); // Red
        }
        drawRect(x - barW/2 + (barW * pct)/2, barY, barW * pct, barH);
    }
}

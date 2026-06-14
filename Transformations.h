/*=============================================================================
 * Transformations.h - Topic 3: Transformations
 *
 * Covers all transformation operations in the game:
 *   - Object translation: entity movement (x += vx, y += vy)
 *   - Dynamic scaling: particle sizes shrinking over lifetime
 *   - Composite transformation matrices:
 *       glPushMatrix + glTranslatef for screen shake effect
 *       glPushMatrix + glTranslatef + glRotatef for scrap rotation
 *
 * Functions implemented in Transformations.cpp:
 *   - Particle::update() / Particle::draw()
 *   - Enemy::update()  (8 enemy movement patterns)
 *   - Scrap::draw()    (composite glTranslatef + glRotatef)
 *   - Game::draw()     (screen shake via glTranslatef)
 *   - Game::update()   (master update loop)
 *   - Game::startNextWave()
 *   - Game::spawnEnemyBullet()
 *
 * Implementation: Transformations.cpp
 *=============================================================================*/

#ifndef TRANSFORMATIONS_H
#define TRANSFORMATIONS_H

#include "ViewingClipping.h"

#endif

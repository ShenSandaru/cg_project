/*=============================================================================
 * RotationVectors.h - Topic 4: Rotation Vectors
 *
 * Covers all trigonometric rotation calculations in the game:
 *   - Radial explosion debris: cos(angle) * speed, sin(angle) * speed
 *   - Polar-to-Cartesian conversion for circular particle bursts
 *   - Hexagon/pentagon/diamond vertex generation via angular iteration
 *   - Boss turret rotation using glRotatef
 *   - Shield ring circle generation using cos(theta), sin(theta)
 *   - Distance calculations using sqrt(dx*dx + dy*dy)
 *
 * Functions implemented in RotationVectors.cpp:
 *   - Game::spawnExplosion()    (radial debris dispersal)
 *   - Game::spawnMuzzleFlash()  (directional particle spray)
 *   - Game::spawnFloatingText() (text spawn helper)
 *   - Game::activateSuperPower() (radial burst + distance check)
 *   - FloatingText::update() / FloatingText::draw()
 *   - Enemy::draw()            (trig vertex gen for all enemy types)
 *
 * Implementation: RotationVectors.cpp
 *=============================================================================*/

#ifndef ROTATION_VECTORS_H
#define ROTATION_VECTORS_H

#include "ViewingClipping.h"

#endif

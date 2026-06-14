/*=============================================================================
 * AnimationPrinciples.h - Topic 5: Animation Principles (Timing & Staging)
 *
 * Covers animation timing, visual staging, and feedback loops:
 *   - Framerate management: glutTimerFunc(16, ...) for ~60fps updates
 *   - Background staging: scrolling star parallax (depth illusion)
 *   - Multi-layer parallax: nebula puffs (slow/bg) + stars (fast/fg)
 *   - Enemy spawn timing: interval-based wave pacing
 *   - UI screen staging: ship selection, upgrade shop, level transitions
 *   - Follow-through/reaction feedback: shield flash, base hit flash
 *
 * Functions implemented in AnimationPrinciples.cpp:
 *   - StarField::init() / update() / draw()   (star parallax)
 *   - NebulaField::init() / update() / draw() (nebula parallax)
 *   - Game::spawnEnemy()          (spawn timing intervals)
 *   - Game::drawShipSelect()      (ship selection staging)
 *   - Game::drawUpgradeShop()     (upgrade shop staging)
 *   - Game::drawLevelTransition() (level transition staging)
 *
 * Implementation: AnimationPrinciples.cpp
 *=============================================================================*/

#ifndef ANIMATION_PRINCIPLES_H
#define ANIMATION_PRINCIPLES_H

#include "ViewingClipping.h"

#endif

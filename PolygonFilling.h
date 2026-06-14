/*=============================================================================
 * PolygonFilling.h - Topic 2: Polygon Filling & Primitive Construction
 *
 * Declares core 2D drawing primitive functions used throughout the game.
 *
 * CG Concepts demonstrated:
 *   - Filled rectangle rendering using GL_QUADS
 *   - Filled triangle rendering using GL_TRIANGLES
 *   - Bresenham's line-drawing algorithm for pixel-perfect rasterization
 *   - Midpoint circle algorithm with 8-way symmetry
 *
 * These primitives form the foundation for all visual elements including
 * ship bodies, enemy shapes, bullets, health bars, and UI components.
 *
 * Implementation: PolygonFilling.cpp
 *=============================================================================*/

#ifndef POLYGON_FILLING_H
#define POLYGON_FILLING_H

/* drawRect - Filled Rectangle Primitive
 * Renders a solid rectangle centered at (cx, cy) with dimensions w x h
 * using GL_QUADS polygon filling. Used for ship bodies, health bars,
 * menu buttons, and HUD elements. */
void drawRect(float cx, float cy, float w, float h);

/* drawTriangle - Filled Triangle Primitive
 * Renders a solid isosceles triangle centered at (cx, cy) pointing upward
 * using GL_TRIANGLES polygon filling. Used for ship hulls, enemy shapes,
 * and decorative arrow indicators. */
void drawTriangle(float cx, float cy, float w, float h);

/* drawBresenhamLine - Bresenham Line Drawing Algorithm
 * Rasterizes a line between two points using Bresenham's integer-based
 * incremental algorithm. Plots individual pixels via GL_POINTS for
 * precise line rendering without floating-point accumulation errors.
 * Used for bullet trails and super power beam effects. */
void drawBresenhamLine(float x0, float y0, float x1, float y1);

/* drawMidpointCircle - Midpoint Circle Algorithm
 * Renders a circle outline using the midpoint circle algorithm with
 * 8-way symmetry optimization. Only computes one octant and mirrors
 * points to all eight symmetric positions. Used for player shield
 * bubbles and super power pulse rings. */
void drawMidpointCircle(float cx, float cy, float radius);

#endif

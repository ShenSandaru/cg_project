# Space Odyssey: Rogue Starship

A 2D space shooter game built with OpenGL/GLUT demonstrating five core Computer Graphics concepts.

## How to Build

1. Open `spaceshooter.cbp` in Code::Blocks
2. Press **Ctrl+F9** to build, or **F9** to build and run
3. Requires: FreeGLUT, OpenGL32, GLU32 libraries

## How to Play

- **Arrow Keys / WASD** — Move your ship
- **Spacebar** — Fire lasers
- **F** — Activate Super Power (Refract Wave)
- **P** — Pause
- **ESC** — Exit
- Destroy enemies, collect scrap, upgrade between waves
- Survive through Level 1 and Level 2 bosses to win

---

## Project Structure — Five CG Topic Files

The codebase is organized into **five files**, each named after a Computer Graphics concept it demonstrates. All files are interconnected through `main.cpp`.

### File Dependency Diagram

```
main.cpp  (entry point — includes all 5 topic headers)
    │
    ├── ViewingClipping.h     (Master header — all type declarations)
    │       │
    │       └── PolygonFilling.h  (Drawing primitive declarations)
    │
    ├── Transformations.h     (includes ViewingClipping.h)
    ├── RotationVectors.h     (includes ViewingClipping.h)
    └── AnimationPrinciples.h (includes ViewingClipping.h)
```

---

### Topic 1 — Viewing & Clipping (`ViewingClipping.h` / `ViewingClipping.cpp`)

**CG Concepts:** gluOrtho2D viewport, boundary clamping, off-screen clipping, AABB collision detection.

| Concept | Function | Description |
|---------|----------|-------------|
| gluOrtho2D viewport | `Game::init()` | Sets up 2D orthographic projection mapping [0,800]×[0,600] |
| Boundary clamping | `Player::update()` | Clamps player position within viewport bounds |
| Off-screen clipping | `Bullet::update()`, `EnemyBullet::update()`, `Scrap::update()` | Removes entities that exit visible area |
| AABB collision | `aabb()`, `Game::checkCollisions()` | Axis-aligned bounding box overlap tests |
| Input handling | `Game::handleInput()`, `Game::handleSpecialInput()` | Maps keys to game actions |
| State management | `Game::Game()`, `Game::init()`, `Game::reset()` | Game lifecycle management |

---

### Topic 2 — Polygon Filling & Primitive Construction (`PolygonFilling.h` / `PolygonFilling.cpp`)

**CG Concepts:** GL_QUADS rectangle filling, GL_TRIANGLES triangle filling, Bresenham line algorithm, Midpoint circle algorithm, health bar rendering.

| Concept | Function | Description |
|---------|----------|-------------|
| Filled rectangle (GL_QUADS) | `drawRect()` | Core primitive for bodies, bars, buttons |
| Filled triangle (GL_TRIANGLES) | `drawTriangle()` | Used for ship hulls and enemy shapes |
| Bresenham line algorithm | `drawBresenhamLine()` | Pixel-perfect line rasterization using integer math |
| Midpoint circle algorithm | `drawMidpointCircle()` | Circle outline with 8-way symmetry |
| Ship rendering | `Player::draw()` | Layered primitives: triangles, rectangles, circles |
| Bullet rendering | `Bullet::draw()`, `EnemyBullet::draw()` | Bresenham line-based laser visuals |
| Health bar filling | `Game::drawHUD()` | Background rect + proportional foreground fill |
| Menu construction | `Game::drawMainMenu()` | GL_POLYGON buttons with angled corners |
| UI screens | `Game::drawGuidelines()`, `Game::drawVictoryScreen()` | Text + polygon layouts |
| Text rendering | `Game::drawText()` | GLUT bitmap character rendering |

---

### Topic 3 — Transformations (`Transformations.h` / `Transformations.cpp`)

**CG Concepts:** Object translation (movement), dynamic scaling (particle sizes), composite matrices (glTranslatef/glRotatef for screen shake and scrap rotation).

| Concept | Function | Description |
|---------|----------|-------------|
| Translation (x += vx) | `Particle::update()` | Basic 2D position translation |
| Dynamic scaling | `Particle::draw()` | Size = 5.0 × lifetime + 1.0 (shrinks over time) |
| 8 movement patterns | `Enemy::update()` | Different translation strategies per enemy type |
| Composite matrix | `Scrap::draw()` | glPushMatrix + glTranslatef + glRotatef |
| Screen shake | `Game::draw()` | glTranslatef applied to entire scene |
| Master update loop | `Game::update()` | Manages all entity position updates |
| Wave management | `Game::startNextWave()` | Wave progression and difficulty |
| Bullet offsets | `Game::spawnEnemyBullet()` | Position offset calculations |

---

### Topic 4 — Rotation Vectors (`RotationVectors.h` / `RotationVectors.cpp`)

**CG Concepts:** cos(angle)/sin(angle) for radial debris dispersal, trigonometric vertex generation for regular polygons, boss turret glRotatef, Euclidean distance.

| Concept | Function | Description |
|---------|----------|-------------|
| Radial debris (cos/sin) | `Game::spawnExplosion()` | vx=cos(θ)×speed, vy=sin(θ)×speed |
| Muzzle flash spray | `Game::spawnMuzzleFlash()` | Directional particle burst |
| Euclidean distance | `Game::activateSuperPower()` | sqrt(dx²+dy²) for area-of-effect radius |
| Hexagon vertex gen | `Enemy::draw()` (Type 5) | 6 vertices via cos(i×2π/6), sin(i×2π/6) |
| Pentagon vertex gen | `Enemy::draw()` (Type 6) | 5 vertices via cos(i×2π/5), sin(i×2π/5) |
| Boss body (hexagon) | `Enemy::draw()` (Type 7) | Phase-based color + cos/sin vertices |
| Turret rotation | `Enemy::draw()` (Type 7) | glRotatef(angle, 0, 0, 1) for spinning |
| Shield ring | `Enemy::draw()` (Type 7) | 24-point cos/sin circle outline |
| Text helper | `Game::spawnFloatingText()` | Floating text spawn utility |
| Text animation | `FloatingText::update/draw()` | Position offset + alpha fade |

---

### Topic 5 — Animation Principles (`AnimationPrinciples.h` / `AnimationPrinciples.cpp`)

**CG Concepts:** glutTimerFunc framerate management, parallax scrolling (staging), multi-layer depth, enemy spawn timing, UI screen staging.

| Concept | Function | Description |
|---------|----------|-------------|
| Framerate timing | `main.cpp: timer()` | glutTimerFunc(16, ...) for ~60fps |
| Star parallax | `StarField::init/update/draw()` | 80 stars at varying speeds for depth |
| Nebula parallax | `NebulaField::init/update/draw()` | 2-layer: slow puffs (bg) + fast stars (fg) |
| Spawn timing | `Game::spawnEnemy()` | Interval = max(25, 75-wave×4) frames |
| Ship selection staging | `Game::drawShipSelect()` | 3-card horizontal layout |
| Upgrade shop staging | `Game::drawUpgradeShop()` | 2-panel: stats (left) + upgrades (right) |
| Level transition staging | `Game::drawLevelTransition()` | Dramatic reveal layout |

---

### main.cpp (Entry Point)

Interconnects all five topic modules by:
- Including all 5 topic headers
- Creating the global `Game` instance
- Setting up GLUT callbacks (display, timer, keyboard)
- The `glutTimerFunc(16, timer, 0)` call itself demonstrates Topic 5 (Timing)

---

## Game Features

- **3 Playable Ships**: Interceptor (fast), Aegis Tank (heavy), Vanguard (triple weapons)
- **2 Levels**: Star Field (Level 1) and Nebula Zone (Level 2)
- **8 Enemy Types**: Drones, Shooters, Seekers, Boss (L1), Phantoms, Sentinels, Wraiths, Overlord (L2)
- **Upgrade System**: Weapons, Hull Repair, Shield upgrades between waves
- **Super Power**: Refract Wave reflects bullets and damages nearby enemies
- **Visual Effects**: Explosions, parallax backgrounds, screen shake, shield flash

# Copilot Instructions for IIDX-Screen (Miniature Arcade Arduino)

## Project Overview
- This project is an Arduino sketch for a miniature arcade screen, simulating a rhythm game (IIDX-style) on a 320x170 TFT display.
- Main logic is in `IIDX-Screen.ino`. Supporting files: `bitmap.h` and `scoreboard.h` contain bitmap data for backgrounds and overlays.
- Uses Adafruit GFX and a custom `ST7789_AVR` display driver.

## Architecture & Data Flow
- The display is initialized and rotated for vertical gameplay.
- Notes are managed as an array of structs, each with position and active state.
- Notes scroll horizontally (right to left) in 5 colored lanes, with combo and counter tracking.
- Bitmaps are drawn as backgrounds, with lanes and notes rendered on top.
- Serial input can trigger a soft reset (send 'd' over serial).

## Key Files
- `IIDX-Screen.ino`: Main game loop, note logic, rendering, serial handling.
- `bitmap.h`, `scoreboard.h`: Contain 320x170 RGB565 bitmaps as PROGMEM arrays for backgrounds.

## Developer Workflows
- **Build/Upload:** Use Arduino IDE or CLI to upload `IIDX-Screen.ino` to a compatible board (e.g., AVR-based Arduino).
- **Dependencies:** Requires Adafruit GFX and a custom `ST7789_AVR` library (not included here).
- **Bitmaps:** Update `bitmap.h` and `scoreboard.h` with new images using a tool that exports 320x170 RGB565 arrays.
- **Serial Debug:** Use 9600 baud for serial monitor. Sending 'd' triggers a reset.

## Project-Specific Patterns
- All drawing is double-buffered: erase previous note before redrawing.
- Lane colors are defined in an array; update here to change lane visuals.
- Combo/counter displays use fixed positions and leading zeros for alignment.
- The screen is rotated for intro and gameplay, then reset.
- Game resets after 300 frames or on serial command.

## Integration Points
- Expects `ST7789_AVR` and Adafruit GFX libraries in the Arduino libraries folder.
- Bitmaps must match expected dimensions and format.

## Example: Spawning a Note
```cpp
spawnNote(lane); // Adds a note to the specified lane, if a slot is free
```

## Example: Serial Reset
```cpp
if (Serial.read() == 'd') { /* triggers reset */ }
```

## Conventions
- Use `lcd.setRotation()` to switch between vertical and horizontal drawing.
- All screen coordinates assume 320x170 (rotated as needed).
- Use `lcd.fillRect` and `lcd.drawBitmap` for all drawing.

---

For questions about display drivers or bitmap format, see `ST7789_AVR.h` and Adafruit GFX documentation.

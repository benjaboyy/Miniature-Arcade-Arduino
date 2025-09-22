# Copilot Instructions for AI Coding Agents

## Project Overview
This project is a Python GUI for controlling one or two Arduino devices via serial ports. The UI is built with customtkinter for a modern, responsive look. Users can select COM ports, connect, and send predefined or custom commands to the Arduino(s). The app is designed for a miniature arcade controller setup.

## Key Files
- `arduino_arcade_controller.py`: Main application logic and customtkinter-based GUI.
- `commands.json`: Maps button labels to serial command characters sent to Arduino.
- `build.bat`: Builds a standalone Windows executable using PyInstaller.
- `icon.ico`: Application icon.
- `Readme.txt`: Basic usage instructions.


## Architecture & Patterns
- **Single-file GUI**: All logic is in `arduino_arcade_controller.py`.
- **Command Mapping**: Commands are loaded from `commands.json` and mapped to buttons.
- **Serial Communication**: Uses `pyserial` to send commands to one or two Arduino devices.
- **UI Patterns**: Uses customtkinter for a modern, responsive interface. Layout uses `grid` with weights for resizing. Controls are grouped in frames for clarity and compactness. No hover effects, but color and style are modernized.
- **Idle/Test Modes**: Special buttons toggle idle mode or send a test message to the second device.


## Developer Workflows
- **Run Locally**: `python arduino_arcade_controller.py`
- **Build Executable**: Run `build.bat` (requires PyInstaller). Output is placed one directory up.
- **Dependencies**: Requires `pyserial` and `customtkinter`. Install with `pip install pyserial customtkinter`.
- **Custom Commands**: Add or edit commands in `commands.json` and restart the app to update buttons.


## Conventions & Tips
- **Responsive Layout**: All frames and controls use `grid` with weights and sticky options for resizing. Padding and spacing are set for a clean, modern look.
- **COM Port Selection**: Both primary and secondary serial devices can be selected and connected independently.
- **Error Handling**: Status messages are shown in the GUI for connection and command errors.
- **No Automated Tests**: There are no test scripts or test folders.
- **No External Config**: All configuration is via `commands.json` and the GUI.
- **No advanced patterns**: No OOP beyond the main `SerialApp` class; no plugins, hooks, or advanced event systems.

## Example: Adding a New Command
1. Add a new entry to `commands.json`, e.g. `"Blue": "b"`.
2. Restart the app. A new button labeled "Blue" will appear.


## Build Notes
- The build script (`build.bat`) uses PyInstaller and expects `icon.ico` to exist.
- The executable is moved up one directory after build and temp folders are cleaned up.

---
For more details, see `Readme.txt` or the main Python file. If you are unsure about a workflow or pattern, ask for clarification or check the main script for examples.

---
**UI Modernization Notes:**
- The UI is now built with customtkinter for a more attractive and responsive experience.
- Layout is managed with `grid` and frame grouping for clarity and resizing.
- If you add new controls, use the same grid/weight/padding conventions for consistency.

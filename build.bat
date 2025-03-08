@echo off
echo Building Arduino Arcade Controller...
python -m PyInstaller --onefile --windowed --icon="icon.ico" arduino_arcade_controller.py
echo Build complete! Your executable is in the "dist" folder.
pause

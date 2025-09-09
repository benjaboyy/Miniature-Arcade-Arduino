@echo off
echo Building Arduino Arcade Controller...
python -m PyInstaller --onefile --windowed --icon="icon.ico" arduino_arcade_controller.py
echo ---
echo ---
echo ---
REM Move executable up one folder
move /Y dist\arduino_arcade_controller.exe ..\arduino_arcade_controller.exe
REM Remove build and dist folders
rmdir /S /Q build
rmdir /S /Q dist
echo Build complete! Your executable is in the project folder.
echo Removed the build and dist folders.
echo  ^_^ DONE!
pause
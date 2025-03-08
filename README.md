# Miniature Arcade Arduino

**Arduino project with LED and LCD animations**

## Overview

#### Arduino
Miniature Arcade Arduino is a lights pattern sketch for your Arduino miniature arcade. This project uses a 74HC595 shift register to control multiple LED animations. The LED patterns are stored in two arrays:

- `chip1Pattern`: Patterns for Chip 1.
- `chip2`: Patterns for Chip 2.

#### Phython program

A small python program which you can run to comunicate by com port to the arduino. It has a simple interfase with some buttons to trigger animations

## Features

- Multiple LED animations (Red, Green, Yellow, White, and more)
- Easy-to-modify patterns and animations
- Uses a 74HC595 shift register for LED control

## Setup

1. **Hardware**: Connect your Arduino to a 74HC595 shift register and the LED/LCD components.
2. **Software**: Upload the `Miniature-Arcade-Arduino.ino` sketch to your Arduino.
3. **Run**: Run the python program to communicate via COM port to the arduino

## Author

**Benjamin Vitters**

## Version

0.6

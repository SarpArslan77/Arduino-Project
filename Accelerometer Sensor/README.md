# LED Matrix Gravity Dots

## Software Description

This Arduino sketch creates an interactive physics simulation where multiple dots respond to gravity and accelerometer input on an 8x8 LED matrix. Key features include:

- **Physics Engine**: Dots fall with simulated gravity and respond to tilt inputs
- **Collision System**: Dots detect and avoid collisions with boundaries and other dots
- **Random Placement**: Uses Fisher-Yates algorithm to generate unique starting positions
- **Dynamic Movement**: 
  - Smooth acceleration based on MPU-6050 input
  - Alternative pathfinding when primary path is blocked
  - Boundary checking to keep dots on matrix

- **Object-Oriented Architecture**:
  - `DOT` class manages each dot's state and behavior
  - Static methods handle dot creation and management
  - Global matrix tracks all positions

- **Efficient Rendering**:
  - Single matrix buffer tracks all dots
  - Full-frame refresh after all physics calculations

## Hardware Components
- MPU-6050 Accelerometer
- MAX7219 LED Matrix Driver
- 8x8 LED Matrix Display
- Arduino Microcontroller

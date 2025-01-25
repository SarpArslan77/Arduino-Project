![base](https://github.com/user-attachments/assets/fc35bc1f-e242-4f15-b0b3-e05854e5eb2a)

Arduino LED Matrix Game
Overview

This project is an interactive game built using an Arduino Uno, featuring an LED matrix, LCD display, buzzer, and joystick. The objective is to navigate a dot on the LED matrix to match or get close to a blinking target. Points are awarded based on accuracy, and the difficulty increases with each turn as the time limit decreases.
Hardware Requirements

    Arduino Uno

    8x8 LED Matrix (MAX7219 controller)

    16x2 LCD Display

    Joystick Module

    Buzzer

    7-segment Display (1-digit)

    Resistors and jumper wires

Circuit Connections
Components Pinout
Component	Arduino Pin
Joystick (SW)	Digital 2
Buzzer	Digital 3
MAX7219 (DIN)	Digital 6
MAX7219 (CS)	Digital 4
MAX7219 (CLK)	Digital 5
LCD (RS, EN, D4-D7)	Digital 7, 8, 9, 10, 11, 12
7-segment (Data)	A3
7-segment (Latch)	A4
7-segment (Clock)	A5
Joystick (X-axis)	Analog A1
Joystick (Y-axis)	Analog A0
Installation

    Required Libraries
    Install the following Arduino libraries via the Library Manager:

        LiquidCrystal (for LCD)

        LedControl (for MAX7219 LED matrix)

    Upload Code

        Connect the components as per the pinout table.

        Upload the provided code to the Arduino.

Gameplay Instructions

    Objective
    Navigate the dot (controlled by the joystick) to the blinking target on the LED matrix. Press the joystick button to lock in your position and score points.

    Scoring

        2 points: Exact match with the target.

        1 point: Adjacent to the target (within 1 row/column).

        0 points: Far from the target.

    Difficulty
    Each turn reduces the allowed time, displayed on the 7-segment display. The time starts at 200 units and decreases by 20 units per turn.

Code Structure
Key Functions

    start(): Initializes the game with welcome messages and rules on the LCD.

    sevenSegWrite(): Updates the 7-segment display with the remaining time.

    randomAnswerGenerator(): Generates a random target position on the LED matrix.

    UserControl(): Handles joystick input to move the player's dot.

    firstLevel(): Manages gameplay logic, scoring, and time constraints.

Flow

    Setup: Initializes hardware and displays introductory messages.

    Game Loop: Runs 5 turns of firstLevel(), each with a decreasing time limit.

    Scoring: After 5 turns, the final score is displayed on the LCD.


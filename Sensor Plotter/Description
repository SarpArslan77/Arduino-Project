# Real-Time Arduino Sensor Plotter

This project provides a Python script to visualize real-time data from an Arduino-connected HC-SR04 ultrasonic sensor. It uses Matplotlib to generate a live, auto-scrolling graph that displays the most recent sensor readings in a "sliding window."

*(Static example of the plot. A live demo would show the line scrolling from right to left.)*

## Features

-   **Real-Time Visualization:** Plots sensor data live as it's received.
-   **Sliding Window:** The graph displays only the last 100 data points, providing a clean, continuously updating view.
-   **Auto-Detection of Arduino Port:** The Python script automatically finds the correct serial port for the Arduino.
-   **Plot Interruption for Invalid Data:** If the sensor reading is out of a specified range (e.g., > 400 cm), the plot line is intelligently broken, preventing visual artifacts from bad data.
-   **Efficient Binary Communication:** The Arduino sends data in a compact binary format for efficient serial communication.

## How It Works

1.  **Arduino:** The Arduino sketch uses the `SR04.h` library to measure distance from the ultrasonic sensor. It sends this distance value as a 4-byte binary `long` integer over the serial port every 100 milliseconds.
2.  **Python:** The Python script listens on the serial port for these 4-byte packets. It uses `struct.unpack` to convert the binary data back into an integer. The script maintains a list of the last 100 data points and updates the Matplotlib plot in a loop, creating the real-time, sliding effect.

import tkinter as tk
import serial
from serial.tools.list_ports import comports
import struct
import numpy as np  # Import numpy to use np.nan
import time
import matplotlib.pyplot as plt

# Auto-detect Arduino port
def find_arduino_port():
    # Look for common Arduino identifiers in port descriptions
    for port in comports():
        if "Arduino" in port.description or "CH340" in port.description or "USB-SERIAL CH340" in port.description:
            return port.device
    return None

arduino_port = find_arduino_port()
if not arduino_port:
    print("Arduino not found! Check connection and drivers.")
    exit()

print(f"Found Arduino on port: {arduino_port}")
arduino = serial.Serial(arduino_port, 9600, timeout=1)
time.sleep(2) # Give a 2-second delay for the serial connection to establish
arduino.reset_input_buffer()  # Clear any old data

# Set up interactive mode
plt.ion()
fig, ax = plt.subplots()
line, = ax.plot([], [], "r-")
ax.set_title("Real-Time Distance Measurement (Sliding Window)")
ax.set_xlabel("Time (s)")
ax.set_ylabel("Distance (cm)")
# Set a fixed Y-axis limit for better visualization, with a little padding
ax.set_ylim(0, 450)
ax.grid(True)

# We only need one set of lists for the data to be plotted
x_current = []
y_current = []
max_points = 100  # The number of points to display on the screen
start_time = time.time()

try:
    while True:
        # Check if there are at least 4 bytes to read (size of a long)
        if arduino.in_waiting >= 4:
            data = arduino.read(4)
            try:
                # Unpack the 4 bytes as a little-endian long integer
                dist = struct.unpack('<l', data)[0]

            except struct.error:
                print("Struct unpack error, resyncing...")
                # If data is bad, clear buffer to find the start of the next valid packet
                arduino.reset_input_buffer()
                continue # Skip to the next loop iteration

            # Get the current time elapsed since the start
            current_time = time.time() - start_time

            # Logic to interrupt the plot
            y_to_plot = dist
            if dist > 400 or dist < 0: # Interrupt if value is > 400 or invalid (e.g. < 0)
                y_to_plot = np.nan # Use NaN to create a break in the plot line

            # Append new data
            x_current.append(current_time)
            y_current.append(y_to_plot) # Append either the distance or NaN

            # If the list is longer than max_points, slice it to keep only the last max_points items
            if len(x_current) > max_points:
                x_current = x_current[-max_points:]
                y_current = y_current[-max_points:]


            # Update the plot data
            line.set_data(x_current, y_current)

            # Rescale the x-axis automatically, but the y-axis is fixed
            ax.relim()
            ax.autoscale_view(scalex=True, scaley=False) # Only autoscale x-axis

            # Redraw the canvas
            fig.canvas.draw()
            fig.canvas.flush_events()

        # A small sleep to prevent the loop from running too fast and using 100% CPU
        time.sleep(0.01)

except KeyboardInterrupt:
    print("Stopping real-time plot.")
    arduino.close() # Close the serial port cleanly
    plt.ioff()
    # Create a final static plot of the last captured window
    plt.show()

except serial.SerialException as e:
    print(f"Serial error: {e}")
    print("Arduino may have been disconnected. Please check the connection.")

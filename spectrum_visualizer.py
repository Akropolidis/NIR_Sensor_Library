import serial
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np
import csv
import copy

# Serial port configuration
SERIAL_PORT = "COM3"
BAUD_RATE = 115200
NUM_CHANNELS = 64

# Initialize ser to None to ensure it's defined
ser = None

try:
    # Set up serial connection
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    print(f"Successfully opened port {SERIAL_PORT}")

except serial.SerialException as e:
    print(f"Error opening serial port: {e}")

# Initialize plotting variables
x_vals = list(range(NUM_CHANNELS))
y_vals = [0] * NUM_CHANNELS

# Initialize the plot
fig, ax = plt.subplots()
lines, = ax.plot(x_vals, y_vals, marker="o", linestyle="--")
ax.set_ylim(0, 500)
ax.set_xlim(0, NUM_CHANNELS - 1)
ax.set_xlabel("Wavelength Channels")
ax.set_ylabel("Amplitude")
ax.set_title("Real-Time Spectral Data")

def writeRow(row, fname):
    with open(fname, 'a', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(row)

# Update function for animation
def update(frame):
    global y_vals
    
    # Check if ser is defined and has data
    if ser is not None and ser.in_waiting > 0: 
        #count = int(ser.readline().decode("utf-8").strip())
        #print(count)
        #mass = float(input("Mass of sample: "))
        #food = input("Food being scanned: ")
        #input("Press Enter once food ready to be scanned...")
        for i in range(NUM_CHANNELS):
            # Read each line as one data point
            line = ser.readline().decode("utf-8").strip()
            y_vals[i] = int(line)
        #outRow = copy.deepcopy(y_vals)
        #outRow.insert(0, mass)
        #outRow.insert(0, food)
        # one row of data has food description, mass, and spectral data
        #writeRow(outRow, "food_data.csv")

    lines.set_ydata(y_vals)
    return lines,

# Animate the plot
ani = animation.FuncAnimation(fig, update, interval=1)  # Adjust interval as needed
plt.show()

# Close the serial connection if it was opened
if ser is not None:  # Check if ser was successfully initialized
    ser.close()


########### NORMALIZED VALUES  #############################################################
# import serial
# import matplotlib.pyplot as plt
# import matplotlib.animation as animation

# # Serial port configuration
# SERIAL_PORT = "COM3"
# BAUD_RATE = 115200
# NUM_CHANNELS = 64

# # Initialize ser to None to ensure it's defined
# ser = None

# try:
#     # Set up serial connection
#     ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
#     print(f"Successfully opened port {SERIAL_PORT}")

# except serial.SerialException as e:
#     print(f"Error opening serial port: {e}")

# # Initialize plotting variables
# x_vals = list(range(NUM_CHANNELS))
# y_vals = [0] * NUM_CHANNELS

# # Initialize the plot
# fig, ax = plt.subplots()
# lines, = ax.plot(x_vals, y_vals, marker="o", linestyle="--")
# ax.set_ylim(0, 1)
# ax.set_xlim(0, NUM_CHANNELS - 1)
# ax.set_xlabel("Wavelength Channels")
# ax.set_ylabel("Amplitude")
# ax.set_title("Real-Time Spectral Data")

# # Update function for animation
# def update(frame):
#     global y_vals
    
#     # Check if ser is defined and has data
#     if ser is not None and ser.in_waiting > 0: 
#         try:
#             for i in range(NUM_CHANNELS):
#                 # Read each line as one data point
#                 line = ser.readline().decode("utf-8").strip()
#                 y_vals[i] = int(line)
#         except ValueError:
#             print("Invalid data:", line)
#             return lines,

#     max_value = max(y_vals)
#     if max_value > 0:
#         for i in range(NUM_CHANNELS):
#             y_vals[i] = y_vals[i] / max_value
#     else:
#         pass
#     lines.set_ydata(y_vals)
#     return lines,

# # Animate the plot
# ani = animation.FuncAnimation(fig, update, interval=50)  # Adjust interval as needed
# plt.show()

# # Close the serial connection if it was opened
# if ser is not None:  # Check if ser was successfully initialized
#     ser.close()


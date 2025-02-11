import serial
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np
import csv
import copy
import time

# Serial port configuration
SERIAL_PORT = "COM3"
BAUD_RATE = 115200
NUM_CHANNELS = 64

def writeRow(row, fname):
    with open(fname, 'a', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(row)

def calc_avg_no_outliers(spec):
    q1 = np.percentile(spec, 25)
    q3 = np.percentile(spec, 75)
    iqr = q3-q1
    count = 0
    sum = 0
    for val in spec:
        if (val <= (q3 + 1.5*iqr)) and (val >= (q1 - 1.5*iqr)):
            sum = sum + val
            count += 1
    num_outliers = len(spec) - count
    print(f"outliers: {num_outliers}")
    return sum/count

# input is spec data
# returns averaged spectra excluding outliers
def avg_spec_no_outliers(data):
    transpose = np.transpose(data) 
    avg_spec = np.zeros(len(transpose))
    for i, spec in enumerate(transpose):
        avg_spec[i] = calc_avg_no_outliers(spec)
    return avg_spec

# get spectra
def getSpectrum(ser):
    # Check if ser is defined and has data
    spec_vals = [0] * (NUM_CHANNELS)
    valid = False
    # add flush?
    if ser is not None:
        ser.reset_input_buffer()
        while ser.in_waiting <= 0: 
            pass #stuck here
        while ser.readline().decode("utf-8").strip() != "-1":
            pass
        ser.readline()
        for i in range(NUM_CHANNELS):
            # Read each line as one data point
            while ser.in_waiting <= 0: 
                pass
            line = ser.readline().decode("utf-8").strip()
            spec_vals[i] = int(line)
        valid = True
    return spec_vals, valid

def plotSpec(spec):
    x_vals = list(range(NUM_CHANNELS))

    # Initialize the plot
    fig, ax = plt.subplots()
    lines, = ax.plot(x_vals, spec, marker="o", linestyle="--")
    ax.set_ylim(0, 1000)
    ax.set_xlim(0, NUM_CHANNELS - 1)
    ax.set_xlabel("Wavelength Channels")
    ax.set_ylabel("Amplitude")
    ax.set_title("Real-Time Spectral Data")
    plt.show()

def main():
    # Initialize ser to None to ensure it's defined
    ser = None
    try:
        # Set up serial connection
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        print(f"Successfully opened port {SERIAL_PORT}")

    except serial.SerialException as e:
        print(f"Error opening serial port: {e}")


    AVG_WINDOW = 30
    exit = None
    print("starting...")
    plate_mass = float(input("Mass of plate: "))
    input("Press Enter to start scanning")
    while(exit != 'e'):
        #spec, valid = getSpectrum(ser)
        #if valid:
            #print(spec)
            #plotSpec(spec)
        mass = float(input("Mass of sample: "))
        mass = mass - plate_mass
        food = input("Food being scanned: ")
        exit = input("Press Enter once food ready to be scanned or 'e' to exit...")
        spectrums = []
        i = 0
        while i < AVG_WINDOW:
            print(f"on iter: {i}")
            spec, valid = getSpectrum(ser)
            if valid:
                spectrums.append(spec)
                i += 1
        spectrums = np.array(spectrums)
        avg_spec = avg_spec_no_outliers(spectrums)
        print(avg_spec)
        plotSpec(avg_spec)
        avg_spec = avg_spec.tolist()
        avg_spec.insert(0, mass)
        avg_spec.insert(0, food)
        # one row of data has food description, mass, and spectral data
        writeRow(avg_spec, "food_data.csv")

    # Close the serial connection if it was opened
    if ser is not None:  # Check if ser was successfully initialized
        ser.close()
    print("connection closed")

if __name__ == "__main__":
    main()




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
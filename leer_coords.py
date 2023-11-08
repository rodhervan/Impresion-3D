import serial
import time

# Open a serial connection to the Arduino
ser = serial.Serial(port='COM4', baudrate=9600, timeout=.1)  # Replace 'COM3' with the correct port for your Arduino

# Read the text file and send coordinates line by line
with open('coordenadas.txt', 'r') as file:
    for line in file:
        # Send the line (coordinate) to the Arduino
        ser.write(line.encode())
        time.sleep(1)  # Wait for some time before sending the next coordinate

# Close the serial connection when done
ser.close()
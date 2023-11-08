import serial
import time
# import keyboard

# Set the COM port and baud rate
com_port = 'COM4'
baud_rate = 9600

# Open the serial port
ser = serial.Serial(com_port, baud_rate, timeout=1)
file_path = 'coordenadas.txt'

while True:
    command = input("Gcode: ")
    if command == 'q':
        ser.close()  # Close the serial connection when done
        break
    if command == 'file':
        try:
            with open(file_path, 'r') as file:
                x = file.readlines()
                num = len(x)
                count=0
                while count < num:
                    try:
                        respuesta = ser.readline().decode('ASCII').rstrip()
                    except:
                        resupuesta = "error"
                    print(respuesta)
                    if "esperando" in respuesta:
                        line = x[count]
                        ser.write(line.encode('utf-8'))
                        print(f'Sent: {line.strip()}')
                        count += 1
        except FileNotFoundError:
            print(f"File not found: {file_path}")
    else:
        ser.write(command.encode('utf-8'))
ser.close()  # Close the serial connection when done
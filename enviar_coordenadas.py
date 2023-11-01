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
    ser.write(command.encode('utf-8'))
    if command == 'q':
        ser.close()  # Close the serial connection when done
        break
    if command == 'file':
        try:
            with open(file_path, 'r') as file:
                while True:
                    for line in file:
                        # Send each line over the serial connection
                        ser.write(line.encode('utf-8'))
                        print(f'Sent: {line.strip()}')
                        a = line.split()
                        b = [0,0]
                        b[0] = float(a[1].replace('X',''))
                        b[0] = float(a[2].replace('Y',''))
                        A = b[0] + b[1]
                        B = b[0] - b[1]
                        stepper_max = max(abs(A),abs(B))
                        t_constant = 0.025
                        time.sleep(t_constant*stepper_max)
            
                        try:
                            respuesta = ser.readline().decode('ASCII').rstrip()
                        except:
                            time.sleep(0.5)
                            respuesta = ser.readline().decode('ASCII').rstrip()
                        print(respuesta)
            
                        if respuesta == "llego":
                            print("fin del trayecto")
                            break  # Exit the for loop if respuesta is "Recibido"
                    else:
                        print("for out")
                        break
                        
                    
                        # if (keyboard.is_pressed("q")|str(ser.readline()).replace("b'","").replace("\r\n'","")=='llego'):
                        #     print("q pressed, ending loop")
                        #     break
                        # else:
                        #  	print(str(ser.readline()).replace("b'","").replace("Recibido

        except FileNotFoundError:
            print(f"File not found: {file_path}")

ser.close()  # Close the serial connection when done
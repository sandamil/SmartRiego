#!/usr/bin/env python3
import serial, time, sys

puerto = '/dev/ttyACM0'
baud = 9600

try:
    arduino = serial.Serial(puerto, baud, timeout=1)
    time.sleep(2)
except serial.SerialException:
    print(f"No se pudo abrir {puerto}")
    sys.exit(1)

def enviar(cmd):
    arduino.write((cmd + '\n').encode())
    print(f"Enviado: {cmd}")

if len(sys.argv) > 1:
    enviar(sys.argv[1])
else:
    print("Uso: control_riego.py R1ON|R1OFF|R2ON|R2OFF")
#!/usr/bin/env python3
from flask import Flask, redirect
import serial, time

app = Flask(__name__)
arduino = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
time.sleep(2)

def enviar(cmd):
    arduino.write((cmd + '\n').encode())

@app.route('/')
def index():
    return '''<h1>SmartRiego</h1>
    <a href="/R1ON">Válvula 1 ON</a><br>
    <a href="/R1OFF">Válvula 1 OFF</a><br>
    <a href="/R2ON">Válvula 2 ON</a><br>
    <a href="/R2OFF">Válvula 2 OFF</a><br>
    '''

@app.route('/<cmd>')
def cmd(cmd):
    enviar(cmd)
    return redirect('/')

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8080)
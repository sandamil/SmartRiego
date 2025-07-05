from flask import Flask, request, jsonify, send_from_directory
import serial
import time
import logging
import os
import threading
import requests

app = Flask(__name__)

# Logging
logging.basicConfig(filename='logs/smartriego.log',
                    level=logging.INFO,
                    format='%(asctime)s %(levelname)s: %(message)s')

TELEGRAM_TOKEN = os.environ.get('TELEGRAM_TOKEN')
TELEGRAM_CHAT_ID = os.environ.get('TELEGRAM_CHAT_ID')

arduino = serial.Serial('/dev/ttyACM0', 9600, timeout=2)
time.sleep(2)

def enviar_telegram(mensaje):
    if not TELEGRAM_TOKEN or not TELEGRAM_CHAT_ID:
        logging.error("Variables de entorno no definidas")
        return
    try:
        requests.post(
            f"https://api.telegram.org/bot{TELEGRAM_TOKEN}/sendMessage",
            json={'chat_id': TELEGRAM_CHAT_ID, 'text': mensaje},
            timeout=5
        )
    except Exception as e:
        logging.error(f"Error enviando mensaje Telegram: {e}")

def enviar_comando_arduino(comando):
    try:
        arduino.write((comando + '\n').encode())
        respuesta = arduino.readline().decode().strip()
        logging.info(f"Arduino respondió: {respuesta}")
        return respuesta
    except Exception as e:
        logging.error(f"Error en comunicación serial: {e}")
        enviar_telegram(f"❌ Error en serial: {e}")
        return None

@app.route('/')
def interfaz_web():
    return send_from_directory('templates', 'index.html')

@app.route('/riego/<int:zona>/<estado>', methods=['GET'])
def controlar_valvula_get(zona, estado):
    tiempo = request.args.get('tiempo')
    comando = f"{zona}:{estado}"
    if tiempo:
        comando += f":{tiempo}"
    respuesta = enviar_comando_arduino(comando)
    return jsonify({'comando': comando, 'respuesta': respuesta})

@app.route('/valvula', methods=['POST'])
def controlar_valvula_post():
    data = request.get_json()
    zona = data.get('zona')
    estado = data.get('estado')
    tiempo = data.get('tiempo')
    if zona not in [1, 2] or estado not in ['on', 'off']:
        return jsonify({'error': 'Parámetros inválidos'}), 400
    comando = f"{zona}:{estado}"
    if tiempo:
        comando += f":{tiempo}"
    respuesta = enviar_comando_arduino(comando)
    return jsonify({'comando': comando, 'respuesta': respuesta})

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)

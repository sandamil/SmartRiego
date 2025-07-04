import serial
import time
import logging
import requests
from flask import Flask, jsonify

# 📌 Configuración
SERIAL_PORT = "/dev/ttyACM0"
BAUDRATE = 9600
TELEGRAM_TOKEN = "7978337371:AAFge993RCRdXBcArZEzXQtuFumGBWiyydc"
TELEGRAM_CHAT_ID = "162303510"  # ← Reemplazá esto con tu chat ID real

# 🛠️ Configurar logging
logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s [%(levelname)s] %(message)s",
    handlers=[
        logging.FileHandler("smartriego.log"),
        logging.StreamHandler()
    ]
)

# ✉️ Envío a Telegram
def enviar_telegram(mensaje):
    try:
        url = f"https://api.telegram.org/bot{TELEGRAM_TOKEN}/sendMessage"
        data = {"chat_id": TELEGRAM_CHAT_ID, "text": mensaje}
        response = requests.post(url, data=data)
        if response.status_code != 200:
            logging.warning(f"⚠️ Error de Telegram: {response.text}")
    except Exception as e:
        logging.error(f"❌ Fallo al enviar a Telegram: {str(e)}")

# 🚀 Crear API Flask
app = Flask(__name__)

# 🌐 Ruta principal de control
@app.route('/riego/<int:zona>/<estado>', methods=['GET'])
@app.route('/riego/<int:zona>/<estado>/<int:tiempo>', methods=['GET'])
def controlar_riego(zona, estado, tiempo=0):
    if zona not in [1, 2]:
        return jsonify({"error": True, "mensaje": "Zona inválida. Solo se permiten 1 y 2"}), 400
    if estado not in ['on', 'off']:
        return jsonify({"error": True, "mensaje": "Estado inválido. Usa 'on' u 'off'"}), 400

    comando = f"{zona}:{estado}"
    if tiempo > 0:
        comando += f":{tiempo}"
    comando += "\n"

    logging.info(f"🔁 Comando recibido: {comando.strip()}")

    try:
        with serial.Serial(SERIAL_PORT, BAUDRATE, timeout=2) as arduino:
            time.sleep(2)  # Esperar al Arduino
            arduino.write(comando.encode())
            logging.info(f"📤 Enviado al Arduino: {comando.strip()}")
            time.sleep(1)
            respuesta = arduino.readline().decode().strip()
            logging.info(f"📥 Respuesta del Arduino: {respuesta}")

            mensaje = f"💧 Zona {zona} → {estado.upper()} {'⏱️ ' + str(tiempo) + 's' if tiempo > 0 else ''} ✅ Respuesta: {respuesta}"
            enviar_telegram(mensaje)

            return jsonify({
                "zona": zona,
                "estado": estado,
                "tiempo": tiempo,
                "respuesta": respuesta
            })

    except Exception as e:
        error_msg = f"❌ Error zona {zona}: {str(e)}"
        logging.error(error_msg)
        enviar_telegram(error_msg)
        return jsonify({
            "error": True,
            "mensaje": str(e)
        }), 500

# 🩺 Ruta de prueba
@app.route('/status', methods=['GET'])
def status():
    try:
        with serial.Serial(SERIAL_PORT, BAUDRATE, timeout=2) as arduino:
            time.sleep(2)
            arduino.write(b'status\n')
            respuesta = arduino.readline().decode().strip()
            return jsonify({"estado": "ok", "arduino": respuesta})
    except Exception as e:
        return jsonify({"estado": "error", "mensaje": str(e)})

# 🏁 Iniciar servidor
if __name__ == '__main__':
    logging.info(f"✅ Conectado exitosamente a {SERIAL_PORT}")
    logging.info("🚀 Iniciando servidor Flask en 0.0.0.0:5000")
    app.run(host='0.0.0.0', port=5000)

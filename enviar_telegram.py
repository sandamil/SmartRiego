import os
import requests
import logging

TELEGRAM_TOKEN = os.environ.get("TELEGRAM_TOKEN")
TELEGRAM_CHAT_ID = os.environ.get("TELEGRAM_CHAT_ID")

def enviar_telegram(mensaje):
    if not TELEGRAM_TOKEN or not TELEGRAM_CHAT_ID:
        logging.error("⚠️ Variables de entorno TELEGRAM_TOKEN o TELEGRAM_CHAT_ID no definidas.")
        return

    url = f"https://api.telegram.org/bot{TELEGRAM_TOKEN}/sendMessage"
    data = {"chat_id": TELEGRAM_CHAT_ID, "text": mensaje}
    
    try:
        response = requests.post(url, json=data)
        if not response.ok:
            logging.error(f"Telegram error: {response.text}")
    except Exception as e:
        logging.exception("Error enviando mensaje a Telegram:")

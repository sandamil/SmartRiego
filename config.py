import os
from dotenv import load_dotenv

load_dotenv("/root/SmartRiego/.env")

TELEGRAM_TOKEN = os.getenv("TELEGRAM_TOKEN")
TELEGRAM_CHAT_ID = os.getenv("TELEGRAM_CHAT_ID")

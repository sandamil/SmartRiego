# 💧 SmartRiego

Sistema de riego inteligente basado en **Arduino Mega** y **Orange Pi Zero**, controlado mediante una API Flask desde Python. Permite controlar hasta dos válvulas con comandos simples y temporizadores opcionales.

---

## 🚀 Características principales

- Control de válvulas desde navegador o curl (`/riego/<zona>/<on|off>[:tiempo]`)
- Comunicación serie Arduino <--> Orange Pi
- API web simple con Flask
- Registro de logs
- Notificaciones por Telegram
- Servicio autoiniciado con `systemd`
- Control manual y temporizado

---

## 🧠 Requisitos

### Hardware
- Orange Pi Zero con Armbian
- Arduino Mega conectado por USB (`/dev/ttyACM0`)
- Válvulas conectadas a pines digitales (7 y 8)
- (Opcional) LED de estado conectado a pin 13

### Software
- Python 3.9+
- `venv`, Flask, pyserial, requests
- Git, systemd, curl

---

## ⚙️ Instalación

```bash
git clone https://github.com/sandamil/SmartRiego.git
cd SmartRiego
python3 -m venv venv
source venv/bin/activate
pip install -r requirements.txt
```

Crea un archivo `.env` con tu token y chat ID de Telegram:

```env
TELEGRAM_TOKEN=tu_token_aqui
TELEGRAM_CHAT_ID=tu_chatid_aqui
```

---

## 🔌 Servicio systemd

Archivo: `smartriego.service`

```ini
[Unit]
Description=SmartRiego API
After=network.target

[Service]
ExecStart=/root/SmartRiego/venv/bin/python /root/SmartRiego/api.py
WorkingDirectory=/root/SmartRiego
StandardOutput=inherit
StandardError=inherit
Restart=always
User=root

[Install]
WantedBy=multi-user.target
```

Habilita el servicio:

```bash
sudo cp smartriego.service /etc/systemd/system/
sudo systemctl daemon-reload
sudo systemctl enable smartriego
sudo systemctl start smartriego
```

---

## 🖥️ API HTTP

```http
GET /riego/1/on           # Enciende válvula 1
GET /riego/2/off          # Apaga válvula 2
GET /riego/1/on/30        # Enciende válvula 1 durante 30 segundos
GET /status               # Estado general y comunicación con Arduino
```

---

## 🧪 Ejemplo de uso

```bash
curl http://192.168.1.159:5000/riego/1/on
curl http://192.168.1.159:5000/riego/2/on/45
curl http://192.168.1.159:5000/status
```

---

## 📟 Arduino (Sketch)

Controla dos válvulas (pines 7 y 8) y un LED de estado (pin 13):

- Comandos esperados: `1:on`, `2:off:30`, etc.
- LED indica actividad, errores o heartbeat
- Temporizador por válvula con `millis()`
- Protocolo serie simple (`zona:estado[:tiempo]`)

---

## 📁 Estructura del proyecto

```plaintext
SmartRiego/
├── api.py               # Servidor Flask principal
├── riego.ino            # Sketch de Arduino
├── smartriego.log       # Archivo de logs
├── smartriego.service   # Servicio systemd
├── requirements.txt     # Dependencias Python
├── .env.example         # Variables de entorno (plantilla)
└── README.md            # Este documento
```

---

## ✅ Pendientes / Ideas futuras

- Interfaz web para usuarios
- Soporte para más zonas
- Control por humedad o clima
- Base de datos de riegos históricos
- Seguridad con autenticación

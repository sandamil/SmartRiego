#!/bin/bash
set -e

echo "Actualizando e instalando dependencias..."
sudo apt update
sudo apt install -y python3 python3-pip git

echo "Instalando dependencias Python..."
pip3 install -r requirements.txt

echo "Añadiendo usuario actual al grupo dialout para acceso a puerto serial..."
sudo usermod -a -G dialout $USER

echo "Copiando servicio systemd..."
sudo cp SmartRiego.service /etc/systemd/system/
sudo systemctl daemon-reload
sudo systemctl enable SmartRiego
sudo systemctl start SmartRiego

echo "Instalación completada. Accede a http://<IP-de-la-Pi>:8080"
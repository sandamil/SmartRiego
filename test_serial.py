import serial
import time

SERIAL_PORT = "/dev/ttyACM0"
BAUD_RATE = 9600

def main():
    try:
        with serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=2) as ser:
            print(f"✅ Conectado a {SERIAL_PORT}")
            time.sleep(2)  # Esperar a que el Arduino reinicie

            comando = "1:on\n"
            ser.write(comando.encode())
            print(f"📤 Enviado: {comando.strip()}")

            # Esperar un momento para recibir la respuesta
            time.sleep(1)
            respuesta = ser.readline().decode(errors='ignore').strip()
            if respuesta:
                print(f"📥 Respuesta del Arduino: {respuesta}")
            else:
                print("⚠️ No se recibió respuesta del Arduino.")

    except Exception as e:
        print(f"❌ Error: {e}")

if __name__ == "__main__":
    main()

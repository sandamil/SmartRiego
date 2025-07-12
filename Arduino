// SmartRiego - Sketch para 10 válvulas con relés activos en bajo (LOW)

const int ZONAS = 10;
const int LED = 13;

const int pinesZonas[ZONAS] = {7, 8, 2, 3, 4, 5, 6, 9, 10, 11}; // Puedes modificar si lo necesitas
bool zonasActivas[ZONAS] = {false};
unsigned long tiemposApagado[ZONAS] = {0};

void setup() {
  Serial.begin(9600);
  while (!Serial);

  for (int i = 0; i < ZONAS; i++) {
    pinMode(pinesZonas[i], OUTPUT);
    digitalWrite(pinesZonas[i], HIGH);  // OFF inicial (activo en bajo)
  }

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  Serial.println("OK");
}

void loop() {
  unsigned long ahora = millis();

  // ⏱️ Comprobar temporizadores
  for (int i = 0; i < ZONAS; i++) {
    if (zonasActivas[i] && tiemposApagado[i] > 0 && ahora >= tiemposApagado[i]) {
      digitalWrite(pinesZonas[i], HIGH);  // OFF (relé activo en LOW)
      zonasActivas[i] = false;
      tiemposApagado[i] = 0;
      Serial.print("⏱️ Apagando zona ");
      Serial.println(i + 1);
    }
  }

  // 💡 LED de estado
  bool algunaActiva = false;
  for (int i = 0; i < ZONAS; i++) {
    if (zonasActivas[i]) {
      algunaActiva = true;
      break;
    }
  }
  digitalWrite(LED, algunaActiva ? HIGH : millis() / 1000 % 2);

  // 📥 Comando por Serial
  if (Serial.available()) {
    String comando = Serial.readStringUntil('\n');
    comando.trim();

    if (comando == "status") {
      for (int i = 0; i < ZONAS; i++) {
        Serial.print("Z");
        Serial.print(i + 1);
        Serial.print(":");
        Serial.print(zonasActivas[i] ? "ON " : "OFF ");
      }
      Serial.println();
      return;
    }

    int pos1 = comando.indexOf(':');
    int pos2 = comando.indexOf(':', pos1 + 1);

    int zona = comando.substring(0, pos1).toInt();
    String estado = comando.substring(pos1 + 1, pos2 == -1 ? comando.length() : pos2);
    int tiempo = (pos2 != -1) ? comando.substring(pos2 + 1).toInt() : 0;

    bool exito = false;

    if (zona >= 1 && zona <= ZONAS) {
      int idx = zona - 1;
      if (estado == "on") {
        digitalWrite(pinesZonas[idx], LOW);  // ON
        zonasActivas[idx] = true;
        if (tiempo > 0) tiemposApagado[idx] = ahora + (unsigned long) tiempo * 1000UL;
        exito = true;
      } else if (estado == "off") {
        digitalWrite(pinesZonas[idx], HIGH); // OFF
        zonasActivas[idx] = false;
        tiemposApagado[idx] = 0;
        exito = true;
      }
    }

    if (exito) {
      Serial.println("OK");
      parpadeoRapido(LED, 2, 80);
    } else {
      Serial.println("ERROR");
      parpadeoRapido(LED, 4, 80);
    }
  }
}

// 💡 Parpadeo rápido del LED
void parpadeoRapido(int pin, int veces, int duracion) {
  for (int i = 0; i < veces; i++) {
    digitalWrite(pin, HIGH);
    delay(duracion);
    digitalWrite(pin, LOW);
    delay(duracion);
  }
}

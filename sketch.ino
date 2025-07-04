// SmartRiego - Sketch para 2 válvulas con relés activos en bajo y LED de estado

const int VALVULA_1 = 7;
const int VALVULA_2 = 8;
const int LED = 13;

// Tiempos de apagado automático
unsigned long tiempoApagado1 = 0;
unsigned long tiempoApagado2 = 0;

// Estado actual de válvulas
bool valvula1Activa = false;
bool valvula2Activa = false;

void setup() {
  pinMode(VALVULA_1, OUTPUT);
  pinMode(VALVULA_2, OUTPUT);
  pinMode(LED, OUTPUT);

  // Apagar válvulas (relés activos en bajo → HIGH = OFF)
  digitalWrite(VALVULA_1, HIGH);
  digitalWrite(VALVULA_2, HIGH);
  digitalWrite(LED, LOW);

  Serial.begin(9600);
  while (!Serial);
  Serial.println("OK");
}

void loop() {
  unsigned long ahora = millis();

  // Apagar válvulas si expira temporizador
  if (valvula1Activa && tiempoApagado1 > 0 && ahora >= tiempoApagado1) {
    digitalWrite(VALVULA_1, HIGH);  // OFF
    valvula1Activa = false;
    tiempoApagado1 = 0;
  }

  if (valvula2Activa && tiempoApagado2 > 0 && ahora >= tiempoApagado2) {
    digitalWrite(VALVULA_2, HIGH);  // OFF
    valvula2Activa = false;
    tiempoApagado2 = 0;
  }

  // LED: encendido si alguna válvula activa, sino parpadeo heartbeat
  if (valvula1Activa || valvula2Activa) {
    digitalWrite(LED, HIGH);
  } else {
    digitalWrite(LED, millis() / 1000 % 2);
  }

  // Leer comando por Serial
  if (Serial.available()) {
    String comando = Serial.readStringUntil('\n');
    comando.trim();

    if (comando == "status") {
      Serial.print("Z1:");
      Serial.print(valvula1Activa ? "ON" : "OFF");
      Serial.print(" Z2:");
      Serial.println(valvula2Activa ? "ON" : "OFF");
      return;
    }

    // Formato: zona:estado[:tiempo]
    int pos1 = comando.indexOf(':');
    int pos2 = comando.indexOf(':', pos1 + 1);

    int zona = comando.substring(0, pos1).toInt();
    String estado = comando.substring(pos1 + 1, pos2 == -1 ? comando.length() : pos2);
    int tiempo = (pos2 != -1) ? comando.substring(pos2 + 1).toInt() : 0;

    bool exito = false;

    if (zona == 1) {
      if (estado == "on") {
        digitalWrite(VALVULA_1, LOW);  // ON
        valvula1Activa = true;
        if (tiempo > 0) tiempoApagado1 = ahora + tiempo * 1000UL;
        exito = true;
      } else if (estado == "off") {
        digitalWrite(VALVULA_1, HIGH);  // OFF
        valvula1Activa = false;
        tiempoApagado1 = 0;
        exito = true;
      }
    } else if (zona == 2) {
      if (estado == "on") {
        digitalWrite(VALVULA_2, LOW);  // ON
        valvula2Activa = true;
        if (tiempo > 0) tiempoApagado2 = ahora + tiempo * 1000UL;
        exito = true;
      } else if (estado == "off") {
        digitalWrite(VALVULA_2, HIGH);  // OFF
        valvula2Activa = false;
        tiempoApagado2 = 0;
        exito = true;
      }
    }

    // LED feedback rápido por actividad
    if (exito) {
      Serial.println("OK");
      parpadeoRapido(LED, 2, 80);  // Confirmación visual
    } else {
      Serial.println("ERROR");
      parpadeoRapido(LED, 4, 80);  // Error visual
    }
  }
}

// 💡 Parpadeo rápido del LED (actividad o error)
void parpadeoRapido(int pin, int veces, int duracion) {
  for (int i = 0; i < veces; i++) {
    digitalWrite(pin, HIGH);
    delay(duracion);
    digitalWrite(pin, LOW);
    delay(duracion);
  }
}

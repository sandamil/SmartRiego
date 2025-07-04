// SmartRiego - Sketch para 2 válvulas con relés activos en bajo y LED de estado

const int VALVULA_1 = 7;
const int VALVULA_2 = 8;
const int LED = 13;

unsigned long tiempoApagado1 = 0;
unsigned long tiempoApagado2 = 0;

bool valvula1Activa = false;
bool valvula2Activa = false;

void setup() {
  pinMode(VALVULA_1, OUTPUT);
  pinMode(VALVULA_2, OUTPUT);
  pinMode(LED, OUTPUT);

  digitalWrite(VALVULA_1, HIGH); // Relé OFF (activo en bajo)
  digitalWrite(VALVULA_2, HIGH); // Relé OFF
  digitalWrite(LED, LOW);

  Serial.begin(9600);
  while (!Serial);
  Serial.println("OK");
}

void loop() {
  unsigned long ahora = millis();

  // ⏱️ Apagado automático por temporizador
  if (valvula1Activa && tiempoApagado1 > 0 && ahora >= tiempoApagado1) {
    Serial.print("⏱️ Apagando válvula 1. ahora=");
    Serial.print(ahora);
    Serial.print(" tiempoApagado1=");
    Serial.println(tiempoApagado1);

    digitalWrite(VALVULA_1, HIGH);  // OFF
    valvula1Activa = false;
    tiempoApagado1 = 0;
  }

  if (valvula2Activa && tiempoApagado2 > 0 && ahora >= tiempoApagado2) {
    Serial.print("⏱️ Apagando válvula 2. ahora=");
    Serial.print(ahora);
    Serial.print(" tiempoApagado2=");
    Serial.println(tiempoApagado2);

    digitalWrite(VALVULA_2, HIGH);  // OFF
    valvula2Activa = false;
    tiempoApagado2 = 0;
  }

  // 🔆 LED de estado
  if (valvula1Activa || valvula2Activa) {
    digitalWrite(LED, HIGH);
  } else {
    digitalWrite(LED, millis() / 1000 % 2);  // Heartbeat
  }

  // 📥 Leer comandos por Serial
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

    int pos1 = comando.indexOf(':');
    int pos2 = comando.indexOf(':', pos1 + 1);

    int zona = comando.substring(0, pos1).toInt();
    String estado = comando.substring(pos1 + 1, pos2 == -1 ? comando.length() : pos2);
    int tiempo = (pos2 != -1) ? comando.substring(pos2 + 1).toInt() : 0;

    bool exito = false;

    // ✅ ZONA 1
    if (zona == 1) {
      if (estado == "on") {
        digitalWrite(VALVULA_1, LOW);  // ON
        valvula1Activa = true;
        if (tiempo > 0) tiempoApagado1 = millis() + (unsigned long) tiempo * 1000UL;
        exito = true;
      } else if (estado == "off") {
        digitalWrite(VALVULA_1, HIGH); // OFF
        valvula1Activa = false;
        tiempoApagado1 = 0;
        exito = true;
      }
    }

    // ✅ ZONA 2
    else if (zona == 2) {
      if (estado == "on") {
        digitalWrite(VALVULA_2, LOW);  // ON
        valvula2Activa = true;
        if (tiempo > 0) tiempoApagado2 = millis() + (unsigned long) tiempo * 1000UL;
        exito = true;
      } else if (estado == "off") {
        digitalWrite(VALVULA_2, HIGH); // OFF
        valvula2Activa = false;
        tiempoApagado2 = 0;
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

// 💡 Parpadeo rápido del LED (actividad o error)
void parpadeoRapido(int pin, int veces, int duracion) {
  for (int i = 0; i < veces; i++) {
    digitalWrite(pin, HIGH);
    delay(duracion);
    digitalWrite(pin, LOW);
    delay(duracion);
  }
}

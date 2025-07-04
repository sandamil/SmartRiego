// Pines de las válvulas
const int VALVULA_1 = 7;
const int VALVULA_2 = 8;

// LED de estado
const int LED = 13;

// Temporizadores de apagado
unsigned long tiempoApagado1 = 0;
unsigned long tiempoApagado2 = 0;

// Heartbeat
unsigned long ultimoHeartbeat = 0;
bool estadoHeartbeat = false;

void setup() {
  pinMode(VALVULA_1, OUTPUT);
  pinMode(VALVULA_2, OUTPUT);
  pinMode(LED, OUTPUT);

  digitalWrite(VALVULA_1, LOW);
  digitalWrite(VALVULA_2, LOW);
  digitalWrite(LED, LOW);

  Serial.begin(9600);
  while (!Serial);
  Serial.println("OK");
}

void loop() {
  unsigned long ahora = millis();

  // Apagado automático
  if (tiempoApagado1 > 0 && ahora >= tiempoApagado1) {
    digitalWrite(VALVULA_1, LOW);
    tiempoApagado1 = 0;
  }
  if (tiempoApagado2 > 0 && ahora >= tiempoApagado2) {
    digitalWrite(VALVULA_2, LOW);
    tiempoApagado2 = 0;
  }

  // LED ON si alguna válvula está activa
  if (digitalRead(VALVULA_1) || digitalRead(VALVULA_2)) {
    digitalWrite(LED, HIGH);
  } else {
    // Heartbeat cada 1s
    if (ahora - ultimoHeartbeat > 1000) {
      ultimoHeartbeat = ahora;
      estadoHeartbeat = !estadoHeartbeat;
      digitalWrite(LED, estadoHeartbeat);
    }
  }

  // Leer comando
  if (Serial.available()) {
    String comando = Serial.readStringUntil('\n');
    comando.trim();

    int zona = comando.substring(0, comando.indexOf(':')).toInt();
    int pos1 = comando.indexOf(':');
    int pos2 = comando.indexOf(':', pos1 + 1);
    String estado = comando.substring(pos1 + 1, pos2 == -1 ? comando.length() : pos2);
    int tiempo = (pos2 != -1) ? comando.substring(pos2 + 1).toInt() : 0;

    bool exito = false;

    if (zona == 1) {
      if (estado == "on") {
        digitalWrite(VALVULA_1, HIGH);
        if (tiempo > 0) tiempoApagado1 = millis() + tiempo * 1000UL;
        exito = true;
      } else if (estado == "off") {
        digitalWrite(VALVULA_1, LOW);
        tiempoApagado1 = 0;
        exito = true;
      }
    } else if (zona == 2) {
      if (estado == "on") {
        digitalWrite(VALVULA_2, HIGH);
        if (tiempo > 0) tiempoApagado2 = millis() + tiempo * 1000UL;
        exito = true;
      } else if (estado == "off") {
        digitalWrite(VALVULA_2, LOW);
        tiempoApagado2 = 0;
        exito = true;
      }
    }

    // LED parpadeo corto (OK) o rápido (ERROR)
    if (exito) {
      blinkLED(1, 100);  // una vez, 100ms
      Serial.println("OK");
    } else {
      blinkLED(3, 100);  // tres veces, 100ms
      Serial.println("ERROR");
    }
  }
}

void blinkLED(int veces, int duracion) {
  for (int i = 0; i < veces; i++) {
    digitalWrite(LED, HIGH);
    delay(duracion);
    digitalWrite(LED, LOW);
    delay(duracion);
  }
}

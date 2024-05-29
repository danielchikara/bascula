#include <LiquidCrystal_I2C.h>
#include "HX711.h"

const int buttonPin = 33;
const int tareButtonPin = 34; // Definir un pin para el botón de tara

// Variables para la lógica del botón
int buttonState = 0;
int lastButtonState = 0;
int tareButtonState = 0;
int lastTareButtonState = 0;
unsigned long lastDebounceTime = 0;
unsigned long lastTareDebounceTime = 0;
unsigned long debounceDelay = 50;
int unidadActual = 0;

// Variables de celda
const int LOADCELL_DOUT_PIN = 19;
const int LOADCELL_SCK_PIN = 18;
HX711 balanza;

// Define los pines a los que está conectada la pantalla LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Function declarations
void printLCD(float ValueG);
float convertGKg(float valueG);
float convertGLb(float valueG);
float getValueOrZero(float valueG);
void cambiarUnidad();
void tareBalanza();

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(tareButtonPin, INPUT); // Configurar el pin del botón de tara como entrada
  // Inicializa la pantalla LCD con el número de columnas y filas de tu LCD
  Serial.begin(115200);
  lcd.init();                       // Initialize the LCD
  lcd.backlight();                  // Turn on the backlight
  lcd.clear();
  balanza.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  delay(250);
  balanza.set_scale(426.52);
  balanza.tare(10);  // Hacer 10 lecturas, el promedio es la tara
}

void loop() {
  int reading = digitalRead(buttonPin);
  int tareReading = digitalRead(tareButtonPin);

  if (balanza.is_ready()) {
    // Debounce para el botón de cambio de unidad
    if (reading != lastButtonState) {
      lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
      if (reading != buttonState) {
        buttonState = reading;
        Serial.println(buttonState);

        if (buttonState == 1) {
          cambiarUnidad();
          Serial.println("Unidad cambiada");
        }
      }
    }
    lastButtonState = reading;

    // Debounce para el botón de tara
    if (tareReading != lastTareButtonState) {
      lastTareDebounceTime = millis();
    }

    if ((millis() - lastTareDebounceTime) > debounceDelay) {
      if (tareReading != tareButtonState) {
        tareButtonState = tareReading;
        Serial.println(tareButtonState);

        if (tareButtonState == 1) {
          tareBalanza();
          Serial.println("Tara realizada");
        }
      }
    }
    lastTareButtonState = tareReading;

    float getValueG = balanza.get_units(10);
    Serial.println(getValueG);
    lcd.clear();
    float realValue = getValueOrZero(getValueG);
    printLCD(realValue);
  } else {
    Serial.println("HX711 not found.");
  }
}

void printLCD(float ValueG){
  lcd.clear();
  if (unidadActual == 1){
    float valueData = convertGLb(ValueG);
    String value = String(valueData,3)+" :lb";
    lcd.setCursor(4,0);
    lcd.print(value);
  } else if (unidadActual == 2){
    float valueData = convertGKg(ValueG);
    String value = String(valueData,3)+" :Kg";
    lcd.setCursor(4,0);
    lcd.print(value);
  } else {
    String value = String(ValueG,1)+" :g";
    lcd.setCursor(4,0);
    lcd.print(value);
  }
  
  lcd.setCursor(3,1);
  lcd.print("Daniel");
  delay(200);
}

float convertGKg(float valueG){
  if (valueG == 0) {
    return 0;
  } else {
    return valueG/1000;
  }
}

float convertGLb(float valueG){
  return valueG * 0.0022046;
}
// function para eliminar errores del cero
float getValueOrZero(float valueG){
  if (valueG > -1 && valueG < 0.99){
    return 0;
  } else {
    return valueG;
  }
}
//funcion que cambia la unidad de gramos, libras,  kilos
void cambiarUnidad() {
  if (unidadActual == 0) {
    unidadActual = 1;
  } else if (unidadActual == 1) {
    unidadActual = 2;
  } else {
    unidadActual = 0;
  }
}

//funcion que realiza tare en la bascula
void tareBalanza() {
  lcd.setCursor(0,0);
  lcd.print("Realizando tara ....");
  Serial.println("Realizando tara ....");
  balanza.tare(100);  // Realizar 10 lecturas y promediar para tara
}

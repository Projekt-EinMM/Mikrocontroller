#include <Arduino.h>

/**
 *
 *
 *
 * Variablendefinition
 *
 *
 *
 **/

int sensorAnalogBigSound;
int sensorDigitalBigSound;
float valueAnalog;
int valueDigital;

#include "arduinoFFT.h"

#define SAMPLES 128             // SAMPLES-pt FFT - muss eine Zahl zur Basis 2 sein, maximal 128 für Arduino Uno
#define SAMPLING_FREQUENCY 2048 // Ts = basierend auf Nyquist, muss das zweifache der höchsten erwarteten Frequenz sein

arduinoFFT FFT = arduinoFFT();

unsigned int samplingPeriod;
unsigned long microSeconds;

double vReal[SAMPLES]; // Erzeugen eines Vektors der Größe SAMPLES zur Aufnahme realer Werte
double vImag[SAMPLES]; // Erstellung eines Vektors der Größe SAMPLES zur Aufnahme imaginärer Werte

/**
 *
 *
 *
 * SETUP
 *
 *
 *
 **/

void setup()
{
  Serial.begin(9600);
  // Serial.begin(115200); // Baudrate für den seriellen Monitor
  samplingPeriod = round(1000000 * (1.0 / SAMPLING_FREQUENCY)); // Zeitraum in Mikrosekunden
  pinMode(LED_BUILTIN, OUTPUT);
  // Wait for the serial port to connect.
  while (!Serial)
  {
  }
}

/**
 *
 *
 *
 * LOOP
 *
 *
 *
 **/
void testing()
{
  // Check if there is data available in the read buffer
  if (Serial.available() > 0)
  {
    // Got something
    int read = Serial.read();
    if (read >= 0)
    {
      // Print out what we read
      Serial.print(read);
    }
  }
}
void audioControll()
{

  /* Probe SAMPLES Zeiten */
  for (int i = 0; i < SAMPLES; i++)
  {
    microSeconds = micros(); // gibt die Anzahl der Mikrosekunden zurück, seit der Arduino mit der Ausführung des aktuellen Skripts begonnen hat

    vReal[i] = analogRead(0); // liest den Wert von Analog-Pin 0 (A0), quantisiert den und speichert den als realen Term
    vImag[i] = 0;             // macht den imaginären Term immer zu 0

    /* ggf. verbleibende Wartezeit zwischen den Proben */
    while (micros() < (microSeconds + samplingPeriod))
    {
      // tue nichts
    }
  }

  /* FFT für Stichproben durchführen */
  FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
  FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);

  /* Spitzenfrequenz ermitteln und Spitzenwert ausgeben */
  double peak = FFT.MajorPeak(vReal, SAMPLES, SAMPLING_FREQUENCY);
  if (peak < 5)
  {
    Serial.write(0);
    Serial.flush();
    // Serial.println("0");
  }
  else if (peak < 135 && peak > 5)
  {
    Serial.write(1);
    Serial.flush();
    // Serial.print("1 ");
    // Serial.println(peak);
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else
  {
    Serial.write(2);
    Serial.flush();
    // Serial.print("2 ");
    // Serial.println(peak);
    digitalWrite(LED_BUILTIN, LOW);
  }
  // Serial.write("Mikrocontroller funktioniert");

  /* Skript stoppt hier, Hardware-Reset erforderlich */
  // while (1); // einmalig tun
  Serial.flush();
  // delay(15);
}

void loop()
{
  //testing();
  audioControll();
}
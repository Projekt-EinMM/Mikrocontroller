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

#define SAMPLES 128             // SAMPLES-pt FFT. Must be a base 2 number. Max 128 for Arduino Uno.
#define SAMPLING_FREQUENCY 2048 // Ts = Based on Nyquist, must be 2 times the highest expected frequency.

arduinoFFT FFT = arduinoFFT();

unsigned int samplingPeriod;
unsigned long microSeconds;

double vReal[SAMPLES]; // create vector of size SAMPLES to hold real values
double vImag[SAMPLES]; // create vector of size SAMPLES to hold imaginary values

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

void loop()
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

  if (peak < 250 && peak > 180)
  {
    Serial.print("ERSTE:\t\t");
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else if (peak < 300 && peak >= 250)
  {
    Serial.print("ZWEITE:\t\t");
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else if (peak < 500 && peak >= 300)
  {
    Serial.print("DRITTE:\t\t");
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else if (peak < 1000 && peak >= 500)
  {
    Serial.print("DRITTE:\t\t");
    digitalWrite(LED_BUILTIN, HIGH);
  }
  else
  {
    if (!(peak >= 2 && peak <= 3))
    {
      Serial.print("NICHTS:\t\t");
    }
    digitalWrite(LED_BUILTIN, LOW);
  }

  if (!(peak >= 2 && peak <= 3))
  {
    Serial.println(peak); // gibt die dominanteste Frequenz aus
  }

  /* Skript stoppt hier, Hardware-Reset erforderlich */
  // while (1); // einmalig tun

  delay(1000);
}
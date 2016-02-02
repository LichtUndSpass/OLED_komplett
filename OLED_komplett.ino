#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <FreqMeasure.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>
#include <SPI.h>

#define sclk 13
#define miso 12
#define mosi 11
#define dc   7
#define oledcs   10
#define rst  9

//Farben
#define	BLACK           0b0000000000000000
#define WHITE           0b1111111111111111
#define GRAY1          0b11000110000110000
#define GRAY2          0b11100111000111000
#define GRAY3          0b11110111100111100
#define GRAY4          0b11011111001110111

#define	BLUE            0b0000000000011111
#define	RED             0b1111100000000000
#define YELLOW          0b1111111111100000
#define ORANGE          0b1111110001000000

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
Adafruit_SSD1351 tft = Adafruit_SSD1351(oledcs, dc, rst);


void setup()
{
  mlx.begin(); 
  tft.begin();
  tft.setRotation(2);//Dreht Bild um 180 Grad
  tft.fillScreen(BLACK);
}

int counter = 0;
float maxFreq = 40;
float lastFreq = 0;
float currentFreq = 0;
int colorFreq = BLACK;

float maxTemp = 40;
float lastTemp = 0;
float currentTemp = 0;
int colorTemp = BLACK;


void loop()
{
  
  currentTemp =   mlx.readObjectTempC();

  if (currentTemp > maxTemp)
  {
    maxTemp = currentTemp;
  }

  if (currentTemp > 30)
  {
    colorTemp = RED;
  }
  else if (currentTemp > 20)
  {
    colorTemp = ORANGE;
  }
  else if (currentTemp > 15)
  {
    colorTemp = YELLOW;
  }
  else
  {
    colorTemp = BLUE;
  }
  
  if (abs(currentTemp - lastTemp) > 0.15)//Hysterese - vermeidet zu starkes Zappeln
  {
    lastTemp = currentTemp;

    //alten Text löschen
    if (currentTemp > 100)
    {
      tft.fillRect(0, 0, 64, 8, BLACK);
    }
    else
    {
      tft.fillRect(0, 0, 32, 8, BLACK);
    }

    //Text in die Mitte der Bildschirms setzen
    tft.setCursor(0, 0);
    tft.setTextColor(colorTemp);    
    tft.print(currentTemp);
  }
  else
  {
    currentTemp = lastTemp;
  }
  
  float f = currentTemp / maxTemp;
  tft.fillRect(2 * counter, 128 - f * 118, 2, f * 118,  colorTemp); //x,y,w,h,farbe



  
  currentFreq = read_TSL237_Hz();

  if (currentFreq > maxFreq)
  {
    maxFreq = currentFreq;
  }

  if (currentFreq > 100000)
  {
    colorFreq=GRAY4;
  }
  else if (currentFreq > 50000)
  {
        colorFreq=GRAY3;
  }
  else if (currentFreq > 10000)
  {
        colorFreq=GRAY2;
  }
  else
  {
       colorFreq=GRAY1; 
  }

  if (abs(currentFreq - lastFreq) > 1000)//Hysterese - vermeidet zu starkes Zappeln
  {
    lastFreq = currentFreq;

    //alten Text löschen
    tft.fillRect(64, 0, 64, 8, BLACK);

    tft.setCursor(64, 0);
    tft.setTextColor(colorFreq);    
    tft.print(currentFreq);  
  }
  else
  {
    currentFreq = lastFreq;
  }
  
  f = currentFreq / maxFreq;
  tft.fillRect(128 - 2 * counter -1, 128 - f * 118, 2, f * 118,  colorFreq); //x,y,w,h,farbe



  delay(50);

  counter++;
  if (counter > 31)
  {
    counter = 0;
//    maxValue = currentFreq;
    //Graph löschen
    tft.fillRect(0, 10, 128, 118,  BLACK);//x,y,w,farbe
  }
}


float read_TSL237_Hz()
{
  double sum = 0; //Summe von 30 Messwerten
  double frequenz = 0.0f;//gemessene Frequenz des Sensors
  int count = 0; //wird unten in der for Schleife verwendet

  FreqMeasure.begin();////Frequenz-Messung beenden
  for (count = 0; count < 30; count++) //30 Messwerte werden erfasst
  {
    while (!FreqMeasure.available())
    {
      //auf neuen Messwert warten
    }
    // Einzelmessungen summieren
    sum = sum + FreqMeasure.read();
  }
  FreqMeasure.end();//Frequenz-Messung beenden
  frequenz = F_CPU / (sum / count);//F_CPU ist die Taktfrequenz des Arduinos

  return frequenz;
}

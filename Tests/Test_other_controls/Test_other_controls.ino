/*
  Very simple program to check and tune the home screen
*/

// Resources
#include <Inkplate.h>
#include "controls.h"
#include "Fonts.h"

Inkplate display(INKPLATE_1BIT);

void setup()
{
  // Initializing
  Serial.begin(115200);
  display.begin();
  display.clearDisplay(); // Clear Buffer

  // Text
  display.setTextColor(BLACK);
  display.setFont(&Roboto_Light_48);
  display.setCursor(340, 350);
//  display.print(F("36 W"));
//  display.drawCircle(400, 300, 100, BLACK);
//  fillArc2(400, 300, -30, 210, 120, 120, 5, BLACK);

  Gauge gauge1 (400, 300, 0, 100, 120, BLACK);
  gauge1.set_value(10);
  const char prefix[8] = "";
  const char suffix[8] = " W";
  const char subtitle[24] = "Input";
  gauge1.set_text(prefix, suffix, subtitle, &Roboto_Light_48, &Roboto_Light_36, 0);
  gauge1.draw(true);

  //fillRect(250, 150, 300, 300, WHITE);
  //fillCircle(x, y, radius+radius_marker, color);

  display.display();

  for (int i = 2; i <= 9; i++) {
    delay(1000);
    gauge1.set_value(10*i);
    gauge1.draw(true);
    display.partialUpdate();
  }
  
}


void loop()
{

}

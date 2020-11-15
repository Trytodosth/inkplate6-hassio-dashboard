/*
  Very simple program to check and tune the home screen
*/

// Resources
#include "Inkplate.h"
#include "Binary_images/Hassio_logo_200px.h"
#include "Fonts/Roboto_Light_36.h"
#include "Fonts/Roboto_Light_48.h"

Inkplate display(INKPLATE_1BIT);

void setup()
{
  // Initializing
  Serial.begin(115200);
  display.begin();
  display.clearDisplay(); // Clear Buffer

  // 1bit mode: display.drawBitmap(pos_x, pos_y, progmem_const, width, height, color = BLACK | WHITE, color_bg = WHITE | BLACK);
  display.drawBitmap(50, 212, Hassio_logo_200px, 200, 176, BLACK);
  display.display();
  delay(1000);

  // Text
  display.setTextColor(BLACK);
  display.setFont(&Roboto_Light_48);
  display.setCursor(270, 260);
  display.print(F("Home Assistant Hub"));
  display.partialUpdate();
  delay(1000);
  
  display.setFont(&Roboto_Light_36);
  display.setCursor(270, 310);
  display.print(F("> Powered by an Inkplate 6"));
  display.partialUpdate();
  delay(1000);
  display.setCursor(270, 360);
  display.print(F("> Created by Trytodosth"));
  display.partialUpdate();
  delay(1000);
 
}


void loop()
{

}

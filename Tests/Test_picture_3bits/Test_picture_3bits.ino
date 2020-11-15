/*
  Very simple program to check how your picture will look like
  3bit_version
*/

// Your header file
#include "icon_Hassio_logo_200px.h" // <--- EDIT

#include "Inkplate.h"
Inkplate display(INKPLATE_3BIT);

void setup()
{
  // Initializing
  Serial.begin(115200);
  display.begin();
  display.clearDisplay(); // Clear Buffer

  // 1bit mode: display.drawBitmap3Bit(pos_x, pos_y, progmem_const, width, height);
  display.drawBitmap3Bit(0, 0, icon_Hassio_logo_200px, 200, 176); // <--- EDIT

  display.display();
}


void loop()
{

}

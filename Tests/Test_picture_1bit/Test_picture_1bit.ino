/*
  Very simple program to check how your picture will look like
  1bit_version
*/

// Your header file
#include "Binary_images/Low_battery.h" // <--- EDIT

#include "Inkplate.h"
Inkplate display(INKPLATE_1BIT);

void setup()
{
  // Initializing
  Serial.begin(115200);
  display.begin();
  display.clearDisplay(); // Clear Buffer

  // 1bit mode: display.drawBitmap(pos_x, pos_y, progmem_const, width, height, color = BLACK | WHITE, color_bg = WHITE | BLACK);
  display.drawBitmap(0, 0, Low_battery, 168, 260, BLACK); // <--- EDIT

  display.display();
}


void loop()
{

}

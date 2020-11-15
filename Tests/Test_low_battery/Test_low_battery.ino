/*
  Very simple program to check and tune the low battery screen
*/

// Resources
#include "Inkplate.h"
#include "Binary_images/Low_battery.h"
#include "Fonts/Roboto_Light_36.h"
#include "Fonts/Roboto_Light_48.h"

Inkplate display(INKPLATE_1BIT);

void setup()
{
  // Initializing
  Serial.begin(115200);
  display.begin();
  display.clearDisplay(); // Clear Buffer

  // 1bit mode: display.drawBitmap(pos_x, pos_y, progmem_const, width, height, color=BLACK | WHITE, color_bg = BLACK | WHITE);
  display.drawBitmap(120, 170, Low_battery, 168, 260, BLACK);

  // Text
  display.setTextColor(BLACK);
  display.setFont(&Roboto_Light_48);
  display.setCursor(330, 300);
  display.print(F("Low battery"));
  display.setFont(&Roboto_Light_36);
  display.setCursor(330, 350);
  display.print(F("Please charge me"));
  display.setCursor(330, 390);
  display.print(F("Press Reset when ready"));
 
  display.display();
}


void loop()
{

}

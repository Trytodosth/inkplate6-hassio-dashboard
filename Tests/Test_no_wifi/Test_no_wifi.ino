/*
  Very simple program to check and tune the low battery screen
*/

// Resources
#include "Inkplate.h"
#include "Binary_images/no_wifi_icon.h"
#include "Fonts/Roboto_Light_36.h"
#include "Fonts/Roboto_Light_48.h"

Inkplate display(INKPLATE_1BIT);


char *WiFi_ssid = "Box_4G";

void setup()
{
  // Initializing
  Serial.begin(115200);
  display.begin();
  display.clearDisplay(); // Clear Buffer

  // 1bit mode: display.drawBitmap(pos_x, pos_y, progmem_const, width, height, color=BLACK | WHITE, color_bg = BLACK | WHITE);
  display.drawBitmap(120, 220, no_wifi_icon, 200, 167, WHITE, BLACK);

  // Text
  display.setTextColor(BLACK);
  display.setFont(&Roboto_Light_48);
  display.setCursor(330, 280);
  display.print(F("WiFi non trouvee"));
  display.setFont(&Roboto_Light_36);
  display.setCursor(330, 330);
  display.print(F("Recherche > "));
  display.print(WiFi_ssid);
  display.setCursor(330, 370);
  display.print(F("Redemarrage dans 5 sec"));
 
  display.display();
}


void loop()
{

}

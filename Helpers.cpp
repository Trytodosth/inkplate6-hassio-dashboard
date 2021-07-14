/*
    This file contains simple function to plot the weather icons.
    Grouped here for project readability.
    If changing from 1bit to 3bit, it can be done from here for all icons (.h files must be updated).
    It also include the hard coded dimensions.
*/

//#include "Arduino.h"
//#include "Inkplate.h"
#include "Helpers.h"
#include "Images.h"


// Contants used for drawing icons
char abbrs[15][24] = {"clear-night", "cloudy", "fog", "hail", "lightning", "lightning-rainy", "partlycloudy",
                      "pouring", "rainy", "snowy", "snowy-rainy", "sunny", "windy", "windy-variant", "exceptional"
                     };

// Logo associated with keywords. You can use several times the same (here windy-variant = windy)
const uint8_t *logos_100px[15] = {
  icon_clear_night_100px, icon_cloudy_100px, icon_fog_100px, icon_hail_100px, icon_lightning_rainy_100px, icon_lightning_100px, icon_partlycloudy_100px,
  icon_pouring_100px, icon_rainy_100px, icon_snowy_100px, icon_snowy_rainy_100px, icon_sunny_100px, icon_windy_100px, icon_windy_100px, icon_exceptional_100px
};
const uint8_t *logos_200px[15] = {
  icon_clear_night_200px, icon_cloudy_200px, icon_fog_200px, icon_hail_200px, icon_lightning_rainy_200px, icon_lightning_200px, icon_partlycloudy_200px,
  icon_pouring_200px, icon_rainy_200px, icon_snowy_200px, icon_snowy_rainy_200px, icon_sunny_200px, icon_windy_200px, icon_windy_200px, icon_exceptional_200px
};


// ---- Weather icons ----

void drawWeatherIcon_small(int pos_x, int pos_y, char *icon_abbr)
{
  int i = 0;
  while (strcmp(abbrs[i], icon_abbr) != 0 | i == 14)
  {
    i++;
  }
  //display.drawBitmap3Bit(pos_x, pos_y, logos_100px[i], 100, 100);
  display.drawBitmap(pos_x, pos_y, logos_100px[i], 100, 100, BLACK, WHITE);
};

void drawWeatherIcon_medium(int pos_x, int pos_y, char *icon_abbr)
{
  int i = 0;
  while (strcmp(abbrs[i], icon_abbr) != 0 | i == 14)
  {
    i++;
  }
  //display.drawBitmap3Bit(pos_x, pos_y, logos_200px[i], 200, 200);
  display.drawBitmap(pos_x, pos_y, logos_200px[i], 200, 200, BLACK, WHITE);
};


// ---- Minor icons ----

void drawWindIcon(int pos_x, int pos_y)
{
  //display.drawBitmap3Bit(pos_x, pos_y, icon_wind_50px, 50, 50);
  display.drawBitmap(pos_x, pos_y, icon_wind_50px, 50, 50, BLACK, WHITE);
}

void drawRaindropIcon(int pos_x, int pos_y)
{
  //display.drawBitmap3Bit(pos_x, pos_y, icon_raindrop_50px, 50, 50);
  display.drawBitmap(pos_x, pos_y, icon_raindrop_50px, 50, 50, BLACK, WHITE);
}

void drawSunriseIcon(int pos_x, int pos_y)
{
  //display.drawBitmap3Bit(pos_x, pos_y, icon_sunrise_50px, 50, 50);
  display.drawBitmap(pos_x, pos_y, icon_sunrise_50px, 50, 41, BLACK, WHITE);
}

void drawSunsetIcon(int pos_x, int pos_y)
{
  //display.drawBitmap3Bit(pos_x, pos_y, icon_sunset_50px, 50, 50);
  display.drawBitmap(pos_x, pos_y, icon_sunset_50px, 50, 41, BLACK, WHITE);
}



// ---- Battery icon ----

void drawBatteryIcon(int charge, bool isCharging, int pos_x, int pos_y) {
  // Serial.print(F("Plotting battery icon: "));
  // Serial.print(charge);
  // Serial.print(F(" ; "));
  // Serial.println(isCharging);
  if (isCharging == true) {
    if (charge >= 95)
      display.drawBitmap(pos_x, pos_y, icon_battery_100c, 35, 20, BLACK, WHITE);
    if (charge < 95 && charge >= 80)
      display.drawBitmap(pos_x, pos_y, icon_battery_80c, 35, 20, BLACK, WHITE);
    if (charge < 80 && charge >= 60)
      display.drawBitmap(pos_x, pos_y, icon_battery_60c, 35, 20, BLACK, WHITE);
    if (charge < 60 && charge >= 40)
      display.drawBitmap(pos_x, pos_y, icon_battery_40c, 35, 20, BLACK, WHITE);
    if (charge < 40 && charge >= 20)
      display.drawBitmap(pos_x, pos_y, icon_battery_20c, 35, 20, BLACK, WHITE);
    if (charge < 20)
      display.drawBitmap(pos_x, pos_y, icon_battery_00c, 35, 20, BLACK, WHITE);
  } else {
    if (charge >= 95)
      display.drawBitmap(pos_x, pos_y, icon_battery_100, 35, 20, BLACK, WHITE);
    if (charge < 95 && charge >= 80)
      display.drawBitmap(pos_x, pos_y, icon_battery_80, 35, 20, BLACK, WHITE);
    if (charge < 80 && charge >= 60)
      display.drawBitmap(pos_x, pos_y, icon_battery_60, 35, 20, BLACK, WHITE);
    if (charge < 60 && charge >= 40)
      display.drawBitmap(pos_x, pos_y, icon_battery_40, 35, 20, BLACK, WHITE);
    if (charge < 40 && charge >= 20)
      display.drawBitmap(pos_x, pos_y, icon_battery_20, 35, 20, BLACK, WHITE);
    if (charge < 20)
      display.drawBitmap(pos_x, pos_y, icon_battery_00, 35, 20, BLACK, WHITE);
  }
}

// ---- Print assist ----

void printCelsiusCircle(int fontsize_px, int color)
{
  // Prints de non supported "°" character based on cursor position and fontsize
  display.drawCircle(display.getCursorX() + fontsize_px * 0.15, display.getCursorY() - fontsize_px * 0.6, fontsize_px * 0.125, color);
  display.setCursor(display.getCursorX() + fontsize_px * 0.3, display.getCursorY());
}

void printCelsiusAtCursor(const GFXfont *f, int fontsize_px, int color)
{
  // Prints the celsius circle and the C
  printCelsiusCircle(fontsize_px, color);
  //printTextColor((char *)F("C"), display.getCursorX(), display.getCursorY(), f, color); // Make sure color is OK but looks unnecessary
  printTextAtCursor((char *)F("C"), f);
}

void printTextAtCursor(char *text, const GFXfont *f)
{
  // Prints text at cursor position, without changing the color
  display.setFont(f);
  display.print(text);
}

void printText(char *text, int pos_x, int pos_y, const GFXfont *f)
{
  // Prints text, without changing the color
  display.setCursor(pos_x, pos_y);
  printTextAtCursor(text, f);
}

void printTextColor(char *text, int pos_x, int pos_y, const GFXfont *f, int color)
{
  // Prints text, changing the color
  display.setTextColor(color, 7 - color);
  printText(text, pos_x, pos_y, f);
}

void eraseText(int pos_x, int pos_y, int w, int h, int color)
{
  // Fills an area with color to erase the text
  display.fillRect(pos_x, pos_y - h + 2, w, h, color); //+2 to get commas, etc.
}

void eraseTextAtCursor(int w, int h, int color)
{
  // Fills an area with color to erase the text, using cursor position
  eraseText(display.getCursorX(), display.getCursorY() + 2, w, h, color); //+2 to get commas, etc.
}

void eraseAndPrint(char *text, int pos_x, int pos_y, const GFXfont *f, int textColor, int widthErase, int heightErase)
{
  // Erase and then print text
  if (widthErase == 0) widthErase = (int) (strlen(text) * 36 * 0.6);
  if (heightErase == 0) heightErase = 36;
  // Serial.println(text);
  // Serial.println(widthErase);

  if (textColor == BLACK)
    eraseText(pos_x, pos_y, widthErase, heightErase, WHITE);
  else
    eraseText(pos_x, pos_y, widthErase, heightErase, BLACK);

  printText(text, pos_x, pos_y, f);
}

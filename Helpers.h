// General includes
#include <Arduino.h>
#include <Inkplate.h>

extern Inkplate display;

// Global functions

#ifndef HELPERS_H
#define HELPERS_H

// Weather icons
void drawWeatherIcon_small(int pos_x, int pos_y, char *icon_abbr);
void drawWeatherIcon_medium(int pos_x, int pos_y, char *icon_abbr);

// Minor icons
void drawWindIcon(int pos_x = 50, int pos_y = 325);
void drawRaindropIcon(int pos_x = 50, int pos_y = 372);
void drawSunriseIcon(int pos_x = 280, int pos_y = 330);
void drawSunsetIcon(int pos_x = 280, int pos_y = 380);

// Print helper

void printCelsiusCircle(int fontsize_px, int color = BLACK);
void printCelsiusAtCursor(const GFXfont *f, int fontsize_px, int color = BLACK);

void printTextAtCursor(char *text, const GFXfont *f);
void printText(char *text, int pos_x, int pos_y, const GFXfont *f);
void printTextColor(char *text, int pos_x, int pos_y, const GFXfont *f = NULL, int color = BLACK);
void eraseText(int pos_x, int pos_y, int w, int h, int color = BLACK);
void eraseTextAtCursor(int w, int h, int color = WHITE);
void eraseAndPrint(char *text, int pos_x, int pos_y, const GFXfont *f, int textColor = BLACK, int widthErase = 0, int heightErase = 0);


#endif

#include "Arduino.h"
#include "Fonts/FreeSerifBold24pt7b.h"
#include "Fonts/FreeSerifBold18pt7b.h"
#include "Inkplate.h"
#include "Verbs.h"

extern Inkplate display;


const GFXfont *normal_font = &FreeSerifBold24pt7b;
const GFXfont *smaller_font = &FreeSerifBold18pt7b;

const int WIDTH = 1024;
const int HEIGHT = 758;

bool menu = 0;
bool conjugacio = 1;

char title[12] = "Conjugacio";
char menu_actions[8] = {'<', '>', '  x'};
int size_menu_buttons = 70;

int margin = 25;
int button_padding_x = 20;
int button_padding_y = 10;

int menu_h = 100;
int toolbar_h = 80;

char modes[3][12] = 
{
    "Indicatif",
    "Subjonctif",
    "Autres"
};



void mainDrawConjugacio()
{
    display.setFont(normal_font);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    
    // MENU
    display.setCursor(margin, menu_h-30);
    display.print(title);

    int i = 0;
    int pos_y = (menu_h-size_menu_buttons)*0.5;
    int pos_x = 1;
    for (i = 0; i < 3; i++)
    {
        pos_x = 1023 -(margin + size_menu_buttons)*(1+i);
        display.drawRoundRect(pos_x, pos_y, size_menu_buttons, size_menu_buttons, 15, BLACK);
        display.setCursor(pos_x + 17, pos_y+size_menu_buttons-25);
        display.print(menu_actions[2-i]);
    }

    display.drawFastHLine(0, menu_h, WIDTH, BLACK);


    // TOOLBAR
    pos_y = menu_h+toolbar_h-25;
    pos_x = margin+button_padding_x;
    int pos_x_2 = pos_x;

    // Text Bounds
    int16_t x, y;
    uint16_t w, h;

    for (i = 0; i < 3; i++)
    {
        Serial.print(modes[i]);
        display.getTextBounds(modes[i], pos_x, pos_y, &x, &y, &w, &h);
        display.drawRoundRect(pos_x-button_padding_x, y-button_padding_y, w+2*button_padding_x, 38+2*button_padding_y, 10, BLACK);
        display.setCursor(pos_x, pos_y);
        display.print(modes[i]);
        pos_x = x + w + button_padding_x*2 + margin;
    }

    display.drawFastHLine(0, menu_h+toolbar_h, WIDTH, BLACK);


    // TEMPS
    int j = -1;
    int index = 0;
    pos_y = menu_h + toolbar_h + 50;
    int delta_y = 0;
    int space_pronom = 250;
    for (i = 0; i < 21; i++)
    {        
        if (i % 7 == 0) // Header
        {
            j++;
            display.setFont(normal_font);
            index = -1;
            if (j % 2 == 0) { pos_x = margin; } // Horizontal position
            else { pos_x = display.width()*0.5 + margin; }
            if (j > 1) { delta_y = 300; } // Vertical position
            else { delta_y = 0; }
            display.setCursor(pos_x, pos_y + delta_y);
            display.print(verb_fer[i]);
        }
        else // Normal text
        {
            display.setFont(smaller_font);
            display.setCursor(pos_x, pos_y + delta_y);
            display.print(pronoms[index]);
            display.setCursor(pos_x + space_pronom, pos_y + delta_y);
            display.print(verb_fer[i]);
        }        
        delta_y += 40;
        index++;
    }
    

}



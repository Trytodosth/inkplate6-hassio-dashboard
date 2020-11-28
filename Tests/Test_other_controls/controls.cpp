
#define DEG2RAD 0.0174532925

#include "controls.h"
#include "draw_functions.h"


Gauge::Gauge (int pos_x, int pos_y, float min_val, float max_val, int rad, int colour) {  
  x = pos_x;
  y = pos_y;
  min_value = min_val;
  max_value = max_val;
  radius = rad;
  color = colour;


  // default values
  half_start_angle = 30;
  angle_span = 360 - 2 * half_start_angle;
  thickness_on = int(radius / 6);
  thickness_off = int(radius / 12);
  marker_radius = thickness_on;
  marker_filled = false;
  is_reverse = false;
  
  GFXfont font = Roboto_Light_48;
  GFXfont font_sub = Roboto_Light_36;

  value_decimal_places = 0;
  
  if (color == BLACK) {
    color_bg = WHITE;
  } else {
    color_bg = BLACK;
  }
}

void Gauge::draw(bool force_draw) {
  if (!up_to_date | force_draw)
  {
    // ERASE with background color
    erase();
    
    // ARC
    fillArc2(x, y, -90 + half_start_angle, -90 - half_start_angle, radius - thickness_off * 0.5, radius - thickness_off * 0.5, thickness_off, true, color);
    int angle_marker;
    if (is_reverse)
    {
      angle_marker = -90 + half_start_angle + angle_span * value / (max_value - min_value);
      fillArc2(x, y, -90 + half_start_angle,  angle_marker,radius, radius, thickness_on, true, color);
    } else {
      angle_marker = -90 - half_start_angle - angle_span * value / (max_value - min_value);
      fillArc2(x, y, angle_marker, -90 - half_start_angle, radius, radius, thickness_on, true, color);
    }

    // MARKER
    if (marker_radius > 0) {
      int marker_x = x + (radius - thickness_on * 0.5) * cos(-angle_marker * DEG2RAD);
      int marker_y = y + (radius - thickness_on * 0.5) * sin(-angle_marker * DEG2RAD);
      if (marker_filled)
      {
        display.fillCircle(marker_x, marker_y, marker_radius, color);
      } else {
        display.fillCircle(marker_x, marker_y, marker_radius, color);
        display.fillCircle(marker_x, marker_y, marker_radius-4, color_bg);
      }
    }

    // TEXT
    char text[24];
    char val_char[8];
    int16_t x1, y1;
    uint16_t w, h1, h2;
    dtostrf(value, 1, value_decimal_places, val_char);
    sprintf(text,"%s%s%s",prefix, val_char, suffix);
    
    display.setTextColor(color);
    display.setFont(&font);
    // Value
    display.getTextBounds(text, x, y, &x1, &y1, &w, &h1);
    display.setCursor(x-w*0.5, y+h1*0.5);
    display.print(text);
    
    if (sizeof(subtext) > 0) {
      // Subtext
      display.setFont(&font_sub);
      display.getTextBounds(subtext, x, y, &x1, &y1, &w, &h2);
      display.setCursor(x-w*0.5, y+h1*0.5+h2+3);
      display.print(subtext);
    }

  }
}

void Gauge::erase() {
  display.fillCircle(x, y, radius + marker_radius, color_bg);
}


void Gauge::set_value(float val) {
  value = max(min(val, max_value), min_value);
  up_to_date = false;
}

void Gauge::set_line(int thick_on, int thick_off) {
  thickness_on = thick_on;
  thickness_off = thick_off;
}
void Gauge::set_marker(int marker_rad, bool is_filled) {
  marker_radius = marker_rad;
  marker_filled = is_filled;
}
void Gauge::set_text(const char pref[], const char suff[], const char subtitle[], const GFXfont *f, const GFXfont *f_sub, int decimal_places) {
  strcpy(prefix, pref);
  strcpy(suffix, suff);
  strcpy(subtext, subtitle);
  value_decimal_places = decimal_places;
  font = *f;
  font_sub = *f_sub;
}

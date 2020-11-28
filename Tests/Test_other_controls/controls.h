

#include <Inkplate.h>
#include "Fonts.h"

extern Inkplate display;

class Gauge {
    int x, y; // Center coordinates
    float value, max_value, min_value; // Gauge values
    int radius, half_start_angle, angle_span, color, color_bg;
    int thickness_on, thickness_off; // line
    int marker_radius;
    bool marker_filled, is_reverse;
    bool up_to_date;
    char prefix[8];
    char suffix[8];
    char subtext[24];
    GFXfont font;
    GFXfont font_sub;
    int value_decimal_places;
  public:
    Gauge (int pos_x, int pos_y, float min_val, float max_val, int rad, int colour);
    void set_value(float val);
    void set_line(int thick_on, int thick_off);
    void set_marker(int marker_rad, bool is_filled);
    void set_text(const char pref[], const char suff[], const char subtitle[], const GFXfont *f, const GFXfont *f_sub, int decimal_places);
    
    void draw(bool force_draw);
    void erase();
    
    int is_up_to_date() {return up_to_date;}
};

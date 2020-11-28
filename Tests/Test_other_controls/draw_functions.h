

#include <Inkplate.h>

extern Inkplate display;

// Public
void draw_arc(int pos_x, int pos_y, int radius, int alpha, int beta, int color);
int fillArc2(int x, int y, int start_angle, int end_angle, int rx, int ry, int w, bool drawRounds, unsigned int colour);

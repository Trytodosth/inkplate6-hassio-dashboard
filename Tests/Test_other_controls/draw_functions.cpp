
#define DEG2RAD 0.0174532925

#include "controls.h"


// #########################################################################
// Draw a circular or elliptical arc with a defined thickness
// #########################################################################

// x,y == coords of centre of arc
// start_angle = 0 - 359
// seg_count = number of 3 degree segments to draw (120 => 360 degree arc)
// rx = x axis radius
// yx = y axis radius
// w  = width (thickness) of arc in pixels
// colour = 16 bit colour value
// Note if rx and ry are the same then an arc of a circle is drawn

int fillArc2(int x, int y, int start_angle, int end_angle, int rx, int ry, int w, bool drawRounds, unsigned int colour)
{
  // Courtesy of Bodmer: https://forum.arduino.cc/index.php?topic=413186.msg2845839#msg2845839
  // With some modifications

  start_angle = start_angle % 360 + 90;
  end_angle = end_angle % 360 + 90;
  if (end_angle < start_angle) end_angle += 360;
  //  if (start_angle < end_angle){
  //    int tmp = end_angle;
  //    end_angle = start_angle;
  //    start_angle = tmp;
  //  }

  byte seg = 3; // Segments are 3 degrees wide = 120 segments for 360 degrees
  byte inc = 3; // Draw segments every 3 degrees, increase to 6 for segmented ring

  int seg_count = int((end_angle - start_angle) / seg);

  // Calculate first pair of coordinates for segment start
  float sx = cos((start_angle - 90) * DEG2RAD);
  float sy = -sin((start_angle - 90) * DEG2RAD);
  uint16_t x0 = sx * (rx - w) + x;
  uint16_t y0 = sy * (ry - w) + y;
  uint16_t x1 = sx * rx + x;
  uint16_t y1 = sy * ry + y;

  if (drawRounds) {
    // Draw circle at the beginning
    display.fillCircle((x0 + x1) * 0.5, (y0 + y1) * 0.5, w*0.5, colour);
  }

  // Draw colour blocks every inc degrees
  for (int i = start_angle; i < start_angle + seg * seg_count; i += inc) {

    // Calculate pair of coordinates for segment end
    float sx2 = cos((i + seg - 90) * DEG2RAD);
    float sy2 = -sin((i + seg - 90) * DEG2RAD);
    int x2 = sx2 * (rx - w) + x;
    int y2 = sy2 * (ry - w) + y;
    int x3 = sx2 * rx + x;
    int y3 = sy2 * ry + y;

    display.fillTriangle(x0, y0, x1, y1, x2, y2, colour);
    display.fillTriangle(x1, y1, x2, y2, x3, y3, colour);

    // Copy segment end to segment start for next segment
    x0 = x2;
    y0 = y2;
    x1 = x3;
    y1 = y3;
  }

  if (drawRounds) {
    // Draw circle at the end
    display.fillCircle((x0 + x1) * 0.5, (y0 + y1) * 0.5, w*0.5, colour);
  }
}







void add_pixel_if(int x, int y, int pos_x, int pos_y, int scenario, int x_begin, int x_end, int color);
int radius_error(int x, int y, int r_square);
int int_pow(int x, int p);


void draw_arc(int pos_x, int pos_y, int radius, int alpha, int beta, int color) {
  alpha = alpha % 360;
  beta = beta % 360;

  int r_square = int_pow(radius, 2);
  int scenario;

  int x_begin = radius * cos(alpha * M_PI / 180);
  int x_end = radius * sin(alpha * M_PI / 180);

  // To handle partial symmetries from mid-point algo
  if ((alpha - 180) * (beta - 180) >= 0) {
    if (beta > alpha) {
      if (alpha < 180) {
        scenario = 1;
      } else {
        scenario = 2;
      }
    } else {
      if (alpha < 180) {
        scenario = 3;
      } else {
        scenario = 4;
      }
    }
  } else {
    if (alpha < 180) {
      scenario = 4;
    } else {
      scenario = 5;
    }
  }

  int x = radius;
  int y = 0;

  // Initial points at 90°
  add_pixel_if(x, y, pos_x, pos_y, scenario, x_begin, x_end, color);
  add_pixel_if(-y, x, pos_x, pos_y, scenario, x_begin, x_end, color);
  add_pixel_if(-x, -y, pos_x, pos_y, scenario, x_begin, x_end, color);
  add_pixel_if(y, -x, pos_x, pos_y, scenario, x_begin, x_end, color);

  // Mid-point algorythm until the 45° mark
  while (y < x)
  {
    if (2 * (radius_error(x, y, r_square) + (2 * y - 1)) + 1 - 2 * x > 0) {
      x -= 1;
    }
    y += 1;

    // Drawing for each octan
    add_pixel_if(x, y, pos_x, pos_y, scenario, x_begin, x_end, color);
    add_pixel_if(y, x, pos_x, pos_y, scenario, x_begin, x_end, color);
    add_pixel_if(-y, x, pos_x, pos_y, scenario, x_begin, x_end, color);
    add_pixel_if(-x, y, pos_x, pos_y, scenario, x_begin, x_end, color);

    add_pixel_if(-x, -y, pos_x, pos_y, scenario, x_begin, x_end, color);
    add_pixel_if(-y, -x, pos_x, pos_y, scenario, x_begin, x_end, color);
    add_pixel_if(y, -x, pos_x, pos_y, scenario, x_begin, x_end, color);
    add_pixel_if(x, -y, pos_x, pos_y, scenario, x_begin, x_end, color);
  }

}


void add_pixel_if(int x, int y, int pos_x, int pos_y, int scenario, int x_begin, int x_end, int color) {

  if (scenario == 1) {
    if (y >= 0 & x <= x_begin & x >= x_end) {
      display.drawPixel(x + pos_x, -y + pos_y, color);
      return;
    }
  }
  if (scenario == 2) {
    if (y <= 0 & x >= x_begin & x <= x_end) {
      display.drawPixel(x + pos_x, -y + pos_y, color);
      return;
    }
  }
  if (scenario == 3) {
    if ((y >= 0 & (x <= x_begin | x >= x_end)) | y < 0) {
      display.drawPixel(x + pos_x, -y + pos_y, color);
      return;
    }
  }
  if (scenario == 4) {
    if ((y <= 0 & (x >= x_begin | x <= x_end)) | y > 0) {
      display.drawPixel(x + pos_x, -y + pos_y, color);
      return;
    }
  }
  if (scenario == 5) {
    if ((y >= 0 & x <= x_begin) | y < 0 & x <= x_end) {
      display.drawPixel(x + pos_x, -y + pos_y, color);
      return;
    }
  }
  if (scenario == 6) {
    if ((y <= 0 & x >= x_begin) | y > 0 & x >= x_end) {
      display.drawPixel(x + pos_x, -y + pos_y, color);
      return;
    }
  }
}


int radius_error(int x, int y, int r_square)
{
  return (int_pow(x, 2) + int_pow(y, 2) - r_square);
}


int int_pow(int x, int p)
{
  // Thanks to: https://stackoverflow.com/a/1505791/11055786
  if (p == 0) return 1;
  if (p == 1) return x;

  int tmp = int_pow(x, p / 2);
  if (p % 2 == 0) return tmp * tmp;
  else return x * tmp * tmp;
}

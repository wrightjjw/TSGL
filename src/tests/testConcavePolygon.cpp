/*
 * testConcavePolygon.cpp
 *
 *  Created on: May 27, 2015
 *      Author: cpd5
 */

#include <tsgl.h>

#ifdef _WIN32
const double PI = 3.1415926535;
#else
const double PI = M_PI;
#endif
const double RAD = PI / 180;  // One radian in degrees

/**
 * \brief Draw concave polygons, which have one or more interior angles > 180
 * ( see http://www.mathopenref.com/polygonconcave.html )
 * \details
 * - Initialize a constant \b PSIZE
 * - Have four arrays of integers \b x, \b y, \b xx, and \b yy and set them to have size \b PSIZE
 * - Create an empty array of colors of size \b PSIZE and fill it with random colors.
 * - Fill the arrays of integers, \b x and \b y with specific values (which will then be used in the while loop to draw a concave polygon).
 * - Fill the other arrays of integers, \b xx and \b yy, with specific values.
 * - While the Canvas is open:
 *   - Sleep the internal timer of the Canvas until the Canvas is ready to draw.
 *   - Draw a concave polygon on the Canvas and pass \b PSIZE, the arrays \b x and \b y, and the array of colors as arguments.
 *   - Draw another concave polygon on the Canvas and pass \b PSIZE, the arrays \b x and \b y, and the array of colors as arguments.
 *
 * \param can, Reference to the Canvas being drawn to
 */
void concavePolygonFunction(Canvas& can) {
  const int PSIZE = 64;

  int x[PSIZE], xx[PSIZE];
  int y[PSIZE], yy[PSIZE];
  ColorFloat color[PSIZE];
  for (unsigned i = 0; i < PSIZE; ++i)
    color[i] = Colors::randomColor(1.0f);

  x[0] = 100; y[0] = 100;
  x[1] = 200; y[1] = 100;
  x[2] = 200; y[2] = 200;
  x[3] = 300; y[3] = 200;
  x[4] = 300; y[4] = 100;
  x[5] = 400; y[5] = 100;
  x[6] = 400; y[6] = 400;
  x[7] = 300; y[7] = 300;
  x[8] = 250; y[8] = 400;
  x[9] = 200; y[9] = 300;
  x[10] = 100; y[10] = 400;
  x[11] = 100; y[11] = 100;


//Could use #pragma omp parallel for to speed this up?
  for (int i = 0; i < PSIZE; ++i) {
    if (i % 2 == 0) {
      xx[i] = 600 + 150 * sin((1.0f*i)/(PSIZE) * PI * 2);
      yy[i] = 450 - 150 * cos((1.0f*i)/(PSIZE) * PI * 2);
    } else {
      xx[i] = 600 + 300 * sin((1.0f*i)/(PSIZE) * PI * 2);
      yy[i] = 450 - 300 * cos((1.0f*i)/(PSIZE) * PI * 2);
    }
    std::cout << i << ":" << x[i] << "," << y[i] << std::endl;
  }

  while (can.getIsOpen()) {  // Checks to see if the window has been closed
    can.sleep();
//    for (unsigned i = 0; i < PSIZE; ++i)
//      color[i] = Colors::randomColor(1.0f);
    can.drawConcavePolygon(PSIZE, x, y, color, true);
    can.drawConcavePolygon(PSIZE, xx, yy, color, true);
  }
}

int main(int argc, char* argv[]) {
    int w = (argc > 1) ? atoi(argv[1]) : 960;
    int h = (argc > 2) ? atoi(argv[2]) : w;
    if (w <= 0 || h <= 0)     //Checked the passed width and height if they are valid
      w = h = 960;              //If not, set the width and height to a default value
    Canvas c33(0, 0, w, h, "", FRAME);
    c33.setBackgroundColor(WHITE);
    c33.start();
    concavePolygonFunction(c33);
    c33.wait();
}

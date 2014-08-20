#ifndef DRAW_DYNAMIC
#define DRAW_DYNAMIC

#include "draw.hpp"

/*

naive pass system:
draw obj full lit w/ambient value
then draw a pass for each light; alpha value is the falloff

use shadow mapped shadows

*/

#ifdef PIPELINE_DYNAMIC
void drawLevelDynamic();
void drawInitDynamic();
#endif

#endif
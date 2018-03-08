#include "modelerview.h"
#include "modelerapp.h"
#include "modelerdraw.h"
#include <FL/gl.h>

#include "bitmap.h"
#include "vec.h"
#include "spellbreaker_globals.h"

//Borrow internal function from modeldraw.cpp as we don't want to change any model*.*
extern void _setupOpenGl();
extern void _dump_current_modelview();
extern void _dump_current_material();

//Draw Hilbert curve
void drawAura(float deg, float len, float decay, float width, int iter) {
	glLineWidth(width);
	

}

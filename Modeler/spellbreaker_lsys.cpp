#include "modelerview.h"
#include "modelerapp.h"
#include "modelerdraw.h"
#include <FL/gl.h>
#include <random>

#include "bitmap.h"
#include "vec.h"
#include "spellbreaker_globals.h"

//Borrow internal function from modeldraw.cpp as we don't want to change any model*.*
extern void _setupOpenGl();
extern void _dump_current_modelview();
extern void _dump_current_material();

std::mt19937 mt;
std::uniform_real_distribution<float> dis;
float dense;

static void l_system_forward(float deg, float len, float decay, float width, int iter) {
	float c = dis(mt);
	if (iter <= 0 || c >= dense) {
		//drawBox(width, 0.01, len);
	}
	else {
		//glPushMatrix();
		{
			drawBox(width, 0.01, len);
			glTranslatef(0, 0, len);
			l_system_forward(deg, len*decay, decay, width, iter - 1);
			
			glPushMatrix();
			{
				glRotatef(deg, 0, 1, 0);
				l_system_forward(deg, len*decay, decay, width, iter - 1);
			}
			glPopMatrix();

			drawBox(width, 0.01, len);
			glTranslatef(0, 0, len);
			l_system_forward(deg, len*decay, decay, width, iter - 1);
			
			glPushMatrix();
			{
				glRotatef(-deg, 0, 1, 0);
				l_system_forward(deg, len*decay, decay, width, iter - 1);
			}
			glPopMatrix();

			//drawBox(width, 0.01, len);
			//glTranslatef(0, 0, len);
			//l_system_forward(deg, len*decay, decay, width, iter - 1);

			glPushMatrix();
			{
				glRotatef(deg, 1, 0, 0);
				l_system_forward(deg, len*decay, decay, width, iter - 1);
			}
			glPopMatrix();

			//drawBox(width, 0.01, len);
			//glTranslatef(0, 0, len);
			//l_system_forward(deg, len*decay, decay, width, iter - 1);

			glPushMatrix();
			{
				glRotatef(-deg, 1, 0, 0);
				l_system_forward(deg, len*decay, decay, width, iter - 1);
			}
			glPopMatrix();
		}
		//glPopMatrix();
	}
}

//Draw 3 shape defined by the L-system
void drawPlant(float deg, float len, float decay, float width, int iter, int seed, float _dense) {
	mt.seed(seed);
	dis = std::uniform_real_distribution<float>();
	dense = _dense;

	glPushMatrix();
	l_system_forward(deg, len, decay, width, iter);
	glPopMatrix();
}

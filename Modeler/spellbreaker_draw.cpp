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

void drawTorso(double x, double y, double z, GLuint front_texture, GLuint back_texture) {
	    ModelerDrawState *mds = ModelerDrawState::Instance();

	_setupOpenGl();
    
    if (mds->m_rayFile)
    {
        _dump_current_modelview();
        fprintf(mds->m_rayFile,  
            "scale(%f,%f,%f,translate(0.5,0.5,0.5,box {\n", x, y, z );
        _dump_current_material();
        fprintf(mds->m_rayFile,  "})))\n" );
    }
    else
    {
        /* remember which matrix mode OpenGL was in. */
        int savemode;
        glGetIntegerv( GL_MATRIX_MODE, &savemode );
        
        /* switch to the model matrix and scale by x,y,z. */
        glMatrixMode( GL_MODELVIEW );
        glPushMatrix();
        glScaled( x, y, z );
        
		//Draw the un-textured part
        glBegin( GL_QUADS );
        
        glNormal3d( 0.0, -1.0, 0.0 );
        glVertex3d( 0.0, 0.0, 0.0 ); glVertex3d( 1.0, 0.0, 0.0 );
        glVertex3d( 1.0, 0.0, 1.0 ); glVertex3d( 0.0, 0.0, 1.0 );
        
        glNormal3d( -1.0, 0.0, 0.0 );
        glVertex3d( 0.0, 0.0, 0.0 ); glVertex3d( 0.0, 0.0, 1.0 );
        glVertex3d( 0.0, 1.0, 1.0 ); glVertex3d( 0.0, 1.0, 0.0 );
        
        glNormal3d( 0.0, 1.0, 0.0 );
        glVertex3d( 0.0, 1.0, 0.0 ); glVertex3d( 0.0, 1.0, 1.0 );
        glVertex3d( 1.0, 1.0, 1.0 ); glVertex3d( 1.0, 1.0, 0.0 );
        
        glNormal3d( 1.0, 0.0, 0.0 );
        glVertex3d( 1.0, 0.0, 0.0 ); glVertex3d( 1.0, 1.0, 0.0 );
        glVertex3d( 1.0, 1.0, 1.0 ); glVertex3d( 1.0, 0.0, 1.0 );
        
        glEnd();

		//Draw the front textured part
		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, front_texture);

		glBegin(GL_QUADS);

		glNormal3d(0.0, 0.0, 1.0);
		glTexCoord2f(0, 0);
		glVertex3d(0.0, 0.0, 1.0);
		glTexCoord2f(1, 0);
		glVertex3d(1.0, 0.0, 1.0);
		glTexCoord2f(1, 1);
		glVertex3d(1.0, 1.0, 1.0);
		glTexCoord2f(0, 1);
		glVertex3d(0.0, 1.0, 1.0);

		glEnd();
		glDisable(GL_TEXTURE_2D);

		//Draw the back textured part
		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glBindTexture(GL_TEXTURE_2D, back_texture);

		glBegin(GL_QUADS);

		glNormal3d(0.0, 0.0, -1.0);
		glTexCoord2f(0, 0);
		glVertex3d(0.0, 0.0, 0.0);
		glTexCoord2f(0, 8);
		glVertex3d(0.0, 1.0, 0.0);
		glTexCoord2f(8, 8);
		glVertex3d(1.0, 1.0, 0.0);
		glTexCoord2f(8, 0);
		glVertex3d(1.0, 0.0, 0.0);

		glEnd();
		glDisable(GL_TEXTURE_2D);
        
        /* restore the model matrix stack, and switch back to the matrix
        mode we were in. */
        glPopMatrix();
        glMatrixMode( savemode );
    }
}

void drawTriangle(Vec3d a, Vec3d b, Vec3d c) {
	drawTriangle(a[0], a[1], a[2], b[0], b[1], b[2], c[0], c[1], c[2]);
}

/**
 The shield shape consist of 6 base points and 1 center point
 All 6 base points lie on the same y-plane, every two on a x-row
 Top two rows are closer to each other
 */
void drawShield(double w, double h, double d) {
	Vec3d shield_point(w/2, h/2, d);

	Vec3d bottom_left(w / 4, 0, 0);
	Vec3d bottom_right(w * 3 / 4, 0, 0);

	Vec3d mid_left(0, h * 3 / 4, 0);
	Vec3d mid_right(w, h * 3 / 4, 0);

	Vec3d top_left(w / 4, h, 0);
	Vec3d top_right(w * 3 / 4, h, 0);

	setDiffuseColor(COLOR_LIGHT_BLUE);
	drawTriangle(bottom_right, shield_point, bottom_left);
	setDiffuseColor(COLOR_BLUE);

	drawTriangle(mid_right, shield_point, bottom_right);

	setDiffuseColor(COLOR_LIGHT_BLUE);
	drawTriangle(top_right, shield_point, mid_right);
	setDiffuseColor(COLOR_BLUE);

	drawTriangle(top_left, shield_point, top_right);

	setDiffuseColor(COLOR_LIGHT_BLUE);
	drawTriangle(mid_left, shield_point, top_left);
	setDiffuseColor(COLOR_BLUE);

	drawTriangle(bottom_left, shield_point, mid_left);
}

/**
* Draw a surface obtained by sweeping a curve along another curve
* Pass two functions that takes a float and returns a Vec3f
* Note that the sweeping function should return a offset to the along function value
*/
#define CURVE_FINENESS 25

void drawSweepedCurve(
	Vec3f (*sweep_fun)(float), const float sweep_min, const float sweep_max,
	Vec3f (*along_fun)(float), const float along_min, const float along_max,
	int degree
)
{
	const int order = degree + 1;
	GLfloat* control_points = new GLfloat[order * order * 3];

	//Not divided by order!
	const float sweep_step = (sweep_max - sweep_min) / degree;
	const float along_step = (along_max - along_min) / degree;

	//Display the control point for a bit
	//glPointSize(5.0);
	//glBegin(GL_POINTS);
	float curr_along = along_min;
	for (int i = 0; i < order; i++) {
		//Compute points along the curve
		curr_along += along_step;
		float curr_sweep = sweep_min;
		Vec3f along_v = along_fun(curr_along);
		for (int j = 0; j < order; j++) {
			//Compute offset from points on the curve
			curr_sweep += sweep_step;
			Vec3f sweep_v = sweep_fun(curr_sweep);
			//Copy to the control point array
			for (int s = 0; s < 3; s++) {
				control_points[(i*order + j) * 3 + s] = along_v[s] + sweep_v[s];
				//glVertex3fv(&control_points[(i*order + j) * 3]);
			}
		}
	}
	//glEnd();

	glMap2f(GL_MAP2_VERTEX_3, 0.0, 1.0, 3, order, 0.0, 1.0, order * 3, order, control_points);

	glEnable(GL_MAP2_VERTEX_3);

	//Fill the mesh
	glMapGrid2f(CURVE_FINENESS, 0.0, 1.0, CURVE_FINENESS, 0.0, 1.0);
	glEvalMesh2(GL_FILL, 0, CURVE_FINENESS, 0, CURVE_FINENESS);

	////Actually drawing them out
	//for (int j = 0; j < CURVE_FINENESS; j++) {
	//	glBegin(GL_QUAD_STRIP);
	//	for (int i = 0; i < CURVE_FINENESS - 1; i++) {
	//		glEvalCoord2f(i / CURVE_FINENESS, j / CURVE_FINENESS);
	//		glEvalCoord2f((i+1) / CURVE_FINENESS, j / CURVE_FINENESS);
	//	}
	//	glEnd();
	//}

	//for (int j = 0; j < CURVE_FINENESS; j++) {
	//	glBegin(GL_LINE_STRIP);
	//	for (int i = 0; i < CURVE_FINENESS; i++) {
	//		glEvalCoord2f((float)i / CURVE_FINENESS, (float)j / CURVE_FINENESS);
	//	}
	//	glEnd();

	//	glBegin(GL_LINE_STRIP);
	//	for (int i = 0; i < CURVE_FINENESS; i++) {
	//		glEvalCoord2f((float)j / CURVE_FINENESS, (float)i / CURVE_FINENESS);
	//	}
	//	glEnd();
	//}

	delete[] control_points;
}

float sin_angle_offset;
//Input in degree, varying in y-direction
Vec3f sin_sweep(float input) {
	return Vec3f(input / 400.0, sinf(input*360/2/M_PI)/2, 0);
}

//Varying in x-direction
Vec3f sqrt_along(float input) {
	return Vec3f(0, powf(input, 1.0/ sin_angle_offset), input);
}

//Draw a horizontal laying cape, sweeping a sine curve along a sqrt curve.
void drawCape(float sweep_angle) {
	sin_angle_offset = sweep_angle;
	glPushMatrix();
	{
		glScaled(1.0, -1.5, 3.0);
		drawSweepedCurve(sin_sweep, -360, 360, sqrt_along, 0, 1, 6);
	}
	glPopMatrix();
}

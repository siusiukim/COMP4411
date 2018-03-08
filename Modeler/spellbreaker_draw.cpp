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

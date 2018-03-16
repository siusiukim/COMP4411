#include "modelerview.h"
#include "modelerapp.h"
#include "modelerdraw.h"
#include <FL/gl.h>

#include "vec.h"
#include "bitmap.h"
#include "spellbreaker_globals.h"

void drawTorso(double x, double y, double z, GLuint front_texture, GLuint back_texture);
void drawShield(double w, double h, double d);
void drawCape(float sweep_angle);
void drawPlant(float deg, float len, float decay, float width, int iter, int seed, float dense);

ModelerView* createSpellBreaker(int x, int y, int w, int h, char *label)
{
	return new SpellBreaker(x, y, w, h, label);
}

void SpellBreaker::draw()
{
	if (!bitmap_loaded)
	{
		bitmap_loaded = true;
		initTexture();
	}

	ModelerView::draw();

	//Translate the entire thing down a bit
	glTranslated(0, -1.5, 0);

	// Draw the floor
	//setAmbientColor(.5f, .1f, .1f);
	setSpecularColor(.5, .05, .05);
	setDiffuseColor(COLOR_WHITE);
	glPushMatrix();
	{
		glTranslated(-2.5, -1, -2.5);
		drawBox(7, 1, 5);
	}
	glPopMatrix();

	//Draw the plant
	glPushMatrix();
	{
		glTranslated(3, 0, 0);
		glRotated(-90, 1, 0, 0);
		drawPlant(VAL(LS_DEGREE), VAL(LS_LENGTH), VAL(LS_DECAY), 0.05, VAL(LS_ITERATION), VAL(LS_SEED), VAL(LS_DENSITY));
	}
	glPopMatrix();

	// Draw the spellbreaker model
	setDiffuseColor(COLOR_GREEN);

	updateParam();

	//Entire model
	glPushMatrix();
	{
		//Translation of the entire model
		glTranslated(VAL(TORSO_XPOS), VAL(TORSO_YPOS), VAL(TORSO_ZPOS));
		glRotated(VAL(TORSO_TURN), 0, 1, 0);

		//Torso
		glPushMatrix();
		{
			glTranslated(-0.75, 1, 0);
			drawTorso(1.5, 2, 1, armor_texture_id, diamond_texture_id);

			if (level_of_detail > 3) {
				//My awesome CAPE
				glPushMatrix();
				{
					glTranslated(0.5f, -0.2f, -3.0f);
					glRotated(-60, 1.0, 0, 0);
					drawCape(cape_wave);
				}
				glPopMatrix();
			}

			//Head
			glPushMatrix();
			{
				glTranslated(0.25, 2 + breath_y, 0.1);
				{
					//Rotate around neck position
					glTranslated(0.5, 0, 0.2);
					glRotated(head_look_x, 1.0, 0, 0);
					glRotated(head_look_y + breath_head, 0, 1.0, 0);
					glTranslated(-0.5, 0, -0.2);
				}
				drawBox(1, 1, 0.8);

				setDiffuseColor(COLOR_RED);
				//Left eye
				if (level_of_detail > 2) {
					glPushMatrix();
					{
						glTranslated(0.3, 0.7, 0.8);
						glScaled(0.15, 0.15, 0.1);
						drawBox(1, 1, 1);
					}
					glPopMatrix();

					//Right eye
					glPushMatrix();
					{
						glTranslated(0.6, 0.7, 0.8);
						glScaled(0.15, 0.15, 0.1);
						drawBox(1, 1, 1);
					}
					glPopMatrix();
				}
			}
			glPopMatrix();

			setDiffuseColor(COLOR_GREEN);
			//Left leg
			glPushMatrix();
			{
				glTranslated(0.2, -1, 0.3);
				drawBox(0.5, 0.95, 0.6);
			}
			glPopMatrix();

			//Right leg
			glPushMatrix();
			{
				glTranslated(0.8, -1, 0.3);
				drawBox(0.5, 0.95, 0.6);
			}
			glPopMatrix();

			//Upper left arm
			glPushMatrix();
			{
				glTranslated(-0.55, 1 + breath_y/2, 0.25);
				{
					//Rotate around near-top center of upper arm
					glTranslated(0.25, 0.8, 0.3);
					glRotated(left_upper_arm_rise, -1.0, 0, 0);
					glTranslated(-0.25, -0.8, -0.3);
					drawBox(0.5, 0.9, 0.6);
				}

				//Lower left arm
				glPushMatrix();
				{
					glTranslated(0, -1, 0);
					{
						//Rotate around near-top center of lower arm
						glTranslated(0.25, 0.8, 0.3);
						glRotated(left_upper_arm_rise, 0, 0, 1.0);
						glTranslated(-0.25, -0.8, -0.3);
					}
					drawBox(0.5, 0.9, 0.6);
					//Shield
					if (level_of_detail > 1) {
						glPushMatrix();
						{
							glTranslated(0, 0.9, -1.2);
							glRotated(-90, 0, 1, 0);
							glRotated(-90, 0, 0, 1);
							drawShield(1.8, 3, 0.5);
							setDiffuseColor(COLOR_GREEN);
						}
						glPopMatrix();
					}
				}
				glPopMatrix();
			}
			glPopMatrix();

			//Upper right arm
			glPushMatrix();
			{
				glTranslated(1.55, 1 + breath_y/2, 0.25);
				{
					//Rotate around near-top center of upper arm
					glTranslated(0.25, 0.8, 0.3);
					glRotated(right_upper_arm_rise, 0, 0, 1.0);
					glTranslated(-0.25, -0.8, -0.3);
					drawBox(0.5, 0.9, 0.6);
				}

				//Lower right arm
				glPushMatrix();
				{
					glTranslated(0, -1, 0);
					{
						glTranslated(0.25, 0.8, 0.3);
						glRotated(right_lower_arm_rise, -1.0, 0, 0);
						glTranslated(-0.25, -0.8, -0.3);
						drawBox(0.5, 0.9, 0.6);
					}

					//Staff
					if (level_of_detail > 1) {
						glPushMatrix();
						{
							glTranslated(0.2, 0.2, -0.9);
							setDiffuseColor(COLOR_PURPLE);
							{
								glTranslated(0, 0, 1.3);
								glRotated(staff_angle, 0, 1.0, 0);
								glTranslated(0, 0, -1.3);
							}
							drawCylinder(4, 0.1, 0.1);

							setDiffuseColor(COLOR_LIGHT_ORANGE);
							//Orb
							if (level_of_detail > 3) {
								glPushMatrix();
								{
									//Rotate around a different axis
									glRotated(rotate_orb, 0, 0, 1.0);
									glTranslated(0.2, 0, 0);
									glTranslated(-0.1, -0.1, 4.1 + breath_orb);
									//Cube for now, meatball later
									drawBox(0.2, 0.2, 0.2);
								}
								glPopMatrix();
							}
							setDiffuseColor(COLOR_GREEN);
						}
						glPopMatrix();
					}
				}
				glPopMatrix();
			}
			glPopMatrix();
		}
		glPopMatrix();
	}
	glPopMatrix();
}

int main()
{
	// Initialize the controls
	// Constructor is ModelerControl(name, minimumvalue, maximumvalue, 
	// stepsize, defaultvalue)
	ModelerControl controls[MY_NUM_CONTROLS];
	controls[TORSO_XPOS] = ModelerControl("X Position", -5, 5, 0.1f, 0);
	controls[TORSO_YPOS] = ModelerControl("Y Position", 0, 5, 0.1f, 0);
	controls[TORSO_ZPOS] = ModelerControl("Z Position", -5, 5, 0.1f, 0);
	controls[TORSO_TURN] = ModelerControl("Rotation", 0, 359, 1, 0);

	controls[HEAD_LOOK_X] = ModelerControl("Head Look X", -20, 20, 1, 0);
	controls[HEAD_LOOK_Y] = ModelerControl("Head Look Y", -60, 60, 1, 0);

	controls[SHIELD_RISE] = ModelerControl("Shield rise", 0, 90, 1, 0);

	controls[LEVEL_OF_DETAIL] = ModelerControl("Level of detail", 1, 4, 1, 4);

	controls[CAPE_WAVE] = ModelerControl("Cape's wave", 0.3, 20, 0.1, 0.5);

	//L-System
	controls[LS_DEGREE] = ModelerControl("LS - Degree", 0, 180, 1, 27);
	controls[LS_DECAY] = ModelerControl("LS - Decay", 0, 1.0, 0.01, 1.0);
	controls[LS_LENGTH] = ModelerControl("LS - Length", 0.01, 0.1, 0.01, 0.05);
	controls[LS_ITERATION] = ModelerControl("LS - Iteration", 1, 8, 1, 5);
	controls[LS_SEED] = ModelerControl("LS - Random Seed", 0, 100, 1, 0);
	controls[LS_DENSITY] = ModelerControl("LS - Density", 0, 1.0, 0.01, 0.9);

	//Forward, debug only
	controls[FK_UPPER_ARM] = ModelerControl("FK - Upper arm", -90, 180, 1, 0);
	controls[FK_LOWER_ARM] = ModelerControl("FK - Lower arm", -90, 90, 1, 0);
	controls[FK_STAFF_ANGLE] = ModelerControl("FK - Staff", -90, 90, 1, 0);

	ModelerApplication::Instance()->Init(&createSpellBreaker, controls, MY_NUM_CONTROLS);
	return ModelerApplication::Instance()->Run();
}

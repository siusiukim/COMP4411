#include "modelerview.h"
#include "modelerapp.h"
#include "modelerdraw.h"
#include <FL/gl.h>

#include "spellbreaker_globals.h"

class SpellBreaker : public ModelerView
{
private:
	double head_look_x, head_look_y;
	double left_upper_arm_rise, left_lower_arm_rise;
public:
	SpellBreaker(int x, int y, int w, int h, char *label)
		: ModelerView(x, y, w, h, label) { }

	virtual void draw();
	void updateParam();
};

ModelerView* createSpellBreaker(int x, int y, int w, int h, char *label)
{
	return new SpellBreaker(x, y, w, h, label);
}

void SpellBreaker::updateParam() {
	//Direct control
	head_look_x = VAL(HEAD_LOOK_X);
	head_look_y = VAL(HEAD_LOOK_Y);

	//Indirect control
	left_upper_arm_rise = VAL(SHIELD_RISE);
	left_lower_arm_rise = VAL(SHIELD_RISE)/2;
}

void SpellBreaker::draw()
{
	ModelerView::draw();

	// Draw the floor
	setAmbientColor(.1f, .1f, .1f);
	setDiffuseColor(COLOR_RED);
	glPushMatrix();
	{
		glTranslated(-5, 0, -5);
		drawBox(10, 0.01f, 10);
	}
	glPopMatrix();

	// Draw the spellbreaker model
	setAmbientColor(.1f, .1f, .1f);
	setDiffuseColor(COLOR_GREEN);

	updateParam();

	//Entire model
	glPushMatrix();
	{
		//Translation of the entire model
		glTranslated(VAL(XPOS), VAL(YPOS), VAL(ZPOS));

		//Torso
		glPushMatrix();
		{
			glTranslated(-0.75, 1, 0);
			drawBox(1.5, 2, 1);

			//Head
			glPushMatrix();
			{
				glTranslated(0.25, 2, 0.1);
				{
					//Rotate around neck position
					glTranslated(0.5, 0, 0);
					glRotated(head_look_x, 1.0, 0, 0);
					glRotated(head_look_y, 0, 1.0, 0);
					glTranslated(-0.5, 0, 0);
				}
				drawBox(1, 1, 0.8);

				setDiffuseColor(COLOR_RED);
				//Left eye
				glPushMatrix();
				{
					glTranslated(0.3, 0.7, 0.8);
					drawBox(0.15, 0.15, 0.1);
				}
				glPopMatrix();

				//Right eye
				glPushMatrix();
				{
					glTranslated(0.6, 0.7, 0.8);
					drawBox(0.15, 0.15, 0.1);
				}
				glPopMatrix();

				//glRotated(VAL(ROTATE), 0.0, 1.0, 0.0);
				//glRotated(-90, 1.0, 0.0, 0.0);
				//drawCylinder(VAL(HEIGHT), 0.1, 0.1);

				//glTranslated(0.0, 0.0, VAL(HEIGHT));
				//drawCylinder(1, 1.0, 0.9);

				//glTranslated(0.0, 0.0, 0.5);
				//glRotated(90, 1.0, 0.0, 0.0);
				//drawCylinder(4, 0.1, 0.2);
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
				glTranslated(-0.55, 1, 0.25);
				{
					//Rotate around near-top center of upper arm
					glTranslated(0.25, 0.8, 0.3);
					glRotated(left_upper_arm_rise, -1.0, 0, 0);
					glTranslated(-0.25, -0.8, -0.3);
				}
				drawBox(0.5, 0.9, 0.6);

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
					glPushMatrix();
					{
						setDiffuseColor(COLOR_BLUE);
						glTranslated(-0.25, -0.8, -1.5);
						drawBox(0.2, 1.8, 3);
						setDiffuseColor(COLOR_GREEN);
					}
					glPopMatrix();
				}
				glPopMatrix();
			}
			glPopMatrix();

			//Upper right arm
			glPushMatrix();
			{
				glTranslated(1.55, 1, 0.25);
				drawBox(0.5, 0.9, 0.6);

				//Lower right arm
				glPushMatrix();
				{
					glTranslated(0, -1, 0);
					drawBox(0.5, 0.9, 0.6);
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
	ModelerControl controls[NUMCONTROLS];
	controls[TORSO_XPOS] = ModelerControl("X Position", -5, 5, 0.1f, 0);
	controls[TORSO_YPOS] = ModelerControl("Y Position", 0, 5, 0.1f, 0);
	controls[TORSO_ZPOS] = ModelerControl("Z Position", -5, 5, 0.1f, 0);

	controls[HEAD_LOOK_X] = ModelerControl("Head Look X", -20, 20, 1, 0);
	controls[HEAD_LOOK_Y] = ModelerControl("Head Look Y", -60, 60, 1, 0);

	controls[SHIELD_RISE] = ModelerControl("Shield rise", 0, 90, 1, 0);

	ModelerApplication::Instance()->Init(&createSpellBreaker, controls, MY_NUM_CONTROLS);
	return ModelerApplication::Instance()->Run();
}

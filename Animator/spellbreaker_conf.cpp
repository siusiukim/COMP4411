#include "modelerview.h"
#include "modelerapp.h"
#include "modelerdraw.h"
#include "modelerui.h"
#include <FL/gl.h>

#include "bitmap.h"
#include "spellbreaker_globals.h"

void SpellBreaker::initTexture() {
	//Load the texture file
	armor_bitmap = readBMP("logo_eye.bmp", armor_width, armor_height);
	diamond_bitmap = readBMP("diamond.bmp", diamond_width, diamond_height);

	//Create the texture from the bitmap
	glGenTextures(1, &armor_texture_id);
	glBindTexture(GL_TEXTURE_2D, armor_texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, armor_width, armor_height, 0,
		GL_BGR_EXT, GL_UNSIGNED_BYTE, armor_bitmap);

	glGenTextures(1, &diamond_texture_id);
	glBindTexture(GL_TEXTURE_2D, diamond_texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, diamond_width, diamond_height, 0,
		GL_BGR_EXT, GL_UNSIGNED_BYTE, diamond_bitmap);

	//Just init along the way
	right_upper_arm_rise = 0;
	right_lower_arm_rise = 0;
	staff_angle = 0;
}

void SpellBreaker::updateParam() {
	//Direct control
	head_look_x = VAL(HEAD_LOOK_X);
	head_look_y = VAL(HEAD_LOOK_Y);

	//Indirect control
	left_upper_arm_rise = VAL(SHIELD_RISE);
	left_lower_arm_rise = VAL(SHIELD_RISE) / 2;

	level_of_detail = VAL(LEVEL_OF_DETAIL);
	cape_wave = VAL(CAPE_WAVE);

	//IK control
	//right_upper_arm_rise = VAL(FK_UPPER_ARM);
	//right_lower_arm_rise = VAL(FK_LOWER_ARM);;
	//staff_angle = VAL(FK_STAFF_ANGLE);

	//Compute IK
	//doFK();
	doIK();

	//if ((ModelerUserInterface::m_controlsAnimOnMenu->value() == 0)) {
		animation_counter = 0;

		//Update from toolbar
		cape_wave = VAL(CAPE_WAVE);
	//}
	//else {
	//	animation_counter = (animation_counter + 1) % 200;

	//	//Update from animation
	//	cape_wave = (1/(abs(cos(animation_counter / 100.0 * 2 * M_PI)) + 0.001f)) * 20 + 0.5;
	//}

	//Animate
	breath_y = (sin(animation_counter / 100.0 * 2 * M_PI) + 0.8) / 10;
	breath_head = (sin(animation_counter / 100.0 * M_PI)) * 30;
	breath_orb = (sin(animation_counter / 100.0 * 4 * M_PI)) / 10;
	rotate_orb = (sin(animation_counter / 100.0 * M_PI)) * 360;
}

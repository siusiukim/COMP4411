#ifndef _SPELL_BREAKER_GLOBALS
#define _SPELL_BREAKER_GLOBALS

#ifndef M_PI
#define M_PI 3.141592653589793238462643383279502
#endif

// This is a list of the controls for the SampleModel
// We'll use these constants to access the values 
// of the controls from the user interface.
enum SpellBreakerModelControl
{ 
	TORSO_XPOS,
	TORSO_YPOS,
	TORSO_ZPOS,
	TORSO_TURN,
	HEAD_LOOK_X,
	HEAD_LOOK_Y,
	SHIELD_RISE,
	LEVEL_OF_DETAIL,
	MY_NUM_CONTROLS
};

// Colors
#define COLOR_RED		1.0f, 0.0f, 0.0f
#define COLOR_GREEN		0.0f, 1.0f, 0.0f
#define COLOR_BLUE		0.0f, 0.0f, 1.0f
#define COLOR_WHITE		1.0f, 1.0f, 1.0f

// We'll be getting the instance of the application a lot; 
// might as well have it as a macro.
#define VAL(x) (ModelerApplication::Instance()->GetControlValue(x))

class SpellBreaker : public ModelerView
{
private:
	double head_look_x, head_look_y;
	double left_upper_arm_rise, left_lower_arm_rise;

	/**
		4 - all
		3 - no orb
		2 - no eyes
		1 - no shield
	*/
	int level_of_detail;
	int animation_counter;
	bool bitmap_loaded;

	//Armor texture
	unsigned char* armor_bitmap;
	int armor_width, armor_height;
	GLuint armor_texture_id;
public:
	SpellBreaker(int x, int y, int w, int h, char *label)
		: ModelerView(x, y, w, h, label),
		bitmap_loaded(false), 
		animation_counter(0) {}

	virtual void draw();
	void updateParam();
	void initTexture();
};

#endif
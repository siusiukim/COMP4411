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
	HEAD_LOOK_X,
	HEAD_LOOK_Y,
	SHIELD_RISE,
	MY_NUM_CONTROLS
};

// Colors
#define COLOR_RED		1.0f, 0.0f, 0.0f
#define COLOR_GREEN		0.0f, 1.0f, 0.0f
#define COLOR_BLUE		0.0f, 0.0f, 1.0f

// We'll be getting the instance of the application a lot; 
// might as well have it as a macro.
#define VAL(x) (ModelerApplication::Instance()->GetControlValue(x))

#endif
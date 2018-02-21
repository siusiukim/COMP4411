// Code to construct the UI

#include <FL/fl_ask.h>

#include <math.h>

#include "impressionistUI.h"
#include "impressionistDoc.h"

#include "MathUtil.hpp"

/*
//------------------------------ Widget Examples -------------------------------------------------
Here is some example code for all of the widgets that you may need to add to the
project.  You can copy and paste these into your code and then change them to
make them look how you want.  Descriptions for all of the widgets here can be found
in links on the fltk help session page.

//---------Window/Dialog and Menubar-----------------------------------

//----To install a window--------------------------
Fl_Window* myWindow = new Fl_Window(600, 300, "MyWindow");
myWindow->user_data((void*)(this));	// record self to be used by static callback functions

// install menu bar
myMenubar = new Fl_Menu_Bar(0, 0, 600, 25);
Fl_Menu_Item ImpressionistUI::myMenuItems[] = {
{ "&File",		0, 0, 0, FL_SUBMENU },
{ "&Load...",	FL_ALT + 'l', (Fl_Callback *)ImpressionistUI::cb_load },
{ "&Save...",	FL_ALT + 's', (Fl_Callback *)ImpressionistUI::cb_save }.
{ "&Quit",			FL_ALT + 'q', (Fl_Callback *)ImpressionistUI::cb_exit },
{ 0 },
{ "&Edit",		0, 0, 0, FL_SUBMENU },
{ "&Copy",FL_ALT + 'c', (Fl_Callback *)ImpressionistUI::cb_copy, (void *)COPY },
{ "&Cut",	FL_ALT + 'x', (Fl_Callback *)ImpressionistUI::cb_cut, (void *)CUT },
{ "&Paste",	FL_ALT + 'v', (Fl_Callback *)ImpressionistUI::cb_paste, (void *)PASTE },
{ 0 },
{ "&Help",		0, 0, 0, FL_SUBMENU },
{ "&About",	FL_ALT + 'a', (Fl_Callback *)ImpressionistUI::cb_about },
{ 0 },
{ 0 }
};
myMenubar->menu(myMenuItems);
m_mainWindow->end();

//----The window callback--------------------------
// One of the callbacks
void ImpressionistUI::cb_load(Fl_Menu_* o, void* v)
{
ImpressionistDoc *pDoc=whoami(o)->getDocument();

char* newfile = fl_file_chooser("Open File?", "*.bmp", pDoc->getImageName() );
if (newfile != NULL) {
pDoc->loadImage(newfile);
}
}


//------------Slider---------------------------------------

//----To install a slider--------------------------
Fl_Value_Slider * mySlider = new Fl_Value_Slider(10, 80, 300, 20, "My Value");
mySlider->user_data((void*)(this));	// record self to be used by static callback functions
mySlider->type(FL_HOR_NICE_SLIDER);
mySlider->labelfont(FL_COURIER);
mySlider->labelsize(12);
mySlider->minimum(1);
mySlider->maximum(40);
mySlider->step(1);
mySlider->value(m_nMyValue);
mySlider->align(FL_ALIGN_RIGHT);
mySlider->callback(cb_MyValueSlides);

//----The slider callback--------------------------
void ImpressionistUI::cb_MyValueSlides(Fl_Widget* o, void* v)
{
((ImpressionistUI*)(o->user_data()))->m_nMyValue=int( ((Fl_Slider *)o)->value() ) ;
}


//------------Choice---------------------------------------

//----To install a choice--------------------------
Fl_Choice * myChoice = new Fl_Choice(50,10,150,25,"&myChoiceLabel");
myChoice->user_data((void*)(this));	 // record self to be used by static callback functions
Fl_Menu_Item ImpressionistUI::myChoiceMenu[3+1] = {
{"one",FL_ALT+'p', (Fl_Callback *)ImpressionistUI::cb_myChoice, (void *)ONE},
{"two",FL_ALT+'l', (Fl_Callback *)ImpressionistUI::cb_myChoice, (void *)TWO},
{"three",FL_ALT+'c', (Fl_Callback *)ImpressionistUI::cb_myChoice, (void *)THREE},
{0}
};
myChoice->menu(myChoiceMenu);
myChoice->callback(cb_myChoice);

//-----The choice callback-------------------------
void ImpressionistUI::cb_myChoice(Fl_Widget* o, void* v)
{
ImpressionistUI* pUI=((ImpressionistUI *)(o->user_data()));
ImpressionistDoc* pDoc=pUI->getDocument();

int type=(int)v;

pDoc->setMyType(type);
}


//------------Button---------------------------------------

//---To install a button---------------------------
Fl_Button* myButton = new Fl_Button(330,220,50,20,"&myButtonLabel");
myButton->user_data((void*)(this));   // record self to be used by static callback functions
myButton->callback(cb_myButton);

//---The button callback---------------------------
void ImpressionistUI::cb_myButton(Fl_Widget* o, void* v)
{
ImpressionistUI* pUI=((ImpressionistUI*)(o->user_data()));
ImpressionistDoc* pDoc = pUI->getDocument();
pDoc->startPainting();
}


//---------Light Button------------------------------------

//---To install a light button---------------------
Fl_Light_Button* myLightButton = new Fl_Light_Button(240,10,150,25,"&myLightButtonLabel");
myLightButton->user_data((void*)(this));   // record self to be used by static callback functions
myLightButton->callback(cb_myLightButton);

//---The light button callback---------------------
void ImpressionistUI::cb_myLightButton(Fl_Widget* o, void* v)
{
ImpressionistUI *pUI=((ImpressionistUI*)(o->user_data()));

if (pUI->myBool==TRUE) pUI->myBool=FALSE;
else pUI->myBool=TRUE;
}

//----------Int Input--------------------------------------

//---To install an int input-----------------------
Fl_Int_Input* myInput = new Fl_Int_Input(200, 50, 5, 5, "&My Input");
myInput->user_data((void*)(this));   // record self to be used by static callback functions
myInput->callback(cb_myInput);

//---The int input callback------------------------
void ImpressionistUI::cb_myInput(Fl_Widget* o, void* v)
{
((ImpressionistUI*)(o->user_data()))->m_nMyInputValue=int( ((Fl_Int_Input *)o)->value() );
}

//------------------------------------------------------------------------------------------------
*/

// Main menu definition
Fl_Menu_Item ImpressionistUI::menuitems[] = {
	{ "&File",		0, 0, 0, FL_SUBMENU },
	{ "&Load Image...",	FL_ALT + 'l', (Fl_Callback *)ImpressionistUI::cb_load_image },
	{ "&Save Image...",	FL_ALT + 's', (Fl_Callback *)ImpressionistUI::cb_save_image },
	{ "&Brushes...",	FL_ALT + 'b', (Fl_Callback *)ImpressionistUI::cb_brushes },
	{ "&Filter...",	FL_ALT + 'f', (Fl_Callback *)ImpressionistUI::cb_filters },
	{ "&Clear Canvas", FL_ALT + 'c', (Fl_Callback *)ImpressionistUI::cb_clear_canvas, 0, FL_MENU_DIVIDER },

	{ "&Quit",			FL_ALT + 'q', (Fl_Callback *)ImpressionistUI::cb_exit },
	{ 0 },

	{ "&Help",		0, 0, 0, FL_SUBMENU },
	{ "&About",	FL_ALT + 'a', (Fl_Callback *)ImpressionistUI::cb_about },
	{ 0 },

	{ 0 }
};

// Brush choice menu definition
Fl_Menu_Item ImpressionistUI::brushTypeMenu[NUM_BRUSH_TYPE + 1] = {
	{ "Points",			FL_ALT + 'p', (Fl_Callback *)ImpressionistUI::cb_brushChoice, (void *)BRUSH_POINTS },
	{ "Lines",				FL_ALT + 'l', (Fl_Callback *)ImpressionistUI::cb_brushChoice, (void *)BRUSH_LINES },
	{ "Circles",			FL_ALT + 'c', (Fl_Callback *)ImpressionistUI::cb_brushChoice, (void *)BRUSH_CIRCLES },
	{ "Scattered Points",	FL_ALT + 'q', (Fl_Callback *)ImpressionistUI::cb_brushChoice, (void *)BRUSH_SCATTERED_POINTS },
	{ "Scattered Lines",	FL_ALT + 'm', (Fl_Callback *)ImpressionistUI::cb_brushChoice, (void *)BRUSH_SCATTERED_LINES },
	{ "Scattered Circles",	FL_ALT + 'd', (Fl_Callback *)ImpressionistUI::cb_brushChoice, (void *)BRUSH_SCATTERED_CIRCLES },
	{ 0 }
};

// Brush direction menu definition
Fl_Menu_Item ImpressionistUI::brushDirectionMenu[NUM_DIRECTION_TYPE + 1] = {
	{ "Slider/Right mouse",		FL_ALT + 'p', (Fl_Callback *)ImpressionistUI::cb_dirTypeChoice, (void *)DIRECTION_BY_SLIDER_OR_RIGHT },
	{ "Cursor movement",		FL_ALT + 'l', (Fl_Callback *)ImpressionistUI::cb_dirTypeChoice, (void *)DIRECTION_BY_MOVEMENT },
	{ "Gradient",				FL_ALT + 'c', (Fl_Callback *)ImpressionistUI::cb_dirTypeChoice, (void *)DIRECTION_BY_GRADIENT },
	{ 0 }
};



//----------------------------------------------------
// Constructor.  Creates all of the widgets.
// Add new widgets here
//----------------------------------------------------
ImpressionistUI::ImpressionistUI() {
	// Create the main window
	m_mainWindow = new Fl_Window(600, 300, "Impressionist");
	m_mainWindow->user_data((void*)(this));	// record self to be used by static callback functions
											// install menu bar
	m_menubar = new Fl_Menu_Bar(0, 0, 600, 25);
	m_menubar->menu(menuitems);

	// Create a group that will hold two sub windows inside the main
	// window
	Fl_Group* group = new Fl_Group(0, 25, 600, 275);

	// install paint view window
	m_paintView = new PaintView(300, 25, 300, 275, "This is the paint view");//0jon
	m_paintView->box(FL_DOWN_FRAME);

	// install original view window
	m_origView = new OriginalView(0, 25, 300, 275, "This is the orig view");//300jon
	m_origView->box(FL_DOWN_FRAME);
	m_origView->deactivate();

	group->end();
	Fl_Group::current()->resizable(group);
	m_mainWindow->end();

	// init values

	m_nSize = 10;
	m_thickness = 1;
	m_angle = 0;
	m_opacity = 1;
	m_direction = DIRECTION_BY_SLIDER_OR_RIGHT;
	m_spacing = MIN_SPACING;
	m_normalize = false;
	m_learnNumber = 20;
	m_iterNumber = 1000;
	m_threshold = 100;

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			m_filterValue[i][j] = 1;
		}
	}

	// brush dialog definition
	m_brushDialog = new Fl_Window(400, 425, "Brush Dialog");

	// Add a brush type choice to the dialog
	m_BrushTypeChoice = new Fl_Choice(50, 10, 150, 25, "&Brush");
	m_BrushTypeChoice->user_data((void*)(this));	// record self to be used by static callback functions
	m_BrushTypeChoice->menu(brushTypeMenu);
	//m_BrushTypeChoice->callback(cb_brushChoice);

	m_DirectionTypeChoice = new Fl_Choice(50, 30, 150, 25, "&Dir");
	m_DirectionTypeChoice->user_data((void*)(this));	// record self to be used by static callback functions
	m_DirectionTypeChoice->menu(brushDirectionMenu);
	//m_DirectionTypeChoice->callback(cb_dirTypeChoice);
	m_DirectionTypeChoice->deactivate();

	m_ClearCanvasButton = new Fl_Button(240, 10, 150, 25, "&Clear Canvas");
	m_ClearCanvasButton->user_data((void*)(this));
	m_ClearCanvasButton->callback(cb_clear_canvas_button);

	// Add brush size slider to the dialog 
	m_BrushSizeSlider = new Fl_Value_Slider(10, 80, 300, 20, "Size");
	m_BrushSizeSlider->user_data((void*)(this));	// record self to be used by static callback functions
	m_BrushSizeSlider->type(FL_HOR_NICE_SLIDER);
	m_BrushSizeSlider->labelfont(FL_COURIER);
	m_BrushSizeSlider->labelsize(12);
	m_BrushSizeSlider->minimum(MIN_POINT_SIZE);
	m_BrushSizeSlider->maximum(MAX_POINT_SIZE);
	m_BrushSizeSlider->step(1);
	m_BrushSizeSlider->value(m_nSize);
	m_BrushSizeSlider->align(FL_ALIGN_RIGHT);
	m_BrushSizeSlider->callback(cb_sizeSlides);

	// Line width (thickness) slider
	m_BrushThicknessSlider = new Fl_Value_Slider(10, 110, 300, 20, "Line width");
	m_BrushThicknessSlider->user_data((void*)(this));	// record self to be used by static callback functions
	m_BrushThicknessSlider->type(FL_HOR_NICE_SLIDER);
	m_BrushThicknessSlider->labelfont(FL_COURIER);
	m_BrushThicknessSlider->labelsize(12);
	m_BrushThicknessSlider->minimum(MIN_THICKNESS);
	m_BrushThicknessSlider->maximum(MAX_THICKNESS);
	m_BrushThicknessSlider->step(1);
	m_BrushThicknessSlider->value(m_thickness);
	m_BrushThicknessSlider->align(FL_ALIGN_RIGHT);
	m_BrushThicknessSlider->callback(cb_thicknessSlides);
	m_BrushThicknessSlider->deactivate();

	// Angle slider
	m_BrushAngleSlider = new Fl_Value_Slider(10, 140, 300, 20, "Line Angle");
	m_BrushAngleSlider->user_data((void*)(this));	// record self to be used by static callback functions
	m_BrushAngleSlider->type(FL_HOR_NICE_SLIDER);
	m_BrushAngleSlider->labelfont(FL_COURIER);
	m_BrushAngleSlider->labelsize(12);
	m_BrushAngleSlider->minimum(MIN_ANGLE);
	m_BrushAngleSlider->maximum(MAX_ANGLE);
	m_BrushAngleSlider->step(1);
	m_BrushAngleSlider->value(m_angle);
	m_BrushAngleSlider->align(FL_ALIGN_RIGHT);
	m_BrushAngleSlider->callback(cb_angleSlides);
	m_BrushAngleSlider->deactivate();

	// Opacity slider
	m_BrushOpacitySlider = new Fl_Value_Slider(10, 170, 300, 20, "Opacity");
	m_BrushOpacitySlider->user_data((void*)(this));	// record self to be used by static callback functions
	m_BrushOpacitySlider->type(FL_HOR_NICE_SLIDER);
	m_BrushOpacitySlider->labelfont(FL_COURIER);
	m_BrushOpacitySlider->labelsize(12);
	m_BrushOpacitySlider->minimum(MIN_OPACITY);
	m_BrushOpacitySlider->maximum(MAX_OPACITY);
	m_BrushOpacitySlider->step(0.01);
	m_BrushOpacitySlider->value(m_opacity);
	m_BrushOpacitySlider->align(FL_ALIGN_RIGHT);
	m_BrushOpacitySlider->callback(cb_opacitySlides);

	// Spacing slider
	m_BrushSpacingSlider = new Fl_Value_Slider(10, 200, 200, 20, "Spacing");
	m_BrushSpacingSlider->user_data((void*)(this));	// record self to be used by static callback functions
	m_BrushSpacingSlider->type(FL_HOR_NICE_SLIDER);
	m_BrushSpacingSlider->labelfont(FL_COURIER);
	m_BrushSpacingSlider->labelsize(12);
	m_BrushSpacingSlider->minimum(MIN_SPACING);
	m_BrushSpacingSlider->maximum(MAX_SPACING);
	m_BrushSpacingSlider->step(1);
	m_BrushSpacingSlider->value(m_spacing);
	m_BrushSpacingSlider->align(FL_ALIGN_RIGHT);
	m_BrushSpacingSlider->callback(cb_spacingSlides);

	m_AutoPaintButton = new Fl_Button(280, 200, 50, 20, "Paint");
	m_AutoPaintButton->user_data((void*)(this));
	m_AutoPaintButton->callback(autoDrawAction);

	m_CopyCanvasButton = new Fl_Button(340, 200, 50, 20, "Copy");
	m_CopyCanvasButton->user_data((void*)(this));
	m_CopyCanvasButton->callback(cb_copy_canvas);

	m_ThresholdSlider = new Fl_Value_Slider(10, 225, 200, 20, "Threshold");
	m_ThresholdSlider->user_data((void*)(this));	// record self to be used by static callback functions
	m_ThresholdSlider->type(FL_HOR_NICE_SLIDER);
	m_ThresholdSlider->labelfont(FL_COURIER);
	m_ThresholdSlider->labelsize(12);
	m_ThresholdSlider->minimum(MIN_THRESHOLD);
	m_ThresholdSlider->maximum(MAX_THRESHOLD);
	m_ThresholdSlider->step(1);
	m_ThresholdSlider->value(m_threshold);
	m_ThresholdSlider->align(FL_ALIGN_RIGHT);
	m_ThresholdSlider->callback(cb_thresholdSlides);

	m_CopyCanvasButton = new Fl_Button(280, 225, 110, 20, "Multi-res Paint");
	m_CopyCanvasButton->user_data((void*)(this));
	m_CopyCanvasButton->callback(autoMultiDrawAction);

	// Learning slider
	m_LearnNumberSlider = new Fl_Value_Slider(10, 250, 200, 20, "Blocks");
	m_LearnNumberSlider->user_data((void*)(this));	// record self to be used by static callback functions
	m_LearnNumberSlider->type(FL_HOR_NICE_SLIDER);
	m_LearnNumberSlider->labelfont(FL_COURIER);
	m_LearnNumberSlider->labelsize(12);
	m_LearnNumberSlider->minimum(MIN_LEARN);
	m_LearnNumberSlider->maximum(MAX_LEARN);
	m_LearnNumberSlider->step(1);
	m_LearnNumberSlider->value(m_learnNumber);
	m_LearnNumberSlider->align(FL_ALIGN_RIGHT);
	m_LearnNumberSlider->callback(cb_learnNumberSlides);

	m_IterNumberSlider = new Fl_Value_Slider(10, 280, 200, 20, "Iter");
	m_IterNumberSlider->user_data((void*)(this));	// record self to be used by static callback functions
	m_IterNumberSlider->type(FL_HOR_NICE_SLIDER);
	m_IterNumberSlider->labelfont(FL_COURIER);
	m_IterNumberSlider->labelsize(12);
	m_IterNumberSlider->minimum(MIN_ITER);
	m_IterNumberSlider->maximum(MAX_ITER);
	m_IterNumberSlider->step(1);
	m_IterNumberSlider->value(m_iterNumber);
	m_IterNumberSlider->align(FL_ALIGN_RIGHT);
	m_IterNumberSlider->callback(cb_iterNumberSlides);

	m_AutoLearnButton = new Fl_Button(300, 250, 100, 20, "Auto relax");
	m_AutoLearnButton->user_data((void*)(this));
	m_AutoLearnButton->callback(cb_autoLearnDrawing);

	m_brushDialog->end();

	m_filterDialog = new Fl_Window(400, 325, "Filter Dialog");

	m_ApplyFilterButton = new Fl_Button(10, 10, 100, 25, "Apply filter");
	m_ApplyFilterButton->user_data((void*)(this));
	m_ApplyFilterButton->callback(cb_applyFilter);

	m_NormalizeSwitch = new Fl_Light_Button(150, 10, 15, 20, "Normalize");
	m_NormalizeSwitch->user_data((void*)(this));
	m_NormalizeSwitch->callback(cb_normalizeSwitch);

	m_FilterInput[0][0] = new Fl_Int_Input(100, 50, 25, 25, "[0][0]");
	m_FilterInput[0][0]->user_data((void*)(this));   // record self to be used by static callback functions
	m_FilterInput[0][0]->callback(cb_filterInput_00);

	m_FilterInput[0][1] = new Fl_Int_Input(100, 150, 25, 25, "[0][1]");
	m_FilterInput[0][1]->user_data((void*)(this));   // record self to be used by static callback functions
	m_FilterInput[0][1]->callback(cb_filterInput_01);

	m_FilterInput[0][2] = new Fl_Int_Input(100, 250, 25, 25, "[0][2]");
	m_FilterInput[0][2]->user_data((void*)(this));   // record self to be used by static callback functions
	m_FilterInput[0][2]->callback(cb_filterInput_02);

	m_FilterInput[1][0] = new Fl_Int_Input(200, 50, 25, 25, "[1][0]");
	m_FilterInput[1][0]->user_data((void*)(this));   // record self to be used by static callback functions
	m_FilterInput[1][0]->callback(cb_filterInput_10);

	m_FilterInput[1][1] = new Fl_Int_Input(200, 150, 25, 25, "[1][1]");
	m_FilterInput[1][1]->user_data((void*)(this));   // record self to be used by static callback functions
	m_FilterInput[1][1]->callback(cb_filterInput_11);

	m_FilterInput[1][2] = new Fl_Int_Input(200, 250, 25, 25, "[1][2]");
	m_FilterInput[1][2]->user_data((void*)(this));   // record self to be used by static callback functions
	m_FilterInput[1][2]->callback(cb_filterInput_12);

	m_FilterInput[2][0] = new Fl_Int_Input(300, 50, 25, 25, "[2][0]");
	m_FilterInput[2][0]->user_data((void*)(this));   // record self to be used by static callback functions
	m_FilterInput[2][0]->callback(cb_filterInput_20);

	m_FilterInput[2][1] = new Fl_Int_Input(300, 150, 25, 25, "[2][1]");
	m_FilterInput[2][1]->user_data((void*)(this));   // record self to be used by static callback functions
	m_FilterInput[2][1]->callback(cb_filterInput_21);

	m_FilterInput[2][2] = new Fl_Int_Input(300, 250, 25, 25, "[2][2]");
	m_FilterInput[2][2]->user_data((void*)(this));   // record self to be used by static callback functions
	m_FilterInput[2][2]->callback(cb_filterInput_22);

	m_filterDialog->end();
}


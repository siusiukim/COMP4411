//
// impressionistUI.h
//
// The user interface part for the program.
//


#include <FL/fl_ask.h>

#include <math.h>

#include "impressionistUI.h"
#include "impressionistDoc.h"

#include "MathUtil.hpp"

//------------------------------------------------------------
// This returns the UI, given the menu item.  It provides a
// link from the menu items to the UI
//------------------------------------------------------------
ImpressionistUI* ImpressionistUI::whoami(Fl_Menu_* o)
{
	return ((ImpressionistUI*)(o->parent()->user_data()));
}

//------------------------------------------------------------------
// Brings up a file chooser and then loads the chosen image
// This is called by the UI when the load image menu item is chosen
//------------------------------------------------------------------
void ImpressionistUI::cb_load_image(Fl_Menu_* o, void* v)
{
	ImpressionistDoc *pDoc = whoami(o)->getDocument();

	char* newfile = fl_file_chooser("Open File?", "*.bmp", pDoc->getImageName());
	if (newfile != NULL) {
		pDoc->loadImage(newfile);
	}
}

//------------------------------------------------------------------
// Brings up a file chooser and then saves the painted image
// This is called by the UI when the save image menu item is chosen
//------------------------------------------------------------------
void ImpressionistUI::cb_save_image(Fl_Menu_* o, void* v)
{
	ImpressionistDoc *pDoc = whoami(o)->getDocument();

	char* newfile = fl_file_chooser("Save File?", "*.bmp", "save.bmp");
	if (newfile != NULL) {
		pDoc->saveImage(newfile);
	}
}

//-------------------------------------------------------------
// Brings up the paint dialog
// This is called by the UI when the brushes menu item
// is chosen
//-------------------------------------------------------------
void ImpressionistUI::cb_brushes(Fl_Menu_* o, void* v)
{
	whoami(o)->m_brushDialog->show();
}

//------------------------------------------------------------
// Clears the paintview canvas.
// Called by the UI when the clear canvas menu item is chosen
//------------------------------------------------------------
void ImpressionistUI::cb_clear_canvas(Fl_Menu_* o, void* v)
{
	ImpressionistDoc* pDoc = whoami(o)->getDocument();

	pDoc->clearCanvas();
}

//------------------------------------------------------------
// Causes the Impressionist program to exit
// Called by the UI when the quit menu item is chosen
//------------------------------------------------------------
void ImpressionistUI::cb_exit(Fl_Menu_* o, void* v)
{
	whoami(o)->m_mainWindow->hide();
	whoami(o)->m_brushDialog->hide();
}


//-----------------------------------------------------------
// Brings up an about dialog box
// Called by the UI when the about menu item is chosen
//-----------------------------------------------------------
void ImpressionistUI::cb_about(Fl_Menu_* o, void* v)
{
	fl_message("Impressionist FLTK version for COMP4411, Spring 2018, Work By Rex Cheng");
}

//------- UI should keep track of the current for all the controls for answering the query from Doc ---------
//-------------------------------------------------------------
// Sets the type of brush to use to the one chosen in the brush 
// choice.  
// Called by the UI when a brush is chosen in the brush choice
//-------------------------------------------------------------
void ImpressionistUI::cb_brushChoice(Fl_Widget* o, void* v)
{
	ImpressionistUI* pUI = ((ImpressionistUI *)(o->user_data()));
	ImpressionistDoc* pDoc = pUI->getDocument();

	int type = (int)v;

	if (type == BRUSH_LINES || type == BRUSH_SCATTERED_LINES) {
		//Enable thickness and angle in line mode only
		pUI->m_BrushThicknessSlider->activate();
		pUI->m_BrushAngleSlider->activate();
		pUI->m_DirectionTypeChoice->activate();
	}
	else {
		pUI->m_BrushThicknessSlider->deactivate();
		pUI->m_BrushAngleSlider->deactivate();
		pUI->m_DirectionTypeChoice->deactivate();
	}

	pDoc->setBrushType(type);
}

void ImpressionistUI::cb_dirTypeChoice(Fl_Widget* o, void* v)
{
	ImpressionistUI* pUI = ((ImpressionistUI *)(o->user_data()));
	ImpressionistDoc* pDoc = pUI->getDocument();

	pUI->setDirectionType ((int)v);
}

//------------------------------------------------------------
// Clears the paintview canvas.
// Called by the UI when the clear canvas button is pushed
//------------------------------------------------------------
void ImpressionistUI::cb_clear_canvas_button(Fl_Widget* o, void* v)
{
	ImpressionistDoc * pDoc = ((ImpressionistUI*)(o->user_data()))->getDocument();

	pDoc->clearCanvas();
}


//-----------------------------------------------------------
// Updates the brush size to use from the value of the size
// slider
// Called by the UI when the size slider is moved
//-----------------------------------------------------------
void ImpressionistUI::cb_sizeSlides(Fl_Widget* o, void* v)
{
	((ImpressionistUI*)(o->user_data()))->m_nSize = int(((Fl_Slider *)o)->value());
}

void ImpressionistUI::cb_thicknessSlides(Fl_Widget* o, void* v)
{
	((ImpressionistUI*)(o->user_data()))->m_thickness = int(((Fl_Slider *)o)->value());
}

void ImpressionistUI::cb_angleSlides(Fl_Widget* o, void* v)
{
	((ImpressionistUI*)(o->user_data()))->m_angle = int(((Fl_Slider *)o)->value());
}

void ImpressionistUI::cb_opacitySlides(Fl_Widget* o, void* v)
{
	((ImpressionistUI*)(o->user_data()))->m_opacity = float(((Fl_Slider *)o)->value());
}

void ImpressionistUI::cb_spacingSlides(Fl_Widget* o, void* v)
{
	((ImpressionistUI*)(o->user_data()))->m_spacing = float(((Fl_Slider *)o)->value());
}


ImpressionistDoc* ImpressionistUI::getDocument()
{
	return m_pDoc;
}

//------------------------------------------------
// Draw the main window
//------------------------------------------------
void ImpressionistUI::show() {
	m_mainWindow->show();
	m_paintView->show();
	m_origView->show();
}

void ImpressionistUI::resize_windows(int w, int h) {
	m_paintView->size(w, h);
	m_origView->size(w, h);
}

void ImpressionistUI::setDocument(ImpressionistDoc* doc)
{
	m_pDoc = doc;

	m_origView->m_pDoc = doc;
	m_paintView->m_pDoc = doc;
}

int ImpressionistUI::getSize()
{
	return m_nSize;
}

void ImpressionistUI::setSize(int size)
{
	m_nSize = size;
	if (is_in_range(size, MIN_POINT_SIZE, MAX_POINT_SIZE)) {
		m_BrushSizeSlider->value(size);
	}
}

int ImpressionistUI::getThickness()
{
	return m_thickness;
}

void ImpressionistUI::setThickess(int thickness)
{
	m_thickness = thickness;
	if (is_in_range(thickness, MIN_THICKNESS, MAX_THICKNESS)) {
		m_BrushThicknessSlider->value(thickness);
	}
}

int ImpressionistUI::getAngle()
{
	return m_angle;
}

void ImpressionistUI::setAngle(int angle)
{
	m_angle = angle;
	if (is_in_range(angle, MIN_ANGLE, MAX_ANGLE)) {
		m_BrushAngleSlider->value(angle);
	}
}

float ImpressionistUI::getOpacity()
{
	return m_opacity;
}

void ImpressionistUI::setOpacity(float opacity)
{
	m_opacity = opacity;
	if (is_in_range(m_opacity, MIN_OPACITY, MAX_OPACITY)) {
		m_BrushOpacitySlider->value(m_opacity);
	}
}

int ImpressionistUI::getDirectionType()
{
	return m_direction;
}

void ImpressionistUI::setDirectionType(int direction)
{
	m_direction = direction;
}

int ImpressionistUI::getSpacing()
{
	return m_spacing;
}

void ImpressionistUI::setSpacing(int spacing)
{
	m_spacing = spacing;
}

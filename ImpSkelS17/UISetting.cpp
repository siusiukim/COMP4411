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
#include <string>

//------------------------------------------------------------
// This returns the UI, given the menu item.  It provides a
// link from the menu items to the UI
//------------------------------------------------------------
ImpressionistUI* ImpressionistUI::whoami(Fl_Menu_* o)
{
	return ((ImpressionistUI*)(o->parent()->user_data()));
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

void ImpressionistUI::cb_thresholdSlides(Fl_Widget* o, void* v)
{
	((ImpressionistUI*)(o->user_data()))->m_threshold = float(((Fl_Slider *)o)->value());
}

void ImpressionistUI::cb_learnNumberSlides(Fl_Widget* o, void* v)
{
	((ImpressionistUI*)(o->user_data()))->m_learnNumber = float(((Fl_Slider *)o)->value());
}

void ImpressionistUI::cb_iterNumberSlides(Fl_Widget* o, void* v)
{
	((ImpressionistUI*)(o->user_data()))->m_iterNumber = float(((Fl_Slider *)o)->value());
}

void ImpressionistUI::cb_rScaleSlides(Fl_Widget* o, void* v)
{
	((ImpressionistUI*)(o->user_data()))->m_r_scale = float(((Fl_Slider *)o)->value());
	((ImpressionistUI*)(o->user_data()))->adjustColor();
}

void ImpressionistUI::cb_gScaleSlides(Fl_Widget* o, void* v)
{
	((ImpressionistUI*)(o->user_data()))->m_g_scale = float(((Fl_Slider *)o)->value());
	((ImpressionistUI*)(o->user_data()))->adjustColor();
}

void ImpressionistUI::cb_bScaleSlides(Fl_Widget* o, void* v)
{
	((ImpressionistUI*)(o->user_data()))->m_b_scale = float(((Fl_Slider *)o)->value());
	((ImpressionistUI*)(o->user_data()))->adjustColor();
}

//Filter input

void ImpressionistUI::cb_filterInput(Fl_Widget* o, void* v)
{
	((ImpressionistUI*)(o->user_data()))->m_filterValue = std::string(((Fl_Int_Input *)o)->value());
}

void ImpressionistUI::cb_normalizeSwitch(Fl_Widget* o, void* v)
{
	ImpressionistUI *pUI = ((ImpressionistUI*)(o->user_data()));
	if (pUI->m_normalize == TRUE) pUI->m_normalize = FALSE;
	else pUI->m_normalize = TRUE;
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

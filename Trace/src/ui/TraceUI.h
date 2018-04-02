//
// rayUI.h
//
// The header file for the UI part
//

#ifndef __rayUI_h__
#define __rayUI_h__

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Button.H>

#include <FL/fl_file_chooser.H>		// FLTK file chooser

#include "TraceGLWindow.h"

class TraceUI {
public:
	TraceUI(RayTracer *tracer);

	// The FLTK widgets
	Fl_Window*			m_mainWindow;
	Fl_Menu_Bar*		m_menubar;

	Fl_Slider*			m_sizeSlider;
	Fl_Slider*			m_depthSlider;

	Fl_Slider*			m_ambientAttenSlider;
	Fl_Slider*			m_disAttenASlider;
	Fl_Slider*			m_disAttenBSlider;
	Fl_Slider*			m_disAttenCSlider;

	Fl_Slider*			m_disScaleSlider;

	Fl_Slider*			m_recurThresholdSlider;

	Fl_Slider*			m_superResSlider;

	Fl_Button*			m_renderButton;
	Fl_Button*			m_stopButton;

	Fl_Light_Button*	m_adaptiveAntiSwitch;
	Fl_Light_Button*	m_superSampleSwitch;
	Fl_Light_Button*	m_sampleJitterSwitch;

	TraceGLWindow*		m_traceGlWindow;

	// member functions
	void show();

	int			getSize();
	//int			getDepth();

private:
	RayTracer* const raytracer;

	int			m_nSize;
	//int			m_nDepth;

// static class members
	static Fl_Menu_Item menuitems[];

	static TraceUI* whoami(Fl_Menu_* o);

	static void cb_load_scene(Fl_Menu_* o, void* v);
	static void cb_save_image(Fl_Menu_* o, void* v);
	static void cb_exit(Fl_Menu_* o, void* v);
	static void cb_about(Fl_Menu_* o, void* v);

	static void cb_exit2(Fl_Widget* o, void* v);

	static void cb_sizeSlides(Fl_Widget* o, void* v);
	static void cb_depthSlides(Fl_Widget* o, void* v);

	static void cb_ambientAttenSlides(Fl_Widget* o, void* v);
	static void cb_disAttenASlides(Fl_Widget* o, void* v);
	static void cb_disAttenBSlides(Fl_Widget* o, void* v);
	static void cb_disAttenCSlides(Fl_Widget* o, void* v);
	static void cb_recurThresholdSlides(Fl_Widget* o, void* v);
	static void cb_disScaleSlides(Fl_Widget* o, void* v);
	static void cb_superResSlides(Fl_Widget* o, void* v);

	static void cb_adaptiveAntiSwitch(Fl_Widget* o, void* v);
	static void cb_superSampleSwitch(Fl_Widget* o, void* v);
	static void cb_sampleJitterSwitch(Fl_Widget* o, void* v);

	static void cb_render(Fl_Widget* o, void* v);
	static void cb_stop(Fl_Widget* o, void* v);
};

#endif

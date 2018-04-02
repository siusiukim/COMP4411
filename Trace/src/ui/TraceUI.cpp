//
// TraceUI.h
//
// Handles FLTK integration and other user interface tasks
//
#include <stdio.h>
#include <time.h>
#include <string.h>

#include <FL/fl_ask.h>

#include "TraceUI.h"
#include "../RayTracer.h"

static bool done;

//------------------------------------- Help Functions --------------------------------------------
TraceUI* TraceUI::whoami(Fl_Menu_* o)	// from menu item back to UI itself
{
	return ((TraceUI*)(o->parent()->user_data()));
}

//--------------------------------- Callback Functions --------------------------------------------
void TraceUI::cb_load_scene(Fl_Menu_* o, void* v)
{
	TraceUI* pUI = whoami(o);

	char* newfile = fl_file_chooser("Open Scene?", "*.ray", NULL);

	if (newfile != NULL) {
		char buf[256];

		if (pUI->raytracer->loadScene(newfile)) {
			sprintf(buf, "Ray <%s>", newfile);
			done = true;	// terminate the previous rendering
		}
		else {
			sprintf(buf, "Ray <Not Loaded>");
		}

		pUI->m_mainWindow->label(buf);
	}
}

void TraceUI::cb_save_image(Fl_Menu_* o, void* v)
{
	TraceUI* pUI = whoami(o);

	char* savefile = fl_file_chooser("Save Image?", "*.bmp", "save.bmp");
	if (savefile != NULL) {
		pUI->m_traceGlWindow->saveImage(savefile);
	}
}

void TraceUI::cb_exit(Fl_Menu_* o, void* v)
{
	TraceUI* pUI = whoami(o);

	// terminate the rendering
	done = true;

	pUI->m_traceGlWindow->hide();
	pUI->m_mainWindow->hide();
}

void TraceUI::cb_exit2(Fl_Widget* o, void* v)
{
	TraceUI* pUI = (TraceUI *)(o->user_data());

	// terminate the rendering
	done = true;

	pUI->m_traceGlWindow->hide();
	pUI->m_mainWindow->hide();
}

void TraceUI::cb_about(Fl_Menu_* o, void* v)
{
	fl_message("RayTracer Project, FLTK version for CS 341 Spring 2002. Latest modifications by Jeff Maurer, jmaurer@cs.washington.edu");
}

void TraceUI::cb_sizeSlides(Fl_Widget* o, void* v)
{
	TraceUI* pUI = (TraceUI*)(o->user_data());

	pUI->m_nSize = int(((Fl_Slider *)o)->value());
	int	height = (int)(pUI->m_nSize / pUI->raytracer->aspectRatio() + 0.5);
	pUI->m_traceGlWindow->resizeWindow(pUI->m_nSize, height);
}

void TraceUI::cb_depthSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->raytracer->recurDepth = int(((Fl_Slider *)o)->value());
}

void TraceUI::cb_ambientAttenSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->raytracer->globalAmbient = double(((Fl_Slider *)o)->value());
}

void TraceUI::cb_disAttenASlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->raytracer->constAtten = double(((Fl_Slider *)o)->value());
}

void TraceUI::cb_disAttenBSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->raytracer->linearAtten = double(((Fl_Slider *)o)->value());
}

void TraceUI::cb_disAttenCSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->raytracer->quadAtten = double(((Fl_Slider *)o)->value());
}

void TraceUI::cb_recurThresholdSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->raytracer->adaptiveAmount = double(((Fl_Slider *)o)->value());
}

void TraceUI::cb_disScaleSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->raytracer->distanceScale = double(((Fl_Slider *)o)->value());
}

void TraceUI::cb_superResSlides(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->raytracer->samplePixel = double(((Fl_Slider *)o)->value());
}

void TraceUI::cb_adaptiveAntiSwitch(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->raytracer->adaptiveAnti = !((TraceUI*)(o->user_data()))->raytracer->adaptiveAnti;
}

void TraceUI::cb_superSampleSwitch(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->raytracer->superSample = !((TraceUI*)(o->user_data()))->raytracer->superSample;
}

void TraceUI::cb_sampleJitterSwitch(Fl_Widget* o, void* v)
{
	((TraceUI*)(o->user_data()))->raytracer->sampleJitter = !((TraceUI*)(o->user_data()))->raytracer->sampleJitter;
}

void TraceUI::cb_render(Fl_Widget* o, void* v)
{
	char buffer[256];

	TraceUI* pUI = ((TraceUI*)(o->user_data()));

	if (pUI->raytracer->sceneLoaded()) {
		int width = pUI->getSize();
		int	height = (int)(width / pUI->raytracer->aspectRatio() + 0.5);
		pUI->m_traceGlWindow->resizeWindow(width, height);

		pUI->m_traceGlWindow->show();

		pUI->raytracer->traceSetup(width, height);

		// Save the window label
		const char *old_label = pUI->m_traceGlWindow->label();

		// start to render here	
		done = false;
		clock_t prev, now;
		prev = clock();

		pUI->m_traceGlWindow->refresh();
		Fl::check();
		Fl::flush();

		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				if (done) break;

				// current time
				now = clock();

				// check event every 1/2 second
				if (((double)(now - prev) / CLOCKS_PER_SEC) > 0.5) {
					prev = now;

					if (Fl::ready()) {
						// refresh
						pUI->m_traceGlWindow->refresh();
						// check event
						Fl::check();

						if (Fl::damage()) {
							Fl::flush();
						}
					}
				}

				pUI->raytracer->tracePixel(x, y);

			}
			if (done) break;

			// flush when finish a row
			if (Fl::ready()) {
				// refresh
				pUI->m_traceGlWindow->refresh();

				if (Fl::damage()) {
					Fl::flush();
				}
			}
			// update the window label
			sprintf(buffer, "(%d%%) %s", (int)((double)y / (double)height * 100.0), old_label);
			pUI->m_traceGlWindow->label(buffer);

		}
		done = true;
		pUI->m_traceGlWindow->refresh();

		// Restore the window label
		pUI->m_traceGlWindow->label(old_label);
	}
}

void TraceUI::cb_stop(Fl_Widget* o, void* v)
{
	done = true;
}

void TraceUI::show()
{
	m_mainWindow->show();
}

int TraceUI::getSize()
{
	return m_nSize;
}

// menu definition
Fl_Menu_Item TraceUI::menuitems[] = {
	{ "&File",		0, 0, 0, FL_SUBMENU },
		{ "&Load Scene...",	FL_ALT + 'l', (Fl_Callback *)TraceUI::cb_load_scene },
		{ "&Save Image...",	FL_ALT + 's', (Fl_Callback *)TraceUI::cb_save_image },
		{ "&Exit",			FL_ALT + 'e', (Fl_Callback *)TraceUI::cb_exit },
		{ 0 },

	{ "&Help",		0, 0, 0, FL_SUBMENU },
		{ "&About",	FL_ALT + 'a', (Fl_Callback *)TraceUI::cb_about },
		{ 0 },

	{ 0 }
};

TraceUI::TraceUI(RayTracer *tracer) :
	raytracer(tracer)
{
	// init.
	m_nSize = 150;

	m_mainWindow = new Fl_Window(100, 40, 400, 500, "Ray <Not Loaded>");
	m_mainWindow->user_data((void*)(this));	// record self to be used by static callback functions
	// install menu bar
	m_menubar = new Fl_Menu_Bar(0, 0, 400, 25);
	m_menubar->menu(menuitems);

	// install slider depth
	m_depthSlider = new Fl_Value_Slider(10, 30, 180, 20, "Depth");
	m_depthSlider->user_data((void*)(this));	// record self to be used by static callback functions
	m_depthSlider->type(FL_HOR_NICE_SLIDER);
	m_depthSlider->labelfont(FL_COURIER);
	m_depthSlider->labelsize(12);
	m_depthSlider->minimum(0);
	m_depthSlider->maximum(10);
	m_depthSlider->step(1);
	m_depthSlider->value(raytracer->recurDepth);
	m_depthSlider->align(FL_ALIGN_RIGHT);
	m_depthSlider->callback(cb_depthSlides);

	// install slider size
	m_sizeSlider = new Fl_Value_Slider(10, 55, 180, 20, "Size");
	m_sizeSlider->user_data((void*)(this));	// record self to be used by static callback functions
	m_sizeSlider->type(FL_HOR_NICE_SLIDER);
	m_sizeSlider->labelfont(FL_COURIER);
	m_sizeSlider->labelsize(12);
	m_sizeSlider->minimum(64);
	m_sizeSlider->maximum(512);
	m_sizeSlider->step(1);
	m_sizeSlider->value(m_nSize);
	m_sizeSlider->align(FL_ALIGN_RIGHT);
	m_sizeSlider->callback(cb_sizeSlides);

	// install slider ambient
	m_ambientAttenSlider = new Fl_Value_Slider(10, 80, 180, 20, "Ambient");
	m_ambientAttenSlider->user_data((void*)(this));	// record self to be used by static callback functions
	m_ambientAttenSlider->type(FL_HOR_NICE_SLIDER);
	m_ambientAttenSlider->labelfont(FL_COURIER);
	m_ambientAttenSlider->labelsize(12);
	m_ambientAttenSlider->minimum(0.0);
	m_ambientAttenSlider->maximum(1.0);
	m_ambientAttenSlider->step(0.01);
	m_ambientAttenSlider->value(raytracer->globalAmbient);
	m_ambientAttenSlider->align(FL_ALIGN_RIGHT);
	m_ambientAttenSlider->callback(cb_ambientAttenSlides);

	// install slider dis atten A
	m_disAttenASlider = new Fl_Value_Slider(10, 105, 180, 20, "Dis. atten. 0th order");
	m_disAttenASlider->user_data((void*)(this));	// record self to be used by static callback functions
	m_disAttenASlider->type(FL_HOR_NICE_SLIDER);
	m_disAttenASlider->labelfont(FL_COURIER);
	m_disAttenASlider->labelsize(12);
	m_disAttenASlider->minimum(0.0);
	m_disAttenASlider->maximum(1.0);
	m_disAttenASlider->step(0.01);
	m_disAttenASlider->value(raytracer->constAtten);
	m_disAttenASlider->align(FL_ALIGN_RIGHT);
	m_disAttenASlider->callback(cb_disAttenASlides);

	// install slider dis atten B
	m_disAttenBSlider = new Fl_Value_Slider(10, 130, 180, 20, "Dis. atten. 1st oder");
	m_disAttenBSlider->user_data((void*)(this));	// record self to be used by static callback functions
	m_disAttenBSlider->type(FL_HOR_NICE_SLIDER);
	m_disAttenBSlider->labelfont(FL_COURIER);
	m_disAttenBSlider->labelsize(12);
	m_disAttenBSlider->minimum(0.0);
	m_disAttenBSlider->maximum(1.0);
	m_disAttenBSlider->step(0.01);
	m_disAttenBSlider->value(raytracer->linearAtten);
	m_disAttenBSlider->align(FL_ALIGN_RIGHT);
	m_disAttenBSlider->callback(cb_disAttenBSlides);

	// install slider dis atten C
	m_disAttenCSlider = new Fl_Value_Slider(10, 155, 180, 20, "Dis. atten. 2nd order");
	m_disAttenCSlider->user_data((void*)(this));	// record self to be used by static callback functions
	m_disAttenCSlider->type(FL_HOR_NICE_SLIDER);
	m_disAttenCSlider->labelfont(FL_COURIER);
	m_disAttenCSlider->labelsize(12);
	m_disAttenCSlider->minimum(0.0);
	m_disAttenCSlider->maximum(1.0);
	m_disAttenCSlider->step(0.01);
	m_disAttenCSlider->value(raytracer->quadAtten);
	m_disAttenCSlider->align(FL_ALIGN_RIGHT);
	m_disAttenCSlider->callback(cb_disAttenCSlides);

	// install slider recur threshold
	m_recurThresholdSlider = new Fl_Value_Slider(10, 180, 180, 20, "Adaptive Amount");
	m_recurThresholdSlider->user_data((void*)(this));	// record self to be used by static callback functions
	m_recurThresholdSlider->type(FL_HOR_NICE_SLIDER);
	m_recurThresholdSlider->labelfont(FL_COURIER);
	m_recurThresholdSlider->labelsize(12);
	m_recurThresholdSlider->minimum(0.0);
	m_recurThresholdSlider->maximum(1.73);
	m_recurThresholdSlider->step(0.01);
	m_recurThresholdSlider->value(raytracer->adaptiveAmount);
	m_recurThresholdSlider->align(FL_ALIGN_RIGHT);
	m_recurThresholdSlider->callback(cb_recurThresholdSlides);

	// install dist scale
	m_disScaleSlider = new Fl_Value_Slider(10, 205, 180, 20, "Distance Scale");
	m_disScaleSlider->user_data((void*)(this));	// record self to be used by static callback functions
	m_disScaleSlider->type(FL_HOR_NICE_SLIDER);
	m_disScaleSlider->labelfont(FL_COURIER);
	m_disScaleSlider->labelsize(12);
	m_disScaleSlider->minimum(0.0);
	m_disScaleSlider->maximum(100.0);
	m_disScaleSlider->step(0.1);
	m_disScaleSlider->value(raytracer->distanceScale);
	m_disScaleSlider->align(FL_ALIGN_RIGHT);
	m_disScaleSlider->callback(cb_disScaleSlides);

	m_adaptiveAntiSwitch = new Fl_Light_Button(10, 230, 160, 20, "Adaptive Termination");
	m_adaptiveAntiSwitch->user_data((void*)(this));
	m_adaptiveAntiSwitch->callback(cb_adaptiveAntiSwitch);

	m_superSampleSwitch = new Fl_Light_Button(180, 230, 80, 20, "Super");
	m_superSampleSwitch->user_data((void*)(this));
	m_superSampleSwitch->callback(cb_superSampleSwitch);

	m_sampleJitterSwitch = new Fl_Light_Button(270, 230, 80, 20, "Jitter");
	m_sampleJitterSwitch->user_data((void*)(this));
	m_sampleJitterSwitch->callback(cb_sampleJitterSwitch);

	// install dist scale
	m_recurThresholdSlider = new Fl_Value_Slider(10, 255, 180, 20, "Super-sample");
	m_recurThresholdSlider->user_data((void*)(this));	// record self to be used by static callback functions
	m_recurThresholdSlider->type(FL_HOR_NICE_SLIDER);
	m_recurThresholdSlider->labelfont(FL_COURIER);
	m_recurThresholdSlider->labelsize(12);
	m_recurThresholdSlider->minimum(1);
	m_recurThresholdSlider->maximum(5);
	m_recurThresholdSlider->step(1);
	m_recurThresholdSlider->value(raytracer->samplePixel);
	m_recurThresholdSlider->align(FL_ALIGN_RIGHT);
	m_recurThresholdSlider->callback(cb_superResSlides);

	m_renderButton = new Fl_Button(240, 27, 70, 25, "&Render");
	m_renderButton->user_data((void*)(this));
	m_renderButton->callback(cb_render);

	m_stopButton = new Fl_Button(240, 55, 70, 25, "&Stop");
	m_stopButton->user_data((void*)(this));
	m_stopButton->callback(cb_stop);

	m_mainWindow->callback(cb_exit2);
	m_mainWindow->when(FL_HIDE);
	m_mainWindow->end();

	// image view
	m_traceGlWindow = new TraceGLWindow(100, 150, m_nSize, m_nSize, "Rendered Image");
	m_traceGlWindow->setRayTracer(raytracer);
	m_traceGlWindow->end();
	m_traceGlWindow->resizable(m_traceGlWindow);
}
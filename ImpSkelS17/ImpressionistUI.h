//
// impressionistUI.h
//
// The header file for the UI part
//

#ifndef ImpressionistUI_h
#define ImpressionistUI_h

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/fl_file_chooser.H>		// FLTK file chooser
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Int_Input.H>

#include "Impressionist.h"
#include "OriginalView.h"
#include "PaintView.h"

#include "ImpBrush.h"
#include <string>

#define MIN_POINT_SIZE 5
#define MAX_POINT_SIZE 100

#define MIN_THICKNESS 1
#define MAX_THICKNESS 50

#define MIN_ANGLE 0
#define MAX_ANGLE 359

#define MIN_OPACITY 0.0f
#define MAX_OPACITY 1.0f

#define MIN_SPACING 1
#define MAX_SPACING 50

#define MIN_LEARN 5
#define MAX_LEARN 500

#define MIN_ITER 10
#define MAX_ITER 1000

#define MIN_THRESHOLD 5
#define MAX_THRESHOLD 500

class ImpressionistUI {
public:
	ImpressionistUI();

	// The FLTK widgets
	Fl_Window*			m_mainWindow;
	Fl_Menu_Bar*		m_menubar;

	PaintView*			m_paintView;
	OriginalView*		m_origView;

	// for brush dialog
	Fl_Window*			m_brushDialog;
	Fl_Window*			m_filterDialog;
	Fl_Choice*			m_BrushTypeChoice;
	Fl_Choice*			m_DirectionTypeChoice;

	Fl_Slider*			m_BrushSizeSlider;
	Fl_Slider*			m_BrushThicknessSlider;
	Fl_Slider*			m_BrushAngleSlider;
	Fl_Slider*			m_BrushOpacitySlider;
	Fl_Slider*			m_BrushSpacingSlider;
	Fl_Slider*			m_ThresholdSlider;
	Fl_Slider*			m_LearnNumberSlider;
	Fl_Slider*			m_IterNumberSlider;

	Fl_Slider*			m_rScaleSlider;
	Fl_Slider*			m_gScaleSlider;
	Fl_Slider*			m_bScaleSlider;

	Fl_Button*          m_ClearCanvasButton;
	Fl_Button*          m_LoadBrushButton;

	Fl_Button*          m_CopyCanvasButton;

	Fl_Button*          m_AutoPaintButton;
	Fl_Button*          m_MultiAutoPaintButton;
	Fl_Button*          m_AutoLearnButton;

	Fl_Button*          m_SwapContentButton;

	Fl_Input*			m_FilterInput;
	Fl_Button*          m_ApplyFilterButton;
	Fl_Light_Button*	m_NormalizeSwitch;

	Fl_Button*          m_loadEdgeButton;
	Fl_Button*          m_computeEdgeButton;
	Fl_Light_Button*	m_EdgeClipSwitch;

	// Member functions
	void				setDocument(ImpressionistDoc* doc);
	ImpressionistDoc*	getDocument();

	void				show();
	void				resize_windows(int w, int h);
	void				adjustColor();

	// Interface to get attribute
	int					getSize();
	void				setSize(int size);

	int					getThickness();
	void				setThickess(int thickness);

	int					getAngle();
	void				setAngle(int angle);

	float				getOpacity();
	void				setOpacity(float opacity);

	void				setDirectionType(int type);
	int					getDirectionType();

	void				setSpacing(int spacing);
	int					getSpacing();

	bool				m_normalize, m_clipEdge;
	std::string			m_filterValue;
	int					m_learnNumber;
	int					m_iterNumber;
	int					m_threshold;

	float				m_r_scale, m_g_scale, m_b_scale;
private:
	ImpressionistDoc*	m_pDoc;		// pointer to document to communicate with the document

	// All attributes here
	int	m_nSize, m_thickness, m_angle, m_direction, m_spacing;
	float m_opacity;

	// Static class members
	static Fl_Menu_Item		menuitems[];
	static Fl_Menu_Item		brushTypeMenu[NUM_BRUSH_TYPE + 1];
	static Fl_Menu_Item		brushDirectionMenu[NUM_DIRECTION_TYPE + 1];

	static ImpressionistUI*	whoami(Fl_Menu_* o);

	// All callbacks here.  Callbacks are declared 
	// static
	static void	cb_load_image(Fl_Menu_* o, void* v);
	static void	cb_load_grad_image(Fl_Menu_* o, void* v);
	static void	cb_save_image(Fl_Menu_* o, void* v);
	static void	cb_load_mural(Fl_Menu_* o, void* v);
	static void	cb_brushes(Fl_Menu_* o, void* v);
	static void	cb_filters(Fl_Menu_* o, void* v);
	static void	cb_clear_canvas(Fl_Menu_* o, void* v);
	static void	cb_copy_canvas(Fl_Widget* o, void* v);
	static void	cb_exit(Fl_Menu_* o, void* v);
	static void	cb_about(Fl_Menu_* o, void* v);
	static void	cb_brushChoice(Fl_Widget* o, void* v);
	static void	cb_clear_canvas_button(Fl_Widget* o, void* v);
	static void	cb_sizeSlides(Fl_Widget* o, void* v);
	static void	cb_thicknessSlides(Fl_Widget* o, void* v);
	static void	cb_angleSlides(Fl_Widget* o, void* v);
	static void	cb_opacitySlides(Fl_Widget* o, void* v);
	static void	cb_spacingSlides(Fl_Widget* o, void* v);
	static void	cb_thresholdSlides(Fl_Widget* o, void* v);

	static void	cb_load_brush(Fl_Widget* o, void* v);

	static void	cb_rScaleSlides(Fl_Widget* o, void* v);
	static void	cb_gScaleSlides(Fl_Widget* o, void* v);
	static void	cb_bScaleSlides(Fl_Widget* o, void* v);

	static void	cb_dirTypeChoice(Fl_Widget* o, void* v);

	static void	autoDrawAction(Fl_Widget* o, void* v);
	static void	autoMultiDrawAction(Fl_Widget* o, void* v);

	static void cb_filterInput(Fl_Widget* o, void* v);
	
	static void cb_applyFilter(Fl_Widget* o, void* v);
	static void cb_normalizeSwitch(Fl_Widget* o, void* v);
	static void cb_clipEdgeSwitch(Fl_Widget* o, void* v);
	static void cb_loadEdge(Fl_Widget* o, void* v);
	static void cb_computeEdge(Fl_Widget* o, void* v);

	static void cb_autoLearnDrawing(Fl_Widget* o, void* v);
	static void cb_learnNumberSlides(Fl_Widget* o, void* v);
	static void cb_iterNumberSlides(Fl_Widget* o, void* v);

	static void cb_swapContentButton(Fl_Widget* o, void* v);
};

#endif

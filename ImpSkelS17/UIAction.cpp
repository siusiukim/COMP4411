#include <FL/fl_ask.h>

#include <math.h>

#include "impressionistUI.h"
#include "impressionistDoc.h"

#include "MathUtil.hpp"

#include <string>
#include <sstream>
#include <vector>
#include <iterator>

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

void ImpressionistUI::cb_load_mural(Fl_Menu_* o, void* v)
{
	ImpressionistDoc *pDoc = whoami(o)->getDocument();

	char* newfile = fl_file_chooser("Open File?", "*.bmp", pDoc->getImageName());
	if (newfile != NULL) {
		pDoc->loadMural(newfile);
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

void ImpressionistUI::cb_filters(Fl_Menu_* o, void* v)
{
	whoami(o)->m_filterDialog->show();
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

//------------------------------------------------------------
// Clears the paintview canvas.
// Called by the UI when the clear canvas button is pushed
//------------------------------------------------------------
void ImpressionistUI::cb_clear_canvas_button(Fl_Widget* o, void* v)
{
	ImpressionistDoc * pDoc = ((ImpressionistUI*)(o->user_data()))->getDocument();

	pDoc->clearCanvas();
}

void ImpressionistUI::cb_copy_canvas(Fl_Widget* o, void* v)
{
	ImpressionistDoc * pDoc = ((ImpressionistUI*)(o->user_data()))->getDocument();

	pDoc->copyCanvas();
}

void split(const std::string &s, char delim, std::vector<std::string>& result) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		if (!item.empty()) {
			result.push_back(item);
		}
	}
}

void ImpressionistUI::cb_applyFilter(Fl_Widget* o, void* v)
{
	ImpressionistUI * pUI = (ImpressionistUI*)(o->user_data());
	ImpressionistDoc * pDoc = ((ImpressionistUI*)(o->user_data()))->getDocument();
	
	//Parse the filter string
	std::vector<std::string> rows;
	split(pUI->m_filterValue, ';', rows);
	int nRows = rows.size();
	int nCols;
	float* filter;
	for (int i = 0; i < rows.size(); i++) {
		std::vector<std::string> elems;
		split(rows[i], ',', elems);

		if (i == 0) {
			nCols = elems.size();
			filter = new float[nRows*nCols];
		}
		else if (nCols != elems.size()){
			printf("Size mismatch");
			fl_alert("Size mismatch");
			delete[] filter;
			return;
		}

		for (int j = 0; j < elems.size(); j++) {
			printf("%d ", atoi(elems[j].c_str()));
		}
		printf("\n");
	}

	printf("Filter size: %d*%d", nRows, nCols);

	//if (pUI->m_normalize) {
	//	float sum = 0;
	//	for (int i = 0; i < 3; i++) {
	//		for (int j = 0; j < 3; j++) {
	//			sum += (float)pUI->m_filterValue[i][j];
	//		}
	//	}
	//	for (int i = 0; i < 3; i++) {
	//		for (int j = 0; j < 3; j++) {
	//			filter[i][j] = (pUI->m_filterValue[i][j]) / sum;
	//			printf("%d %d %f\n", i, j, filter[i][j]);
	//		}
	//	}
	//}

	//else {
	//	for (int i = 0; i < 3; i++) {
	//		for (int j = 0; j < 3; j++) {
	//			filter[i][j] = pUI->m_filterValue[i][j];
	//			printf("%d %d %f\n", i, j, filter[i][j]);
	//		}
	//	}
	//}
	//pDoc->applyFilter3x3(filter);

	delete[] filter;
}

void ImpressionistUI::autoDrawAction(Fl_Widget* o, void*) {
	ImpressionistUI *pUI = ((ImpressionistUI*)(o->user_data()));
	ImpressionistDoc *pDoc = pUI->getDocument();

	int spacing = pUI->getSpacing();

	pUI->m_paintView->autoPaint(spacing);
	pUI->m_paintView->refresh();
}

void ImpressionistUI::autoMultiDrawAction(Fl_Widget* o, void*) {
	ImpressionistUI *pUI = ((ImpressionistUI*)(o->user_data()));
	ImpressionistDoc *pDoc = pUI->getDocument();

	int spacing = pUI->getSpacing();

	pUI->m_paintView->autoMultiPaint(spacing);
	pUI->m_paintView->refresh();
}

void ImpressionistUI::cb_autoLearnDrawing(Fl_Widget* o, void*) {
	ImpressionistUI *pUI = ((ImpressionistUI*)(o->user_data()));

	int numLearn = pUI->m_learnNumber;

	pUI->m_paintView->autoLearnPaint(numLearn);
	pUI->m_paintView->refresh();
}

void ImpressionistUI::cb_swapContentButton(Fl_Widget* o, void*) {
	ImpressionistUI *pUI = ((ImpressionistUI*)(o->user_data()));
	ImpressionistDoc *pDoc = pUI->getDocument();

	unsigned char* temp = pDoc->m_ucPainting;
	pDoc->m_ucPainting = pDoc->m_ucBitmap;
	pDoc->m_ucBitmap = temp;

	pUI->m_paintView->refresh();
	pUI->m_origView->refresh();
}

void ImpressionistUI::adjustColor() {
	ImpressionistDoc *pDoc = getDocument();

	pDoc->reloadBitmap();
	for (int i = 0; i < pDoc->m_nHeight*pDoc->m_nWidth; i++) {
		unsigned char* pt = &pDoc->m_ucBitmap[i * 3];

		pt[0] = cap_range((int)(pt[0] * m_r_scale), 0, 255);
		pt[1] = cap_range((int)(pt[1] * m_g_scale), 0, 255);
		pt[2] = cap_range((int)(pt[2] * m_b_scale), 0, 255);
	}
	m_origView->refresh();
}
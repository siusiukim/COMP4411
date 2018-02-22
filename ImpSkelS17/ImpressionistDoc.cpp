// 
// impressionistDoc.cpp
//
// It basically maintain the bitmap for answering the color query from the brush.
// It also acts as the bridge between brushes and UI (including views)
//

#include <FL/fl_ask.H>

#include "impressionistDoc.h"
#include "impressionistUI.h"

#include "ImpBrush.h"

// Include individual brush headers here.
#include "Brushes.h"
#include "MathUtil.hpp"

#define DESTROY(p)	{  if ((p)!=NULL) {delete [] p; p=NULL; } }

ImpressionistDoc::ImpressionistDoc()
{
	// Set NULL image name as init. 
	m_imageName[0] = '\0';

	m_nWidth = -1;
	m_ucBitmap = NULL;
	m_ucRawBitmap = NULL;
	m_ucPainting = NULL;


	// create one instance of each brush
	ImpBrush::c_nBrushCount = NUM_BRUSH_TYPE;
	ImpBrush::c_pBrushes = new ImpBrush*[ImpBrush::c_nBrushCount];

	ImpBrush::c_pBrushes[BRUSH_POINTS] 
		= new PointBrush(this, "Points");

	ImpBrush::c_pBrushes[BRUSH_LINES]
		= new LineBrush(this, "Lines");

	ImpBrush::c_pBrushes[BRUSH_CIRCLES]
		= new CircleBrush(this, "Circles");

	ImpBrush::c_pBrushes[BRUSH_SCATTERED_POINTS]
		= new ScatterPointBrush(this, "Scattered Points");

	ImpBrush::c_pBrushes[BRUSH_SCATTERED_LINES]
		= new ScatterLineBrush(this, "Scattered Lines");

	ImpBrush::c_pBrushes[BRUSH_SCATTERED_CIRCLES]
		= new ScatterCircleBrush(this, "Scattered Circles");

	ImpBrush::c_pBrushes[BRUSH_RUBBER]
		= new RubberBrush(this, "Rubber");

	// make one of the brushes current
	m_pCurrentBrush = ImpBrush::c_pBrushes[0];

}


//---------------------------------------------------------
// Set the current UI 
//---------------------------------------------------------
void ImpressionistDoc::setUI(ImpressionistUI* ui)
{
	m_pUI = ui;
}

//---------------------------------------------------------
// Returns the active picture/painting name
//---------------------------------------------------------
char* ImpressionistDoc::getImageName()
{
	return m_imageName;
}

//---------------------------------------------------------
// Called by the UI when the user changes the brush type.
// type: one of the defined brush types.
//---------------------------------------------------------
void ImpressionistDoc::setBrushType(int type)
{
	m_pCurrentBrush = ImpBrush::c_pBrushes[type];
}

//---------------------------------------------------------
// Returns the size of the brush.
//---------------------------------------------------------
int ImpressionistDoc::getSize()
{
	return m_pUI->getSize();
}

//---------------------------------------------------------
// Load the specified image
// This is called by the UI when the load image button is 
// pressed.
//---------------------------------------------------------
int ImpressionistDoc::loadImage(char *iname)
{
	// try to open the image to read
	unsigned char*	data;
	int				width,
		height;

	if ((data = readBMP(iname, width, height)) == NULL)
	{
		fl_alert("Can't load bitmap file");
		return 0;
	}

	// reflect the fact of loading the new image
	m_nWidth = width;
	m_nPaintWidth = width;
	m_nHeight = height;
	m_nPaintHeight = height;

	// release old storage
	if (m_ucBitmap) delete[] m_ucBitmap;
	if (m_ucPainting) delete[] m_ucPainting;
	if (m_ucRawBitmap) delete[] m_ucRawBitmap;

	m_ucRawBitmap = data;

	m_ucBitmap = new unsigned char[width*height * 3];
	memcpy(m_ucBitmap, m_ucRawBitmap, width*height * 3);

	// allocate space for draw view
	m_ucPainting = new unsigned char[width*height * 3];
	memset(m_ucPainting, 0, width*height * 3);

	m_pUI->m_mainWindow->resize(m_pUI->m_mainWindow->x(),
		m_pUI->m_mainWindow->y(),
		width * 2,
		height + 25);

	// display it on origView
	m_pUI->m_origView->resizeWindow(width, height);
	m_pUI->m_origView->refresh();

	// refresh paint view as well
	m_pUI->m_paintView->resizeWindow(width, height);
	m_pUI->m_paintView->refresh();


	return 1;
}

//---------------------------------------------------------
// Load the specified image
// This is called by the UI when the load image button is 
// pressed.
//---------------------------------------------------------
int ImpressionistDoc::loadMural(char *iname)
{
	// try to open the image to read
	unsigned char*	data;
	int				width,
		height;

	if ((data = readBMP(iname, width, height)) == NULL)
	{
		fl_alert("Can't load bitmap file");
		return 0;
	}

	// reflect the fact of loading the new image
	int new_width = max(width, m_nWidth);
	int new_height = max(height, m_nHeight);

	unsigned char* new_original = new unsigned char[new_width * new_height * 3];
	unsigned char* new_painting = new unsigned char[new_width * new_height * 3];

	memset(new_original, 0, new_width * new_height * 3);
	memset(new_painting, 0, new_width * new_height * 3);

	//Copy old to current
	for (int y = 0; y < m_nHeight; y++) {
		for (int x = 0; x < m_nWidth; x++) {
			for (int i = 0; i < 3; i++) {
				//new_original[(y*new_width + x)*3+i] = data[(y*width + x)*3+i];
				new_painting[(y*new_width + x)*3+i] = m_ucPainting[(y*m_nWidth + x)*3+i];
			}
		}
	}
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			for (int i = 0; i < 3; i++) {
				new_original[(y*new_width + x) * 3 + i] = data[(y*width + x) * 3 + i];
				//new_painting[(y*new_width + x) * 3 + i] = m_ucPainting[(y*m_nWidth + x) * 3 + i];
			}
		}
	}

	m_nWidth = new_width;
	m_nPaintWidth = new_width;
	m_nHeight = new_height;
	m_nPaintHeight = new_height;

	// release old storage
	if (m_ucBitmap) delete[] m_ucBitmap;
	if (m_ucPainting) delete[] m_ucPainting;
	if (m_ucRawBitmap) delete[] m_ucRawBitmap;

	m_ucRawBitmap = new_original;
	m_ucBitmap = new unsigned char[new_width*new_height * 3];
	m_ucPainting = new_painting;
	memcpy(m_ucBitmap, m_ucRawBitmap, new_width*new_height * 3);

	// allocate space for draw view
	//m_ucPainting = new unsigned char[width*height * 3];
	//memset(m_ucPainting, 0, width*height * 3);

	m_pUI->m_mainWindow->resize(m_pUI->m_mainWindow->x(),
		m_pUI->m_mainWindow->y(),
		new_width * 2,
		new_height + 25);

	// display it on origView
	m_pUI->m_origView->resizeWindow(new_width, new_height);
	m_pUI->m_origView->refresh();

	// refresh paint view as well
	m_pUI->m_paintView->resizeWindow(new_width, new_height);
	m_pUI->m_paintView->refresh();


	return 1;
}

void ImpressionistDoc::reloadBitmap() {
	memcpy(m_ucBitmap, m_ucRawBitmap, m_nWidth*m_nHeight * 3);
}

//----------------------------------------------------------------
// Save the specified image
// This is called by the UI when the save image menu button is 
// pressed.
//----------------------------------------------------------------
int ImpressionistDoc::saveImage(char *iname)
{

	writeBMP(iname, m_nPaintWidth, m_nPaintHeight, m_ucPainting);

	return 1;
}

//----------------------------------------------------------------
// Clear the drawing canvas
// This is called by the UI when the clear canvas menu item is 
// chosen
//-----------------------------------------------------------------
int ImpressionistDoc::clearCanvas()
{

	// Release old storage
	if (m_ucPainting)
	{
		delete[] m_ucPainting;

		// allocate space for draw view
		m_ucPainting = new unsigned char[m_nPaintWidth*m_nPaintHeight * 3];
		memset(m_ucPainting, 0, m_nPaintWidth*m_nPaintHeight * 3);

		// refresh paint view as well	
		m_pUI->m_paintView->refresh();
	}

	return 0;
}

int ImpressionistDoc::copyCanvas()
{
	// Release old storage
	if (m_ucPainting)
	{
		delete[] m_ucPainting;

		// allocate space for draw view
		m_ucPainting = new unsigned char[m_nPaintWidth*m_nPaintHeight * 3];
		memcpy(m_ucPainting, m_ucBitmap, m_nPaintWidth*m_nPaintHeight * 3);

		// refresh paint view as well	
		m_pUI->m_paintView->refresh();
	}

	return 0;
}

//------------------------------------------------------------------
// Get the color of the pixel in the original image at coord x and y
//------------------------------------------------------------------
GLubyte* ImpressionistDoc::GetOriginalPixel(int x, int y)
{
	if (x < 0)
		x = 0;
	else if (x >= m_nWidth)
		x = m_nWidth - 1;

	if (y < 0)
		y = 0;
	else if (y >= m_nHeight)
		y = m_nHeight - 1;

	return (GLubyte*)(m_ucBitmap + 3 * (y*m_nWidth + x));
}

//----------------------------------------------------------------
// Get the color of the pixel in the original image at point p
//----------------------------------------------------------------
GLubyte* ImpressionistDoc::GetOriginalPixel(const Point p)
{
	return GetOriginalPixel(p.x, p.y);
}

//------------------------------------------------------------------
// Get the color of the pixel in the Painted image at coord x and y
//------------------------------------------------------------------
GLubyte* ImpressionistDoc::GetPaintedPixel(int x, int y)
{
	if (x < 0)
		x = 0;
	else if (x >= m_nWidth)
		x = m_nWidth - 1;

	if (y < 0)
		y = 0;
	else if (y >= m_nHeight)
		y = m_nHeight - 1;

	return (GLubyte*)(m_ucPainting + 3 * (y*m_nWidth + x));
}

//----------------------------------------------------------------
// Get the color of the pixel in the Painted image at point p
//----------------------------------------------------------------
GLubyte* ImpressionistDoc::GetPaintedPixel(const Point p)
{
	return GetPaintedPixel(p.x, p.y);
}
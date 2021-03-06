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
	m_ucBrush = NULL;
	m_ucGradImage = NULL;
	m_ucEdgeImage = NULL;

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

	ImpBrush::c_pBrushes[BRUSH_CUSTOM]
		= new CustomBrush(this, "Custom");

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

	if (m_ucRawBitmap) {
		delete[] m_ucEdgeImage;
		m_ucEdgeImage = NULL;
	}

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

	m_pUI->m_origView->bitmap_bt = m_ucBitmap;

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
	if (m_ucRawBitmap) {
		delete[] m_ucEdgeImage;
		m_ucEdgeImage = NULL;
	}

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

int ImpressionistDoc::loadBrush(char *iname) {
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
	m_nBrushWidth = width;
	m_nBrushHeight = height;

	if (m_ucBrush) delete[] m_ucBrush;

	m_ucBrush = data;
}

int ImpressionistDoc::loadGradientImage(char *iname) {
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
	m_nGradImageWidth = width;
	m_nGradImageHeight = height;

	if (m_ucGradImage) delete[] m_ucGradImage;

	m_ucGradImage = data;
}

int ImpressionistDoc::loadEdgeImage(char* iname) {
	// try to open the image to read
	unsigned char*	data;
	int				width,
		height;

	if ((data = readBMP(iname, width, height)) == NULL)
	{
		fl_alert("Can't load bitmap file");
		return 0;
	}

	if (width != m_nWidth || height != m_nHeight) {
		fl_alert("Dimension mismatch");
		return 0;
	}

	if (m_ucEdgeImage) delete[] m_ucEdgeImage;

	m_ucEdgeImage = data;
}

//0.8 sigma
static const float gau_filter[5][5] = {
	{ 0.000874, 0.006976, 0.01386, 0.006976, 0.000874 },
	{ 0.006976,0.0557,0.110656,0.0557,0.006976 },
	{ 0.01386,0.110656,0.219833,0.110656,0.01386 },
	{ 0.006976,0.0557,0.110656,0.0557,0.006976 },
	{ 0.000874,0.006976,0.01386,0.006976,0.000874 }
};

static const float sobel_x[3][3] = {
	{1.0 / 8.0, 0, -1.0 / 8.0 },
	{2.0 / 8.0, 0, -2.0 / 8.0 },
	{1.0 / 8.0, 0, -1.0 / 8.0 }
};

static const float sobel_y[3][3] = {
	{ 1.0 / 8.0, 2.0/8.0, 1.0 / 8.0 },
{ 0, 0, 0 },
{ -1.0 / 8.0, -2.0 / 8.0, -1.0 / 8.0 }
};

void ImpressionistDoc::computeEdgeImage() {
	if (!m_ucBitmap) return;

	unsigned char* gray = new unsigned char[m_nWidth*m_nHeight];
	//copy
	if (m_ucEdgeImage) {
		delete[] m_ucEdgeImage;
	}
	m_ucEdgeImage = new unsigned char[m_nWidth*m_nHeight * 3];

	//Blur
	apply_filter(m_ucBitmap, m_ucEdgeImage, m_nWidth, m_nHeight, gau_filter[0], 5, 5);

	//Turn gray
	for (int y = 0; y < m_nHeight; y++) {
		for (int x = 0; x < m_nWidth; x++) {
			int idx = y * m_nWidth + x;
			gray[idx] = m_ucEdgeImage[idx*3] * 0.299 
				+ m_ucEdgeImage[idx*3+1] * 0.587 + m_ucEdgeImage[idx*3+2] * 0.114;
		}
	}

	//Re-claim
	memset(m_ucEdgeImage, 0, m_nWidth*m_nHeight * 3);

#define SOBEL_THRESHOLD 10
	//Compute sobel and threshold
	for (int y = 1; y < m_nHeight-1; y++) {
		for (int x = 1; x < m_nWidth-1; x++) {
			int idx = y * m_nWidth + x;
			float sobel_x = gray[idx- m_nWidth-1] + 2 * gray[idx-1] + gray[idx+ m_nWidth-1]
				- gray[idx-m_nWidth+1] - 2 * gray[idx+1] - gray[idx+ m_nWidth+1];

			float sobel_y = gray[idx - m_nWidth - 1] + 2 * gray[idx - m_nWidth] + gray[idx - m_nWidth + 1]
				- gray[idx + m_nWidth - 1] - 2 * gray[idx + m_nWidth] - gray[idx + m_nWidth + 1];

			sobel_x /= 8;
			sobel_y /= 8;

			float mag = sqrtf(sobel_x * sobel_x + sobel_y * sobel_y);
			if (mag > SOBEL_THRESHOLD) {
				m_ucEdgeImage[idx * 3] = 255;
				m_ucEdgeImage[idx * 3 + 1] = 255;
				m_ucEdgeImage[idx * 3 + 2] = 255;
			}
		}
	}

	delete[] gray;
	printf("Edge Done!");

	//Debug
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

//------------------------------------------------------------------
// Get the color of the pixel in the Gradient image at coord x and y
//------------------------------------------------------------------
GLubyte* ImpressionistDoc::GetGradImagePixel(int x, int y)
{
	if (x < 0)
		x = 0;
	else if (x >= m_nGradImageWidth)
		x = m_nGradImageWidth - 1;

	if (y < 0)
		y = 0;
	else if (y >= m_nGradImageHeight)
		y = m_nGradImageHeight - 1;

	return (GLubyte*)(m_ucGradImage + 3 * (y*m_nGradImageWidth + x));
}

//----------------------------------------------------------------
// Get the color of the pixel in the Gradient image at point p
//----------------------------------------------------------------
GLubyte* ImpressionistDoc::GetGradImagePixel(const Point p)
{
	return GetGradImagePixel(p.x, p.y);
}


bool ImpressionistDoc::IsEdgeAtPixel(int x, int y)
{
	if (!m_ucEdgeImage) return false;

	if (x < 0)
		x = 0;
	else if (x >= m_nWidth)
		x = m_nWidth - 1;

	if (y < 0)
		y = 0;
	else if (y >= m_nHeight)
		y = m_nHeight - 1;

	return m_ucEdgeImage[3 * (y*m_nWidth + x)] != 0;
}


bool ImpressionistDoc::IsEdgeAtPixel(const Point p)
{
	return IsEdgeAtPixel(p.x, p.y);
}
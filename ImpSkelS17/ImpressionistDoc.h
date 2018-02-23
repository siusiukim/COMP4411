// 
// impressionistDoc.h
//
// header file for Doc 
//

#ifndef ImpressionistDoc_h
#define ImpressionistDoc_h

#include "impressionist.h"
#include "bitmap.h"

class ImpressionistUI;

class ImpressionistDoc
{
public:
	ImpressionistDoc();

	void	setUI(ImpressionistUI* ui);		// Assign the UI to use

	int		loadImage(char *iname);			// called by the UI to load image
	int		loadMural(char *iname);			// called by the UI to load image
	int		loadGradientImage(char *iname); 
	int		loadBrush(char *iname);
	int		saveImage(char *iname);			// called by the UI to save image

	int		loadEdgeImage(char* iname);
	void	computeEdgeImage();

	int     clearCanvas();                  // called by the UI to clear the drawing canvas
	int		copyCanvas();					// Copy the raw image to the canvas
	void	setBrushType(int type);			// called by the UI to set the brushType
	int		getSize();						// get the UI size
	void	setSize(int size);				// set the UI size
	char*	getImageName();					// get the current image name


// Attributes
public:
	// Dimensions of original window.
	int				m_nWidth, m_nHeight;
	// Dimensions of the paint window.
	int				m_nPaintWidth, m_nPaintHeight;
	// Dimensions of the loaded brush
	int				m_nBrushWidth, m_nBrushHeight;
	// Dimensions of the gradient image
	int				m_nGradImageWidth, m_nGradImageHeight;

	// Bitmaps for original image and painting.
	unsigned char*	m_ucRawBitmap;
	unsigned char*	m_ucBitmap;
	unsigned char*	m_ucPainting;
	unsigned char*	m_ucBrush;
	unsigned char*	m_ucGradImage;
	unsigned char*	m_ucEdgeImage;


	// The current active brush.
	ImpBrush*			m_pCurrentBrush;
	// Size of the brush.
	int m_nSize;

	ImpressionistUI*	m_pUI;

	// Operations
public:
	// Get the color of the original picture at the specified coord
	GLubyte* GetOriginalPixel(int x, int y);
	// Get the color of the original picture at the specified point	
	GLubyte* GetOriginalPixel(const Point p);

	// Get the color of the Painted picture at the specified coord
	GLubyte* GetPaintedPixel(int x, int y);
	// Get the color of the Painted picture at the specified point	
	GLubyte* GetPaintedPixel(const Point p);

	// Get the color of the Grad picture at the specified coord
	GLubyte* GetGradImagePixel(int x, int y);
	// Get the color of the Grad picture at the specified point	
	GLubyte* GetGradImagePixel(const Point p);

	// Get the color of the Edge picture at the specified coord
	bool IsEdgeAtPixel(int x, int y);
	// Get the color of the Edge picture at the specified point	
	bool IsEdgeAtPixel(const Point p);

	void reloadBitmap();


private:
	char			m_imageName[256];

};

extern void MessageBox(char *message);

#endif

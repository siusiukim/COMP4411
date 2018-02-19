//
// paintview.cpp
//
// The code maintaining the painting view of the input images
//

#include "impressionist.h"
#include "impressionistDoc.h"
#include "impressionistUI.h"
#include "paintview.h"
#include "ImpBrush.h"
#include <math.h>


#define LEFT_MOUSE_DOWN		1
#define LEFT_MOUSE_DRAG		2
#define LEFT_MOUSE_UP		3
#define RIGHT_MOUSE_DOWN	4
#define RIGHT_MOUSE_DRAG	5
#define RIGHT_MOUSE_UP		6


extern float frand();

#ifndef WIN32
#define min(a, b)	( ( (a)<(b) ) ? (a) : (b) )
#define max(a, b)	( ( (a)>(b) ) ? (a) : (b) )
#endif

static int		eventToDo;
static int		isAnEvent = 0;
static Point	coord;

PaintView::PaintView(int			x,
	int			y,
	int			w,
	int			h,
	const char*	l)
	: Fl_Gl_Window(x, y, w, h, l)
{
	m_nWindowWidth = w;
	m_nWindowHeight = h;

}


void PaintView::convertPoint(Point point, Point* source, Point* target) {
	Point scrollpos;// = GetScrollPosition();
	scrollpos.x = 0;
	scrollpos.y = 0;

	m_nWindowWidth = w();
	m_nWindowHeight = h();

	int drawWidth, drawHeight;
	drawWidth = min(m_nWindowWidth, m_pDoc->m_nPaintWidth);
	drawHeight = min(m_nWindowHeight, m_pDoc->m_nPaintHeight);

	int startrow = m_pDoc->m_nPaintHeight - (scrollpos.y + drawHeight);
	if (startrow < 0) startrow = 0;

	m_pPaintBitstart = m_pDoc->m_ucPainting +
		3 * ((m_pDoc->m_nPaintWidth * startrow) + scrollpos.x);

	m_nDrawWidth = drawWidth;
	m_nDrawHeight = drawHeight;

	m_nStartRow = startrow;
	m_nEndRow = startrow + drawHeight;
	m_nStartCol = scrollpos.x;
	m_nEndCol = m_nStartCol + drawWidth;

	*source = Point(point.x + m_nStartCol, m_nEndRow - point.y);
	*target = Point(point.x, m_nWindowHeight - point.y);
}

void PaintView::draw()
{
#ifndef MESA
	// To avoid flicker on some machines.
	glDrawBuffer(GL_FRONT_AND_BACK);
#endif // !MESA

	if (!valid())
	{

		glClearColor(0.7f, 0.7f, 0.7f, 1.0);

		// We're only using 2-D, so turn off depth 
		glDisable(GL_DEPTH_TEST);

		ortho();

		glClear(GL_COLOR_BUFFER_BIT);
	}

	Point source, target;
	convertPoint(coord, &source, &target);

	if (m_pDoc->m_ucPainting && !isAnEvent)
	{
		RestoreContent();
	}

	if (m_pDoc->m_ucPainting && isAnEvent)
	{

		// Clear it after processing.
		isAnEvent = 0;

		// This is the event handler
		switch (eventToDo)
		{
		case LEFT_MOUSE_DOWN:
			m_pDoc->m_pCurrentBrush->BrushBegin(source, target);
			break;
		case LEFT_MOUSE_DRAG:
			m_pDoc->m_pCurrentBrush->BrushMove(source, target);
			break;
		case LEFT_MOUSE_UP:
			m_pDoc->m_pCurrentBrush->BrushEnd(source, target);

			SaveCurrentContent();
			RestoreContent();
			break;
		case RIGHT_MOUSE_DOWN:
			SaveCurrentContent();
			RestoreContent();
			m_pDoc->m_pCurrentBrush->DragDirectionBegin(target);
			break;
		case RIGHT_MOUSE_DRAG:
			RestoreContent();
			m_pDoc->m_pCurrentBrush->DragDirectionMove(target);
			break;
		case RIGHT_MOUSE_UP:
			RestoreContent();
			SaveCurrentContent();
			m_pDoc->m_pCurrentBrush->DragDirectionEnd(target);
			break;

		default:
			printf("Unknown event!!\n");
			break;
		}
	}

	glFlush();

#ifndef MESA
	// To avoid flicker on some machines.
	glDrawBuffer(GL_BACK);
#endif // !MESA

}

int PaintView::handle(int event)
{
	switch (event)
	{
	case FL_ENTER:
		redraw();
		break;
	case FL_PUSH:
		coord.x = Fl::event_x();
		coord.y = Fl::event_y();
		if (Fl::event_button() > 1)
			eventToDo = RIGHT_MOUSE_DOWN;
		else
			eventToDo = LEFT_MOUSE_DOWN;
		isAnEvent = 1;
		redraw();
		break;
	case FL_DRAG:
		coord.x = Fl::event_x();
		coord.y = Fl::event_y();
		if (Fl::event_button() > 1)
			eventToDo = RIGHT_MOUSE_DRAG;
		else
			eventToDo = LEFT_MOUSE_DRAG;
		isAnEvent = 1;
		redraw();
		break;
	case FL_RELEASE:
		coord.x = Fl::event_x();
		coord.y = Fl::event_y();
		if (Fl::event_button() > 1)
			eventToDo = RIGHT_MOUSE_UP;
		else
			eventToDo = LEFT_MOUSE_UP;
		isAnEvent = 1;
		redraw();
		break;
	case FL_MOVE:
		coord.x = Fl::event_x();
		coord.y = Fl::event_y();
		break;
	default:
		return 0;
		break;

	}



	return 1;
}

void PaintView::refresh()
{
	redraw();
}

void PaintView::resizeWindow(int width, int height)
{
	resize(x(), y(), width, height);
}

void PaintView::SaveCurrentContent()
{
	// Tell openGL to read from the front buffer when capturing
	// out paint strokes
	glReadBuffer(GL_FRONT);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ROW_LENGTH, m_pDoc->m_nPaintWidth);

	glReadPixels(0,
		m_nWindowHeight - m_nDrawHeight,
		m_nDrawWidth,
		m_nDrawHeight,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		m_pPaintBitstart);
}


void PaintView::RestoreContent()
{
	glDrawBuffer(GL_BACK);

	glClear(GL_COLOR_BUFFER_BIT);

	glRasterPos2i(0, m_nWindowHeight - m_nDrawHeight);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, m_pDoc->m_nPaintWidth);
	glDrawPixels(m_nDrawWidth,
		m_nDrawHeight,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		m_pPaintBitstart);

	//	glDrawBuffer(GL_FRONT);
}

void PaintView::autoPaint(int spacing) {

#ifndef MESA
	// To avoid flicker on some machines.
	glDrawBuffer(GL_FRONT_AND_BACK);
#endif // !MESA

	if (!valid())
	{
		glClearColor(0.7f, 0.7f, 0.7f, 1.0);

		// We're only using 2-D, so turn off depth 
		glDisable(GL_DEPTH_TEST);

		ortho();

		glClear(GL_COLOR_BUFFER_BIT);
	}

	ImpressionistUI* ui = m_pDoc->m_pUI;

	int orig_size = ui->getSize();
	int orig_thickness = ui->getThickness();
	int orig_angle = ui->getAngle();

	if (m_pDoc->m_ucPainting) {
		for (int i = 0; i < m_nDrawWidth; i += spacing) {
			for (int j = 0; j < m_nDrawHeight; j += spacing) {
				Point draw(i + (frand()-0.5f) * spacing * 0.5f, j + (frand()-0.5f) * spacing * 0.5f);
				Point source, target;
				convertPoint(draw, &source, &target);

				//Randomness in brush attributes
				ui->setSize(orig_size + (frand() - 0.5f) * 0.3f * orig_size);
				ui->setThickess(orig_thickness + (frand() - 0.5f) * 0.3f * orig_thickness);
				ui->setAngle(orig_angle + (frand() - 0.5f) * 18);

				m_pDoc->m_pCurrentBrush->BrushBegin(source, target);
			}
		}
	}

	ui->setSize(orig_size);
	ui->setThickess(orig_thickness);
	ui->setAngle(orig_angle);

	SaveCurrentContent();
	RestoreContent();
	glFlush();

#ifndef MESA
	// To avoid flicker on some machines.
	glDrawBuffer(GL_BACK);
#endif // !MESA
}

void PaintView::autoLearnPaint(int numLearn) {
	double minLoss = INFINITY;
	unsigned char* best_paint = new unsigned char[m_nDrawWidth*m_nDrawHeight*3];
	unsigned char* cand[5];
	for (int i = 0; i < 5; i++) {
		cand[i] = new unsigned char[m_nDrawWidth*m_nDrawHeight * 3];
	}

	Point *posArray = new Point[6 * numLearn];
	int *sizeArray = new int[6 * numLearn];
	int *thicknessArray = new int[6 * numLearn];
	int *angleArray = new int[6 * numLearn];

	//randomly init params

	for (int i = 0; i < LEARN_ITERATION; i++) {

	}

	delete[] best_paint;
	for (int i = 0; i < 5; i++) {
		delete[] cand[i];
	}
}


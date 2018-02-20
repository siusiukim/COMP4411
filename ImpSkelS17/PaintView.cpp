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
#include "MathUtil.hpp"
#include <math.h>


#define LEFT_MOUSE_DOWN		1
#define LEFT_MOUSE_DRAG		2
#define LEFT_MOUSE_UP		3
#define RIGHT_MOUSE_DOWN	4
#define RIGHT_MOUSE_DRAG	5
#define RIGHT_MOUSE_UP		6


extern float frand();
float z_frand() {
	return frand() - 0.5f;
}

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
	//glDrawBuffer(GL_FRONT);
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
	//glDrawBuffer(GL_BACK);
	glDrawBuffer(GL_FRONT_AND_BACK);
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
	glReadBuffer(GL_FRONT_AND_BACK);

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
	//glDrawBuffer(GL_BACK);
	glDrawBuffer(GL_FRONT_AND_BACK);

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
	glDrawBuffer(GL_FRONT_AND_BACK);
#endif // !MESA
}

double PaintView::getRootMeanSquareCost(const unsigned char* org, const unsigned char* cmp) {
	double sum = 0;
	for (int i = 0; i < m_nDrawWidth*m_nDrawHeight; i++) {
		sum += (org[i * 3 + 0] - cmp[i * 3 + 0]) * (org[i * 3 + 0] - cmp[i * 3 + 0]) +
			(org[i * 3 + 1] - cmp[i * 3 + 1]) * (org[i * 3 + 1] - cmp[i * 3 + 1]) +
			(org[i * 3 + 2] - cmp[i * 3 + 2]) * (org[i * 3 + 2] - cmp[i * 3 + 2]);
	}
	return sqrt(sum / m_nDrawWidth / m_nDrawHeight / 3);
}

#define MUTANT_RATE 0.1f*(i/numLearn+1)
#define SIZE_MUTANT_RATE 0.1f*(i/numLearn+1)
void PaintView::autoLearnPaint(int numLearn) {

	if (!m_pDoc->m_ucPainting) return;

	double minCost = INFINITY;
	int min_train_idx = 0;
	unsigned char* best_paint = new unsigned char[m_nDrawWidth*m_nDrawHeight*3];

	Point *posArray = new Point[6 * numLearn];
	int *sizeArray = new int[6 * numLearn];
	int *thicknessArray = new int[6 * numLearn];
	int *angleArray = new int[6 * numLearn];

	//randomly init params
	for (int i = 5*numLearn; i < 6*numLearn; i++) {
		posArray[i] = Point(frand()*m_nDrawWidth, frand()*m_nDrawHeight);
		sizeArray[i] = frand()*(MAX_POINT_SIZE - MIN_POINT_SIZE) + MIN_POINT_SIZE;
		//thicknessArray[i] = frand()*(MAX_THICKNESS - MIN_THICKNESS) + MIN_THICKNESS;
		thicknessArray[i] = sizeArray[i]*0.3f;
		angleArray[i] = frand()*(MAX_ANGLE - MIN_ANGLE) + MIN_ANGLE;
	}

	ImpressionistUI* ui = m_pDoc->m_pUI;

	SaveCurrentContent();
	m_pDoc->clearCanvas();
	RestoreContent();

	//Save things
	int orig_size = ui->getSize();
	int orig_thickness = ui->getThickness();
	int orig_angle = ui->getAngle();

	for (int i = 0; i < LEARN_ITERATION; i++) {

#ifndef MESA
		// To avoid flicker on some machines.
		//glDrawBuffer(GL_FRONT_AND_BACK);
		glDrawBuffer(GL_FRONT_AND_BACK);
#endif // !MESA

		SaveCurrentContent();
		m_pDoc->clearCanvas();
		RestoreContent();

		//Pass on genes
		for (int i = 0; i < 3 * numLearn; i++) {
			if (frand() > 0.5f) continue;
			int idx = 5 * numLearn + i%numLearn;
			posArray[i] = Point(z_frand()*m_nDrawWidth*MUTANT_RATE + posArray[idx].x, z_frand()*m_nDrawHeight*MUTANT_RATE + posArray[idx].y);
			sizeArray[i] = z_frand()*(MAX_POINT_SIZE - MIN_POINT_SIZE)*SIZE_MUTANT_RATE + sizeArray[idx];
			//thicknessArray[i] = z_frand()*(MAX_THICKNESS - MIN_THICKNESS)*SIZE_MUTANT_RATE + thicknessArray[idx];
			thicknessArray[i] = sizeArray[i] * 0.3f;
			angleArray[i] = z_frand()*(MAX_ANGLE - MIN_ANGLE)*MUTANT_RATE + angleArray[idx];

			posArray[i].x = cap_range(posArray[i].x, 0, m_nDrawWidth);
			posArray[i].y = cap_range(posArray[i].y, 0, m_nDrawHeight);
			sizeArray[i] = cap_range(sizeArray[i], MIN_POINT_SIZE, MAX_POINT_SIZE);
			thicknessArray[i] = cap_range(thicknessArray[i], MIN_THICKNESS, MAX_THICKNESS);
			angleArray[i] = cap_range(angleArray[i], MIN_ANGLE, MAX_ANGLE);
		}

		//Random the last two
		for (int i = 3 * numLearn; i < 5 * numLearn; i++) {
			posArray[i] = Point(frand()*m_nDrawWidth, frand()*m_nDrawHeight);
			sizeArray[i] = frand()*(MAX_POINT_SIZE - MIN_POINT_SIZE) + MIN_POINT_SIZE;
			//thicknessArray[i] = frand()*(MAX_THICKNESS - MIN_THICKNESS) + MIN_THICKNESS;
			thicknessArray[i] = sizeArray[i] * 0.3f;
			angleArray[i] = frand()*(MAX_ANGLE - MIN_ANGLE) + MIN_ANGLE;
		}

		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < numLearn; j++) {
				Point source, target;
				convertPoint(posArray[j], &source, &target);

				//Randomness in brush attributes
				ui->setSize(sizeArray[i*numLearn + j]);
				ui->setThickess(thicknessArray[i*numLearn + j]);
				ui->setAngle(angleArray[i*numLearn + j]);

				m_pDoc->m_pCurrentBrush->BrushBegin(source, target);
			}

			SaveCurrentContent();
			glFlush();

#ifndef MESA
			// To avoid flicker on some machines.
			//glDrawBuffer(GL_BACK);
			glDrawBuffer(GL_FRONT_AND_BACK);
#endif // !MESA

			double cost = getRootMeanSquareCost(m_pDoc->m_ucBitmap, m_pDoc->m_ucPainting);
			if (cost < minCost) {
				printf("%d %f\n", i, cost);
				minCost = cost;
				min_train_idx = i;
				memcpy(best_paint, m_pDoc->m_ucPainting, m_nDrawWidth*m_nDrawHeight*3);

				for (int i = 0; i < numLearn; i++) {
					int idx = 5 * numLearn + i%numLearn;
					posArray[idx] = posArray[min_train_idx*numLearn + i];
					sizeArray[idx] = sizeArray[min_train_idx*numLearn + i];
					thicknessArray[idx] = thicknessArray[min_train_idx*numLearn + i];
					angleArray[idx] = angleArray[min_train_idx*numLearn + i];
				}
			}
		}
	}

	memcpy(m_pDoc->m_ucPainting, best_paint, m_nDrawWidth*m_nDrawHeight * 3);

	RestoreContent();
	glFlush();

	//Restore so that user is happy
	ui->setSize(orig_size);
	ui->setThickess(orig_thickness);
	ui->setAngle(orig_angle);

	//Release memory
	delete[] best_paint;
	delete[] posArray;
	delete[] sizeArray;
	delete[] thicknessArray;
	delete[] angleArray;
}


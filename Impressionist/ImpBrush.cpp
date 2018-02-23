//
// ImpBrush.cpp
//
// The implementation of virtual brush. All the other brushes inherit from it.
//

#include "impressionistDoc.h"
#include "impressionistUI.h"
#include "ImpBrush.h"
#include <math.h>

// Static class member initializations
int			ImpBrush::c_nBrushCount = 0;
ImpBrush**	ImpBrush::c_pBrushes = NULL;

ImpBrush::ImpBrush(ImpressionistDoc*	pDoc,
	char*				name) :
	m_pDoc(pDoc),
	m_pBrushName(name)
{
}

//---------------------------------------------------
// Return m_pDoc, which connects the UI and brushes
//---------------------------------------------------
ImpressionistDoc* ImpBrush::GetDocument(void)
{
	return m_pDoc;
}

//---------------------------------------------------
// Return the name of the current brush
//---------------------------------------------------
char* ImpBrush::BrushName(void)
{
	return m_pBrushName;
}

//----------------------------------------------------
// Set the color to paint with to the color at source,
// which is the coord at the original window to sample 
// the color from
//----------------------------------------------------
void ImpBrush::SetColor(const Point source)
{
	ImpressionistDoc* pDoc = GetDocument();
	GLubyte color[3];
	memcpy(color, pDoc->GetOriginalPixel(source), 3);
	glColor3ubv(color);
}

//----------------------------------------------------
// Set the color to paint with to the color at source,
// which is the coord at the original window to sample 
// the color from
//----------------------------------------------------
void ImpBrush::SetColorWithAlpha(const Point source, float alpha)
{
	ImpressionistDoc* pDoc = GetDocument();
	GLubyte color[3];
	memcpy(color, pDoc->GetOriginalPixel(source), 3);

	glColor4ub(color[0], color[1], color[2], (GLubyte)(alpha * 255));
}

void ImpBrush::SetColorFromPainted(const Point source)
{
	ImpressionistDoc* pDoc = GetDocument();
	GLubyte color[3];
	memcpy(color, pDoc->GetPaintedPixel(source), 3);
	glColor3ubv(color);
}

void ImpBrush::setOriginalPixelColor(unsigned char* rgb) {
	return;
}

float ImpBrush::GetDirection(const Point source) {
	ImpressionistUI* ui = GetDocument()->m_pUI;
	ImpressionistDoc* pDoc = GetDocument();
	float gray[3][3];
	float sobel_x, sobel_y;

	switch (ui->getDirectionType()) {
	case DIRECTION_BY_SLIDER_OR_RIGHT:
		return (180-GetDocument()->m_pUI->getAngle()) * 2 * M_PI / 360;
	case DIRECTION_BY_MOVEMENT:
		return cursor_direction;
	case DIRECTION_BY_GRADIENT:
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				GLubyte color[3];
				memcpy(color, pDoc->GetOriginalPixel(Point(source.x + i - 1, source.y + j - 1)), 3);
				gray[i][j] = color[0] * 0.299 + color[1] * 0.587 + color[2] * 0.114;
			}
		}
		sobel_x = -gray[0][0] - 2 * gray[0][1] - gray[0][2]
			+ gray[2][0] + 2 * gray[2][1] + gray[2][2];
		sobel_y = gray[0][0] + 2 * gray[1][0] + gray[2][0]
			- gray[0][2] - 2 * gray[1][2] - gray[2][2];
		return atan2f(sobel_x, sobel_y);
	case DIRECTION_BY_ANOTHER_IMAGE:
		if (pDoc->m_ucGradImage) {
			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					GLubyte color[3];
					memcpy(color, pDoc->GetGradImagePixel(Point(source.x + i - 1, source.y + j - 1)), 3);
					gray[i][j] = color[0] * 0.299 + color[1] * 0.587 + color[2] * 0.114;
				}
			}
			sobel_x = -gray[0][0] - 2 * gray[0][1] - gray[0][2]
				+ gray[2][0] + 2 * gray[2][1] + gray[2][2];
			sobel_y = gray[0][0] + 2 * gray[1][0] + gray[2][0]
				- gray[0][2] - 2 * gray[1][2] - gray[2][2];
			return atan2f(sobel_x, sobel_y);
		}
		else {
			return 0;
		}
	}
}

void ImpBrush::CursorBegin(Point point) {
	cursor_last_point = point;
	cursor_curr_point = point;
}

void ImpBrush::CursorMove(Point point) {
	cursor_last_point = cursor_curr_point;
	cursor_curr_point = point;
	if (cursor_last_point != cursor_curr_point) {
		//Compute direction when two points are not the same
		//In radian
		cursor_direction = atan2(cursor_curr_point.y - cursor_last_point.y, cursor_curr_point.x - cursor_last_point.x);
	}
}

void ImpBrush::DragDirectionBegin(const Point point) {
	dir_start_point = point;
}

void ImpBrush::DragDirectionMove(const Point point) {
	if (GetDocument()->m_pUI->getDirectionType() == DIRECTION_BY_SLIDER_OR_RIGHT) {
		glLineWidth(1);
		glBegin(GL_LINES);
		glColor3ub(255, 0, 0);

		glVertex2d(dir_start_point.x, dir_start_point.y);
		glVertex2d(point.x, point.y);

		glEnd();
	}
}

void ImpBrush::DragDirectionEnd(const Point point) {
	//Convert to deg and set it
	if (GetDocument()->m_pUI->getDirectionType() == DIRECTION_BY_SLIDER_OR_RIGHT) {
		float direction = atan2(point.y - dir_start_point.y, point.x - dir_start_point.x) * 360 / 2 / M_PI;
		GetDocument()->m_pUI->setAngle((int)direction);
	}
}

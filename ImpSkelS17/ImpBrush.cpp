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

float ImpBrush::GetDirection() {
	ImpressionistUI* ui = GetDocument()->m_pUI;
	printf("GetDir %d", ui->getDirectionType());
	switch (ui->getDirectionType()) {
	case DIRECTION_BY_SLIDER_OR_RIGHT:
		return GetDocument()->m_pUI->getAngle() * 2 * M_PI / 360;
	case DIRECTION_BY_MOVEMENT:
		return cursor_direction;
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
	glBegin(GL_LINES);
	glLineWidth(1);
	glColor3ub(255, 0, 0);

	glVertex2d(dir_start_point.x, dir_start_point.y);
	glVertex2d(point.x, point.y);

	glEnd();
}

void ImpBrush::DragDirectionEnd(const Point point) {
	float direction = atan2(point.y - dir_start_point.y, point.x - dir_start_point.x) * 360 / 2 / M_PI;
	printf("%f", direction);
	GetDocument()->m_pUI->setAngle((int)direction);
}

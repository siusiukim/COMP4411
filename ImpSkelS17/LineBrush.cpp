#include "impressionistDoc.h"
#include "impressionistUI.h"
#include "Brushes.h"
#include <math.h>

extern float frand();

LineBrush::LineBrush(ImpressionistDoc* pDoc, char* name) :
	ImpBrush(pDoc, name)
{
}

void LineBrush::BrushBegin(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;

	int size = pDoc->getSize();
	glPointSize((float)size);
	BrushMove(source, target);
}

void LineBrush::BrushMove(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;

	if (pDoc == NULL) {
		printf("LineBrush::BrushMove  document is NULL\n");
		return;
	}

	double angle = dlg->getAngle() * 2 * M_PI / 360;
	int thickness = dlg->getThickness();
	int length = dlg->getSize();

	glLineWidth((float)thickness);
	glBegin(GL_LINES);
	SetColor(source);

	double x_adj = cos(angle)*length;
	double y_adj = sin(angle)*length;

	glVertex2d(target.x - x_adj/2, target.y - y_adj/2);
	glVertex2d(target.x + x_adj/2, target.y + y_adj/2);

	glEnd();
}

void LineBrush::BrushEnd(const Point source, const Point target)
{
	// do nothing so far
}


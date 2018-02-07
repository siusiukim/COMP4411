#include "impressionistDoc.h"
#include "impressionistUI.h"
#include "Brushes.h"
#include <math.h>

extern float frand();

#define FILL_RATIO 0.1

ScatterLineBrush::ScatterLineBrush(ImpressionistDoc* pDoc, char* name) :
	ImpBrush(pDoc, name)
{
}

void ScatterLineBrush::BrushBegin(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;

	int size = pDoc->getSize();
	glPointSize((float)size);
	BrushMove(source, target);
}

void ScatterLineBrush::BrushMove(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;

	if (pDoc == NULL) {
		printf("ScatterPointBrush::BrushMove  document is NULL\n");
		return;
	}

	double angle = dlg->getAngle() * 2 * M_PI / 360;
	int thickness = dlg->getThickness();
	int length = dlg->getSize();
	int points = (int)(length*FILL_RATIO + 1);

	for (int i = 0; i < points; i++) {
		glLineWidth((float)thickness);
		glBegin(GL_LINES);
		double x = target.x + frand()*length - length / 2.0;
		double y = target.y + frand()*length - length / 2.0;
		SetColor(Point(x, y));

		double x_adj = cos(angle)*length;
		double y_adj = sin(angle)*length;
		glVertex2d(x - x_adj / 2, y - y_adj / 2);
		glVertex2d(x + x_adj / 2, y + y_adj / 2);
		glEnd();
	}
}

void ScatterLineBrush::BrushEnd(const Point source, const Point target)
{
	// do nothing so far
}


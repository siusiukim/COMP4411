#include "impressionistDoc.h"
#include "impressionistUI.h"
#include "Brushes.h"

extern float frand();

#define FILL_RATIO 0.2

ScatterPointBrush::ScatterPointBrush(ImpressionistDoc* pDoc, char* name) :
	ImpBrush(pDoc, name)
{
}

void ScatterPointBrush::BrushBegin(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;

	glPointSize(1.0f);

	BrushMove(source, target);
}

void ScatterPointBrush::BrushMove(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;

	if (pDoc == NULL) {
		printf("ScatterPointBrush::BrushMove  document is NULL\n");
		return;
	}

	int size = dlg->getSize();
	int points = (int)(size*size*FILL_RATIO);

	glBegin(GL_POINTS);

	for (int i = 0; i < points; i++) {
		double x = target.x + frand()*size - size / 2.0;
		double y = target.y + frand()*size - size / 2.0;
		SetColor(Point(x, y));
		glVertex2d(x, y);
	}

	glEnd();
}

void ScatterPointBrush::BrushEnd(const Point source, const Point target)
{
	// do nothing so far
}


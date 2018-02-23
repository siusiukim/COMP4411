#include "impressionistDoc.h"
#include "impressionistUI.h"
#include "Brushes.h"
#include <math.h>

extern float frand();

#define CIRCLE_FINENESS 36
#define FILL_RATIO 0.2

ScatterCircleBrush::ScatterCircleBrush(ImpressionistDoc* pDoc, char* name) :
	ImpBrush(pDoc, name)
{
}

void ScatterCircleBrush::BrushBegin(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;

	int size = pDoc->getSize();
	glPointSize((float)size);
	BrushMove(source, target);
}

void ScatterCircleBrush::BrushMove(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;

	if (pDoc == NULL) {
		printf("ScatterCircleBrush::BrushMove  document is NULL\n");
		return;
	}

	int size = dlg->getSize();
	int points = (int)(size*size*FILL_RATIO);
	float alpha = dlg->getOpacity();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (int i = 0; i < points; i++) {
		glBegin(GL_POLYGON);
		double x = target.x + frand()*size - size / 2.0;
		double y = target.y + frand()*size - size / 2.0;
		SetColorWithAlpha(Point(x, y), alpha);

		for (int i = 0; i < CIRCLE_FINENESS; i++) {
			double angle = i * 2 * M_PI / CIRCLE_FINENESS;
			glVertex2d(x + cos(angle)*size/2, y + sin(angle)*size/2);
		}
		
		glEnd();
	}
	glDisable(GL_BLEND);
}

void ScatterCircleBrush::BrushEnd(const Point source, const Point target)
{
	// do nothing so far
}


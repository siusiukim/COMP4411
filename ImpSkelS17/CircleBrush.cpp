#include "impressionistDoc.h"
#include "impressionistUI.h"
#include "Brushes.h"
#include <math.h>

extern float frand();

#define CIRCLE_FINENESS 36

CircleBrush::CircleBrush(ImpressionistDoc* pDoc, char* name) :
	ImpBrush(pDoc, name)
{
}

void CircleBrush::BrushBegin(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;

	int size = pDoc->getSize();
	glPointSize((float)size);

	BrushMove(source, target);
}

void CircleBrush::BrushMove(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;

	if (pDoc == NULL) {
		printf("CircleBrush::BrushMove  document is NULL\n");
		return;
	}

	int radius = dlg->getSize()/2;
	float alpha = dlg->getOpacity();

	//Init alpha
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glBegin(GL_POLYGON);
	SetColorWithAlpha(source, alpha);

	for (int i = 0; i < CIRCLE_FINENESS; i++) {
		double angle = i * 2 * M_PI / CIRCLE_FINENESS;
		glVertex2d(target.x + cos(angle)*radius, target.y + sin(angle)*radius);
	}

	glEnd();
	glDisable(GL_BLEND);
}

void CircleBrush::BrushEnd(const Point source, const Point target)
{
	// do nothing so far
}


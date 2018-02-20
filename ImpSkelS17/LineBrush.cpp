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
	CursorBegin(target);
	BrushMove(source, target);
}

void LineBrush::BrushMove(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;

	CursorMove(target);

	if (pDoc == NULL) {
		printf("LineBrush::BrushMove  document is NULL\n");
		return;
	}

	double angle = GetDirection(source);
	float alpha = dlg->getOpacity();
	float thickness = (float)dlg->getThickness();
	int length = dlg->getSize();

	//Init alpha
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glLineWidth((float)thickness);
	glBegin(GL_POLYGON);
	SetColorWithAlpha(source, alpha);;

	double left_x_adj = -length / 2;
	double right_x_adj = length / 2;
	double top_y_adj = -thickness / 2;
	double btm_y_adj = thickness / 2;

	double sin_val = sin(angle);
	double cos_val = cos(angle);

	glVertex2d(target.x + left_x_adj*cos_val + top_y_adj*sin_val, target.y - left_x_adj*sin_val + top_y_adj*cos_val);
	glVertex2d(target.x + left_x_adj*cos_val + btm_y_adj*sin_val, target.y - left_x_adj*sin_val + btm_y_adj*cos_val);
	glVertex2d(target.x + right_x_adj*cos_val + btm_y_adj*sin_val, target.y - right_x_adj*sin_val + btm_y_adj*cos_val);
	glVertex2d(target.x + right_x_adj*cos_val + top_y_adj*sin_val, target.y - right_x_adj*sin_val + top_y_adj*cos_val);

	glEnd();
	glDisable(GL_BLEND);
}

void LineBrush::BrushEnd(const Point source, const Point target)
{
	// do nothing so far
}


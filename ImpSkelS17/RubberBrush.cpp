#include "impressionistDoc.h"
#include "impressionistUI.h"
#include "Brushes.h"
#include <math.h>
#include <vector>

extern float frand();

RubberBrush::RubberBrush(ImpressionistDoc* pDoc, char* name) :
	ImpBrush(pDoc, name)
{
}

void RubberBrush::BrushBegin(const Point source, const Point target)
{
	//ImpressionistDoc* pDoc = GetDocument();
	//ImpressionistUI* dlg = pDoc->m_pUI;
	last_point = target;
	BrushMove(source, target);
}

void RubberBrush::BrushMove(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;
	PaintView* pv = dlg->m_paintView;

	CursorMove(target);

	if (pDoc == NULL) {
		printf("LineBrush::BrushMove  document is NULL\n");
		return;
	}

	double angle = GetDirection(source);
	int size = dlg->getSize() * 5;

	glPointSize((float)1);
	glBegin(GL_POINTS);
	
	//Drag a rectangle of pixel in front of the cursor
	//From far to near
	const double max_dist = size/2;
	const int x_diff = target.x - last_point.x;
	const int y_diff = target.y - last_point.y;
	printf("Diff: %d %d\n", x_diff, y_diff);
	
	struct brush_paint {
		int x;
		int y;
		GLubyte color_r;
		GLubyte color_g;
		GLubyte color_b;
	};

	std::vector<brush_paint> brush_store;
	for (int y = size/2; y >= -size/2; y--) {
		for (int x = size/2; x >= -size/2; x--) {
			double dist = sqrt(y*y+x*x);
			if (dist > max_dist) continue;

			Point dragging(target.x + x, target.y + y);

			int rep_x = x_diff * (max_dist - dist) / max_dist;
			int rep_y = y_diff * (max_dist - dist) / max_dist;
			//int rep_x = x_diff;
			//int rep_y = y_diff;

			Point rep_point(target.x + x - rep_x, target.y + y - rep_y);

			//if (x==0 && y==0)
			//printf("Dag: %d %d Rep: %d %d\n", dragging.x, dragging.y, rep_point.x, rep_point.y);

			brush_paint this_brush;
			this_brush.x = dragging.x;
			this_brush.y = dragging.y;
			GLubyte* pix = pDoc->GetPaintedPixel(rep_point.x, rep_point.y);

			this_brush.color_r = pix[0];
			this_brush.color_g = pix[1];
			this_brush.color_b = pix[2];
			brush_store.push_back(this_brush);
		}
	}

	//Apply the brush here
	for (int i = 0; i < brush_store.size(); i++) {
		brush_paint this_brush = brush_store[i];
		glColor3ub(this_brush.color_r, this_brush.color_g, this_brush.color_b);
		glVertex2d(this_brush.x, this_brush.y);
	}

	glEnd();
	last_point = target;

	dlg->m_paintView->SaveCurrentContent();
	dlg->m_paintView->RestoreContent();
}

void RubberBrush::BrushEnd(const Point source, const Point target)
{
	// do nothing so far
}


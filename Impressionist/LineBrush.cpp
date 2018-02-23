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

	float single_x_adj = length * cos(angle);
	float single_y_adj = length * sin(angle);

	int left_x = 0, left_y = 0;
	int right_x = 0, right_y = 0;

	{
		//Check for the right end
		float pixel_dist = 0;
		int curr_x = target.x, curr_y = target.y;
		float x_rem=0, y_rem=0;
		while (pixel_dist <= length) {
			//Compute dist
			float this_dist = sqrtf((curr_x - target.x)*(curr_x - target.x) + (curr_y - target.y)*(curr_y - target.y));
			pixel_dist++;
			x_rem += 1 * abs(single_x_adj) / length;
			y_rem += 1 * abs(single_y_adj) / length;

			if (dlg->m_clipEdge) {
				//Check for edge
				if (pDoc->IsEdgeAtPixel(curr_x, curr_y)) {
					break;
				}
			}

			//Compute x, y
			if (x_rem > 1) {
				x_rem--;
				curr_x++;
			}
			if (y_rem > 1) {
				y_rem--;
				curr_y++;
			}

		}
		right_x = curr_x;
		right_y = curr_y;
	}

	{
		//Check for the left end
		float pixel_dist = 0;
		int curr_x = target.x, curr_y = target.y;
		float x_rem = 0, y_rem = 0;
		while (pixel_dist <= length) {
			//Compute dist
			float this_dist = sqrtf((curr_x - target.x)*(curr_x - target.x) + (curr_y - target.y)*(curr_y - target.y));
			pixel_dist++;
			x_rem += 1 * abs(single_x_adj) / length;
			y_rem += 1 * abs(single_y_adj) / length;

			//Compute x, y
			if (x_rem > 1) {
				x_rem--;
				curr_x--;
			}
			if (y_rem > 1) {
				y_rem--;
				curr_y--;
			}

			//Check for edge
			if (dlg->m_clipEdge) {
				if (pDoc->IsEdgeAtPixel(curr_x, curr_y)) {
					break;
				}
			}
		}

		left_x = curr_x;
		left_y = curr_y;
	}

	//Init alpha
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glLineWidth(thickness);
	//glBegin(GL_POLYGON);
	glBegin(GL_LINES);
	SetColorWithAlpha(source, alpha);

	//double left_x_adj = -length / 2;
	//double right_x_adj = length / 2;
	//double top_y_adj = -thickness / 2;
	//double btm_y_adj = thickness / 2;

	//double sin_val = sin(angle);
	//double cos_val = cos(angle);

	//glVertex2d(target.x + left_x_adj*cos_val + top_y_adj*sin_val, target.y - left_x_adj*sin_val + top_y_adj*cos_val);
	//glVertex2d(target.x + left_x_adj*cos_val + btm_y_adj*sin_val, target.y - left_x_adj*sin_val + btm_y_adj*cos_val);
	//glVertex2d(target.x + right_x_adj*cos_val + btm_y_adj*sin_val, target.y - right_x_adj*sin_val + btm_y_adj*cos_val);
	//glVertex2d(target.x + right_x_adj*cos_val + top_y_adj*sin_val, target.y - right_x_adj*sin_val + top_y_adj*cos_val);

	//glVertex2d(left_x + sin_val * thickness / 2, left_y + cos_val * thickness / 2);
	//glVertex2d(left_x - sin_val * thickness / 2, left_y - cos_val * thickness / 2);

	//glVertex2d(right_x - sin_val * thickness / 2, left_y - cos_val * thickness / 2);
	//glVertex2d(right_x + sin_val * thickness / 2, left_y + cos_val * thickness / 2);

	//printf("T:%.0f C:%.2f S:%.2f\n", thickness, cos_val, sin_val);
	//printf("L: %d %d R: %d %d\n", left_x, left_y, right_x, right_y);
	//printf("%f %f\n", left_x + sin_val * thickness / 2, left_y + cos_val * thickness / 2);
	//printf("%f %f\n", left_x - sin_val * thickness / 2, left_y - cos_val * thickness / 2);
	//printf("%f %f\n", right_x - sin_val * thickness / 2, right_y - cos_val * thickness / 2);
	//printf("%f %f\n", right_x + sin_val * thickness / 2, right_y + cos_val * thickness / 2);

	glVertex2d(left_x, left_y);
	glVertex2d(right_x, right_y);

	glEnd();
	glDisable(GL_BLEND);
}

void LineBrush::BrushEnd(const Point source, const Point target)
{
	// do nothing so far
}


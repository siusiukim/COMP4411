//
// CustomBrush.cpp
//
// The implementation of Point Brush. It is a kind of ImpBrush. All your brush implementations
// will look like the file with the different GL primitive calls.
//

#include "impressionistDoc.h"
#include "impressionistUI.h"
#include "Brushes.h"

extern float frand();

CustomBrush::CustomBrush(ImpressionistDoc* pDoc, char* name) :
	ImpBrush(pDoc, name)
{
}

void CustomBrush::setOriginalPixelColor(unsigned char* rgb) {
	brush_rgb[0] = rgb[0];
	brush_rgb[1] = rgb[1];
	brush_rgb[2] = rgb[2];
	printf("Color set: %d %d %d\n", brush_rgb[0], brush_rgb[1], brush_rgb[2]);
}

void CustomBrush::BrushBegin(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;

	int size = pDoc->getSize();
	glPointSize((float)size);
	BrushMove(source, target);
}

void CustomBrush::BrushMove(const Point source, const Point target)
{
	ImpressionistDoc* pDoc = GetDocument();
	ImpressionistUI* dlg = pDoc->m_pUI;

	if (pDoc == NULL) {
		printf("CustomBrush::BrushMove  document is NULL\n");
		return;
	}
	glPointSize((float)1);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBegin(GL_POINTS);

	int x_adj = pDoc->m_nBrushWidth / 2;
	int y_adj = pDoc->m_nBrushHeight / 2;

	for (int y = 0; y < pDoc->m_nBrushHeight; y++) {
		for (int x = 0; x < pDoc->m_nBrushWidth; x++) {
			float alpha = (pDoc->m_ucBrush[(y*pDoc->m_nBrushWidth + x) * 3] 
				+ pDoc->m_ucBrush[(y*pDoc->m_nBrushWidth + x) * 3 + 1] 
				+ pDoc->m_ucBrush[(y*pDoc->m_nBrushWidth + x) * 3 + 2])/3.0f/255.0f;

			Point adj_source(source.x - x_adj + x, source.y - y_adj + y);
			unsigned char* original_rgb = pDoc->GetOriginalPixel(adj_source);

			unsigned char real_color[3];
			for (int i = 0; i < 3; i++) {
				real_color[i] = sqrt(original_rgb[i] * brush_rgb[i] * alpha);
			}

			glColor3ub(real_color[0], real_color[1], real_color[2]);

			glVertex2d(target.x - x_adj + x, target.y - y_adj + y);
		}
	}

	glEnd();
	glDisable(GL_BLEND);
}

void CustomBrush::BrushEnd(const Point source, const Point target)
{
	// do nothing so far
}


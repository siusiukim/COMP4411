#include "impressionist.h"
#include "impressionistDoc.h"
#include "impressionistUI.h"
#include "paintview.h"
#include "ImpBrush.h"
#include "MathUtil.hpp"
#include <math.h>
#include <vector>
#include <random>
#include <algorithm>

#define LEFT_MOUSE_DOWN		1
#define LEFT_MOUSE_DRAG		2
#define LEFT_MOUSE_UP		3
#define RIGHT_MOUSE_DOWN	4
#define RIGHT_MOUSE_DRAG	5
#define RIGHT_MOUSE_UP		6

extern float frand();
extern float z_frand();

void PaintView::autoPaint(int spacing) {

	if (!m_pDoc->m_ucPainting) return;
	draw();

	std::random_device rd;
	std::mt19937 g(rd());

	glDrawBuffer(GL_FRONT_AND_BACK);

	//Copy first
	SaveCurrentContent();
	m_pDoc->copyCanvas();
	RestoreContent();
	SaveCurrentContent();
	m_pDoc->copyCanvas();
	RestoreContent();

	ImpressionistUI* ui = m_pDoc->m_pUI;

	int orig_size = ui->getSize();
	int orig_thickness = ui->getThickness();
	int orig_angle = ui->getAngle();

	struct brush_attr {
		Point source, target;
		int size;
		int thickness;
		int angle;
	};

	std::vector<brush_attr> brush_store;
	for (int i = 0; i < m_nDrawWidth; i += spacing) {
		for (int j = 0; j < m_nDrawHeight; j += spacing) {
			//Point draw(i + (frand() - 0.5f) * spacing * 0.5f, j + (frand() - 0.5f) * spacing * 0.5f);
			Point draw(i, j);
			Point source, target;
			convertPoint(draw, &source, &target);

			//Randomness in brush attributes
			int thickness = orig_thickness + (frand() - 0.5f) * 0.3f * orig_thickness;
			int angle = orig_angle + (frand() - 0.5f) * 18;
			int size = orig_size + (frand() - 0.5f) * 0.3f * orig_size;

			brush_store.push_back({source, target, size, orig_thickness, angle});
		}
	}

	std::shuffle(brush_store.begin(), brush_store.end(), g);
	for (int i = 0; i < brush_store.size(); i++) {
		ui->setSize(brush_store[i].size);
		ui->setThickess(brush_store[i].thickness);
		ui->setAngle(brush_store[i].angle);
		m_pDoc->m_pCurrentBrush->BrushBegin(brush_store[i].source, brush_store[i].target);
	}

	ui->setSize(orig_size);
	ui->setThickess(orig_thickness);
	ui->setAngle(orig_angle);

	SaveCurrentContent();
	RestoreContent();
	glFlush();

	glDrawBuffer(GL_FRONT_AND_BACK);

	m_pDoc->m_pUI->m_origView->redraw();
}

const int resolution_steps[] = { 32, 16, 8, 4, 2 };
const int resolution_count = 5;
const float res_gau_kernel[resolution_count][5][5] = {
	{
	{ 0.032741,0.037861,0.03974,0.037861,0.032741 },
	{0.037861,0.043782,0.045955,0.043782,0.037861},
	{0.03974,0.045955,0.048235,0.045955,0.03974},
	{0.037861,0.043782,0.045955,0.043782,0.037861},
	{0.032741,0.037861,0.03974,0.037861,0.032741}
	}
	,
	{
	{ 0.017056,0.030076,0.036334,0.030076,0.017056 },
	{0.030076,0.053035,0.064071,0.053035,0.030076},
	{0.036334,0.064071,0.077404,0.064071,0.036334},
	{0.030076,0.053035,0.064071,0.053035,0.030076},
	{0.017056,0.030076,0.036334,0.030076,0.017056}
	}
	,
	{
	{ 0.000874,0.006976,0.01386,0.006976,0.000874 },
	{0.006976,0.0557,0.110656,0.0557,0.006976},
	{0.01386,0.110656,0.219833,0.110656,0.01386},
	{0.006976,0.0557,0.110656,0.0557,0.006976},
	{0.000874,0.006976,0.01386,0.006976,0.000874}
	}
	,
	{
	{ 0,0.000009,0.000069,0.000009,0 },
	{0.000009,0.011143,0.083256,0.011143,0.000009},
	{0.000069,0.083256,0.622048,0.083256,0.000069},
	{0.000009,0.011143,0.083256,0.011143,0.000009},
	{0,0.000009,0.000069,0.000009,0}
	}
	,
	{
		{ 0,0,0,0,0 },
	{0,0.000039,0.006133,0.000039,0},
	{0,0.006133,0.975316,0.006133,0},
	{0,0.000039,0.006133,0.000039,0},
	{0,0,0,0,0}
	}
};
void PaintView::autoMultiPaint(int spacing) {

	if (!m_pDoc->m_ucPainting) return;
	draw();

	glDrawBuffer(GL_FRONT_AND_BACK);

	ImpressionistUI* ui = m_pDoc->m_pUI;

	int orig_size = ui->getSize();
	int orig_thickness = ui->getThickness();
	int orig_angle = ui->getAngle();

	//Random
	std::random_device rd;
	std::mt19937 g(rd());

	//Clear first
	SaveCurrentContent();
	m_pDoc->clearCanvas();
	RestoreContent();
	SaveCurrentContent();
	m_pDoc->clearCanvas();
	RestoreContent();

	for (int res_idx = 0; res_idx < resolution_count; res_idx++) {
		//Blur the image to a suitable degree
		unsigned char * blurred_raw = new unsigned char[m_nDrawWidth*m_nDrawHeight * 3];
		
		apply_filter(m_pDoc->m_ucBitmap, blurred_raw, m_nDrawWidth, m_nDrawHeight, res_gau_kernel[res_idx][0], 5, 5);

		int *difference_map = new int[m_nDrawWidth*m_nDrawHeight];
		if (res_idx == 0) {
			//Max diff for the first painting, according to the paper
			for (int i = 0; i < m_nDrawWidth*m_nDrawHeight; i++) {
				difference_map[i] = 65535;
			}
		}
		else {
			//Find the difference
			unsigned char* newArray = m_pDoc->m_ucPainting;
			unsigned char* orgArray = blurred_raw;
			for (int i = 0; i < m_nDrawWidth*m_nDrawHeight; i++) {
				difference_map[i] = (int)sqrtf(((int)newArray[i * 3 + 0] - (int)orgArray[i * 3 + 0])*((int)newArray[i * 3 + 0] - (int)orgArray[i * 3 + 0])
					+ ((int)newArray[i * 3 + 1] - (int)orgArray[i * 3 + 1]) * ((int)newArray[i * 3 + 1] - (int)orgArray[i * 3 + 1])
					+ ((int)newArray[i * 3 + 2] - (int)orgArray[i * 3 + 2]) * ((int)newArray[i * 3 + 2] - (int)orgArray[i * 3 + 2]));
			}
		}

		struct brush_attr{
			int x;
			int y;
		};

		std::vector<brush_attr> brush_store;

		for (int gridY = spacing / 2; gridY < m_nDrawHeight - spacing / 2; gridY += spacing) {
			for (int gridX = spacing/2; gridX < m_nDrawWidth - spacing / 2; gridX += spacing) {
			
				//Gather information within the grid
				int error_sum = 0, max_error = 0, max_error_x = -1, max_error_y = -1;
				for (int adj_y = -spacing / 2; adj_y <= spacing / 2; adj_y++) {
					int y_grow = (gridY + adj_y) * m_nDrawWidth;
					for (int adj_x = -spacing / 2; adj_x <= spacing / 2; adj_x++) {
						int total_grow = y_grow + adj_x + gridX;
						int this_error = difference_map[total_grow];
						
						error_sum += this_error;
						if (this_error > max_error) {
							max_error = this_error;
							max_error_x = gridX + adj_x;
							max_error_y = gridY + adj_y;
						}
					}
				}

				if (error_sum / spacing / spacing > ui->m_threshold) {
					//Paint at max_error point when average paint error > threshold
					brush_attr attr = { max_error_x, max_error_y };
					brush_store.push_back(attr);
				}
			}
		}

		//Apply the paint brush in random order
		std::shuffle(brush_store.begin(), brush_store.end(), g);
		ui->setSize(resolution_steps[res_idx]);
		for (int i = 0; i < brush_store.size(); i++) {
			Point draw(brush_store[i].x, brush_store[i].y);
			Point source, target;
			convertPoint(draw, &source, &target);
			m_pDoc->m_pCurrentBrush->BrushBegin(source, target);
		}

		//Claim memory
		delete[] blurred_raw;

		SaveCurrentContent();
	}

	ui->setSize(orig_size);
	ui->setThickess(orig_thickness);
	ui->setAngle(orig_angle);

	SaveCurrentContent();
	RestoreContent();
	glFlush();

	glDrawBuffer(GL_FRONT_AND_BACK);
	m_pDoc->m_pUI->m_origView->redraw();
}

double PaintView::getRootMeanSquareCost(const unsigned char* org, const unsigned char* cmp) {
	double sum = 0;
	for (int i = 0; i < m_nDrawWidth*m_nDrawHeight; i++) {
		sum += (org[i * 3 + 0] - cmp[i * 3 + 0]) * (org[i * 3 + 0] - cmp[i * 3 + 0]) +
			(org[i * 3 + 1] - cmp[i * 3 + 1]) * (org[i * 3 + 1] - cmp[i * 3 + 1]) +
			(org[i * 3 + 2] - cmp[i * 3 + 2]) * (org[i * 3 + 2] - cmp[i * 3 + 2]);
	}
	return sqrt(sum / m_nDrawWidth / m_nDrawHeight / 3);
}

#define MUTANT_RATE 0.1f
#define SIZE_MUTANT_RATE 0.1f
void PaintView::autoLearnPaint(int numLearn) {

	if (!m_pDoc->m_ucPainting) return;
	draw();

	double minCost = INFINITY;
	int min_train_idx = 0;
	unsigned char* best_paint = new unsigned char[m_nDrawWidth*m_nDrawHeight * 3];

	Point *posArray = new Point[6 * numLearn];
	int *sizeArray = new int[6 * numLearn];
	int *thicknessArray = new int[6 * numLearn];
	int *angleArray = new int[6 * numLearn];

	//randomly init params
	for (int i = 5 * numLearn; i < 6 * numLearn; i++) {
		posArray[i] = Point(frand()*m_nDrawWidth, frand()*m_nDrawHeight);
		sizeArray[i] = frand()*(MAX_POINT_SIZE - MIN_POINT_SIZE) + MIN_POINT_SIZE;
		//thicknessArray[i] = frand()*(MAX_THICKNESS - MIN_THICKNESS) + MIN_THICKNESS;
		thicknessArray[i] = sizeArray[i] * 0.3f;
		angleArray[i] = frand()*(MAX_ANGLE - MIN_ANGLE) + MIN_ANGLE;
	}

	ImpressionistUI* ui = m_pDoc->m_pUI;

	SaveCurrentContent();
	m_pDoc->clearCanvas();
	RestoreContent();
	SaveCurrentContent();
	m_pDoc->clearCanvas();
	RestoreContent();

	//Save things
	int orig_size = ui->getSize();
	int orig_thickness = ui->getThickness();
	int orig_angle = ui->getAngle();

	for (int i = 0; i < ui->m_iterNumber; i++) {

		if (i % 1000 == 0) {
			printf("Iter: %d\n", i);
		}

		glDrawBuffer(GL_FRONT_AND_BACK);

		SaveCurrentContent();
		m_pDoc->clearCanvas();
		RestoreContent();

		//Pass on genes
		for (int i = 0; i < 3 * numLearn; i++) {
			if (frand() > 0.5f) continue;
			int idx = 5 * numLearn + i % numLearn;
			posArray[i] = Point(z_frand()*m_nDrawWidth*MUTANT_RATE + posArray[idx].x, z_frand()*m_nDrawHeight*MUTANT_RATE + posArray[idx].y);
			sizeArray[i] = z_frand()*(MAX_POINT_SIZE - MIN_POINT_SIZE)*SIZE_MUTANT_RATE + sizeArray[idx];
			//thicknessArray[i] = z_frand()*(MAX_THICKNESS - MIN_THICKNESS)*SIZE_MUTANT_RATE + thicknessArray[idx];
			thicknessArray[i] = sizeArray[i] * 0.3f;
			angleArray[i] = z_frand()*(MAX_ANGLE - MIN_ANGLE)*MUTANT_RATE + angleArray[idx];

			posArray[i].x = cap_range(posArray[i].x, 0, m_nDrawWidth);
			posArray[i].y = cap_range(posArray[i].y, 0, m_nDrawHeight);
			sizeArray[i] = cap_range(sizeArray[i], MIN_POINT_SIZE, MAX_POINT_SIZE);
			thicknessArray[i] = cap_range(thicknessArray[i], MIN_THICKNESS, MAX_THICKNESS);
			angleArray[i] = cap_range(angleArray[i], MIN_ANGLE, MAX_ANGLE);
		}

		//Random the last two
		for (int i = 3 * numLearn; i < 5 * numLearn; i++) {
			posArray[i] = Point(frand()*m_nDrawWidth, frand()*m_nDrawHeight);
			sizeArray[i] = frand()*(MAX_POINT_SIZE - MIN_POINT_SIZE) + MIN_POINT_SIZE;
			//thicknessArray[i] = frand()*(MAX_THICKNESS - MIN_THICKNESS) + MIN_THICKNESS;
			thicknessArray[i] = sizeArray[i] * 0.3f;
			angleArray[i] = frand()*(MAX_ANGLE - MIN_ANGLE) + MIN_ANGLE;
		}

		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < numLearn; j++) {
				Point source, target;
				convertPoint(posArray[j], &source, &target);

				//Randomness in brush attributes
				ui->setSize(sizeArray[i*numLearn + j]);
				ui->setThickess(thicknessArray[i*numLearn + j]);
				ui->setAngle(angleArray[i*numLearn + j]);

				m_pDoc->m_pCurrentBrush->BrushBegin(source, target);
			}

			SaveCurrentContent();
			glFlush();

			glDrawBuffer(GL_FRONT_AND_BACK);

			double cost = getRootMeanSquareCost(m_pDoc->m_ucBitmap, m_pDoc->m_ucPainting);
			if (cost < minCost) {
				printf("%d %f\n", i, cost);
				minCost = cost;
				min_train_idx = i;
				memcpy(best_paint, m_pDoc->m_ucPainting, m_nDrawWidth*m_nDrawHeight * 3);

				for (int i = 0; i < numLearn; i++) {
					int idx = 5 * numLearn + i % numLearn;
					posArray[idx] = posArray[min_train_idx*numLearn + i];
					sizeArray[idx] = sizeArray[min_train_idx*numLearn + i];
					thicknessArray[idx] = thicknessArray[min_train_idx*numLearn + i];
					angleArray[idx] = angleArray[min_train_idx*numLearn + i];
				}
			}
		}
	}

	memcpy(m_pDoc->m_ucPainting, best_paint, m_nDrawWidth*m_nDrawHeight * 3);

	RestoreContent();
	glFlush();

	//Restore so that user is happy
	ui->setSize(orig_size);
	ui->setThickess(orig_thickness);
	ui->setAngle(orig_angle);

	//Release memory
	delete[] best_paint;
	delete[] posArray;
	delete[] sizeArray;
	delete[] thicknessArray;
	delete[] angleArray;

	m_pDoc->m_pUI->m_origView->redraw();
}

//void PaintView::autoLearnPaint(int numLearn) {
//
//	if (!m_pDoc->m_ucPainting) return;
//
//	double minCost = INFINITY;
//	int min_train_idx = 0;
//	unsigned char* best_paint = new unsigned char[m_nDrawWidth*m_nDrawHeight * 3];
//
//	ImpressionistUI* ui = m_pDoc->m_pUI;
//
//	SaveCurrentContent();
//	m_pDoc->clearCanvas();
//	RestoreContent();
//	SaveCurrentContent();
//	m_pDoc->clearCanvas();
//	RestoreContent();
//
//	//Save things
//	int orig_size = ui->getSize();
//	int orig_thickness = ui->getThickness();
//	int orig_angle = ui->getAngle();
//
//	for (int i = 0; i < ui->m_iterNumber; i++) {
//		autoMultiPaint(ui->getSpacing());
//		double cost = getRootMeanSquareCost(m_pDoc->m_ucBitmap, m_pDoc->m_ucPainting);
//		if (cost < minCost) {
//			printf("%d %f\n", i, cost);
//			minCost = cost;
//			min_train_idx = i;
//			memcpy(best_paint, m_pDoc->m_ucPainting, m_nDrawWidth*m_nDrawHeight * 3);
//		}
//	}
//
//	memcpy(m_pDoc->m_ucPainting, best_paint, m_nDrawWidth*m_nDrawHeight * 3);
//
//	RestoreContent();
//	glFlush();
//
//	//Restore so that user is happy
//	ui->setSize(orig_size);
//	ui->setThickess(orig_thickness);
//	ui->setAngle(orig_angle);
//
//	//Release memory
//	delete[] best_paint;
//}

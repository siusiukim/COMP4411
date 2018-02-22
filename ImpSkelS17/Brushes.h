//
// PointBrush.h
//
// The header file for Point Brush. 
//

#ifndef BRUSHES_H
#define BRUSHES_H

#include "ImpBrush.h"

class PointBrush : public ImpBrush
{
public:
	PointBrush(ImpressionistDoc* pDoc = NULL, char* name = NULL);

	void BrushBegin(const Point source, const Point target);
	void BrushMove(const Point source, const Point target);
	void BrushEnd(const Point source, const Point target);
	char* BrushName(void);

	void DragDirectionBegin(const Point point) {};
	void DragDirectionMove(const Point point) {};
	void DragDirectionEnd(const Point point) {};
};

class LineBrush : public ImpBrush
{
public:
	LineBrush(ImpressionistDoc* pDoc = NULL, char* name = NULL);

	void BrushBegin(const Point source, const Point target);
	void BrushMove(const Point source, const Point target);
	void BrushEnd(const Point source, const Point target);
	char* BrushName(void);
};

class CircleBrush : public ImpBrush
{
public:
	CircleBrush(ImpressionistDoc* pDoc = NULL, char* name = NULL);

	void BrushBegin(const Point source, const Point target);
	void BrushMove(const Point source, const Point target);
	void BrushEnd(const Point source, const Point target);
	char* BrushName(void);

	void DragDirectionBegin(const Point point) {};
	void DragDirectionMove(const Point point) {};
	void DragDirectionEnd(const Point point) {};
};

class ScatterPointBrush : public ImpBrush
{
public:
	ScatterPointBrush(ImpressionistDoc* pDoc = NULL, char* name = NULL);

	void BrushBegin(const Point source, const Point target);
	void BrushMove(const Point source, const Point target);
	void BrushEnd(const Point source, const Point target);
	char* BrushName(void);

	void DragDirectionBegin(const Point point) {};
	void DragDirectionMove(const Point point) {};
	void DragDirectionEnd(const Point point) {};
};

class ScatterLineBrush : public ImpBrush
{
public:
	ScatterLineBrush(ImpressionistDoc* pDoc = NULL, char* name = NULL);

	void BrushBegin(const Point source, const Point target);
	void BrushMove(const Point source, const Point target);
	void BrushEnd(const Point source, const Point target);
	char* BrushName(void);
};

class ScatterCircleBrush : public ImpBrush
{
public:
	ScatterCircleBrush(ImpressionistDoc* pDoc = NULL, char* name = NULL);

	void BrushBegin(const Point source, const Point target);
	void BrushMove(const Point source, const Point target);
	void BrushEnd(const Point source, const Point target);
	char* BrushName(void);

	void DragDirectionBegin(const Point point) {};
	void DragDirectionMove(const Point point) {};
	void DragDirectionEnd(const Point point) {};
};

class RubberBrush : public ImpBrush
{
private:
	Point last_point;
public:
	RubberBrush(ImpressionistDoc* pDoc = NULL, char* name = NULL);

	void BrushBegin(const Point source, const Point target);
	void BrushMove(const Point source, const Point target);
	void BrushEnd(const Point source, const Point target);
	char* BrushName(void);

	void DragDirectionBegin(const Point point) {};
	void DragDirectionMove(const Point point) {};
	void DragDirectionEnd(const Point point) {};
};

class CustomBrush : public ImpBrush
{
private:
	unsigned char brush_rgb[3];
public:
	CustomBrush(ImpressionistDoc* pDoc = NULL, char* name = NULL);

	void setOriginalPixelColor(unsigned char * rgb);

	void BrushBegin(const Point source, const Point target);
	void BrushMove(const Point source, const Point target);
	void BrushEnd(const Point source, const Point target);
	char* BrushName(void);

	void DragDirectionBegin(const Point point) {};
	void DragDirectionMove(const Point point) {};
	void DragDirectionEnd(const Point point) {};
};

#endif
#ifndef IMPBRUSH_H
#define IMPBRUSH_H

//
// ImpBrush.h
//
// The header file of virtual brush. All the other brushes inherit from it.
//

#include <stdlib.h>

// Each brush type has an associated constant.
enum
{
	BRUSH_POINTS = 0,
	BRUSH_LINES,
	BRUSH_CIRCLES,
	BRUSH_SCATTERED_POINTS,
	BRUSH_SCATTERED_LINES,
	BRUSH_SCATTERED_CIRCLES,
	BRUSH_RUBBER,
	NUM_BRUSH_TYPE // Make sure this stays at the end!
};

enum
{
	DIRECTION_BY_SLIDER_OR_RIGHT = 0,
	DIRECTION_BY_MOVEMENT,
	DIRECTION_BY_GRADIENT,
	NUM_DIRECTION_TYPE // Make sure this stays at the end!
};


class ImpressionistDoc; // Pre-declaring class
class ImpressionistUI;

class Point
{
public:
	Point() {};
	Point(int xx, int yy) { x = xx; y = yy; };

	int x, y;

	bool operator==(const Point& point) {
		return x == point.x && y == point.y;
	}

	bool operator!=(const Point& point) {
		return x != point.x || y != point.y;
	}
};

class ImpBrush
{
protected:
	ImpBrush::ImpBrush(ImpressionistDoc* pDoc = NULL, char* name = NULL);

public:
	// The implementation of your brush should realize these virtual functions
	virtual void BrushBegin(const Point source, const Point target) = 0;
	virtual void BrushMove(const Point source, const Point target) = 0;
	virtual void BrushEnd(const Point source, const Point target) = 0;

	// according to the source image and the position, determine the draw color
	void SetColor(const Point source);
	void SetColorFromPainted(const Point source);
	void SetColorWithAlpha(const Point source, float alpha);

	virtual void DragDirectionBegin(const Point point);
	virtual void DragDirectionMove(const Point point);
	virtual void DragDirectionEnd(const Point point);

	//Get direction of the brush, in radian
	float GetDirection(const Point source);
	//Functions for tracking cursor movement
	void CursorBegin(Point point);
	void CursorMove(Point point);

	// get Doc to communicate with it
	ImpressionistDoc* GetDocument(void);

	// Return the name of the brush (not used in this version).
	char* BrushName(void);

	static int			c_nBrushCount;	// How many brushes we have,
	static ImpBrush**	c_pBrushes;		// and what they are.

private:
	ImpressionistDoc*	m_pDoc;

	Point dir_start_point;

	Point cursor_last_point, cursor_curr_point;
	float cursor_direction = 0.0f;

	// Brush's name (not used in this version).
	char*				m_pBrushName;
};

#endif
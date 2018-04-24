#include "CurveEvaluator.h"

float CurveEvaluator::s_fFlatnessEpsilon = 0.00001f;
int CurveEvaluator::s_iSegCount = 16;

CurveEvaluator::~CurveEvaluator(void)
{
}

Mat4f CurveEvaluator::constructMGMatrix(const Mat4f& M, Point p1, Point p2, Point p3, Point p4) {
	return Mat4f(M.get(0) * p1.x + M.get(1) * p2.x + M.get(2) * p3.x + M.get(3) * p4.x, M.get(0) * p1.y + M.get(1) * p2.y + M.get(2) * p3.y + M.get(3) * p4.y, 0, 0,
		M.get(4) * p1.x + M.get(5) * p2.x + M.get(6) * p3.x + M.get(7) * p4.x, M.get(4) * p1.y + M.get(5) * p2.y + M.get(6) * p3.y + M.get(7) * p4.y, 0, 0,
		M.get(8) * p1.x + M.get(9) * p2.x + M.get(10) * p3.x + M.get(11) * p4.x, M.get(8) * p1.y + M.get(9) * p2.y + M.get(10) * p3.y + M.get(11) * p4.y, 0, 0,
		M.get(12) * p1.x + M.get(13) * p2.x + M.get(14) * p3.x + M.get(15) * p4.x, M.get(12) * p1.y + M.get(13) * p2.y + M.get(14) * p3.y + M.get(15) * p4.y, 0, 0);
}


Point CurveEvaluator::getPointAtT(const Mat4f& M, float t) {
	float t_sq = t * t;
	float t_cu = t_sq * t;

	return Point(
		t_cu*M.get(0) + t_sq * M.get(4) + t * M.get(8) + M.get(12),
		t_cu*M.get(1) + t_sq * M.get(5) + t * M.get(9) + M.get(13)
	);
}
float CurveEvaluator::warpFloat(float original, float range) {
	while (original > range) original -= range;
	while (original < -range) original += range;
	return original;
}

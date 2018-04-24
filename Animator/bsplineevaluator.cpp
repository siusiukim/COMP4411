#include "curveevaluator.h"
#include <assert.h>
#include "mat.h"
#include <algorithm>

using namespace std;

void BSplineEvaluator::evaluateCurve(const std::vector<Point>& _ptvCtrlPts,
	std::vector<Point>& ptvEvaluatedCurvePts,
	const float& fAniLength,
	const bool& bWrap) const
{
	static const Mat4f basisMatrix = Mat4f(
		-1, 3, -3, 1,
		3, -6, 3, 0,
		-3, 0, 3, 0,
		1, 4, 1, 0) / 6.0;

	vector<Point> modCtrlPoints;
	modCtrlPoints.assign(_ptvCtrlPts.begin(), _ptvCtrlPts.end());

	int numControlPoints = modCtrlPoints.size();
	ptvEvaluatedCurvePts.clear();

	if (bWrap) {
		//Do nothing is ok
	}
	else {
		//Double start and end point
		Point startPoint = modCtrlPoints[0];
		Point lastPoint = modCtrlPoints[numControlPoints - 1];
		modCtrlPoints.push_back(startPoint);
		modCtrlPoints.push_back(lastPoint);
		modCtrlPoints.push_back(startPoint);
		modCtrlPoints.push_back(lastPoint);
	}

	numControlPoints = modCtrlPoints.size();
	std::sort(modCtrlPoints.begin(),
		modCtrlPoints.end(),
		PointSmallerXCompare());

	//Draw curves
	int idx = 0;
	for (; idx < numControlPoints - 3; idx++) {
		Mat4f MGMatrix = constructMGMatrix(basisMatrix, modCtrlPoints[idx], modCtrlPoints[idx + 1], modCtrlPoints[idx + 2], modCtrlPoints[idx + 3]);
		for (float t = 0; t < 1; t += 0.01) {
			Point p = getPointAtT(MGMatrix, t);
			ptvEvaluatedCurvePts.push_back(p);
		}
	}

	if (bWrap) {
		//Warp around with curve
		for (; idx < numControlPoints; idx++) {
			Point phantomPoint[4];
			Point startPoint = modCtrlPoints[idx];
			phantomPoint[0] = startPoint;
			for (int i = 1; i < 4; i++) {
				phantomPoint[i] = modCtrlPoints[(idx + i) % numControlPoints];
				while (phantomPoint[i].x < phantomPoint[i-1].x) {
					phantomPoint[i].x += fAniLength;
				}
			}

			Mat4f MGMatrix = constructMGMatrix(basisMatrix,
				phantomPoint[0],
				phantomPoint[1],
				phantomPoint[2],
				phantomPoint[3]);

			for (float t = 0; t < 1; t += 0.01) {
				Point p = getPointAtT(MGMatrix, t);
				p.x = warpFloat(p.x, fAniLength);
				ptvEvaluatedCurvePts.push_back(p);
			}
		}
	}
	else {
		//Connect the rest with straight lines
		ptvEvaluatedCurvePts.push_back(Point(0, modCtrlPoints[0].y));
		ptvEvaluatedCurvePts.push_back(Point(fAniLength, modCtrlPoints[numControlPoints - 1].y));
	}
}
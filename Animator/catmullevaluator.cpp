#include "curveevaluator.h"
#include <assert.h>
#include "mat.h"
#include <algorithm>

using namespace std;

void CatmullEvaluator::evaluateCurve(const std::vector<Point>& _ptvCtrlPts,
	std::vector<Point>& ptvEvaluatedCurvePts,
	const float& fAniLength,
	const bool& bWrap) const
{
	static const Mat4f basisMatrix = Mat4f(
		-1, 3, -3, 1,
		2, -5, 4, -1,
		-1, 0, 1, 0,
		0, 2, 0, 0) / 2.0;

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
	}

	numControlPoints = modCtrlPoints.size();
	std::sort(modCtrlPoints.begin(),
		modCtrlPoints.end(),
		PointSmallerXCompare());

	//Draw curves
	int idx = 0;
	float last_valid_x = 0;
	for (; idx < numControlPoints - 3; idx++) {
		Mat4f MGMatrix = constructMGMatrix(basisMatrix, modCtrlPoints[idx], modCtrlPoints[idx + 1], modCtrlPoints[idx + 2], modCtrlPoints[idx + 3]);
		for (float t = 0; t < 1; t += 0.01) {
			Point p = getPointAtT(MGMatrix, t);
			//Special fix to Catmull
			if (p.x > last_valid_x) {
				ptvEvaluatedCurvePts.push_back(p);
				last_valid_x = p.x;
			}
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
				if (p.x > last_valid_x) {
					last_valid_x = p.x;
					p.x = warpFloat(p.x, fAniLength);
					ptvEvaluatedCurvePts.push_back(p);
				}
			}
		}
	}
	else {
		//Connect the rest with straight lines
		//ptvEvaluatedCurvePts.push_back(modCtrlPoints[0]);
		//ptvEvaluatedCurvePts.push_back(modCtrlPoints[numControlPoints - 1]);
		ptvEvaluatedCurvePts.push_back(Point(0, modCtrlPoints[0].y));
		ptvEvaluatedCurvePts.push_back(Point(fAniLength, modCtrlPoints[numControlPoints - 1].y));
	}
}
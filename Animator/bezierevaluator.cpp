#include "curveevaluator.h"
#include <assert.h>
#include "mat.h"

using namespace std;

void BezierCurveEvaluator::evaluateCurve(const std::vector<Point>& ptvCtrlPts,
	std::vector<Point>& ptvEvaluatedCurvePts,
	const float& fAniLength,
	const bool& bWrap) const
{
	static const Mat4f basisMatrix
	(-1, 3, -3, 1,
		3, -6, 3, 0,
		-3, 3, 0, 0,
		1, 0, 0, 0);

	int numControlPoints = ptvCtrlPts.size();
	ptvEvaluatedCurvePts.clear();

	//Draw curves
	int idx = 0;
	for (; idx < numControlPoints - 3; idx = idx + 3) {
		Mat4f MGMatrix = constructMGMatrix(basisMatrix, ptvCtrlPts[idx], ptvCtrlPts[idx + 1], ptvCtrlPts[idx + 2], ptvCtrlPts[idx + 3]);
		for (float t = 0; t < 1; t += 0.01) {
			Point p = getPointAtT(MGMatrix, t);
			ptvEvaluatedCurvePts.push_back(p);
		}
	}

	if (bWrap) {
		if (numControlPoints % 3 == 0 && numControlPoints >= 4) {
			//Warp with curve
			Point startPoint = ptvCtrlPts[idx];
			Point phantomPoint[3];
			for (int i = 0; i < 3; i++) {
				phantomPoint[i] = ptvCtrlPts[(idx + i + 1) % numControlPoints];
				if (phantomPoint[i].x < startPoint.x) {
					phantomPoint[i].x += fAniLength;
				}
			}

			Mat4f MGMatrix = constructMGMatrix(basisMatrix,
				startPoint,
				phantomPoint[0],
				phantomPoint[1],
				phantomPoint[2]);

			for (float t = 0; t < 1; t += 0.01) {
				Point p = getPointAtT(MGMatrix, t);
				p.x = warpFloat(p.x, fAniLength);
				ptvEvaluatedCurvePts.push_back(p);
			}
		}
		else {
			//Warp with straight line
			Point startPoint = ptvCtrlPts[0];
			Point lastPoint = ptvCtrlPts[numControlPoints - 1];
			float x_diff = fAniLength - lastPoint.x + startPoint.x;
			float y_diff = lastPoint.y - startPoint.y;

			ptvEvaluatedCurvePts.push_back(lastPoint);
			ptvEvaluatedCurvePts.push_back(Point(fAniLength, lastPoint.y + (fAniLength - lastPoint.x)*-y_diff/ x_diff));
			ptvEvaluatedCurvePts.push_back(Point(0, lastPoint.y + (fAniLength - lastPoint.x)*-y_diff / x_diff));
			ptvEvaluatedCurvePts.push_back(startPoint);
		}
	}
	else {
		//Connect the rest with straight lines
		for (; idx < numControlPoints; idx++) {
			ptvEvaluatedCurvePts.push_back(ptvCtrlPts[idx]);
		}
		ptvEvaluatedCurvePts.push_back(Point(0, ptvCtrlPts[0].y));
		ptvEvaluatedCurvePts.push_back(Point(fAniLength, ptvCtrlPts[numControlPoints - 1].y));
	}
}
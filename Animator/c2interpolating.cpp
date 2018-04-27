#include "curveevaluator.h"
#include <assert.h>
#include "mat.h"
#include <algorithm>

#include "Eigen/Dense"

using namespace std;
using namespace Eigen;

void C2InterpolatingEvaluator::evaluateCurve(const std::vector<Point>& _ptvCtrlPts,
	std::vector<Point>& ptvEvaluatedCurvePts,
	const float& fAniLength,
	const bool& bWrap) const
{
	static const Mat4f basisMatrix
	(-1, 3, -3, 1,
		3, -6, 3, 0,
		-3, 3, 0, 0,
		1, 0, 0, 0);

	vector<Point> modCtrlPoints;
	modCtrlPoints.assign(_ptvCtrlPts.begin(), _ptvCtrlPts.end());

	int numControlPoints = modCtrlPoints.size();
	ptvEvaluatedCurvePts.clear();

	if (bWrap) {
		//Add an additional control point
		Point firstPoint = modCtrlPoints[0];
		firstPoint.x += fAniLength;
		modCtrlPoints.push_back(firstPoint);
	}

	numControlPoints = modCtrlPoints.size();
	std::sort(modCtrlPoints.begin(),
		modCtrlPoints.end(),
		PointSmallerXCompare());


	//Solve Ax=b
	int numEq = numControlPoints;
	MatrixXf A = MatrixXf::Zero(numEq, numEq);
	MatrixXf b = MatrixXf::Zero(numEq, 2);
	//Set first row and last row to be special
	A(0, 0) = 2;
	A(0, 1) = 1;
	A(numEq - 1, numEq - 2) = 1;
	A(numEq - 1, numEq - 1) = 2;
	Point C1NegC0 = modCtrlPoints[1] - modCtrlPoints[0];
	Point CMNegCM1 = modCtrlPoints[numControlPoints - 1] - modCtrlPoints[numControlPoints - 2];
	b(0, 0) = C1NegC0.x;
	b(0, 1) = C1NegC0.y;
	b(numEq - 1, 0) = CMNegCM1.x * 3;
	b(numEq - 1, 1) = CMNegCM1.y * 3;
	for (int i = 1; i < numEq - 1; i++) {
		A(i, i - 1) = 1;
		A(i, i + 0) = 4;
		A(i, i + 1) = 1;
		Point pointDiff = modCtrlPoints[i + 1] - modCtrlPoints[i - 1];
		b(i, 0) = pointDiff.x * 3;
		b(i, 1) = pointDiff.y * 3;
	}
	//I have my D-matrix here!
	MatrixXf x = A.colPivHouseholderQr().solve(b);

	vector<Point> berizerControls;
	berizerControls.push_back(modCtrlPoints[0]);
	for (int i = 0; i < numControlPoints - 1; i++) {
		Point prev = modCtrlPoints[i];
		Point next = modCtrlPoints[i + 1];
		Point prevD = Point(x(i, 0), x(i, 1));
		Point nextD = Point(x(i + 1, 0), x(i + 1, 1));

		berizerControls.push_back(prev + prevD / 3);
		berizerControls.push_back(next - nextD / 3);
		berizerControls.push_back(next);
	}

	//std::sort(berizerControls.begin(),
	//	berizerControls.end(),
	//	PointSmallerXCompare());

	//for (int i = 0; i < berizerControls.size(); i++) {
	//	cout << berizerControls[i] << endl;
	//}
	//cout << endl << endl;

	//Draw curves
	int numBerizer = berizerControls.size();
	float last_valid_x = 0;
	int idx = 0;
	for (; idx < numBerizer - 3; idx = idx + 3) {
		Mat4f MGMatrix = constructMGMatrix(basisMatrix, berizerControls[idx], berizerControls[idx + 1], berizerControls[idx + 2], berizerControls[idx + 3]);
		for (float t = 0; t < 1; t += 0.01) {
			Point p = getPointAtT(MGMatrix, t);
			if (p.x > last_valid_x) {
				p.x = warpFloat(p.x, fAniLength);
				ptvEvaluatedCurvePts.push_back(p);
				last_valid_x = p.x;
			}
		}
	}

	if (!bWrap) {
		//bWrap case already handled above
		//Connect the rest with straight lines
		ptvEvaluatedCurvePts.push_back(Point(0, modCtrlPoints[0].y));
		ptvEvaluatedCurvePts.push_back(Point(fAniLength, modCtrlPoints[numControlPoints - 1].y));
	}
}
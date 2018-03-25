#include <cmath>
#include <assert.h>

#include "Box.h"
#include "../vecmath/vecmath.h"

bool Box::intersectLocal(const ray& r, isect& i) const
{
	double tMin, tMax;
	bool isInter = ComputeLocalBoundingBox().intersect(r, tMin, tMax);
	if (isInter) {
		double interT = (tMin < 0) ? tMax : tMin;

		//Deleted in isect class
		Material* material = new Material();
		*material = getMaterial();

		Vec3f intersection = r.at(interT);

		if (abs(intersection[0] - (0.5)) < NORMAL_EPSILON) {
			i.setN(Vec3f(1, 0, 0));
		}
		else if (abs(intersection[0] - (-0.5)) < NORMAL_EPSILON) {
			i.setN(Vec3f(-1, 0, 0));
		}
		else if (abs(intersection[1] - (0.5)) < NORMAL_EPSILON) {
			i.setN(Vec3f(0, 1, 0));
		}
		else if (abs(intersection[1] - (-0.5)) < NORMAL_EPSILON) {
			i.setN(Vec3f(0, -1, 0));
		}
		else if (abs(intersection[2] - (0.5)) < NORMAL_EPSILON) {
			i.setN(Vec3f(0, 0, 1));
		}
		else if (abs(intersection[2] - (-0.5)) < NORMAL_EPSILON) {
			i.setN(Vec3f(0, 0, -1));
		}
		else {
			cerr << "Normal error" << endl;
		}

		i.setT(interT);
		i.setMaterial(material);
		return true;
	}
	return false;
}

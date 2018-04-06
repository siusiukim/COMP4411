#include "RayMath.h"
#include <cassert>
#include <cstdlib>

//Reference: http://paulbourke.net/geometry/reflected/
Vec3f getReflection(const Vec3f& incident, const Vec3f& normal) {
	return (incident - 2 * normal*(incident.dot(normal))).normalize();
}

double getCriticalAngle(double incidIndex, double refraIndex) {
	return asin(refraIndex / incidIndex);
}

Vec3f getRefraction(const Vec3f& incident, const Vec3f& normal, double incidIndex, double refraIndex) {
	double incidAngle = incident.getAngleWith(-normal);

	//temp values
	double sinT1, cosT1, cosT2, ratio;
	sinT1 = sin(incidAngle);
	cosT1 = cos(incidAngle);
	ratio = incidIndex / refraIndex;

	double refraAngle = asin(sinT1 * ratio);
	cosT2 = cos(refraAngle);

	Vec3f T = (incident + normal * cosT1) * ratio - normal * cosT2;

	assert(T.isNormalized());

	return T;
}

double randomInRange(double min, double max) {
	return ((double)rand() / RAND_MAX) * (max - min) + min;
}

bool someWhatTheSame(Vec3f v1, Vec3f v2, Vec3f v3, Vec3f v4) {
	if ((v1 - v2).length() > 0.01) return false;
	if ((v1 - v3).length() > 0.01) return false;
	if ((v1 - v4).length() > 0.01) return false;

	return true;
}

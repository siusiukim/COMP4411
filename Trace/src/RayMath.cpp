#include "RayMath.h"
#include <cassert>

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

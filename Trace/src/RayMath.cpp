#include "RayMath.h"

//Reference: http://paulbourke.net/geometry/reflected/
Vec3f getReflection(Vec3f incident, Vec3f normal) {
	return (incident - 2 * normal*(incident.dot(normal))).normalize();
}

double getCriticalAngle(double incidIndex, double refraIndex) {
	return asin(refraIndex / incidIndex);
}

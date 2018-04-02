#pragma once

#include "vecmath\vecmath.h"
#include "scene/scene.h"
#include "scene/ray.h"

#define ALMOST_ZERO(x) (abs(x)<0.0001)

#ifndef M_PI
#define M_PI 3.14159265359
#endif

//Get normalized direction of reflection
Vec3f getReflection(const Vec3f& incident, const Vec3f& normal);

//Get critical angle of refraction. Total internal reflection occurs when > this value. In radian.
double getCriticalAngle(double incidIndex, double refraIndex);

Vec3f getRefraction(const Vec3f& incident, const Vec3f& normal, double incidIndex, double refraIndex);

double randomInRange(double min, double max);

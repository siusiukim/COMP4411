#pragma once

#include "vecmath\vecmath.h"
#include "scene/scene.h"
#include "scene/ray.h"

//Get normalized direction of reflection
Vec3f getReflection(Vec3f incident, Vec3f normal);

//Get critical angle of refraction. Total internal reflection occurs when > this value. In radian.
double getCriticalAngle(double incidIndex, double refraIndex);

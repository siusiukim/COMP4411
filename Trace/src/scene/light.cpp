#include <cmath>

#include "light.h"

double DirectionalLight::distanceAttenuation(const vec3f& P) const
{
	// distance to light is infinite, so f(di) goes to 0.  Return 1.
	return 1.0;
}


vec3f DirectionalLight::shadowAttenuation(const vec3f& P) const
{
	// YOUR CODE HERE:
	// You should implement shadow-handling code here.
	//return vec3f(1, 1, 1);
	ray r(P - orientation * RAY_EPSILON, -orientation);
	return scene->getShadowAttenTo(r, 1e20).clamp();
}

vec3f DirectionalLight::getColor(const vec3f& P) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f DirectionalLight::getDirection(const vec3f& P) const
{
	return -orientation;
}

double PointLight::distanceAttenuation(const vec3f& P) const
{
	// You'll need to modify this method to attenuate the intensity 
	// of the light based on the distance between the source and the 
	// point P.  For now, I assume no attenuation and just return 1.0

	double len = (position - P).length() * scene->distanceScale;
	//cout << scene->constAtten << endl;
	//cout << scene->linearAtten << endl;
	//cout << scene->quadAtten << endl;
	//cout << scene->distanceScale << endl;
	//cout << len << endl;
	//cout << 1.0 / (scene->constAtten * constAtten +
	//	scene->linearAtten*linearAtten*len +
	//	scene->quadAtten*quadAtten*len*len) << endl << endl;

	return min(1.0, 1.0 / (scene->constAtten * constAtten +
		scene->linearAtten*linearAtten*len +
		scene->quadAtten*quadAtten*len*len));
}

vec3f PointLight::getColor(const vec3f& P) const
{
	// Color doesn't depend on P 
	return color;
}

vec3f PointLight::getDirection(const vec3f& P) const
{
	return (position - P).normalize();
}


vec3f PointLight::shadowAttenuation(const vec3f& P) const
{
	// YOUR CODE HERE:
	// You should implement shadow-handling code here.
	//return vec3f(1, 1, 1);

	Vec3f orientation = (P - position).normalize();
	double t = (position - P).length();
	ray r(P - orientation * RAY_EPSILON, -orientation);
	return scene->getShadowAttenTo(r, t).clamp();
}

double AmbientLight::distanceAttenuation(const vec3f& P) const
{
	return 1.0;
}

vec3f AmbientLight::shadowAttenuation(const vec3f& P) const
{
	return vec3f(1, 1, 1);
}

vec3f AmbientLight::getColor(const vec3f& P) const
{
	return color;
}

vec3f AmbientLight::getDirection(const vec3f& P) const
{
	return Vec3f(0, 0, 1);
}
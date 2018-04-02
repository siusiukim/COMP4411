// The main ray tracer.

#include <Fl/fl_ask.h>

#include "RayTracer.h"
#include "scene/light.h"
#include "scene/material.h"
#include "scene/ray.h"
#include "fileio/read.h"
#include "fileio/parse.h"

#include "RayMath.h"

// Trace a top-level ray through normalized window coordinates (x,y)
// through the projection plane, and out into the scene.  All we do is
// enter the main ray-tracing method, getting things started by plugging
// in an initial ray weight of (0.0,0.0,0.0) and an initial recursion depth of 0.
vec3f RayTracer::trace(Scene *scene, double x, double y)
{
	ray r(vec3f(0, 0, 0), vec3f(0, 0, 0));
	scene->getCamera()->rayThrough(x, y, r);
	return traceRay(scene, r, vec3f(1.0, 1.0, 1.0), 0).clamp();
}

// Do recursive ray tracing!  You'll want to insert a lot of code here
// (or places called from here) to handle reflection, refraction, etc etc.
vec3f RayTracer::traceRay(Scene *scene, const ray& r,
	const vec3f& thresh, int depth)
{
	isect i;

	//if (depth > recurDepth || (adaptiveAnti && thresh.length() < adaptiveAmount)) return Vec3f(0, 0, 0);
	if (depth > recurDepth) return Vec3f(0, 0, 0);

	if (scene->intersect(r, i)) {
		// An intersection occured!  We've got work to do.  For now,
		// this code gets the material for the surface that was intersected,
		// and asks that material to provide a color for the ray.  

		// This is a great place to insert code for recursive ray tracing.
		// Instead of just returning the result of shade(), add some
		// more steps: add in the contributions from reflected and refracted
		// rays.

		const Material& m = i.getMaterial();
		const Vec3f point = r.at(i.t);

		//Shade from Phong's model
		Vec3f color = prod(Vec3f(1, 1, 1)-m.kt, m.shade(scene, r, i));
		//Vec3f color = m.shade(scene, r, i);

		//Relationship between ray and normal
		double rDotN = r.getDirection().dot(-i.N);
		bool isRayEnterObject = rDotN > 0;
		Vec3f properNorm = isRayEnterObject  ? i.N : -i.N;

		//Handles reflection
		if (!ALMOST_ZERO(m.kr.length()) && !ALMOST_ZERO(rDotN)) {
			Vec3f reflectDir = getReflection(r.getDirection(), properNorm);
			ray reflectRay(point + reflectDir*RAY_EPSILON, reflectDir);
			Vec3f reflectColor = traceRay(scene, reflectRay, prod(thresh, m.kr), depth + 1);
			color += prod(m.kr, reflectColor).clamp();
		}

		//Handles refraction
		if (!ALMOST_ZERO(m.kt.length()) && !ALMOST_ZERO(rDotN)) {
				double incidIndex, refraIndex;
				if (isRayEnterObject) {
					incidIndex = 1.0;
					refraIndex = m.index;
				}
				else {
					incidIndex = m.index;
					refraIndex = 1.0;
				}

				double critial = getCriticalAngle(incidIndex, refraIndex);
				double incidAngle = r.getDirection().getAngleWith(-properNorm);
				if (incidIndex <= refraIndex || incidAngle < critial) {
					//Handles refraction only when total internal reflection does not occur
					Vec3f refractDir = getRefraction(r.getDirection(), properNorm, incidIndex, refraIndex);
					ray refractRay(point, refractDir);
					color += prod(m.kt, traceRay(scene, refractRay, prod(thresh, m.kt), depth + 1));
				}
		}

		return color.clamp();
	}
	else {
		// No intersection.  This ray travels to infinity, so we color
		// it according to the background color, which in this (simple) case
		// is just black.

		return vec3f(0.0, 0.0, 0.0);
	}
}

RayTracer::RayTracer() :
	globalAmbient(0.20),
	constAtten(0.25),
	linearAtten(0.25),
	quadAtten(0.5),
	adaptiveAmount(1.0),
	recurDepth(0),
	distanceScale(20),
	adaptiveAnti(false)
{
	buffer = NULL;
	buffer_width = buffer_height = 256;
	scene = NULL;

	m_bSceneLoaded = false;
}


RayTracer::~RayTracer()
{
	delete[] buffer;
	delete scene;
}

void RayTracer::getBuffer(unsigned char *&buf, int &w, int &h)
{
	buf = buffer;
	w = buffer_width;
	h = buffer_height;
}

double RayTracer::aspectRatio()
{
	return scene ? scene->getCamera()->getAspectRatio() : 1;
}

bool RayTracer::sceneLoaded()
{
	return m_bSceneLoaded;
}

bool RayTracer::loadScene(char* fn)
{
	try
	{
		scene = readScene(fn);
	}
	catch (ParseError pe)
	{
		fl_alert("ParseError: %s\n", pe);
		return false;
	}

	if (!scene)
		return false;

	buffer_width = 256;
	buffer_height = (int)(buffer_width / scene->getCamera()->getAspectRatio() + 0.5);

	bufferSize = buffer_width * buffer_height * 3;
	buffer = new unsigned char[bufferSize];

	// separate objects into bounded and unbounded
	scene->initScene();

	// Add any specialized scene loading code here

	m_bSceneLoaded = true;

	return true;
}

void RayTracer::traceSetup(int w, int h)
{
	if (buffer_width != w || buffer_height != h)
	{
		buffer_width = w;
		buffer_height = h;

		bufferSize = buffer_width * buffer_height * 3;
		delete[] buffer;
		buffer = new unsigned char[bufferSize];
	}
	memset(buffer, 0, w*h * 3);

	//Putting some of the bundled data into the scene
	scene->constAtten = constAtten;
	scene->linearAtten = linearAtten;
	scene->quadAtten = quadAtten;
	scene->ambientAtten = globalAmbient;
	scene->ambientLight = Vec3f(0, 0, 0);
	scene->distanceScale = distanceScale;
}

void RayTracer::traceLines(int start, int stop)
{
	vec3f col;
	if (!scene)
		return;

	if (stop > buffer_height)
		stop = buffer_height;

	for (int j = start; j < stop; ++j)
		for (int i = 0; i < buffer_width; ++i)
			tracePixel(i, j);
}

void RayTracer::tracePixel(int i, int j)
{
	vec3f col;

	if (!scene)
		return;

	double x = double(i) / double(buffer_width);
	double y = double(j) / double(buffer_height);

	col = trace(scene, x, y);

	unsigned char *pixel = buffer + (i + j * buffer_width) * 3;

	pixel[0] = (int)(255.0 * col[0]);
	pixel[1] = (int)(255.0 * col[1]);
	pixel[2] = (int)(255.0 * col[2]);
}
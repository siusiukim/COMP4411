// The main ray tracer.

#include <Fl/fl_ask.h>

#include <cassert>

#include "RayTracer.h"
#include "scene/light.h"
#include "scene/material.h"
#include "scene/ray.h"
#include "fileio/read.h"
#include "fileio/parse.h"

#include "RayMath.h"

#include <stack>

// Trace a top-level ray through normalized window coordinates (x,y)
// through the projection plane, and out into the scene.  All we do is
// enter the main ray-tracing method, getting things started by plugging
// in an initial ray weight of (0.0,0.0,0.0) and an initial recursion depth of 0.
vec3f RayTracer::trace(Scene *scene, double x, double y, int idx)
{
	if (useSuperSample) {
		double pixel_width = 1.0 / (double)buffer_width;
		double pixel_height = 1.0 / (double)buffer_height;

		if (useAdaptiveAnti) {
			if (samplePixel == 1) {
				//Falls back to simple one-ray tracing
				ray r(vec3f(0, 0, 0), vec3f(0, 0, 0));
				scene->getCamera()->rayThrough(x, y, r);
				adpativeAntiMap[idx] = 1;
				return traceRay(scene, r, vec3f(1.0, 1.0, 1.0), 0).clamp();
			}
			else {
				int totalTraced = 0;
				//First 3 RGB, last one width of pixel
				vector<Vec4f> weightedResult;
				weightedResult.reserve(samplePixel*samplePixel);

				double minPixWidth = pixel_width / samplePixel;

				//In the format (xmin, ymin, xmax, ymax)
				stack<Vec4f> pending;
				pending.push(Vec4f(x - pixel_width / 2, y - pixel_height / 2,
					x + pixel_width / 2, y + pixel_height / 2));

				int currSubSampled = 0;
				while (!pending.empty()) {
					Vec4f searchSpace = pending.top();
					pending.pop();

					double xmin = searchSpace[0];
					double ymin = searchSpace[1];
					double xmax = searchSpace[2];
					double ymax = searchSpace[3];
					double this_width = xmax - xmin;
					double this_height = ymax - ymin;

					ray r(vec3f(0, 0, 0), vec3f(0, 0, 0));
					scene->getCamera()->rayThrough(searchSpace[0], searchSpace[1], r);
					Vec3f tfRay = traceRay(scene, r, vec3f(1.0, 1.0, 1.0), 0).clamp();

					scene->getCamera()->rayThrough(searchSpace[0], searchSpace[3], r);
					Vec3f trRay = traceRay(scene, r, vec3f(1.0, 1.0, 1.0), 0).clamp();

					scene->getCamera()->rayThrough(searchSpace[2], searchSpace[1], r);
					Vec3f llRay = traceRay(scene, r, vec3f(1.0, 1.0, 1.0), 0).clamp();

					scene->getCamera()->rayThrough(searchSpace[2], searchSpace[3], r);
					Vec3f lrRay = traceRay(scene, r, vec3f(1.0, 1.0, 1.0), 0).clamp();

					totalTraced++;

					if (this_width <= minPixWidth) {
						//Just average them if threshold if reached already
						Vec3f average = (tfRay + trRay + llRay + lrRay) / 4;
						weightedResult.push_back(Vec4f(average[0], average[1], average[2], this_width));
					}
					else {
						if (someWhatTheSame(tfRay, trRay, llRay, lrRay)) {
							//Ah ok, return just one ray is good
							weightedResult.push_back(Vec4f(tfRay[0], tfRay[1], tfRay[2], this_width));
						}
						else {
							//Push in next target
							pending.push(Vec4f(xmin, ymin,
								xmin + this_width / 2, ymin + this_height / 2));

							pending.push(Vec4f(xmin + this_width / 2, ymin,
								xmax, ymin + this_height / 2));

							pending.push(Vec4f(xmin, ymin + this_height,
								xmin + this_width / 2, ymax));

							pending.push(Vec4f(xmin + this_width / 2, ymin + this_height,
								xmax, ymax));
						}
					}
				}

				//Sum up all weighted smaples
				double totalWeight = pixel_width * pixel_width;
				Vec3f tracedResult(0, 0, 0);
				for (vector<Vec4f>::iterator i = weightedResult.begin(); i < weightedResult.end(); i++) {
					tracedResult = tracedResult + Vec3f((*i)[0], (*i)[1], (*i)[2]) * (*i)[3] * (*i)[3] / totalWeight;
				}

				adpativeAntiMap[idx] = totalTraced;

				return tracedResult.clamp();
			}
		}
		else {
			vector<Vec3f> traced;
			traced.resize(samplePixel*samplePixel);

			if (!sampleJitter) {
				for (int i = 0; i < samplePixel; i++) {
					double this_x = x + pixel_width * i / samplePixel;
					for (int j = 0; j < samplePixel; j++) {
						double this_y = y + pixel_height * j / samplePixel;
						ray r(vec3f(0, 0, 0), vec3f(0, 0, 0));
						scene->getCamera()->rayThrough(this_x, this_y, r);
						traced[i*samplePixel + j] = traceRay(scene, r, vec3f(1.0, 1.0, 1.0), 0).clamp();
					}
				}
			}
			else {
				for (int i = 0; i < samplePixel; i++) {
					for (int j = 0; j < samplePixel; j++) {
						double this_x = x + pixel_width * i / samplePixel + randomInRange(-pixel_width / samplePixel / 2, pixel_width / samplePixel / 2);
						double this_y = y + pixel_height * j / samplePixel + randomInRange(-pixel_height / samplePixel / 2, pixel_height / samplePixel / 2);;
						ray r(vec3f(0, 0, 0), vec3f(0, 0, 0));
						scene->getCamera()->rayThrough(this_x, this_y, r);
						traced[i*samplePixel + j] = traceRay(scene, r, vec3f(1.0, 1.0, 1.0), 0).clamp();
					}
				}
			}

			Vec3f averageColor(0, 0, 0);
			for (vector<Vec3f>::iterator iter = traced.begin(); iter != traced.end(); iter++) {
				averageColor += *iter;
			}
			return (averageColor / traced.size()).clamp();
		}
	}

	//Simple one-ray center trace
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

	if (depth > recurDepth || (adaptiveThreshold && thresh.length() < adaptiveAmount)) return Vec3f(0, 0, 0);

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
		Vec3f color = prod(Vec3f(1, 1, 1) - m.kt, m.shade(scene, r, i));
		//Vec3f color = m.shade(scene, r, i);

		//Relationship between ray and normal
		double rDotN = r.getDirection().dot(-i.N);
		bool isRayEnterObject = rDotN > 0;
		Vec3f properNorm = isRayEnterObject ? i.N : -i.N;

		//Compute refraction values
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

		double fresnel_r = 1.0, fresnel_t = 1.0;
		if (useFresnel) {
			if (incidIndex > refraIndex && incidAngle > critial) {
				//Total internal reflection
				fresnel_t = 0.0;
				fresnel_r = 1.0;
			}
			else {
				double refraAngle = asin(sin(incidAngle)*incidIndex / refraIndex);
				assert(refraAngle >= 0 && refraAngle <= M_PI);

				//Note: The equation here: https://www.scratchapixel.com/lessons/3d-basic-rendering/introduction-to-shading/reflection-refraction-fresnel
				// IS WRONG!!!

				//See wiki: https://en.wikipedia.org/wiki/Fresnel_equations
				double n1c1 = incidIndex * cos(incidAngle);
				double n1c2 = incidIndex * cos(refraAngle);

				double n2c1 = refraIndex * cos(incidAngle);
				double n2c2 = refraIndex * cos(refraAngle);

				//double frePara = (n2c1 - n1c2) / (n2c1 + n1c2);
				double frePara = (n1c1 - n2c2) / (n1c1 + n2c2);
				frePara = frePara * frePara;

				//double frePerp = (n1c2 - n2c1) / (n1c2 + n2c1);
				double frePerp = (n1c2 - n2c1) / (n1c2 + n2c1);
				frePerp = frePerp * frePerp;

				fresnel_r = (frePara + frePerp) / 2.0;
				fresnel_t = 1.0 - fresnel_r;

				assert(fresnel_r >= 0 && fresnel_r <= 1);
				assert(fresnel_t >= 0 && fresnel_t <= 1);
				//cout << "Incid angle: " << incidAngle << endl;
				//cout << "R: " << fresnel_r << " T: " << fresnel_t << endl << endl;
			}
		}

		//Handles reflection
		if (!ALMOST_ZERO(m.kr.length()) && !ALMOST_ZERO(rDotN)) {
			Vec3f reflectDir = getReflection(r.getDirection(), properNorm);
			ray reflectRay(point + reflectDir * RAY_EPSILON, reflectDir);
			Vec3f reflectColor = traceRay(scene, reflectRay, prod(thresh, m.kr), depth + 1);
			color += (prod(m.kr, reflectColor) * fresnel_r).clamp();
		}

		//Handles refraction
		if (!ALMOST_ZERO(m.kt.length()) && !ALMOST_ZERO(rDotN)) {
			if (incidIndex <= refraIndex || incidAngle < critial) {
				//Handles refraction only when total internal reflection does not occur
				Vec3f refractDir = getRefraction(r.getDirection(), properNorm, incidIndex, refraIndex);
				ray refractRay(point + refractDir * RAY_EPSILON, refractDir);
				color += (prod(m.kt, traceRay(scene, refractRay, prod(thresh, m.kt), depth + 1)) * fresnel_t).clamp();
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
	adaptiveAmount(0.0),
	recurDepth(0),
	distanceScale(20),
	adaptiveThreshold(false), useSuperSample(false), sampleJitter(false),
	samplePixel(1),
	useFresnel(false), useAdaptiveAnti(false),
	adpativeAntiMap(0)
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
	scene->ambientAtten = 1;
	scene->ambientLight = Vec3f(globalAmbient, globalAmbient, globalAmbient);
	scene->distanceScale = distanceScale;

	delete[] adpativeAntiMap;
	adpativeAntiMap = new char[w*h];
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

	col = trace(scene, x, y, i + j * buffer_width);

	unsigned char *pixel = buffer + (i + j * buffer_width) * 3;

	pixel[0] = (int)(255.0 * col[0]);
	pixel[1] = (int)(255.0 * col[1]);
	pixel[2] = (int)(255.0 * col[2]);
}
#include "ray.h"
#include "material.h"
#include "light.h"
#include "../RayMath.h"

// Apply the phong model to this point on the surface of the object, returning
// the color of that point.
vec3f Material::shade(Scene *scene, const ray& r, const isect& i) const
{
	// For now, this method just returns the diffuse color of the object.
	// This gives a single matte color for every distinct surface in the
	// scene, and that's it.  Simple, but enough to get you started.
	// (It's also inconsistent with the phong model...)

	// Your mission is to fill in this method with the rest of the phong
	// shading model, including the contributions of all the light sources.
	// You will need to call both distanceAttenuation() and shadowAttenuation()
	// somewhere in your code in order to compute shadows and light falloff.

	Vec3f shade = ke + prod(scene->ambientLight, ka);

	list<Light*>::const_iterator iter = scene->beginLights();
	Vec3f point(r.at(i.t));
	for (; iter != scene->endLights(); iter++) {
		Light* l = *iter;
		if (l->isAmbient()) {
			shade += prod(ka, l->getColor(point));
		}
		else {
			Vec3f atten = l->shadowAttenuation(point) * l->distanceAttenuation(point);

			Vec3f diffuse = (l->getColor(point) * max(i.N.dot((l->getDirection(point))), 0.0)).clamp();
			Vec3f specular = (l->getColor(point) * pow(max(r.getDirection().dot(getReflection(l->getDirection(point), i.N)), 0.0), shininess*128)).clamp();
			shade += prod(atten, (prod(diffuse, kd) + prod(specular, ks)));
		}
	}
	return shade.clamp();
}

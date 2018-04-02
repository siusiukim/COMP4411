#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "scene.h"

class Light
	: public SceneElement
{
public:
	virtual vec3f shadowAttenuation(const vec3f& P) const = 0;
	virtual double distanceAttenuation(const vec3f& P) const = 0;
	virtual vec3f getColor(const vec3f& P) const = 0;
	virtual vec3f getDirection(const vec3f& P) const = 0;

	virtual bool isAmbient() const {
		return false;
	}
	
protected:
	Light(Scene *scene, const vec3f& col)
		: SceneElement(scene), color(col) {}

	vec3f 		color;
};

class DirectionalLight
	: public Light
{
public:
	DirectionalLight(Scene *scene, const vec3f& orien, const vec3f& color)
		: Light(scene, color), orientation(orien) {}
	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation(const vec3f& P) const;
	virtual vec3f getColor(const vec3f& P) const;
	virtual vec3f getDirection(const vec3f& P) const;

protected:
	vec3f 		orientation;
};

class PointLight
	: public Light
{
public:
	PointLight(Scene *scene, const vec3f& pos, const vec3f& color,
		double constAtten, double linearAtten, double quadAtten)
		: Light(scene, color), position(pos), 
		constAtten(constAtten), linearAtten(linearAtten), quadAtten(quadAtten){}
	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation(const vec3f& P) const;
	virtual vec3f getColor(const vec3f& P) const;
	virtual vec3f getDirection(const vec3f& P) const;

protected:
	vec3f position;

	double constAtten, linearAtten, quadAtten;
};

class SpotLight
	: public Light
{
public:
	SpotLight(Scene *scene, const vec3f& pos, const vec3f& orien
		, double p_value, const vec3f& color, 
		double constAtten, double linearAtten, double quadAtten)
		: Light(scene, color), position(pos),
		orientation(orien), p_value(p_value),
		constAtten(constAtten), linearAtten(linearAtten), quadAtten(quadAtten) {}

	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation(const vec3f& P) const;
	virtual vec3f getColor(const vec3f& P) const;
	virtual vec3f getDirection(const vec3f& P) const;

protected:
	vec3f position, orientation;

	double p_value;
	double constAtten, linearAtten, quadAtten;
};

class AmbientLight
	: public Light
{
public:
	AmbientLight(Scene *scene, const vec3f& color)
		: Light(scene, color) {}
	virtual vec3f shadowAttenuation(const vec3f& P) const;
	virtual double distanceAttenuation(const vec3f& P) const;
	virtual vec3f getColor(const vec3f& P) const;
	virtual vec3f getDirection(const vec3f& P) const;

	virtual bool isAmbient() const {
		return true;
	}
};

#endif // __LIGHT_H__

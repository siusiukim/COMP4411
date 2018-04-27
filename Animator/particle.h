#ifndef PARTICLE_SYSTEM
#define PARTICLE_SYSTEM

#include "vec.h"

class Particle {
public:
	Particle(Vec3f p, Vec3f v, Vec3f f, float m, Vec3f c, int t):
		pos(p),
		vel(v),
		fac(f),
		mass(m),
		life(0),
		color(c),
		type(t)
	{
	}

	Vec3f pos, vel, fac;
	float mass;
	int life;

	Vec3f color;
	int type;

	void doEuler() {
		vel += fac / mass;
		pos += vel;
		fac = Vec3f(0, 0, 0);
	}

	bool willDie(int max) {
		return (life++ > max);
	}
};

#endif
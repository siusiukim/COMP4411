#pragma warning(disable : 4786)

#include "particleSystem.h"
#include "modelerview.h"
#include "modelerapp.h"
#include "modelerdraw.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <limits.h>

using namespace std;

/*************
 * Destructor
 *************/

ParticleSystem::~ParticleSystem()
{
	// TODO

}

/******************
 * Simulation fxns
 ******************/

 /** Start the simulation */
void ParticleSystem::startSimulation(float t)
{
	// TODO
	currParticles.clear();
	// These values are used by the UI ...
	// -ve bake_end_time indicates that simulation
	// is still progressing, and allows the
	// indicator window above the time slider
	// to correctly show the "baked" region
	// in grey.
	bake_end_time = -1;
	simulate = true;
	dirty = true;
}

/** Stop the simulation */
void ParticleSystem::stopSimulation(float t)
{
	// TODO
	currParticles.clear();
	//bake_end_time = t;
	// These values are used by the UI
	simulate = false;
	dirty = true;
}

/** Reset the simulation */
void ParticleSystem::resetSimulation(float t)
{
	// TODO
	currParticles.clear();
	// These values are used by the UI
	simulate = false;
	dirty = true;
}

/** Compute forces and update particles **/
void ParticleSystem::computeForcesAndUpdateParticles(float t)
{
	if (simulate) {
		bakeParticles(t);
	}
}


/** Render particles */
void ParticleSystem::drawParticles(float t)
{
	int key = t * 100;
	map<int, vector<Particle>>::iterator iter = baked.lower_bound(key);
	if (iter != baked.end()) {
		//Have something in it
		if (key - iter->first <= 10) {
			vector<Particle> particles = iter->second;
			for (int i = 0; i < particles.size(); i++) {
				Particle p = particles[i];
				glPushMatrix();
				{
					glTranslated(p.pos[0], p.pos[1], p.pos[2]);
					drawBox(0.1, 0.1, 0.1);
				}
				glPopMatrix();
			}
		}
		else {
			cout << "Time error" << endl;
		}
	}
}

void ParticleSystem::addParticle(Particle p) {
	currParticles.push_back(p);
}

/** Adds the current configuration of particles to
  * your data structure for storing baked particles **/
void ParticleSystem::bakeParticles(float t)
{
	int key = (t * 100);
	vector<Particle> particles;
	for (int i = 0; i < currParticles.size(); i++) {
		Particle p = currParticles[i];
		if (!p.willDie(particleLife)) {
			particles.push_back(p);
		}
	}

	//Apply force and euler
	for (int j = 0; j < particles.size(); j++) {
		for (int i = 0; i < forces.size(); i++) {
			particles[j].fac += forces[i];
		}
		particles[j].doEuler();
	}

	//Put it into map
	baked.insert(pair<int, vector<Particle>>(key, particles));
}

/** Clears out your data structure of baked particles */
void ParticleSystem::clearBaked()
{
	baked.clear();
}






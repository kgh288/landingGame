#pragma once
//----------------------------------------------------------------------------------
//
// Basic Sprite Emitter Class
//
//
//  Kevin M. Smith - CS 134 SJSU

#include "Sprite.h"
#include "Particle.h"
#include "ParticleEmitter.h"

//  General purpose Emitter class for emitting sprites
//  This works similar to a Particle emitter
//
class Emitter : public BaseObject {
public:
	Emitter(SpriteSystem *);
	void draw();
	void start();
	void stop();
	void setLifespan(float);
	void setVelocity(ofVec3f);
	void setChildImage(ofImage);
	void setChildSize(float w, float h) { childWidth = w; childHeight = h; }
	void setImage(ofImage);
	void setRate(float);
	float maxDistPerFrame();
	void setPlayer();
	
	void update();


	SpriteSystem *sys;
	float rate;
	ofVec3f velocity;
	float lifespan;
	bool started;
	float lastSpawned;
	ofImage childImage;
	ofImage image;
	bool drawable;
	bool haveChildImage;
	bool haveImage;
	float width, height;
	float childWidth, childHeight;

	bool isPlayer = false;
	ofSoundPlayer missileSound;

	void setSinWave(bool);
	glm::vec3 curveEval(float x, float scale, float cycles);
	void rotate(const vector<glm::vec3> &vin, vector<glm::vec3> &vout, float angle);
	bool isSinWave = false;
};

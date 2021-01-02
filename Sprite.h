#pragma once
//----------------------------------------------------------------------------------
//
// Basic Sprite Class
//
//
//  Kevin M. Smith - CS 134 SJSU

#include "ofMain.h"
#include "Particle.h"
#include "ParticleEmitter.h"

// This is a base object that all drawable object inherit from
// It is possible this will be replaced by ofNode when we move to 3D
//
class BaseObject {
public:
	BaseObject();
	ofVec2f trans, scale;
	float	rot;
	bool	bSelected;
	void setPosition(ofVec3f);
};

//  General Sprite class  (similar to a Particle)
//
class Sprite : public BaseObject {
public:
	Sprite();
	void draw();
	float age();
	void setImage(ofImage);
	float speed;    //   in pixels/sec
	ofVec3f velocity; // in pixels/sec
	ofImage image;
	float birthtime; // elapsed time in ms
	float lifespan;  //  time in ms
	string name;
	bool haveImage;
	float width, height;
};

//  Manages all Sprites in a system.  You can create multiple systems
//
class SpriteSystem {
public:
	void add(Sprite);
	void remove(int);
	void update();
	//void setup();
	int removeNear(ofVec3f point, float dist);
	void draw();
	//set sin wave to the sprite
	void setSinWave(bool);
	void setSinWave2(bool);
	glm::vec3 curveEval(float x, float scale, float cycles);
	void rotate(const vector<glm::vec3> &vin, vector<glm::vec3> &vout, float angle);


	bool isSinWave = false;
	bool isSinWave2 = false;
	vector<Sprite> sprites;

	ofSoundPlayer removeSound;
	
	ParticleEmitter explosionEmitter;
};

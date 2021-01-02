#include "Sprite.h"

BaseObject::BaseObject() {
	trans = ofVec3f(0,0,0);
	scale = ofVec3f(1, 1, 1);
	rot = 0;
}

void BaseObject::setPosition(ofVec3f pos) {
	trans = pos;
}

//
// Basic Sprite Object
//
Sprite::Sprite() {
	speed = 0;
	velocity = ofVec3f(0, 0, 0);
	lifespan = -1;      // lifespan of -1 => immortal 
	birthtime = 0;
	bSelected = false;
	haveImage = false;
	name = "UnamedSprite";
	width = 20;
	height = 20;
}

// Return a sprite's age in milliseconds
//
float Sprite::age() {
	return (ofGetElapsedTimeMillis() - birthtime);
}

//  Set an image for the sprite. If you don't set one, a rectangle
//  gets drawn.
//
void Sprite::setImage(ofImage img) {
	image = img;
	haveImage = true;
	width = image.getWidth();
	height = image.getHeight();
}


//  Render the sprite
//
void Sprite::draw() {

	ofSetColor(255, 255, 255, 255);

	// draw image centered and add in translation amount
	//
	if (haveImage) {
		image.draw(-width / 2.0 + trans.x, -height / 2.0 + trans.y);
	}
	else {
		// in case no image is supplied, draw something.
		// 
		ofNoFill();
//		ofSetColor(255, 0, 0);
		ofDrawRectangle(-width / 2.0 + trans.x, -height / 2.0 + trans.y, width, height);
	}
}

//  Add a Sprite to the Sprite System
//
void SpriteSystem::add(Sprite s) {
	sprites.push_back(s);
}

// Remove a sprite from the sprite system. Note that this function is not currently
// used. The typical case is that sprites automatically get removed when the reach
// their lifespan.
//
void SpriteSystem::remove(int i) {
	sprites.erase(sprites.begin() + i);
}

// remove all sprites within a given dist of point
//
int SpriteSystem::removeNear(ofVec3f point, float dist) {
	vector<Sprite>::iterator s = sprites.begin();
	vector<Sprite>::iterator tmp;
	int count = 0;

	while (s != sprites.end()) {
		ofVec3f v = s->trans - point;
		if (v.length() < dist) {

			removeSound.load("sounds/remove.mp3");
			removeSound.play();
			tmp = sprites.erase(s);
			count++;
			s = tmp;

		}
		else s++;
	}

	return count;
}

/*
void SpriteSystem::setup() {

	//explosion 
	//
	explosionEmitter.setOneShot(true);
	explosionEmitter.setEmitterType(RadialEmitter);
	explosionEmitter.setGroupSize(50);
	explosionEmitter.setLifespan(0.05);
	explosionEmitter.setParticleRadius(1);
	explosionEmitter.setVelocity(ofVec3f(1000, 0, 0));
}
*/


//  Update the SpriteSystem by checking which sprites have exceeded their
//  lifespan (and deleting).  Also the sprite is moved to it's next
//  location based on velocity and direction.
//
void SpriteSystem::update() {

	if (sprites.size() == 0) return;
	vector<Sprite>::iterator s = sprites.begin();
	vector<Sprite>::iterator tmp;

	// check which sprites have exceed their lifespan and delete
	// from list.  When deleting multiple objects from a vector while
	// traversing at the same time, use an iterator.
	//
	while (s != sprites.end()) {
		if (s->lifespan != -1 && s->age() > s->lifespan) {
			//			cout << "deleting sprite: " << s->name << endl;
			tmp = sprites.erase(s);
			s = tmp;
		}
		else s++;
	}

	//  Move sprite
	//

	if (isSinWave) {

		int scale = 200;
		int num_cycle = 2;

		for (int i = 0; i < sprites.size(); i++) {

			sprites[i].velocity = curveEval(sprites[i].trans.x, scale, num_cycle);
			glm::vec3 temp = glm::vec3(sprites[i].trans.y, sprites[i].trans.x, 0);
			
			sprites[i].velocity = temp;
			sprites[i].trans += sprites[i].velocity / ofGetFrameRate();
		}
	}
	else if (isSinWave2) {

		int scale = 200;
		int num_cycle = 2;

		for (int i = 0; i < sprites.size(); i++) {

			sprites[i].velocity = curveEval(sprites[i].trans.x, scale, num_cycle);
			glm::vec3 temp = glm::vec3(-sprites[i].trans.y, sprites[i].trans.x, 0);

			sprites[i].velocity = temp;
			sprites[i].trans += sprites[i].velocity / ofGetFrameRate();
		}
	}
	else {
		for (int i = 0; i < sprites.size(); i++) {
			
			sprites[i].trans += sprites[i].velocity / ofGetFrameRate();
		}
	}
	//explosionEmitter.update();
}

//  Render all the sprites
//
void SpriteSystem::draw() {
	for (int i = 0; i < sprites.size(); i++) {
		sprites[i].draw();
	}
	explosionEmitter.draw();
}

void SpriteSystem::setSinWave(bool set)
{
	isSinWave = set;
}

void SpriteSystem::setSinWave2(bool set)
{
	isSinWave2 = set;
}

glm::vec3 SpriteSystem::curveEval(float x, float scale, float cycles)
{
	// x is in screen coordinates and his in [0, WindowWidth]
	float u = (cycles * x * PI) / ofGetWidth();
	return (glm::vec3(x, -scale * sin(u) + (ofGetHeight() / 2), 0));
}

// rotate vertices (vin) by "angle" and return result in (vout).
// use standard 2D rotation matrix (covered in class) to compute new x, y;
//
void SpriteSystem::rotate(const vector<glm::vec3> &vin, vector<glm::vec3> &vout, float angle) {
	for (int i = 0; i < vin.size(); i++) {
		vout.push_back(glm::vec3(vin[i].x * cos(angle) - vin[i].y *sin(angle),
			vin[i].x * sin(angle) + vin[i].y *cos(angle), 0));
	}
}


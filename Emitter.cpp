#include "ofMain.h"
#include "Emitter.h"



//  Create a new Emitter - needs a SpriteSystem
//
Emitter::Emitter(SpriteSystem *spriteSys) {
	sys = spriteSys;
	lifespan = 3000;    // milliseconds
	started = false;

	lastSpawned = 0;
	rate = 1;    // sprites/sec
	haveChildImage = false;
	haveImage = false;
	velocity = ofVec3f(100, 100, 0);
	drawable = true;
	width = 50;
	height = 50;
	childWidth = 10;
	childHeight = 10;
}

//  Draw the Emitter if it is drawable. In many cases you would want a hidden emitter
//
//
void Emitter::draw() {
	if (drawable) {

		if (haveImage) {
			image.draw(-image.getWidth() / 2.0 + trans.x, -image.getHeight() / 2.0 + trans.y);
		}
		else {
			//ofSetColor(0, 0, 255);
			ofDrawRectangle(-width / 2 + trans.x, -height / 2 + trans.y, width, height);
		}
	}

	// draw sprite system
	//
	sys->draw();
}

//  Update the Emitter. If it has been started, spawn new sprites with
//  initial velocity, lifespan, birthtime.
//
void Emitter::update() {

	if (!started) {
		sys->update();
		return;
	}
	float time = ofGetElapsedTimeMillis();
	if ((time - lastSpawned) > (1000.0 / rate)) {
		// spawn a new sprite
		Sprite sprite;
		if (haveChildImage) sprite.setImage(childImage);
		sprite.velocity = velocity;
		sprite.lifespan = lifespan;
		sprite.setPosition(trans);
		sprite.birthtime = time;
		sprite.width = childWidth;
		sprite.height = childHeight;
		sys->add(sprite);

		//Sound Check
		if (isPlayer)
		{
			missileSound.load("sounds/laser.wav");
			missileSound.play();
		}

		lastSpawned = time;
	}
	sys->update();
}

// Start/Stop the emitter.
//
void Emitter::start() {
	started = true;
	lastSpawned = ofGetElapsedTimeMillis();
}

void Emitter::stop() {
	started = false;
}


void Emitter::setLifespan(float life) {
	lifespan = life;
}

void Emitter::setVelocity(ofVec3f v) {
	velocity = v;
}

void Emitter::setChildImage(ofImage img) {
	childImage = img;
	haveChildImage = true;
	childWidth = img.getWidth();
	childHeight = img.getHeight();
}

void Emitter::setImage(ofImage img) {
	image = img;
	haveImage = true;
}

void Emitter::setRate(float r) {
	rate = r;
}

float Emitter::maxDistPerFrame() {
	return  velocity.length() / ofGetFrameRate();
}

void Emitter::setPlayer()
{
	isPlayer = true;
}


void Emitter::setSinWave(bool set)
{
	isSinWave = set;
}

glm::vec3 Emitter::curveEval(float x, float scale, float cycles)
{
	// x is in screen coordinates and his in [0, WindowWidth]
	float u = (cycles * x * PI) / ofGetWidth();
	return (glm::vec3(x, -scale * sin(u) + (ofGetHeight() / 2), 0));
}

// rotate vertices (vin) by "angle" and return result in (vout).
// use standard 2D rotation matrix (covered in class) to compute new x, y;
//
void Emitter::rotate(const vector<glm::vec3> &vin, vector<glm::vec3> &vout, float angle) {
	for (int i = 0; i < vin.size(); i++) {
		vout.push_back(glm::vec3(vin[i].x * cos(angle) - vin[i].y *sin(angle),
			vin[i].x * sin(angle) + vin[i].y *cos(angle), 0));
	}
}
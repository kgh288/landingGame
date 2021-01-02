
//--------------------------------------------------------------
//
//  Kevin M. Smith
//
//  Octree Test - startup scene
// 
//
//  Student Name:   Gyeongheun Kim
//  Date: 12/13/2020

#include "ofApp.h"
#include "Util.h"
#include <glm/gtx/intersect.hpp>
#include "Emitter.h"


//--------------------------------------------------------------
// setup scene, lighting, state and load geometry
//
void ofApp::setup(){
	bWireframe = false;
	bDisplayPoints = false;
	bAltKeyDown = false;
	bCtrlKeyDown = false;
	bLanderLoaded = false;
	bTerrainSelected = true;
//	ofSetWindowShape(1024, 768);
	cam.setDistance(10);
	cam.setNearClip(.1);
	cam.setFov(65.5);   // approx equivalent to 28mm in 35mm format
	ofSetVerticalSync(true);
	cam.disableMouseInput();
	ofEnableSmoothing();
	ofEnableDepthTest();

	//camera setup
	mainCam = &trackingCam;


	trackingCam.setPosition(10, 10, 10);
	//sound
	moveSound.load("sounds/thrust.wav");
	backgroundMusic.load("sounds/background.mp3");
	backgroundMusic.setLoop(true);
	backgroundMusic.play();
	// setup rudimentary lighting 
	//
	initLightingAndMaterials();

	mars.loadModel("geo/moon-houdini.obj");
	mars.setScaleNormalization(false);

	//load lander
	//
	loadLander();

	//Emitter setup
	//
	if (turbForce == NULL)
		turbForce = new TurbulenceForce(ofVec3f(-1, -1, -1), ofVec3f(1, 1, 1));
	gravityForce = new GravityForce(ofVec3f(0, -400, 0));
	if (radialForce == NULL)
		radialForce = new ImpulseRadialForce(500);
	radialForce->setHeight(0);

	emitter.setEmitterType(RadialEmitter);

	emitter.sys->addForce(turbForce);
	emitter.sys->addForce(gravityForce);
	emitter.sys->addForce(radialForce);

	emitter.setGroupSize(10);
	emitter.setLifespan(0.2);
	emitter.setRate(0);
	emitter.setParticleRadius(0.3);
	emitter.setVelocity(ofVec3f(0, 1, 0));

	emitter.start();
	
	//explosion Emitter
	if (turbForce2 == NULL)
		turbForce2 = new TurbulenceForce(ofVec3f(-1, -1, -1), ofVec3f(1, 1, 1));
	if (radialForce2 == NULL)
		radialForce2 = new ImpulseRadialForce(500);
	radialForce2->setHeight(0);

	explosionEmitter.setOneShot(true);
	explosionEmitter.setEmitterType(RadialEmitter);
	explosionEmitter.sys->addForce(turbForce2);
	explosionEmitter.sys->addForce(radialForce2);

	explosionEmitter.setGroupSize(10);
	explosionEmitter.setLifespan(0.3);
	explosionEmitter.setRate(4);
	explosionEmitter.setParticleRadius(0.3);
	explosionEmitter.setVelocity(ofVec3f(0, 15, 0));

	

	// create sliders for testing
	//
	gui.setup();
	gui.add(numLevels.setup("Number of Octree Levels", 1, 1, 10));
	bHide = false;


	octree.create(mars.getMesh(0), 20);


	testBox = Box(Vector3(3, 3, 0), Vector3(5, 5, 2));

	//background image
	if (backgroundImage.loadImage("images/background.jpg")) {

	}
	light.setSpotlight();
	light.rotate(0, ofVec3f(1, 0, 0));
	light.setSpotlightCutOff(35);


	light2.setSpotlight();
	light2.rotate(180, ofVec3f(1, 0, 0));
	light2.setSpotlightCutOff(35);



}
 
//--------------------------------------------------------------
// incrementally update scene (animation)
//
void ofApp::update() {

	if (!gameStarted) return;

	cout << lander.getPosition() << "  " << altitude << endl;

	light.setPosition(lander.getPosition().x + 1, lander.getPosition().y - 2, lander.getPosition().z + 1);
	light1.setPosition(0,0,0);
	light2.setPosition(lander.getPosition().x - 1, lander.getPosition().y - 2, lander.getPosition().z - 1);
	
	trackingCam.lookAt(lander.getPosition());
	forwardCam.setPosition(lander.getPosition().x, lander.getPosition().y+ 7, lander.getPosition().z);
	forwardCam.lookAt(
		glm::vec3(
			lander.getPosition().x + glm::cos(90+ lander.getRotationAngle(0) * PI / 180),
			lander.getPosition().y+7,
			lander.getPosition().z - glm::sin(90+ (lander.getRotationAngle(0) * PI / 180))));

	glm::vec3 landerPos = lander.getPosition();
	if (fuel > 0)
	{
		if (leftKeyPressed)
		{
			velocity += accel * dt;
			fuel -= 1;
		}
		if (rightKeyPressed)
		{
			velocity -= accel * dt;
			fuel -= 1;
		}
		if (upKeyPressed)
		{
			sideVelocity += accel * dt;
			fuel -= 1;
		}
		if (downKeyPressed)
		{
			sideVelocity -= accel * dt;
			fuel -= 1;
		}
		if (leftRotationPressed)
		{
			angThrust -= angAccel * dt;
			fuel -= 1;
		}
		if (rightRotationPressed)
		{
			angThrust += angAccel * dt;
			fuel -= 1;
		}
		if (spacePressed)
		{
			yVelocity += accel * dt;
			fuel -= 1;
		}
		if (zPressed)
		{
			yVelocity -= accel * dt;
			fuel -= 1;
		}
	}
	//damping
	velocity = velocity * 0.99;
	sideVelocity = sideVelocity * 0.99;
	angThrust = angThrust * 0.99;
	yVelocity = yVelocity - gravity;
	//yVelocity -= gravity;
	
	landerPos += ofVec3f(sin((-(rotationDeg)) / fr)*velocity, yVelocity, cos((-(rotationDeg)) / fr)*velocity);
	landerPos += ofVec3f(sin((-(rotationDeg+90)) / fr)*sideVelocity, 0, cos((-(rotationDeg+90)) / fr)*sideVelocity);
	rotationDeg += angThrust;
	
	emitter.setPosition(ofVec3f(landerPos.x, landerPos.y, landerPos.z));
	emitter.update();

	explosionEmitter.setPosition(ofVec3f(landerPos.x, landerPos.y, landerPos.z));
	explosionEmitter.update();

	lander.setRotation(0, -rotationDeg+90, 0, 1, 0);
	lander.setPosition(landerPos.x, landerPos.y, landerPos.z);

	ofVec3f p;
	raySelectWithGround(p);
	checkCollisions();
	
	checkWin();
	checkCrashed();
	if (isCrashed && !lost)
	{
		lost = true;
		explosionEmitter.start();
	}

}

//--------------------------------------------------------------
void ofApp::draw() {
	
	if (!gameStarted) {
		ofSetColor(ofColor::blue);
		ofDrawBitmapString("Press spacebar to start\n\nQ/E: Rotate\nArrowKeys: move x axis\nSpacebar: up\nz: down\nnumber 1,2,3: change camera\n\nland the spaceship safely!", ofGetWindowWidth() / 2 - 100, ofGetWindowHeight() / 2 - 100);
		return;
	}

	if (lost)
	{
		ofSetColor(ofColor::white);
		ofDrawBitmapString("Spaceship is broken!", ofGetWindowWidth() / 2 - 100, ofGetWindowHeight() / 2);
		
	}
	if (isWin)
	{
		ofSetColor(ofColor::white);
		ofDrawBitmapString("Safely landed the spaceship!", ofGetWindowWidth() / 2 - 100, ofGetWindowHeight() / 2);
		return;
	}


	//backgroundImage.draw(0, 0, 1000);
	ofBackground(ofColor::black);
	
	glDepthMask(false);
	if (!bHide) gui.draw();
	backgroundImage.draw(0,0,0);

	glDepthMask(true);


	//draw altitude
	if (altitudeOn)
	{
		string altitudeText;
		altitudeText += "Altitude: " + std::to_string(altitude);
		ofDrawBitmapString(altitudeText, ofPoint(ofGetWindowWidth() - 200, 20));

		string fuelText;
		fuelText += "Fuel: " + std::to_string(fuel);
		ofDrawBitmapString(fuelText, ofPoint(ofGetWindowWidth() - 200, 50));
	}

	

	//cam.begin();

	light.enable();
	light1.enable();
	light2.enable();

	mainCam->begin();


	emitter.draw();
	explosionEmitter.draw();


	ofPushMatrix();


	if (bWireframe) {                    // wireframe mode  (include axis)
		ofDisableLighting();
		ofSetColor(ofColor::slateGray);
		mars.drawWireframe();
		if (bLanderLoaded) {
			lander.drawWireframe();
			if (!bTerrainSelected) drawAxis(lander.getPosition());
		}
		if (bTerrainSelected) drawAxis(ofVec3f(0, 0, 0));
	}
	else {
		ofEnableLighting();              // shaded mode
		mars.drawFaces();
		ofMesh mesh;
		if (bLanderLoaded) {
			if (!isCrashed)
			{
				lander.drawFaces();
			}
			
			if (!bTerrainSelected) drawAxis(lander.getPosition());
			if (bDisplayBBoxes) {
				ofNoFill();
				ofSetColor(ofColor::white);
				for (int i = 0; i < lander.getNumMeshes(); i++) {
					ofPushMatrix();
					ofMultMatrix(lander.getModelMatrix());
					ofRotate(-90, 1, 0, 0);
					Octree::drawBox(bboxList[i]);
					ofPopMatrix();
				}
			}

			if (bLanderSelected) {

				ofVec3f min = lander.getSceneMin() + lander.getPosition();
				ofVec3f max = lander.getSceneMax() + lander.getPosition();

				Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
				ofNoFill();
				ofSetColor(ofColor::white);
				Octree::drawBox(bounds);

				// draw colliding boxes
				//
				ofSetColor(ofColor::lightBlue);
				for (int i = 0; i < colBoxList.size(); i++) {
					Octree::drawBox(colBoxList[i]);
				}
			}
		}
	}
	if (bTerrainSelected) drawAxis(ofVec3f(0, 0, 0));



	if (bDisplayPoints) {                // display points as an option    
		glPointSize(3);
		ofSetColor(ofColor::green);
		mars.drawVertices();
	}

	// highlight selected point (draw sphere around selected point)
	//
	if (bPointSelected) {
		ofSetColor(ofColor::blue);
		ofDrawSphere(selectedPoint, .1);
	}


	// recursively draw octree
	//
	ofDisableLighting();
	int level = 0;
	//	ofNoFill();

	if (bDisplayLeafNodes) {
		octree.drawLeafNodes(octree.root);
		cout << "num leaf: " << octree.numLeaf << endl;
    }
	else if (bDisplayOctree) {
		ofNoFill();
		ofSetColor(ofColor::white);
		octree.draw(numLevels, 0);
	}

	// if point selected, draw a sphere
	//
	if (pointSelected) {
		ofVec3f p = octree.mesh.getVertex(selectedNode.points[0]);
		ofVec3f d = p - cam.getPosition();
		ofSetColor(ofColor::lightGreen);
		ofDrawSphere(p, .02 * d.length());
	}


	ofPopMatrix();

	mainCam->end();

	light.disable();
	light1.disable();
	light2.disable();
	//cam.end();


}


// 
// Draw an XYZ axis in RGB at world (0,0,0) for reference.
//
void ofApp::drawAxis(ofVec3f location) {

	ofPushMatrix();
	ofTranslate(location);

	ofSetLineWidth(1.0);

	// X Axis
	ofSetColor(ofColor(255, 0, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(1, 0, 0));
	

	// Y Axis
	ofSetColor(ofColor(0, 255, 0));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 1, 0));

	// Z Axis
	ofSetColor(ofColor(0, 0, 255));
	ofDrawLine(ofPoint(0, 0, 0), ofPoint(0, 0, 1));

	ofPopMatrix();
}


void ofApp::keyPressed(int key) {

	switch (key) {
	case 'B':
	case 'b':
		bDisplayBBoxes = !bDisplayBBoxes;
		break;
	case 'C':
	case 'c':
		if (cam.getMouseInputEnabled()) cam.disableMouseInput();
		else cam.enableMouseInput();
		break;
	case 'F':
	case 'f':
		ofToggleFullscreen();
		break;
	case 'H':
	case 'h':
		break;
	case 'L':
	case 'l':
		bDisplayLeafNodes = !bDisplayLeafNodes;
		break;
	case 'O':
	case 'o':
		bDisplayOctree = !bDisplayOctree;
		break;
	case 'r':
		cam.reset();
		break;
	case 's':
		savePicture();
		break;
	case 't':
		setCameraTarget();
		break;
	case 'u':
		break;
	case 'v':
		togglePointsDisplay();
		break;
	case 'V':
		break;
	case 'w':
		toggleWireframeMode();
		break;
	case 'a':
		altitudeOn = !altitudeOn;
		break;
	case OF_KEY_ALT:
		cam.enableMouseInput();
		bAltKeyDown = true;
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = true;
		break;
	case OF_KEY_SHIFT:
		break;
	case OF_KEY_DEL:
		break;
	case 'q': {
		leftRotationPressed = true;
		break;
	};
	case 'e': {
		rightRotationPressed = true;
		break;
	};
	case ' ': {

		if (!gameStarted) {
			gameStarted = true;
		}
		spacePressed = true;
		emitter.setRate(10);
		if (!moveSound.getIsPlaying())
			moveSound.play();
		moveSound.setLoop(true);
		break;
	};
	case 'z': {
		zPressed = true;
		break;
	};
	case '1': {
		mainCam = &forwardCam;
		break;
	};
	case '2': {
		mainCam = &trackingCam;
		break;
	};
	case '3': {
		mainCam = &cam;
		break;
	};
	case OF_KEY_RIGHT: {
		rightKeyPressed = true;
		break;
	};
	case OF_KEY_LEFT: {
		leftKeyPressed = true;
		break;
	};
	case OF_KEY_UP: {
		upKeyPressed = true;
		break;
	};
	case OF_KEY_DOWN: {
		downKeyPressed = true;
		break;
	};
	default:
		break;
	}
}

void ofApp::toggleWireframeMode() {
	bWireframe = !bWireframe;
}

void ofApp::toggleSelectTerrain() {
	bTerrainSelected = !bTerrainSelected;
}

void ofApp::togglePointsDisplay() {
	bDisplayPoints = !bDisplayPoints;
}

void ofApp::keyReleased(int key) {

	switch (key) {
	
	case OF_KEY_ALT:
		cam.disableMouseInput();
		bAltKeyDown = false;
		break;
	case OF_KEY_CONTROL:
		bCtrlKeyDown = false;
		break;
	case OF_KEY_SHIFT:
		break;
	case OF_KEY_RIGHT: {
		rightKeyPressed = false;
		break;
	};
	case OF_KEY_LEFT: {
		leftKeyPressed = false;
		break;
	};
	case OF_KEY_UP: {
		upKeyPressed = false;
		break;
	};
	case OF_KEY_DOWN: {
		downKeyPressed = false;
		break;
	};
	case ' ': {
		spacePressed = false;
		emitter.setRate(0);
		moveSound.stop();
		break;
	};
	case 'q': {
		leftRotationPressed = false;
		break;
	};
	case 'e': {
		rightRotationPressed = false;
		break;
	};
	case 'z': {
		zPressed = false;
		break;
	};
	default:
		break;

	}
}



//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

	
}


//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

	// if moving camera, don't allow mouse interaction
	//
	if (cam.getMouseInputEnabled()) return;

	// if moving camera, don't allow mouse interaction
//
	if (cam.getMouseInputEnabled()) return;

	// if rover is loaded, test for selection
	//
	if (bLanderLoaded) {
		glm::vec3 origin = cam.getPosition();
		glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
		glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);

		ofVec3f min = lander.getSceneMin() + lander.getPosition();
		ofVec3f max = lander.getSceneMax() + lander.getPosition();

		Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
		bool hit = bounds.intersect(Ray(Vector3(origin.x, origin.y, origin.z), Vector3(mouseDir.x, mouseDir.y, mouseDir.z)), 0, 10000);
		if (hit) {
			bLanderSelected = true;
			mouseDownPos = getMousePointOnPlane(lander.getPosition(), cam.getZAxis());
			mouseLastPos = mouseDownPos;
			bInDrag = true;
		}
		else {
			bLanderSelected = false;
		}
	}
	else {
		ofVec3f p;
		raySelectWithOctree(p);
	}
}

bool ofApp::raySelectWithOctree(ofVec3f &pointRet) {
	ofVec3f mouse(mouseX, mouseY);
	ofVec3f rayPoint = cam.screenToWorld(mouse);
	ofVec3f rayDir = rayPoint - cam.getPosition();
	rayDir.normalize();
	Ray ray = Ray(Vector3(rayPoint.x, rayPoint.y, rayPoint.z),
		Vector3(rayDir.x, rayDir.y, rayDir.z));

	
	float before = ofGetElapsedTimeMillis();
	pointSelected = octree.intersect(ray, octree.root, selectedNode);
	float after = ofGetElapsedTimeMillis();
	//cout << "raySelection time: " << after - before << "ms" << endl;

	if (pointSelected) {
		pointRet = octree.mesh.getVertex(selectedNode.points[0]);
	}
	return pointSelected;
}




//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

	// if moving camera, don't allow mouse interaction
	//
	if (cam.getMouseInputEnabled()) return;

	if (bInDrag) {

		glm::vec3 landerPos = lander.getPosition();

		glm::vec3 mousePos = getMousePointOnPlane(landerPos, cam.getZAxis());
		glm::vec3 delta = mousePos - mouseLastPos;
	
		landerPos += delta;
		lander.setPosition(landerPos.x, landerPos.y, landerPos.z);
		mouseLastPos = mousePos;

		ofVec3f min = lander.getSceneMin() + lander.getPosition();
		ofVec3f max = lander.getSceneMax() + lander.getPosition();

		Box bounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));

		colBoxList.clear();

		octree.intersect(bounds, octree.root, colBoxList);
	
		/*
		if (bounds.overlap(testBox)) {
			cout << "overlap" << endl;
		}
		else {
			cout << "no OK" << endl;
		}
		*/

	}
	else {
		ofVec3f p;
		raySelectWithOctree(p);
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	bInDrag = false;
}



// Set the camera to use the selected point as it's new target
//  
void ofApp::setCameraTarget() {

}


//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}



//--------------------------------------------------------------
// setup basic ambient lighting in GL  (for now, enable just 1 light)
//
void ofApp::initLightingAndMaterials() {

	static float ambient[] =
	{ .5f, .5f, .5, 1.0f };
	static float diffuse[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float position[] =
	{5.0, 5.0, 5.0, 0.0 };

	static float lmodel_ambient[] =
	{ 1.0f, 1.0f, 1.0f, 1.0f };

	static float lmodel_twoside[] =
	{ GL_TRUE };


	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, position);


	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
//	glEnable(GL_LIGHT1);
	glShadeModel(GL_SMOOTH);
} 

void ofApp::savePicture() {
	ofImage picture;
	picture.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
	picture.save("screenshot.png");
	cout << "picture saved" << endl;
}

//--------------------------------------------------------------
//
// support drag-and-drop of model (.obj) file loading.  when
// model is dropped in viewport, place origin under cursor
//
void ofApp::dragEvent2(ofDragInfo dragInfo) {

	ofVec3f point;
	mouseIntersectPlane(ofVec3f(0, 0, 0), cam.getZAxis(), point);
	if (lander.loadModel(dragInfo.files[0])) {
		lander.setScaleNormalization(false);
//		lander.setScale(.1, .1, .1);
	//	lander.setPosition(point.x, point.y, point.z);
		lander.setPosition(1, 1, 0);

		bLanderLoaded = true;
		for (int i = 0; i < lander.getMeshCount(); i++) {
			bboxList.push_back(Octree::meshBounds(lander.getMesh(i)));
		}

		cout << "Mesh Count: " << lander.getMeshCount() << endl;
	}
	else cout << "Error: Can't load model" << dragInfo.files[0] << endl;
}

bool ofApp::mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f &point) {
	ofVec2f mouse(mouseX, mouseY);
	ofVec3f rayPoint = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
	ofVec3f rayDir = rayPoint - cam.getPosition();
	rayDir.normalize();
	return (rayIntersectPlane(rayPoint, rayDir, planePoint, planeNorm, point));
}

//--------------------------------------------------------------
//
// support drag-and-drop of model (.obj) file loading.  when
// model is dropped in viewport, place origin under cursor
//
void ofApp::dragEvent(ofDragInfo dragInfo) {
	if (lander.loadModel(dragInfo.files[0])) {
		bLanderLoaded = true;
		lander.setScaleNormalization(false);
		lander.setPosition(0, 0, 0);
		cout << "number of meshes: " << lander.getNumMeshes() << endl;
		bboxList.clear();
		for (int i = 0; i < lander.getMeshCount(); i++) {
			bboxList.push_back(Octree::meshBounds(lander.getMesh(i)));
		}

		//		lander.setRotation(1, 180, 1, 0, 0);

				// We want to drag and drop a 3D object in space so that the model appears 
				// under the mouse pointer where you drop it !
				//
				// Our strategy: intersect a plane parallel to the camera plane where the mouse drops the model
				// once we find the point of intersection, we can position the lander/lander
				// at that location.
				//

				// Setup our rays
				//
		glm::vec3 origin = cam.getPosition();
		glm::vec3 camAxis = cam.getZAxis();
		glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
		glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);
		float distance;

		bool hit = glm::intersectRayPlane(origin, mouseDir, glm::vec3(0, 0, 0), camAxis, distance);
		if (hit) {
			// find the point of intersection on the plane using the distance 
			// We use the parameteric line or vector representation of a line to compute
			//
			// p' = p + s * dir;
			//
			glm::vec3 intersectPoint = origin + distance * mouseDir;

			// Now position the lander's origin at that intersection point
			//
			glm::vec3 min = lander.getSceneMin();
			glm::vec3 max = lander.getSceneMax();
			float offset = (max.y - min.y) / 2.0;
			lander.setPosition(intersectPoint.x, intersectPoint.y - offset, intersectPoint.z);

			// set up bounding box for lander while we are at it
			//
			landerBounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
		}
	}


}

void ofApp::loadLander()
{
	if (lander.loadModel("geo/lander.obj"))
	{
		bLanderLoaded = true;
		lander.setScaleNormalization(false);
		lander.setPosition(30, 40, 15);
		cout << "number of meshes: " << lander.getNumMeshes() << endl;
		bboxList.clear();
		for (int i = 0; i < lander.getMeshCount(); i++) {
			bboxList.push_back(Octree::meshBounds(lander.getMesh(i)));
		}

		//		lander.setRotation(1, 180, 1, 0, 0);

				// We want to drag and drop a 3D object in space so that the model appears 
				// under the mouse pointer where you drop it !
				//
				// Our strategy: intersect a plane parallel to the camera plane where the mouse drops the model
				// once we find the point of intersection, we can position the lander/lander
				// at that location.
				//

				// Setup our rays
				//
		glm::vec3 origin = cam.getPosition();
		glm::vec3 camAxis = cam.getZAxis();
		glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
		glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);
		float distance;

		bool hit = glm::intersectRayPlane(origin, mouseDir, glm::vec3(0, 0, 0), camAxis, distance);
		if (hit) {
			// find the point of intersection on the plane using the distance 
			// We use the parameteric line or vector representation of a line to compute
			//
			// p' = p + s * dir;
			//
			glm::vec3 intersectPoint = origin + distance * mouseDir;

			// Now position the lander's origin at that intersection point
			//
			glm::vec3 min = lander.getSceneMin();
			glm::vec3 max = lander.getSceneMax();
			float offset = (max.y - min.y) / 2.0;
			lander.setPosition(intersectPoint.x, intersectPoint.y - offset, intersectPoint.z);

			// set up bounding box for lander while we are at it
			//
			landerBounds = Box(Vector3(min.x, min.y, min.z), Vector3(max.x, max.y, max.z));
		}
	}

}

//  intersect the mouse ray with the plane normal to the camera 
//  return intersection point.   (package code above into function)
//
glm::vec3 ofApp::getMousePointOnPlane(glm::vec3 planePt, glm::vec3 planeNorm) {
	// Setup our rays
	//
	glm::vec3 origin = cam.getPosition();
	glm::vec3 camAxis = cam.getZAxis();
	glm::vec3 mouseWorld = cam.screenToWorld(glm::vec3(mouseX, mouseY, 0));
	glm::vec3 mouseDir = glm::normalize(mouseWorld - origin);
	float distance;

	bool hit = glm::intersectRayPlane(origin, mouseDir, planePt, planeNorm, distance);

	if (hit) {
		// find the point of intersection on the plane using the distance 
		// We use the parameteric line or vector representation of a line to compute
		//
		// p' = p + s * dir;
		//
		glm::vec3 intersectPoint = origin + distance * mouseDir;

		return intersectPoint;
	}
	else return glm::vec3(0, 0, 0);
}

void ofApp::updateAltitude()
{

}

void ofApp::raySelectWithGround(ofVec3f &pointRet) {
	
	ofVec3f rayPoint = lander.getPosition();
	ofVec3f rayDir = ofVec3f(0, 1, 0);
	rayDir.normalize();
	Ray ray = Ray(Vector3(rayPoint.x, rayPoint.y, rayPoint.z),
		Vector3(rayDir.x, rayDir.y, rayDir.z));

	pointSelected = octree.intersect(ray, octree.root, selectedNode);

	pointRet = octree.mesh.getVertex(selectedNode.points[0]);
	altitude = lander.getPosition().y - pointRet.y;
	
}

void ofApp::checkCollisions() {

	// for each particle, determine if we hit the groud plane.
	//
	for (int i = 0; i < emitter.sys->particles.size(); i++) {

		// only bother to check for descending particles.
		//
		ofVec3f vel = emitter.sys->particles[i].velocity; // velocity of particle
		if (vel.y >= 0) break;                             // ascending;

		ofVec3f pos = emitter.sys->particles[i].position;

		if (pos.y < emitter.sys->particles[i].radius) {

			// apply impulse function
			//
			ofVec3f norm = ofVec3f(0, 1, 0);  // just use vertical for normal for now
			ofVec3f f = (0.5 + 1.0)*((-vel.dot(norm))*norm);
			emitter.sys->particles[i].forces += ofGetFrameRate() * f;
		}
	}


	float height = altitude;

	if (height < 0 && yVelocity  < 0) {

		float f = (0.5 + 1.0)*(-(yVelocity));
		yVelocity += f;
	}


}

void ofApp::checkCrashed()
{
	if (yVelocity < -0.04 && altitude <= 1)
	{
		isCrashed = true;
	}
}

void ofApp::checkWin()
{
	if (altitude <= 0 && yVelocity > -0.04 && !lost &&
		lander.getPosition().x < 10 && lander.getPosition().x > -10 &&
		lander.getPosition().z < 10 && lander.getPosition().z > -10)
	{
		isWin = true;
	}
}



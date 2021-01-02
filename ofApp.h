#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include  "ofxAssimpModelLoader.h"
#include "Octree.h"
#include "Sprite.h"
#include "Particle.h"
#include "ParticleEmitter.h"


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent2(ofDragInfo dragInfo);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void drawAxis(ofVec3f);
		void initLightingAndMaterials();
		void savePicture();
		void toggleWireframeMode();
		void togglePointsDisplay();
		void toggleSelectTerrain();
		void setCameraTarget();
		bool mouseIntersectPlane(ofVec3f planePoint, ofVec3f planeNorm, ofVec3f &point);
		bool raySelectWithOctree(ofVec3f &pointRet);
		glm::vec3 ofApp::getMousePointOnPlane(glm::vec3 p , glm::vec3 n);

		//project 3
		//
		//load lander
		void loadLander();
		void updateAltitude();
		void raySelectWithGround(ofVec3f &pointRet);
		void checkCollisions();
		void checkCrashed();
		void checkWin();

		//camera setup
		ofCamera *mainCam;
		ofEasyCam cam;
		ofCamera forwardCam;
		ofCamera trackingCam;
		
		
		ofxAssimpModelLoader mars, lander;
		ofLight light;
		Box boundingBox, landerBounds;
		Box testBox;
		vector<Box> colBoxList;
		bool bLanderSelected = false;
		Octree octree;
		TreeNode selectedNode;
		glm::vec3 mouseDownPos, mouseLastPos;
		bool bInDrag = false;


		ofxIntSlider numLevels;
		ofxPanel gui;

		bool bAltKeyDown;
		bool bCtrlKeyDown;
		bool bWireframe;
		bool bDisplayPoints;
		bool bPointSelected;
		bool bHide;
		bool pointSelected = false;
		bool bDisplayLeafNodes = false;
		bool bDisplayOctree = false;
		bool bDisplayBBoxes = false;
		
		bool bLanderLoaded;
		bool bTerrainSelected;
	
		ofVec3f selectedPoint;
		ofVec3f intersectPoint;

		vector<Box> bboxList;

		const float selectionRange = 4.0;


		//project 3
		bool spacePressed = false;
		bool rightKeyPressed = false;
		bool leftKeyPressed = false;
		bool upKeyPressed = false;
		bool downKeyPressed = false;
		bool leftRotationPressed = false;
		bool rightRotationPressed = false;
		bool zPressed = false;

		//rotate
		float rotationDeg = 0;
		float velocity = 0;
		float sideVelocity = 0;
		float upThrust = 0;
		float yVelocity = 0;
		float accel = 0.04;
		float angThrust = 0.0;
		float angAccel = 0.3;
		float gravity = 0.0003;
		float fr = ofGetFrameRate();
		float dt = 1.0 / fr;

		float moveX = 0;
		float moveZ = 0;
		
		float fuel = 10000;
		//altitude
		ofVec3f altitudeVec;
		float altitude;
		bool altitudeOn = false;

		//Emitter
		//
		ParticleEmitter emitter;

		TurbulenceForce *turbForce = NULL;
		GravityForce *gravityForce = NULL;
		ImpulseRadialForce *radialForce = NULL;

		ParticleEmitter explosionEmitter;
		TurbulenceForce *turbForce2 = NULL;
		ImpulseRadialForce *radialForce2 = NULL;

		//images
		ofImage backgroundImage;

		//sound
		ofSoundPlayer moveSound;
		ofSoundPlayer backgroundMusic;

		//light
		ofLight light1;
		ofLight light2;

		bool isCrashed = false;
		bool gameStarted = false;
		bool lost = false;
		bool isWin = false;
};

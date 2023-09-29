//
//  Hand.h
//  HandRig
//
//  Created by Nick Hardeman on 8/31/23.
//

#pragma once
#include "ofxFBX.h"
#include <map>

class Finger {
public:
	typedef enum {
		POINTER=0,
		MIDDLE,
		RING,
		PINKY,
		THUMB
	} Index;
	
	class Joint {
	public:
		
		Joint() {}
		Joint( shared_ptr<ofxFBXBone> abone ) {set(abone);}
		~Joint() {}
		
		void set( shared_ptr<ofxFBXBone> abone );
		void setRotationDeg( float aXDeg, float aYDeg, float aZDeg );
		void lerpRotationDeg( float aXDeg, float aYDeg, float aZDeg, float aLerpAmnt );
		
		void applyAngles(glm::vec3 aAngles);
		
		glm::vec3 getLimitedAnglesInRad( float aXDeg, float aYDeg, float aZDeg );
		
		glm::quat oquatLocal = {};
		glm::vec2 oEulerLocal = {}; // radians
		shared_ptr<ofxFBXBone> bone;
		glm::vec3 limitDegMin = {-30, -30, -30};
		glm::vec3 limitDegMax = {90, 90, 90};
		glm::vec3 currentAngles = {0.f,0.f,0.f};
	};
	
	void setup( shared_ptr<ofxFBXBone> ab1, shared_ptr<ofxFBXBone> ab2, shared_ptr<ofxFBXBone> ab3 );
	// lerp all of the joints in the finger to these angles
	void setJointsRotationDeg( float aXDeg, float aYDeg, float aZDeg );
	void lerpJointsRotationDeg( float aXDeg, float aYDeg, float aZDeg, float aLerpAmnt );
	
	vector< shared_ptr<Joint> > joints;
};

class Hand {
public:
	
	bool load( string aFilePath );
	void setup(string aHandBoneName, string aFingerPrefix);
	void setRotationDeg( float aXDeg, float aYDeg, float aZDeg );
	void setRotationDegGlobal( float aXDeg, float aYDeg, float aZDeg );
	
	shared_ptr<ofxFBX> fbx;
	ofMaterial material;
	map<Finger::Index, shared_ptr<Finger> > fingers;
	shared_ptr<ofxFBXBone> bone;
	glm::quat oquatLocal = {};
};

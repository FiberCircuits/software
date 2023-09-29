#pragma once
#include "ofMain.h"
#include "ofxFBX.h"
#include "Hand.h"
#include "ofxSimpleSerial.h"
#include <vector>
#include "ofUtils.h"

class ofApp : public ofBaseApp{
public:
	void setup() override;
	void update() override;
	void draw() override;

	void keyPressed(int key) override;
	void keyReleased(int key) override;

    // Serial stuff
    void		    onNewMessage(string & message);
    ofxSimpleSerial	serial;
    string		    message;
    vector<string>  sensors_data;

	ofEasyCam cam;

	bool touch;
    float fingerBending;
    float handHeading;

	shared_ptr<Hand> mHand;
	// just having a cubeMap here adds some lighting
	ofCubeMap cubeMap;

};

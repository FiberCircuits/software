#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

    ofSetFrameRate(25); // TODO?
    //ofSetFrameRate(60);

    // convert loaded texture coords from 0->1
    ofDisableArbTex();

    // models downloaded from
    // https://github.com/davidezhang/RoboGlove_Sensing/tree/main/Assets/Import
    mHand = make_shared<Hand>();
    mHand->load("RightHand.fbx");
    mHand->setup("hands:b_r_hand","hands:b_r");

    // load the left hand //
    //	mHand->load("LeftHand.fbx");
    //	mHand->setup("hands:b_l_hand","hands:b_l");

    mHand->fbx->setPosition( glm::vec3(0,0,-10));

    cam.lookAt( ofVec3f(0,0,0) );
    cam.setDistance( 20 );
    cam.setFarClip(100);
    cam.setNearClip( .5f );

    touch = false;
    fingerBending = 0;
    handHeading = 0;

    // Serial stuff
	//ofSetVerticalSync(true); // TODO ?
	serial.setup("/dev/ttyACM0", 115200);
	serial.startContinuousRead();
	ofAddListener(serial.NEW_MESSAGE,this,&ofApp::onNewMessage);
	message = "";
}

//--------------------------------------------------------------
void ofApp::update() {

    // moves the bones into place based on the animation
    mHand->fbx->earlyUpdate();
    float etimef = ofGetElapsedTimef();
    if( mHand->fingers.size() > 0 ) {

        // float fingerBending = (float)ofGetMouseY() / (float)ofGetHeight();
        // fingerBending = (fingerBending * 2.0f - 1.0f) * 120.0f;

        // grab the pointer finger
        auto& pointer = mHand->fingers[Finger::POINTER];
        // lerp all three joints to the same target degree
        pointer->lerpJointsRotationDeg( 0.f, 0.f, fingerBending, 0.1f );

        // grab the tumb
        auto& thumb = mHand->fingers[Finger::THUMB];
        // lerp all three joints to the same target degree
        thumb->lerpJointsRotationDeg( 0.f, 0.f, 10.f, 0.1f );

        // loop through the fingers and close them if they aren't the pointer or thumb
        for( auto& finger : mHand->fingers ) {
            if( finger.first != Finger::POINTER && finger.first != Finger::THUMB ) {
                finger.second->lerpJointsRotationDeg( 0.f, 0.f, 80.f, 0.1f );
            }
        }

    }

    // handHeading = (1.0f -(float)ofGetMouseX() / (float)ofGetWidth());
    // handHeading *= -180.f;

    // set the rotation of the hand //
    mHand->setRotationDegGlobal( -90, handHeading, 0.0 );

    // manipulates the mesh around the positioned bones //
    mHand->fbx->lateUpdate();
}

//--------------------------------------------------------------
void ofApp::draw() {

    ofSetColor(255, 255, 255);
    ofEnableDepthTest();

    cam.begin(); {

        ofSetColor( 255, 255, 255 );
        mHand->material.begin();
        mHand->fbx->draw();
        mHand->material.end();

        if (touch)
            mHand->material.setDiffuseColor( ofFloatColor(0.85, 0.0, 0.0, 1.0));
        else
            mHand->material.setDiffuseColor( ofFloatColor(0.85, 0.85, 0.85, 1.0));

        ofSetColor(90);
        ofPushMatrix(); {
            ofTranslate( 0, -5, 0);
            ofRotateYDeg(90);
            ofRotateZDeg(90);
            ofDrawGridPlane(1, 20);
        } ofPopMatrix();

    } cam.end();

    ofDisableDepthTest();

//  ofSetColor( 160 );
//  stringstream ds;
//  ds << "touch (t): " << touch << endl;
//  ofDrawBitmapString( ds.str(), 50, 30 );

}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    if(key == 't') {
        touch = 1;
    }
}


//--------------------------------------------------------------
void ofApp::keyReleased(int key){

    if(key == 't') {
        touch = 0;
    }
}


//--------------------------------------------------------------
void ofApp::onNewMessage(string & message)
{
    // copy the message in separated fields:
    sensors_data = ofSplitString(message, " ");

    const int message_size = 7;
    if (sensors_data.size() >= message_size) {

        // capacitive touch data:
        float touch_val = ofToFloat(sensors_data.at(0));
        cout << "t: " << touch_val << endl;
        touch = (touch_val > 5);                         // TODO: fix threshold

        // Rotate hand (heading) using the magnetometer with atan2(y,x):
        handHeading = std::atan2(ofToFloat(sensors_data.at(2)), ofToFloat(sensors_data.at(1)));
        handHeading = (handHeading * 180.0f) / M_PI; // => [-180 ; +180]
        handHeading = ofMap(handHeading, 0,50, 1,0);
        handHeading *= -180.f;
        //cout << "h: " << handHeading << endl;

        // bend the finger with the accel:
        // TODO: smoothen it!
        fingerBending = ofToFloat(sensors_data.at(5)); // temporary test: accelerometer y
        fingerBending = ofMap(fingerBending, -20,-70, 0,1); // temporary normalize
        //cout << "b: " << fingerBending << endl;

        //cout << endl;
    }
}

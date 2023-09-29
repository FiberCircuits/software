//
//  Hand.cpp
//  HandRig
//
//  Created by Nick Hardeman on 8/31/23.
//

#include "Hand.h"

//--------------------------------------------------------------
void Finger::Joint::set( shared_ptr<ofxFBXBone> abone ) {
    bone = abone;
    oquatLocal = bone->getOrientationQuat();
    oEulerLocal = bone->getOrientationEulerRad();
}

//--------------------------------------------------------------
void Finger::Joint::setRotationDeg( float aXDeg, float aYDeg, float aZDeg ) {
    if(!bone) return;
    currentAngles = getLimitedAnglesInRad(aXDeg, aYDeg, aZDeg );
    applyAngles(currentAngles);
}

//--------------------------------------------------------------
void Finger::Joint::lerpRotationDeg( float aXDeg, float aYDeg, float aZDeg, float aLerpAmnt ) {
    if(!bone) return;
    auto langles = getLimitedAnglesInRad(aXDeg, aYDeg, aZDeg );
    //            currentAngles = langles;
    currentAngles = glm::mix( currentAngles, langles, aLerpAmnt );
    applyAngles(currentAngles);
}

//--------------------------------------------------------------
void Finger::Joint::applyAngles(glm::vec3 aAngles) {
    if(!bone) return;
    glm::quat nq = glm::angleAxis(aAngles.z, glm::vec3(0.f,0.f,1.f)) *
        glm::angleAxis(aAngles.y, glm::vec3(0.f,1.f,0.f)) *
        glm::angleAxis(aAngles.x, glm::vec3(1.f,0.f,0.f));
    bone->setOrientation(oquatLocal * nq);
}

//--------------------------------------------------------------
glm::vec3 Finger::Joint::getLimitedAnglesInRad( float aXDeg, float aYDeg, float aZDeg ) {
    return glm::vec3(ofDegToRad(ofClamp(aXDeg, limitDegMin.x, limitDegMax.x)),
            ofDegToRad(ofClamp(aYDeg, limitDegMin.y, limitDegMax.y)),
            ofDegToRad(ofClamp(aZDeg, limitDegMin.z, limitDegMax.z))
            );
}

//--------------------------------------------------------------
void Finger::setup( shared_ptr<ofxFBXBone> ab1, shared_ptr<ofxFBXBone> ab2, shared_ptr<ofxFBXBone> ab3 ) {
    if( !ab1 || !ab2 || !ab3 ) {
        ofLogError("Finger :: ERROR WITH BONES! ");
        return;
    }
    joints.push_back( make_shared<Joint>(ab1) );
    joints.push_back( make_shared<Joint>(ab2) );
    joints.push_back( make_shared<Joint>(ab3) );
}

// lerp all of the joints in the finger to these angles
//--------------------------------------------------------------
void Finger::setJointsRotationDeg( float aXDeg, float aYDeg, float aZDeg ) {
    for( auto& joint : joints ) {
        joint->setRotationDeg( aXDeg, aYDeg, aZDeg);
    }
}

//--------------------------------------------------------------
void Finger::lerpJointsRotationDeg( float aXDeg, float aYDeg, float aZDeg, float aLerpAmnt ) {
    for( auto& joint : joints ) {
        joint->lerpRotationDeg( aXDeg, aYDeg, aZDeg, aLerpAmnt);
    }
}

//--------------------------------------------------------------
bool Hand::load( string aFilePath ) {
    fbx = make_shared<ofxFBX>();
    ofxFBXSource::Scene::Settings settings;
    settings.filePath = aFilePath;
    settings.importAnimations = false;
    settings.cacheMeshKeyframes = false;
    settings.importMaterials = false;
    // settings.printInfo = true;
    if( fbx->load(settings) ) {
        ofLogNotice() << "Hand :: load : loaded the scene OK" << endl;
        return true;
    } else {
        ofLogNotice() << "Hand :: load : Error loading the scene" << endl;
    }
    return false;
}

//--------------------------------------------------------------
void Hand::setup(string aHandBoneName, string aFingerPrefix) {
    if(!fbx) {
        ofLogError("Hand :: setup : ERROR Fbx not loaded");
        return;
    }
    fbx->setNodesNeedUpdate(true);
    fbx->update(); // just in case

    bone = fbx->getBone(aHandBoneName);
    if(bone) {
        oquatLocal = bone->getOrientationQuat();
        bone->setGlobalPosition(fbx->getPosition());
    }
    // "hands:b_r_index1"
    // setup the fingers //
    auto pointer = make_shared<Finger>();
    pointer->setup(fbx->getBone(aFingerPrefix+"_index1"),
            fbx->getBone(aFingerPrefix+"_index2"),
            fbx->getBone(aFingerPrefix+"_index3") );
    fingers[Finger::POINTER] = pointer;

    auto middle = make_shared<Finger>();
    middle->setup(fbx->getBone(aFingerPrefix+"_middle1"),
            fbx->getBone(aFingerPrefix+"_middle2"),
            fbx->getBone(aFingerPrefix+"_middle3") );
    fingers[Finger::MIDDLE] = middle;

    auto ring = make_shared<Finger>();
    ring->setup(fbx->getBone(aFingerPrefix+"_ring1"),
            fbx->getBone(aFingerPrefix+"_ring2"),
            fbx->getBone(aFingerPrefix+"_ring3") );
    fingers[Finger::RING] = ring;

    auto pinky = make_shared<Finger>();
    pinky->setup(fbx->getBone(aFingerPrefix+"_pinky1"),
            fbx->getBone(aFingerPrefix+"_pinky2"),
            fbx->getBone(aFingerPrefix+"_pinky3") );
    fingers[Finger::PINKY] = pinky;

    auto thumb = make_shared<Finger>();
    thumb->setup(fbx->getBone(aFingerPrefix+"_thumb1"),
            fbx->getBone(aFingerPrefix+"_thumb2"),
            fbx->getBone(aFingerPrefix+"_thumb3") );
    fingers[Finger::THUMB] = thumb;
    if(thumb->joints.size() > 0 && thumb->joints[0] ) {
        thumb->joints[0]->limitDegMax.z = 30.f;
        thumb->joints[1]->limitDegMax.z = 30.f;
    }

    material.setPBR(true);
    material.setDiffuseColor( ofFloatColor(0.85, 0.85, 0.85, 1.0));
    material.setMetallic(0.0);
    material.setRoughness(0.2);
}

//--------------------------------------------------------------
void Hand::setRotationDeg( float aXDeg, float aYDeg, float aZDeg ) {
    if(!bone) return;
    auto langles = glm::vec3( ofDegToRad(aXDeg), ofDegToRad(aYDeg), ofDegToRad(aZDeg) );
    glm::quat nq = glm::angleAxis(langles.z, glm::vec3(0.f,0.f,1.f)) *
        glm::angleAxis(langles.y, glm::vec3(0.f,1.f,0.f)) *
        glm::angleAxis(langles.x, glm::vec3(1.f,0.f,0.f));
    bone->setOrientation(oquatLocal * nq);
}

//--------------------------------------------------------------
void Hand::setRotationDegGlobal( float aXDeg, float aYDeg, float aZDeg ) {
    if(!bone) return;
    auto langles = glm::vec3( ofDegToRad(aXDeg), ofDegToRad(aYDeg), ofDegToRad(aZDeg) );
    glm::quat nq = glm::angleAxis(langles.z, glm::vec3(0.f,0.f,1.f)) *
        glm::angleAxis(langles.y, glm::vec3(0.f,1.f,0.f)) *
        glm::angleAxis(langles.x, glm::vec3(1.f,0.f,0.f));
    bone->setGlobalOrientation(nq);
}




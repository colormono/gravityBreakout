//
//  Player.h
//  emptyExample
//
//  Created by colormono on 3/24/15.
//
//
#pragma once
#include "ofMain.h"

class Player{
public:
    
    ofPoint location;
    int vidas;
    int w;
    int c;
    
    void setup(){
        location.x = ofGetWidth()/2;
        location.y = ofGetHeight()-30;
        w = 100;
        vidas = 3;
    }
    
    void update(){
        location.x = ofGetMouseX() - w/2;
    }
    
    void draw(){
        ofSetHexColor(0x00CCFF);
        ofFill();
        ofRect( location.x, location.y, w, 10 );
    }
    
};

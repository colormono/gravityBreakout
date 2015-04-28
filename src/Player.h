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
    int lives;
    int w;
    ofColor color;
    
    void setup(){
        location.x = ofGetWidth()/2;
        location.y = ofGetHeight()-30;
        w = 100;
        color = 255;
        lives = 3;
    }
    
    void update( float x ){
        location.x = x - w/2;
    }
    
    void draw(){
        ofSetColor(color);
        ofFill();
        ofRect( location.x, location.y, w, 10 );
    }
    
};

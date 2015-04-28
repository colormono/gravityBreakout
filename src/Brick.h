//
//  Brick.h
//  emptyExample
//
//  Created by colormono on 3/24/15.
//
//
#pragma once
#include "ofMain.h"
#include "ofxBox2d.h"

// A simple little Data class. This is were you can store anything you want.
class Data {
public:
    ofColor color;
    string  name;
    int		id;
};


class Brick : public ofxBox2dCircle {
public:
    
    bool bRemove;
    string bName;
    ofImage logo;
    
    void setupTheCustomData( bool _bRemove ) {
        
        bRemove = _bRemove;        
        static int colors[] = {0xcae72b, 0xe63b8f, 0x2bb0e7};
        static string abc   = "abcd";

        // Load Images
        string imageFolder = "textures/";
        logo.loadImage( imageFolder+"logo.png" );

        // we are using a Data pointer because
        // box2d needs to have a pointer not
        // a referance
        setData(new Data());
        Data * theData = (Data*)getData();
        
        theData->id = ofRandom(0, 100);
        theData->name = abc[(int)ofRandom(0, abc.size())];
        if( theData->name == "a" ) abc = "bcd";
        theData->color.setHex(colors[(int)ofRandom(0, 3)]);
        
        bName = theData->name;
        
        //cout << "nuevo globo, llamado:"+ bName << endl;
        
    }
    
    void draw() {
        Data* theData = (Data*)getData();
        if(theData) {
            
            // Evan though we know the data object lets just
            // see how we can get the data out from box2d
            // you would use this when using a contact listener
            // or tapping into box2d's solver.
            
            float radius = getRadius();
            ofPushMatrix();
            ofTranslate( getPosition() );
            if( theData->name == "a" ){
                ofRotateZ( getRotation() );
                ofSetColor( 255,255,255 );
                logo.draw( -radius, -radius, radius*2, radius*2 );
            } else {
                ofSetColor( theData->color );
                ofFill();
                ofCircle( 0, 0, radius );
            }
            ofPopMatrix();
            
        }
    }
    
};
//
//  Ball.h
//  emptyExample
//
//  Created by colormono on 3/24/15.
//
//
#pragma once
#include "ofMain.h"

class Ball{
public:

    ofVec3f location;       // Posición
    ofVec3f direction;      // Dirección
    int diametro;           // Diametro
    ofColor color;          // Color
    
    void setup(){
        location = ofVec3f( ofGetWidth()/2, ofGetHeight()-30, 0 );
        direction = ofVec3f( 0, 0, 0 );
        diametro = 10;
        color = 0xffffff;
    }
    
    void update(){
        
        // Despegar
        if ( direction.y == 0 ) {
            direction.y = -10;
        }
        
        // Actualizar posición
        location.x += direction.x;
        location.y += direction.y;
        
        // Rebotar en los bordes
        if ( location.x <= 0 || location.x >= ofGetWidth() ){
            direction.x *= -1;
        }
        if ( location.y <= diametro/2 ){
            direction.y *= -1;
            location.y = diametro; // Transportar dentro de la pantalla
        }
        
    }
    
    void draw(){
        ofSetColor(color);
        ofFill();
        ofCircle( location, diametro );
    }
    
};
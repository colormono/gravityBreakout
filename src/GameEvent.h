//
//  GameEvent.h
//  emptyExample
//
//  Created by colormono on 3/24/15.
//
//
#pragma once
#include "ofMain.h"
#include "Player.h"
#include "Brick.h"
#include "Ball.h"


class GameEvent : public ofEventArgs {
    
public:
    
    Ball * ball;
    ofPtr<Brick> * brick;
    Player * player;
    string message;
    
    GameEvent() {
        ball = NULL;
        brick = NULL;
        player = NULL;
    }
    
    static ofEvent <GameEvent> events;
};


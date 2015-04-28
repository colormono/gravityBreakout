#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    
    // Load XML settings
    message = "loading gameSettings.xml";
    if( XML.loadFile("gameSettings.xml") ) message = "gameSettings.xml loaded!";
    else message = "unable to load gameSettings.xml check data/ folder";
    cout << "xml status: " << message << endl;

    // Setup game
    ofSetFrameRate(60);
    debug = XML.getValue("GAME:debug", 0); // If useOSC is false, use the mouse
    useOSC = XML.getValue("GAME:useOSC", 1); // If useOSC is false, use the mouse
    if( useOSC ) receiver.setup( XML.getValue("GAME:oscPort", 12345) ); // start to listen OSC messages on port 12345
    gameState = "loading";
    countdown = true;
    
    // Styles
    ofBackground( XML.getValue("GAME:background", 100) );
    
    // Initialize world
    box2d.init();
    box2d.enableEvents(); // <-- turn on the event listener
    box2d.setGravity(0, -5);
    box2d.createBounds();
    box2d.registerGrabbing();
    //box2d.setFPS(30.0);
    
    // Initialize participants
    player.setup();
    player.w = XML.getValue("PLAYER:width", 100);
    //player.color = XML.getValue("PLAYER:color", 255);
    //player.lives = XML.getValue("PLAYER:lives", 3);
    
    ball.setup();

    // Listen to any of the events for the game
    ofAddListener(GameEvent::events, this, &ofApp::gameEvent);

}

//--------------------------------------------------------------
void ofApp::gameEvent(GameEvent &e) {
    
    cout << "Game Event: "+e.message << endl;

    // Beat brick
    if ( e.message == "beat-brick-a" ) {
        score ++; // Increment score
    }
    else if ( e.message == "beat-brick-b" ){
        // Cambiar color de la pelota por x tiempo
        ball.color.setHex(0xff0000);
        score += 2;
    }
    else if ( e.message == "beat-brick-c" ) {
        score ++; // Increment score by 1
    }
    else if ( e.message == "beat-brick-d" ) {
        score ++; // Increment score by 1
    }
    
}

//--------------------------------------------------------------
bool ofApp::shouldRemoveBrick(ofPtr<Brick> &b) {
    return b.get()->bRemove;
}

//--------------------------------------------------------------
bool ofApp::playerCollision(Player &p) {
    //return p.bRemove;
}

//--------------------------------------------------------------
void ofApp::update(){
    
    // Player position
    if ( useOSC ) {
        while( receiver.hasWaitingMessages() ){
            
            // Get the next message
            ofxOscMessage m;
            receiver.getNextMessage( &m );
            
            // Parse message, for example:
            if( m.getAddress() == "/biggestBlobXPos" ){
                // Get first argument, biggest blog position normalized [0,1]
                float biggestBlobXPos = m.getArgAsFloat(0);
                // Use blob position for our player
                player.update( ofMap( biggestBlobXPos, 0,1, 0,ofGetWidth() ) );
            }
            
            // Get the next message
            ofxOscMessage b;
            receiver.getNextMessage( &b );
            if( b.getAddress() == "/userAvailable" ){
                userAvailable = b.getArgAsInt32(0);
            }
        }
    } else {
        userAvailable = 1;
        player.update( ofGetMouseX() ); // use mouseX position
    }
    
    // Avanzar mundo
    box2d.update();
    
    // Update gameState
    if ( gameState=="loading" ) {
        if( countdown ){
            timer = ofGetElapsedTimef(); // Update clock
            countdown = false;
        } else {
            if( ofGetElapsedTimef() - timer > 3 ){
                gameState = "startGame"; // Wait for a user
            }
        }
    }
    
    else if ( gameState=="startGame" ) {
        timer = ofGetElapsedTimef(); // Update clock

        score = 0; // Initial score
        player.lives = XML.getValue("STYLE:playerLives", 3); // Player lives
        
        gameState = "welcomeVideo"; // Play welcome video
    }

    else if ( gameState=="welcomeVideo" ) {
        timer = ofGetElapsedTimef(); // Update clock
        gameState = "createLevel"; // Create level
    }
    
    else if ( gameState=="createLevel" ) {
        // level.create(); sounds much better than this...

        // Place obstacles
        if( ventanas.size() < 3 ){
            ofPtr <ofxBox2dRect> ventana = ofPtr <ofxBox2dRect>(new ofxBox2dRect); // crear objeto
            ventana.get()->setPhysics(0, 0.5, 0.9); // density, bounce, friction
            ventana.get()->setup( box2d.getWorld(), ofRandom(0, ofGetWidth()), ofGetHeight()/2+ofRandom(-100,100), 40, 40 ); // world, x, y, w, h
            ventanas.push_back(ventana); // sumar al final del arreglo
        }
        
        // Create bricks
        if( bricks.size() < 20 ){
            
            if( (int)ofRandom(0, 50) == 0 ) {
                ofPtr<Brick> b = ofPtr<Brick>(new Brick); // crear objeto
                b.get()->setPhysics(0.9, 0.5, 0.1); // density, bounce, friction
                b.get()->setup(box2d.getWorld(), ofRandom(0, ofGetWidth()), ofGetHeight()-20, ofRandom(20, 60)); // world, x, y, radio
                //b.get()->setVelocity(ofRandom(-30, 30), ofRandom(-30, 30));
                b.get()->setupTheCustomData(false);
                bricks.push_back(b);
            }
            
            timer = ofGetElapsedTimef(); // Update clock
            
        } else {
            gameState = "playLevel"; // Start to play
        }
        
    }
    
    else if ( gameState=="playLevel" ) {
        
        if( userAvailable ){
            // Ball
            ball.update();
        }
        
        // Detect Player Collision
        //ball.collisionPlayer(player);
        if(
           ball.location.x > player.location.x && ball.location.x < player.location.x + player.w &&
           ball.location.y >= player.location.y - ball.diametro/2
           ){
            // Cambiar sentido de la direcci—n
            ball.direction.y *= -1;
            
            // Desviar la pelota
            float d = ofDist( ball.location.x, ball.location.y, player.location.x, player.location.y );
            float error = ofRandom(-0.3,0.3);
            if ( d<player.w*0.25 ) ball.direction.x = -2 + error;
            if ( d>=player.w*0.25 && d<=player.w/2-5 ) ball.direction.x = -1 + error;
            if ( d>player.w*0.5-5 && d<player.w*0.5+5 ) ball.direction.x = 0 + error;
            if ( d>=player.w*0.5+5 && d<player.w*0.75 ) ball.direction.x = 1 + error;
            if ( d>=player.w*0.75 ) ball.direction.x = 2 + error;
        }
        
        // Detectar colisi—n con ventanas
        for( int i=0; i<ventanas.size(); i++){
            float d = ofDist( ball.location.x, ball.location.y, ventanas[i].get()->getPosition().x, ventanas[i].get()->getPosition().y );
            
            if( d < ventanas[i].get()->getWidth() ){
                // Cambiar sentido de la direcci—n
                ball.direction.y *= -1;
                printf("rebotar con ventana\n");
            }
        }
        
        // Detectar colisi—n con globos
        for( int i=0; i<bricks.size(); i++){
            float d = ofDist( ball.location.x, ball.location.y, bricks[i].get()->getPosition().x, bricks[i].get()->getPosition().y );
            
            if( d < bricks[i].get()->getRadius() ){
                
                // Cambiar sentido de la direcci—n
                ball.direction.y *= -1;
                
                // Incrementar velocidad
                if ( ball.direction.y <= 10 ){
                    ball.direction.y += 0.5;
                }
                
                // Enviar mensaje segœn tipo de globo
                static GameEvent newEvent;
                newEvent.message = "beat-brick-" + bricks[i].get()->bName;
                ofNotifyEvent(GameEvent::events, newEvent);
                
                // Eliminar ladrillo
                bricks[i].get()->bRemove = true;
            }
        }
        // Remove beaten bricks
        ofRemove(bricks, shouldRemoveBrick);
        
        // Life lost
        if ( ball.location.y > ofGetHeight() ){
            
            player.lives--; // die
            
            if ( player.lives <= 0 ){
                timer = ofGetElapsedTimef(); // Update clock
                gameState = "gameEnd";  // Game over
            } else {
                ball.location.x = ofGetWidth()/2; // center
                ball.location.y = ofGetHeight()-50; // bottom
                ball.direction.y = -10; // invert y velocity
                ball.direction.x = 0; // reset x velocity
            }

        }
        
    }
    
    else if ( gameState=="gameEnd" ) {
        
        // You Win
        if ( bricks.size() <= 0 ) {
        }
        
        // Game over
        if ( bricks.size() > 0 ) {
            
            // Remove unbeaten bricks
            for( int i=0; i<bricks.size(); i++){
                if( bricks[i].get()->bName == "a" ){
                    static GameEvent newEvent;
                    newEvent.message = "reventar-globo-a";
                    ofNotifyEvent(GameEvent::events, newEvent);
                }
                bricks[i].get()->bRemove = true;
            }
            ofRemove(bricks, shouldRemoveBrick);
        }

        timer = ofGetElapsedTimef(); // Update clock
        gameState = "goodbyeVideo"; // Play goodbye video
    }
    
    
    else if ( gameState=="goodbyeVideo" ) {

        timer = ofGetElapsedTimef(); // Update clock
        gameState = "startGame"; // Re-start game
    }
    
    else {
        // If something fails, re-start game
        timer = ofGetElapsedTimef(); // Update clock
        gameState = "startGame";
    }
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    // Siempre Ventanas
    for(int i=0; i<ventanas.size(); i++) {
        ofFill();
        ofSetHexColor(0xffffff);
        ventanas[i].get()->draw();
    }
    
    // Siempre Globos
    for( int i=0; i<bricks.size(); i++){
        bricks[i].get()->draw();
    }
    
    // Segœn gameState
    if ( gameState=="createLevel" ) {
    }
    else if ( gameState=="playLevel" ) {
        
        // Player
        player.draw();
        
        // Ball
        ball.draw();
    }
    else {
    }
    
    
    // Game info
    ofSetColor(225);
    string info = "";
    info += "VIDAS: "+ofToString(player.lives, 1)+"\n";
    info += "PUNTOS: "+ofToString(score, 1)+"\n";
    ofDrawBitmapString(info, 10, 20);
    
    
    // Show debug info
    if (debug) {
        string debugTxt = "";
        debugTxt += "FPS: "+ofToString(ofGetFrameRate(), 1)+"\n";
        debugTxt += "gameState: "+ofToString(gameState, 1)+"\n";
        debugTxt += "Location.x: "+ofToString(ball.location.x, 1)+"\n";
        ofDrawBitmapString(debugTxt, 30, ofGetHeight()-90);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    switch (key) {
        case ' ':
            break;
        case'd':
            debug = !debug;
            break;
        case 's':
            //update the setup to the XML
            XML.setValue("GAME:debug", debug);
            XML.saveFile("gameSettings.xml");
            message ="settings saved to xml!";
            cout << "xml status: " << message << endl;
            break;
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    box2d.createBounds(); // Update bounds
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
    
}

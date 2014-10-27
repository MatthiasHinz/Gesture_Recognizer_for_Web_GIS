#include "static_training.h"



int s_noOfHands;
int s_noOfTrackedHands;

//----------------------
///nite dependencies



Nite_HandTracker tracker;
#include "fullbodyTracker.h";
FullBodyTracker bodyTracker = FullBodyTracker();

//--------------------------------------------------------------
void static_training::setup(){
    //Set the application frame rate to 30 FPS
    ofSetFrameRate( 30 );

    //old OF default is 96 - but this results in fonts looking larger than in other programs. 
	ofTrueTypeFont::setGlobalDpi(72);
    
    //Load the font for the info messages
	
	font.loadFont("verdana.ttf", 18, true, true);
	font.setLineHeight(18.0f);
	font.setLetterSpacing(1.037);

    //Open the connection with Synapse
   /* synapseStreamer.openSynapseConnection();
    
    //Set which joints we want to track
    synapseStreamer.trackAllJoints(false);
    synapseStreamer.trackLeftHand(true);
    synapseStreamer.trackRightHand(true);
    synapseStreamer.computeHandDistFeature(true);
    
   
    */

	 //Setup the graphs for the input data
    setupGraphs();
    infoText = "";
    
    //Setup the training data
    trainingData.setNumDimensions( 6 );
    trainingModeActive = false;
    predictionModeActive = false;
    trainingClassLabel = 1;
    
    //Setup the classifier
    ANBC anbc;
    anbc.enableNullRejection(true);
    anbc.setNullRejectionCoeff(5);

    pipeline.setClassifier( anbc );

	openniProxie prox;
	prox.initOpenNi();
	tracker.initHandTracker();
	bodyTracker.initTracker();
    
}

//--------------------------------------------------------------
void static_training::update(){


	tracker.updateHandTracker();
	bodyTracker.update();

	/*if(tracker.isLeftHandTracked() && tracker.isRightHandTracked()){
		leftHand = vector<double> (3);
        //Get the left hand and right hand joings
		leftHand.at(0) = tracker.getLeftHandCoordinates().x;
		leftHand.at(1) = tracker.getLeftHandCoordinates().y;
		leftHand.at(2) = tracker.getLeftHandCoordinates().z;
		rightHand = vector<double> (3);
		rightHand.at(0) =  tracker.getRightHandCoordinates().x;
		rightHand.at(1) =  tracker.getRightHandCoordinates().y;
		rightHand.at(2) =  tracker.getRightHandCoordinates().z;*/
		if(bodyTracker.isTracking){
				Point3f input[2];
				input[0] = bodyTracker.getTransformedLeftHandCoordinates();
				input[1] = bodyTracker.getTransformedRightHandCoordinates();
				leftHandGraph.update(point3fToVectorDouble(input[0]));
				rightHandGraph.update(point3fToVectorDouble(input[1]));
				vector<double> inputVector = point3fToVectorDouble(input,2);
				//printf("%f %f %f %f %f %f \n", inputVector[0], inputVector[1], inputVector[2], inputVector[3], inputVector[4], inputVector[5]);

        
        if( trainingModeActive ){
            
            if( trainingTimer.getInRecordingMode() ){
                trainingData.addSample(trainingClassLabel, inputVector);
            }
            
            if( trainingTimer.getRecordingStopped() ){
                trainingModeActive = false;
            }
        }
        
        if( pipeline.getTrained() ){
            if( !pipeline.predict(inputVector) ){
                infoText = "Failed to make prediction";
            }
        }
    }
    
}

//--------------------------------------------------------------
void static_training::draw(){
    ofBackground(0,0,0);
    
    unsigned int x = 20;
    unsigned int y = 20;
    unsigned int graphWidth = TIME_SERIES_GRAPH_WIDTH;
    unsigned int graphHeight = 100;
    ofRectangle fontBox;
    string text;
    
    //Draw the timeseries graphs
    leftHandGraph.draw(x,y,graphWidth,graphHeight);     
    ofSetColor(255, 255, 255);
    text = "Left Hand";
    fontBox = font.getStringBoundingBox(text, 0, 0);
    font.drawString(text, x+(graphWidth/2)-(fontBox.width/2), y+10);
    y += graphHeight + 20;
    
    rightHandGraph.draw(x,y,graphWidth,graphHeight); 
    ofSetColor(255, 255, 255);
    text = "Right Hand";
    fontBox = font.getStringBoundingBox(text, 0, 0);
    font.drawString(text, x+(graphWidth/2)-(fontBox.width/2), y+10);
    y += graphHeight + 20;
    
    
    int textX = 20;
    int textY = y;
    int textSpacer = 20;
    
    //Draw the training info
    ofSetColor(255, 255, 255);
    text = "------------------- TrainingInfo -------------------";
    font.drawString(text,textX,textY);
    
    textY += textSpacer;
    if( trainingModeActive ){
        if( trainingTimer.getInPrepMode() ){
            ofSetColor(255, 200, 0);
            text = "PrepTime: " + ofToString(trainingTimer.getSeconds());
        }
        if( trainingTimer.getInRecordingMode() ){
            ofSetColor(255, 0, 0);
            text = "RecordTime: " + ofToString(trainingTimer.getSeconds());
        }
    }else text = "Not Recording";
    font.drawString(text,textX,textY);
    
    ofSetColor(255, 255, 255);
    textY += textSpacer;
    text = "TrainingClassLabel: " + ofToString(trainingClassLabel);
    font.drawString(text,textX,textY);
    
    textY += textSpacer;
    text = "NumTrainingSamples: " + ofToString(trainingData.getNumSamples());
    font.drawString(text,textX,textY);
    
    
    //Draw the prediction info
    textY += textSpacer*2;
    text = "------------------- Prediction Info -------------------";
    font.drawString(text,textX,textY);
    
    textY += textSpacer;
    text =  pipeline.getTrained() ? "Model Trained: YES" : "Model Trained: NO";
    font.drawString(text,textX,textY);
    
    textY += textSpacer;
    text = "PredictedClassLabel: " + ofToString(pipeline.getPredictedClassLabel());
    font.drawString(text,textX,textY);
    
    textY += textSpacer;
    text = "Likelihood: " + ofToString(pipeline.getMaximumLikelihood());
    font.drawString(text,textX,textY);
    
    
    //Draw the info text
    textY += textSpacer*2;
    text = "InfoText: " + infoText;
    font.drawString(text,textX,textY);
    
    
    //Draw the prediction boxes
    double boxX = textX;
    double boxY = textY + (textSpacer*2);
    double boxSize = 50;
    vector< UINT > classLabels = pipeline.getClassLabels();
	for(unsigned int i=0; i<pipeline.getNumClasses() && classLabels.size() > i; i++){
        if( pipeline.getPredictedClassLabel() == classLabels[i] ){
            ofSetColor(255,255,0);
            ofFill();
            ofRect(boxX, boxY, boxSize, boxSize);
        }
        
        ofSetColor(255,255,255);
        ofNoFill();
        ofRect(boxX, boxY, boxSize, boxSize);
        boxX += boxSize + 10;
    }
        
}

//--------------------------------------------------------------
void static_training::keyPressed(int key){
    
    infoText = "";
    
    switch( key ){
        case 'q':
           // synapseStreamer.openOutgoingConnection();
            break;
        case 'r':
            trainingModeActive = !trainingModeActive;
            if( trainingModeActive ){
                trainingTimer.startRecording(DEFAULT_PREP_TIME, DEFAULT_RECORD_TIME);
            }else trainingTimer.stopRecording();
            break;
        case '[':
            if( trainingClassLabel > 1 )
                trainingClassLabel--;
            break;
        case ']':
            trainingClassLabel++;
            break;
        case 't':
            if( pipeline.train( trainingData ) ){
                infoText = "Pipeline Trained";
            }else infoText = "WARNING: Failed to train pipeline";
            break;
        case 's':
            if( trainingData.saveDatasetToFile("TrainingData.txt") ){
                infoText = "Training data saved to file";
            }else infoText = "WARNING: Failed to save training data to file";
            break;
        case 'l':
            if( trainingData.loadDatasetFromFile("TrainingData.txt") ){
                infoText = "Training data saved to file";
            }else infoText = "WARNING: Failed to load training data from file";
            break;
        case 'c':
            trainingData.clear();
            infoText = "Training data cleared";
            break;
        default:
            printf("Key Pressed: %i\n",key);
            break;
    }

}

//--------------------------------------------------------------
void static_training::keyReleased(int key){

}

//--------------------------------------------------------------
void static_training::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void static_training::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void static_training::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void static_training::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void static_training::windowResized(int w, int h){

}

//--------------------------------------------------------------
void static_training::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void static_training::dragEvent(ofDragInfo dragInfo){ 

}

void static_training::exit(){

}

void static_training::setupGraphs(){
    
    vector< ofColor > axisColors(3);
    axisColors[0] = ofColor(255,0,0);
    axisColors[1] = ofColor(0,255,0);
    axisColors[2] = ofColor(0,0,255);
    
    leftHandGraph.init(TIME_SERIES_GRAPH_WIDTH, 3);
    leftHandGraph.backgroundColor = ofColor(0,0,0);
    leftHandGraph.gridColor = ofColor(200,200,200,100);
    leftHandGraph.drawGrid = true;
    leftHandGraph.drawInfoText = false;
    leftHandGraph.setRanges( vector<double>(3,0), vector<double>(3,1) );
    leftHandGraph.colors = axisColors;
    
    rightHandGraph.init(TIME_SERIES_GRAPH_WIDTH, 3);
    rightHandGraph.backgroundColor = ofColor(0,0,0);
    rightHandGraph.gridColor = ofColor(200,200,200,100);
    rightHandGraph.drawGrid = true;
    rightHandGraph.drawInfoText = false;
    rightHandGraph.setRanges( vector<double>(3,0), vector<double>(3,1) );
    rightHandGraph.colors = axisColors;
}

	vector<double> static_training::point3fToVectorDouble(Point3f point){ 
		vector<double> out = vector<double>(3);
			out[0] = point.x;
			out[1] = point.y;
			out[2] = point.z;
		//	printf("P %f %f %f", out[i+0], out[i+1], out[i+2]);
		
		//printf("\n");

		return out;
	}

	vector<double> static_training::point3fToVectorDouble(Point3f points[], int nPoints){ 
		vector<double> out = vector<double>(nPoints*3);

		for (int i = 0; i < nPoints; i++)
		{
			out[i*3+0] = points[i].x;
			out[i*3+1] = points[i].y;
			out[i*3+2] = points[i].z;
			
		}

		//printf("PPPP %f %f %f %f %f %f", out[0], out[1], out[2], out[3], out[4], out[5]);
		//printf("\n");

		return out;
	}

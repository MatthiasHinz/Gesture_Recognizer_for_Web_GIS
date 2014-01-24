#include "testApp.h"
#include "fullBodyTracker.h"
#include "Static_Recognizer.h"

//CONFIGURATION
//-----------------
//activate or deactivate components from here:
const bool useMouseControl = true;
const bool useOneHandRecognizer = false;
const bool useTwoHandRegognizer = false;
const bool useGrabDetector = true;
const bool useFullbodyTracker = true;
const bool useStaticRecognizer = true;
const int numberOfTrackedHands = 1; //<- TODO: give this number a meaning...
//TwoHandRecognizer would only work for two hands...
//-------------------


MouseControl mouseControl;
GRT_Recognizer recognizer;
GRT_Recognizer oneHandrecognizer;
Nite_HandTracker tracker;
GrabProxie grabLeft;
GrabProxie grabRight;
openniProxie openniP;
FullBodyTracker bodyTracker;
Static_Recognizer static_recognizer;


//--------------------------------------------------------------
void testApp::setup(){
	ofSetFrameRate(60);

	//string train[] = {"zoom_in_out_base.txt"};
	if(useTwoHandRegognizer)
		//recognizer.initPipeline("TrainingData_v3_zoomIn_ZoomOut.txt", 6);
			recognizer.initPipeline("zoom_in_out_base.txt", 6);
	if(useOneHandRecognizer)
		oneHandrecognizer.initPipeline("TrainingData_A_X_S.txt", 3);


	openniP.initOpenNi();
	tracker.initHandTracker();
	if(useFullbodyTracker){
		bodyTracker.initTracker();
	}
	if(useGrabDetector){
		grabLeft.initGrabDetector(openniP.m_device);
		grabRight.initGrabDetector(openniP.m_device);
	}

	if(useStaticRecognizer){
		static_recognizer.initPipeline("TrainingData_static_zoomin_out.txt" ,6);

	}
}

//--------------------------------------------------------------
void testApp::update(){
	float xnew, ynew;
	VectorDouble inputGRT(6);
	VectorDouble inputGRT2(3);
	//retrieve data from HandTracker
	tracker.updateHandTracker();
	openniP.update();

	if(useFullbodyTracker){
		bodyTracker.update();
		if(!tracker.isLeftHandTracked() && bodyTracker.leftHandPositionConfidence > .5){
			tracker.initLeftHand(bodyTracker.getLeftHandCoordinates());
		
		}

		if(!tracker.isRightHandTracked() && bodyTracker.rightHandPositionConfidence > .5){
			tracker.initRightHand(bodyTracker.getRightHandCoordinates());
		}
	
	}

	if(tracker.isRightHandTracked()){
		if(useMouseControl && !mouseControl.isMouseControled())
			mouseControl.startMouseControl();
		
		//retrieve inputs
		inputGRT2[0] = inputGRT[0] = xnew = tracker.getRightHandCoordinates().x;
		inputGRT2[1] = inputGRT[1] = ynew = tracker.getRightHandCoordinates().y;
		inputGRT2[2] = inputGRT[2] = tracker.getRightHandCoordinates().z;
		//-------------------------------------------------------------------------
		if(useMouseControl)
			mouseControl.updateMouseControl(xnew, ynew);

		//-------------------------------------------------------------------------
		if(useOneHandRecognizer){
			GestureRecognitionPipeline &pipeline = oneHandrecognizer.pipeline;
			pipeline.predict(inputGRT2);
			//printf("Input: (%d, %d, %d)\n", inputGRT2[0], inputGRT2[1], inputGRT2[2]);
			string message = oneHandrecognizer.oneHandedLabelMapping(pipeline.getPredictedClassLabel());
			double likelyhood= pipeline.getMaximumLikelihood();
				if(message != "" && likelyhood>0.7)
					printf("\nGesture: %s\n", message.c_str());
		}

		//-------------------------------------------------------------------------
		if(useGrabDetector){
			bool lost = false; //TODO: this is eventually not such a nice solution... find better ways to detect lost hands
			bool track = true;
			
			grabRight.updateAlgorithm(lost, track, tracker.getRightHandCoordinates(), openniP.m_depthFrame, openniP.m_colorFrame);
		}

		//-------------------------------------------------------------------------
		if(tracker.isLeftHandTracked()){
			//-------------------------------------------------------------------------
			if(useGrabDetector){
				bool lost = false; //TODO: this is eventually not such a nice solution... find better ways to detect lost hands
				bool track = true;
				
				grabLeft.updateAlgorithm(lost, track, tracker.getLeftHandCoordinates(), openniP.m_depthFrame, openniP.m_colorFrame);
			}
			//-------------------------------------------------------------------------
			if(useTwoHandRegognizer){
				inputGRT[3] = tracker.getLeftHandCoordinates().x;
				inputGRT[4] = tracker.getLeftHandCoordinates().y;
				inputGRT[5] = tracker.getLeftHandCoordinates().z;
			
				GestureRecognitionPipeline &pipeline = recognizer.pipeline;
				pipeline.predict(inputGRT);
				string message = recognizer.twoHandedLabelMapping(pipeline.getPredictedClassLabel());
				double likelyhood = pipeline.getMaximumLikelihood();
				if(message != ""){
					printf("\nGesture: %s\n", message.c_str());
				}
			}
			//-------------------------------------------------------------------------
			if(useStaticRecognizer && useFullbodyTracker){
					vector<double> 	leftHand = vector<double> (3);
        //Get the left hand and right hand joings


				Point3f input[2];
				input[0] = bodyTracker.getTransformedLeftHandCoordinates();
				input[1] = bodyTracker.getTransformedRightHandCoordinates();
				string gestureMessage = static_recognizer.findGesture(point3fToVectorDouble(input,2));
				
				if(gestureMessage.compare("") !=0){
					printf("%s\n", gestureMessage.c_str());
				}
			}
			//-------------------------------------------------------------------------
		}	
	}else{//if right hand is not tracked (any more...)
		if(useMouseControl && mouseControl.isMouseControled())
			mouseControl.stopMouseControl();
	}



}

//--------------------------------------------------------------
void testApp::draw(){
	   
    ofBackground(0, 0, 0);
    
    string text;
    int textX = 20;
    int textY = 20;
    
    //Draw the training info
    ofSetColor(255, 255, 255);
 
    text = "------------------- Prediction Info -------------------";
    ofDrawBitmapString(text, textX,textY);
    GestureRecognitionPipeline &pipeline = recognizer.pipeline;

    /*textY += 15;
    text =  pipeline.getTrained() ? "Model Trained: YES" : "Model Trained: NO";
    ofDrawBitmapString(text, textX,textY);*/
    
    textY += 15;
    text = "PredictedClassLabel: " + ofToString(pipeline.getPredictedClassLabel());
    ofDrawBitmapString(text, textX,textY);
    
    textY += 15;
    text = "Likelihood: " + ofToString(pipeline.getMaximumLikelihood());
    ofDrawBitmapString(text, textX,textY);
	  textY += 15;
	    text = "------------------- Prediction Info2 -------------------";
    ofDrawBitmapString(text, textX,textY);
    
	GestureRecognitionPipeline &pipeline2 = static_recognizer.getPipeline();
    /*textY += 15;
    text =  pipeline2.getTrained() ? "Model Trained: YES" : "Model Trained: NO";
    ofDrawBitmapString(text, textX,textY);*/
    
    textY += 15;
    text = "PredictedClassLabel: " + ofToString(pipeline2.getPredictedClassLabel());
    ofDrawBitmapString(text, textX,textY);
    
    textY += 15;
    text = "Likelihood: " + ofToString(pipeline2.getMaximumLikelihood());
    ofDrawBitmapString(text, textX,textY);
    
    textY += 15;
    text = "SampleRate: " + ofToString(ofGetFrameRate(),2);
    ofDrawBitmapString(text, textX,textY);
    
    /*
    //Draw the info text
    textY += 30;
    text = "InfoText: " + infoText;
    ofDrawBitmapString(text, textX,textY);*/

	//Draw number of hands currently dragged
	ofSetColor(255, 0, 0);
    textY += 15;
	text = "Left Hand is tracked: "+ ofToString(tracker.isLeftHandTracked());
	ofDrawBitmapString(text, textX,textY);
	    textY += 15;
	text = "Right Hand is tracked: "+ ofToString(tracker.isRightHandTracked());
	ofDrawBitmapString(text, textX,textY);
    ofSetColor(255, 255, 255);

    //Draw the timeseries data
   // if( record  && noOfHands == noOfTrackedHands){
        ofFill();
            //double r = ofMap(i,0,timeseries.getNumRows(),0,255);
            //double g = 0;
            //double b = 255-r;
		    //ofSetColor(r,g,b); 
            ofSetColor(250,0,0);
			int x = tracker.getLeftHandCoordinates().x;
			int	y = tracker.getLeftHandCoordinates().y;
            ofEllipse(250+x,500-y,5,5);

		 //  if(noOfHands >= 2){
				ofSetColor(0,80,255);
				x = tracker.getRightHandCoordinates().x;
				y = tracker.getRightHandCoordinates().y;
				ofEllipse(350+x,500-y,5,5);
			//}
        
   // }
 
    

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}

vector<double> testApp::point3fToVectorDouble(Point3f points[], int nPoints){ 
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


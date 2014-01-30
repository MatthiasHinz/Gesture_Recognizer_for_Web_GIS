#include "Static_Recognizer.h"
#include "GUIConnector.h"
  UINT trainingClassLabel;

Static_Recognizer::Static_Recognizer(void)
{
}


Static_Recognizer::~Static_Recognizer(void)
{
}

GestureRecognitionPipeline pipeline_anbc;

const string ZOOM_IN_GESTURE = "ZOOM_IN";
const string ZOOM_OUT_GESTURE = "ZOOM_OUT";

const UINT POSITION_NEUTRAL = 0;
const UINT POSITION_HANDS_MIDDLE = 1;
const UINT POSITION_HANDS_DIAGONAL = 2;
const int timout = 99999999999;//20;
const int blockzoom = 30;
boolean noZoomIn = false;
boolean noZoomOut = false;

UINT recent_position=POSITION_NEUTRAL;
string recent_gesture="";
int timer = 0;
int blockTimer = 0;

//const int repeat_after = 1000;
//int rep_timer = 0;

bool Static_Recognizer::initPipeline(string trainingdatafile, int dimension)
{
			    //Initialize the training and info variables
   // infoText = "";
   // trainingClassLabel = 1;
   // noOfHands = 2;
	//noOfTrackedHands = 0;
    
	
	//The input to the training data will be the R[x y z]L[x y z] from the left end right hand
	// so we set the number of dimensions to 6
	LabelledClassificationData trainingData; 
    trainingData.setNumDimensions(6);

    if( trainingData.loadDatasetFromFile(trainingdatafile) ){
         printf("Training data loaded from file\n");
	}else {
		printf("WARNING: Failed to load training data from file\n");
	}
     //Setup the training data
    //trainingData.setNumDimensions( 6 );
    //trainingClassLabel = 1;

    //Setup the classifier
    ANBC anbc;
    anbc.enableNullRejection(true);
    anbc.setNullRejectionCoeff(5);
    pipeline_anbc.setClassifier( anbc );

	pipeline_anbc.addPostProcessingModule(ClassLabelTimeoutFilter(1500, ClassLabelTimeoutFilter::ALL_CLASS_LABELS));
  
	if( pipeline_anbc.train( trainingData ) ){
		printf("Pipeline Trained. Num of classes: %i\n", trainingData.getNumClasses());
      }else printf("WARNING: Failed to train pipeline\n");

	return true;
}


std::string Static_Recognizer::findGesture(VectorDouble input)
{
	/*if(recent_position != POSITION_NEUTRAL){
		if(timer <= timout){
			timer++;
		}else{
			printf("TIMEOUT\n");
			timer=0;
			recent_position = POSITION_NEUTRAL;
		}
	}

	if(noZoomIn){
		if(blockTimer < blockzoom){
			blockTimer++;
		}else{
			noZoomIn = false;
			blockTimer = 0;
			printf("unblock zoomin\n");
		}	
	}

	if(noZoomOut){
		if(blockTimer < blockzoom){
			blockTimer++;
		}else{
			noZoomOut = false;
			blockTimer = 0;
			printf("unblock zomout\n");
		}	
	}*/


		if( pipeline_anbc.getTrained()){
			pipeline_anbc.predict(input);
			UINT label = pipeline_anbc.getPredictedClassLabel();
			//printf("classLabel: %i\n",label);
			if(pipeline_anbc.getMaximumLikelihood() < 0.9)
				return "";
			else{
						switch(label){
						case 1:{
							/*	BYTE keys2[1] = {VK_OEM_PLUS};
							GUIConnector::sendKeyboardInput(keys2,1);*/
						//return "ZOOM IN";
							return "HANDS_MIDDLE";
						}
						break;
						case 2:{
							BYTE keys2[1] = {VK_OEM_MINUS};
							GUIConnector::sendKeyboardInput(keys2,1);
							return "ZOOM OUT";	   
						
						}
						break;
				case 3:{
					BYTE keys2[1] = {VK_TAB};
					GUIConnector::sendKeyboardInput(keys2,1);
					return "TAB";	
					;}
				case 4:{
					BYTE keys2[2] = {VK_SHIFT, VK_TAB};
					GUIConnector::sendKeyboardInput(keys2,2);
					return "BACK TAB";	
					;}
				case 5:{
					/*BYTE keys12[3] = {VK_CONTROL,VK_SHIFT,GUIConnector::E};
							GUIConnector::sendKeyboardInput(keys12,3);*/
				
					BYTE keys12[1] = {VK_OEM_PLUS};
							GUIConnector::sendKeyboardInput(keys12,1);
						return "HANDS ABOVE HEAD / ZOOM IN";
					   }
				/*case 6:{
					BYTE keys4[1] = {VK_OEM_PLUS};
					GUIConnector::sendKeyboardInput(keys4,1);
					printf(" (\"+\"-Key pressed / Zoom in), Left button state: %i\n",GetKeyState(VK_LBUTTON));
					return "QUATER TO TWELVE";
					   }
				case 7:{
					BYTE keys4f[1] = {VK_OEM_MINUS};
					GUIConnector::sendKeyboardInput(keys4f,1);
					return "QUATER PAST NINE";
					}
				case 8:{
					BYTE keys4d[1] = {VK_TAB};
					GUIConnector::sendKeyboardInput(keys4d,1);
					return "DIAGONAL-LEFTUP-RIGHTDOWN";
					}*/
				default:
					return "";
				}
			  
				/*switch(label){
					case POSITION_HANDS_MIDDLE:
						if(recent_position == POSITION_HANDS_DIAGONAL){
							if(!noZoomIn){
								recent_position = POSITION_NEUTRAL;
								printf("COMPLETED ZOOM IN\n");
								BYTE keys[1] = {VK_OEM_PLUS};
								GUIConnector::sendKeyboardInput(keys,1);
								printf(" (\"+\"-Key pressed / Zoom in), Left button state: %i\n",GetKeyState(VK_LBUTTON));
								noZoomOut = true; //temporary block zooming out
								blockTimer = 0;
								return ZOOM_IN_GESTURE;
							}
							//alternative: repeat gesture when the postion is hold for some time
						} else{
							if(!noZoomOut){
								recent_position = POSITION_HANDS_MIDDLE;
								timer = 0;
								printf("START ZOOM OUT\n");
							}
						}
						break;
					case POSITION_HANDS_DIAGONAL:
						if(recent_position == POSITION_HANDS_MIDDLE){
							if(!noZoomOut){
							recent_position = POSITION_NEUTRAL;
							BYTE keys2[1] = {VK_OEM_MINUS};
							GUIConnector::sendKeyboardInput(keys2,1);
							printf(" (\"-\"-Key pressed / Zoom out), Left button state: %i\n",GetKeyState(VK_LBUTTON));
							printf("COMPLETED ZOOM OUT\n");
							noZoomIn = true; //temporary block zooming in
							blockTimer = 0;
							return ZOOM_OUT_GESTURE;
							}
							//alternative: repeat gesture when the postion is hold for some time
						} else{
							if(!noZoomIn){
								recent_position = POSITION_HANDS_DIAGONAL;
								timer = 0;
								printf("START ZOOM IN\n");
							}
						}
						break;
				case 3:{
					/*BYTE keys12[3] = {VK_CONTROL,VK_SHIFT,GUIConnector::E};
							GUIConnector::sendKeyboardInput(keys12,3);
					return "HANDS ABOVE HEAD";}*/
				/*
					BYTE keys12[1] = {VK_OEM_PLUS};
							GUIConnector::sendKeyboardInput(keys12,1);
					return "HANDS ABOVE HEAD";}
				case 4:
					return "HANDS CROSSED";
				case 5:{
						BYTE keys3[1] = {VK_OEM_MINUS};
						GUIConnector::sendKeyboardInput(keys3,1);
						return "QUATER PAST TWELVE";
					   }
				case 6:{
					BYTE keys4[1] = {VK_OEM_PLUS};
					GUIConnector::sendKeyboardInput(keys4,1);
					printf(" (\"+\"-Key pressed / Zoom in), Left button state: %i\n",GetKeyState(VK_LBUTTON));
					return "QUATER TO TWELVE";
					   }
				case 7:{
					BYTE keys4f[1] = {VK_OEM_MINUS};
					GUIConnector::sendKeyboardInput(keys4f,1);
					return "QUATER PAST NINE";
					}
				case 8:{
					BYTE keys4d[1] = {VK_TAB};
					GUIConnector::sendKeyboardInput(keys4d,1);
					return "DIAGONAL-LEFTUP-RIGHTDOWN";
					}
				default:
					return "";
				}*/
			}
		}
		return "";
}




string Static_Recognizer::oneHandedLabelMapping(int label){
					switch(label){
				case 1:
					return "A";
				case 2:
					return "X";
				case 3:
					return "S";
				/*case 4:
					return "PAN LEFT";
				case 5:
					return "PAN RIGHT";
				case 6:
					return "PAN UP";
				case 7:
					return "PAN DOWN";
				case 8:
					return "";*/
				default:
					return "";
						
			};

}


string Static_Recognizer::twoHandedLabelMapping(int label){
				switch(label){
				case 1:
					return "ZOOM IN";
				case 2:
					return "ZOOM OUT";
				/*case 3:
					return "ZOOM OUT";
				case 4:
					return "PAN LEFT";
				case 5:
					return "PAN RIGHT";
				case 6:
					return "PAN UP";
				case 7:
					return "PAN DOWN";
				case 8:
					return "";*/
				default:
					return "";
						
			};

}

GestureRecognitionPipeline Static_Recognizer::getPipeline(){
	return pipeline_anbc;
}

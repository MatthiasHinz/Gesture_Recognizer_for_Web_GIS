#include "Nite_HandTracker.h";
#include "GUIConnector.h";
#include "windows.h";

Point3f leftHand;
Point3f rightHand;

bool isLeftHandTrackedV, isRightHandTrackedV = false;
int leftHandInSync, rightHandInSync = 0; //counts how many times a hand is out of sync

nite::HandId leftHandId, rightHandId;
const double likelyhood_threshold = 0.6;


nite::Status niteRc;
nite::HandTracker handTracker;
nite::HandTrackerFrameRef handTrackerFrame;

bool printInitMessageLeft = true;
bool printInitMessageRight = true;

const bool initTrackingByGesture = false;
const bool initTrackngByExternalInput = true;

Nite_HandTracker::Nite_HandTracker(void)
{

}


Nite_HandTracker::~Nite_HandTracker(void)
{
}


int Nite_HandTracker::initHandTracker(void)
{
	niteRc = nite::NiTE::initialize();
	if (niteRc != nite::STATUS_OK)
	{
		printf("NiTE initialization failed\n");
		return 1;
	}
	
	niteRc = handTracker.create();
	if (niteRc != nite::STATUS_OK)
	{
		printf("Couldn't create user tracker\n");
		return 3;
	}
	
	handTracker.setSmoothingFactor(0.2);
	handTracker.startGestureDetection(nite::GESTURE_WAVE);
	handTracker.startGestureDetection(nite::GESTURE_HAND_RAISE);
	handTracker.startGestureDetection(nite::GESTURE_CLICK);
	return 0;
}

void Nite_HandTracker::updateHandTracker(void)
{
	 //================== OPTAIN DATA ========================
	const nite::Array<nite::HandData>& hands = handTrackerFrame.getHands();
	leftHandInSync++;
	rightHandInSync++;

	bool isLeftHandLost = false;
	bool isRightHandLost = false;

	for (int i = 0; i < hands.getSize(); ++i)
	{
			const nite::HandData& hand = hands[i];
			if (hand.isTracking())
			{
				//printf("%d. (%5.2f, %5.2f, %5.2f)\n", hand.getId(), hand.getPosition().x, hand.getPosition().y, hand.getPosition().z);
				if(hand.getId() == rightHandId){
					rightHand = hand.getPosition();
					rightHandInSync = 0;
					
				}else
					if(hand.getId() == leftHandId){
						leftHand = hand.getPosition();
						leftHandInSync = 0;
					}
			}

			if(hand.getId() == rightHandId && hand.isLost()){
				printf("Your right hand should get lost!!!!\n");
				isRightHandLost= true;
					
			}
			if(hand.getId() == leftHandId && hand.isLost()){
				printf("Your left hand should get lost!!!!\n");
				isLeftHandLost=true;
			}
				// indicate that hand is lost
					//hand.isLost();
					//hand.isNew();
				
		}

		if(isRightHandLost || (rightHandInSync>200 && isRightHandTrackedV)){
			printf("\nLost track for right hand!\n");
			isRightHandTrackedV = false;
			printInitMessageRight = true;
			rightHandInSync=0;
			//rightHand = Point3f(0,0,0);

			BYTE keys [3] = {VK_CONTROL,VK_SHIFT,VK_OEM_COMMA};
			GUIConnector::sendKeyboardInput(keys, 3);
		}

		if(isLeftHandLost || (leftHandInSync>200 && isLeftHandTrackedV)){
			printf("\nLost track for left hand!\n");
			isLeftHandTrackedV = false;
			printInitMessageLeft = true;
			leftHandInSync=0;

			BYTE keys [3] = {VK_CONTROL,VK_SHIFT,GUIConnector::L};
			GUIConnector::sendKeyboardInput(keys,3);
		}

		//===== INIT TRACKER IF NEEDED===============

		const nite::Array<nite::GestureData>& gestures = handTrackerFrame.getGestures();

		if(!isRightHandTrackedV && printInitMessageRight){
			printf("\nWave your right hand to start tracking it...\n");
			printInitMessageRight = false; //print only once


		}else if(!isLeftHandTrackedV && isRightHandTrackedV && printInitMessageLeft){
			printf("\nWave your left hand to start tracking it...\n");
			printInitMessageLeft = false;
		}

	    //check whether readframe works
		niteRc = handTracker.readFrame(&handTrackerFrame);
		if (niteRc != nite::STATUS_OK)
		{
			printf("Get next frame failed\n");
			return;
		}

		for (int i = 0; i < gestures.getSize() ; ++i)
		{
			if (gestures[i].isComplete() ){
				if(gestures[i].getType()==nite::GESTURE_CLICK)
				{
					printf("\nGESTURE CLICK" );

					if(GetKeyState(VK_LBUTTON) >= 0){
						BYTE keys[1] = {VK_OEM_PLUS};
						GUIConnector::sendKeyboardInput(keys,1);
						printf(" (\"+\"-Key pressed / Zoom in), Left button state: %i\n",GetKeyState(VK_LBUTTON));
					}else{
						BYTE keys[1] = {VK_OEM_MINUS};
						GUIConnector::sendKeyboardInput(keys,1);
						printf(" (\"-\"-Key pressed / Zoom out), Left button state: %i\n",GetKeyState(VK_LBUTTON));
					} 

				}

				/*if(gestures[i].getType()==nite::GESTURE_HAND_RAISE)
				{
					printf("\nGESTURE HAND RAISE\n" );
				}*/
				
				if(gestures[i].getType()==nite::GESTURE_WAVE)
				{
					printf("\nGESTURE WAVE\n" );
				}
						
			}
				
			

			if (gestures[i].isComplete() && gestures[i].getType() == nite::GESTURE_WAVE && (!isRightHandTrackedV || !isLeftHandTrackedV)
				&& gestures[i].getCurrentPosition()!= leftHand && gestures[i].getCurrentPosition() !=rightHand)
			{
				nite::HandId newId;
				handTracker.startHandTracking(gestures[i].getCurrentPosition(), &newId);
				nite::Point3f start = gestures[i].getCurrentPosition();
				bool foundHand = false;
				if(!isRightHandTrackedV) {
					rightHandId = newId;
					isRightHandTrackedV=true;
					rightHandInSync=0;
					foundHand = true;
					printf("Found right hand!");
					BYTE keys [3] = {VK_CONTROL,VK_SHIFT,VK_F1};
					GUIConnector::sendKeyboardInput(keys,3);

				}else if(!isLeftHandTrackedV){
					leftHandId = newId;
					isLeftHandTrackedV=true;
					leftHandInSync=0;
					foundHand = true;
					printf("Found left hand!");
					BYTE keys [3] = {VK_CONTROL,VK_SHIFT,GUIConnector::K};
					GUIConnector::sendKeyboardInput(keys,3);
				}
			}
		}


}

Point3f Nite_HandTracker::getLeftHandCoordinates(void)
{
	return leftHand;
}



Point3f Nite_HandTracker::getRightHandCoordinates(void)
{
	return rightHand;
}


//bool Nite_HandTracker::isLeftHandRecognized(void)
//{
//	return false;
//}


bool Nite_HandTracker::isRightHandTracked(void)
{
	return isRightHandTrackedV;
}


bool Nite_HandTracker::isLeftHandTracked(void)
{
	return isLeftHandTrackedV;
}





/*
			const nite::Array<nite::GestureData>& gestures = handTrackerFrame.getGestures();

		//these are onhanded gestures
		for (int i = 0; i < gestures.getSize() && (!isLeftHandTracked || !isRightHandTrackedV); ++i)
		{

			if (gestures[i].isComplete())
			{
				if(gestures[i].getType() == nite::GESTURE_CLICK){
					return "CLICK recognized";
				}

				if(gestures[i].getType() == nite::GESTURE_HAND_RAISE){
					return "HAND RAISE recognized";
				}

				if(gestures[i].getType() == nite::GESTURE_WAVE){
					return "WAVING recognized";
				}
					
			}
		}
*/







					/*
					INPUT input = {0};
					//ZeroMemory(&input, sizeof(input));
					input.type = INPUT_MOUSE;
					input.mi.dwFlags = MOUSEEVENTF_WHEEL | MOUSEEVENTF_VIRTUALDESK;
					if(GetKeyState(VK_LBUTTON) >= 0){
						input.mi.mouseData = 5;
						printf(" (Mousewheel +5 / Zoom in), Left button state: %i\n",GetKeyState(VK_LBUTTON));
					}else{
						input.mi.mouseData = -5;
						printf(" (Mousewheel -5 / Zoom out), Left button state: %i\n",GetKeyState(VK_LBUTTON));
					} 

					int returnCode=SendInput(1, &input, sizeof(input));
					*/


			/* 
			INPUT input = {0};
			input.type = INPUT_KEYBOARD;
			input.ki.wVk = VK_CONTROL;
			input.ki.dwFlags = 0; // KEYS DOWN!!!
			int returnCode=SendInput(1, &input, sizeof(input));

			input.ki.wVk = VK_SHIFT;
			returnCode=SendInput(1, &input, sizeof(input));

			input.ki.wVk = 0x4C; //l-key
			returnCode=SendInput(1, &input, sizeof(input));
			
			input.ki.dwFlags = KEYEVENTF_KEYUP; //KEYS UP!!!!
			
			returnCode=SendInput(1, &input, sizeof(input));

			input.ki.wVk = VK_SHIFT;
			returnCode=SendInput(1, &input, sizeof(input));

			input.ki.wVk = VK_CONTROL;
			returnCode=SendInput(1, &input, sizeof(input));*/

			/*
			INPUT input = {0};
			input.type = INPUT_KEYBOARD;
			input.ki.wVk = VK_CONTROL;
			input.ki.dwFlags = 0; // KEYS DOWN!!!
			int returnCode=SendInput(1, &input, sizeof(input));

			input.ki.wVk = VK_SHIFT;
			returnCode=SendInput(1, &input, sizeof(input));

			input.ki.wVk = 0x4C; //l-key
			returnCode=SendInput(1, &input, sizeof(input));
			
			input.ki.dwFlags = KEYEVENTF_KEYUP; //KEYS UP!!!!
			
			returnCode=SendInput(1, &input, sizeof(input));

			input.ki.wVk = VK_SHIFT;
			returnCode=SendInput(1, &input, sizeof(input));

			input.ki.wVk = VK_CONTROL;
			returnCode=SendInput(1, &input, sizeof(input));*/
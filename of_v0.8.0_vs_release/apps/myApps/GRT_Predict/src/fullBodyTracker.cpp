#include "fullBodyTracker.h"

int outOfSync = 0;
const int maxOutOFSync = 500;

FullBodyTracker::FullBodyTracker(void)
{

}


FullBodyTracker::~FullBodyTracker(void)
{
}

void FullBodyTracker::initTracker(void)
{
	niteRc = userTracker.create();
	userTracker.setSkeletonSmoothingFactor(0.8);
	isTracking = false;
}


void FullBodyTracker::update(void)
{
	nite::UserTrackerFrameRef userTrackerFrame;
	niteRc = userTracker.readFrame(&userTrackerFrame);
	//Do other perframe processing stuff

	const nite::Array<nite::UserData>& users = userTrackerFrame.getUsers();
	
	outOfSync++; //in case the user does not appear in the frame
	for (int i = 0; i < users.getSize(); ++i)
	{
		const nite::UserData& user = users[i];
		
		if(!isTracking && user.isVisible() && !user.isLost()){
			userTracker.startSkeletonTracking(user.getId());
			userTracker.startPoseDetection(user.getId(), nite::POSE_CROSSED_HANDS);
			userTracker.startPoseDetection(user.getId(), nite::POSE_PSI);
			isTracking = true;
			printf("Found User!\n");
		}

		if (user.getSkeleton().getState() == nite::SKELETON_TRACKED){
			if(user.isLost() || !user.isVisible()){
				isTracking = false;
				userTracker.stopSkeletonTracking(user.getId());
				userTracker.stopPoseDetection(user.getId(), nite::POSE_CROSSED_HANDS);
				userTracker.stopPoseDetection(user.getId(), nite::POSE_PSI);
				printf("Lost User!\n");
				continue;
			}
			outOfSync=0;//user appears --> user in sync
			const nite::SkeletonJoint& head =	user.getSkeleton().getJoint(nite::JOINT_HEAD);
			const nite::SkeletonJoint& leftHand =	user.getSkeleton().getJoint(nite::JOINT_LEFT_HAND);
			const nite::SkeletonJoint& rightHand =	user.getSkeleton().getJoint(nite::JOINT_RIGHT_HAND);
			if(head.getPositionConfidence() > 0.1){  //rejecting very uncertain values
				headCoordinates.x = head.getPosition().x;
				headCoordinates.y = head.getPosition().y;
				headCoordinates.z = head.getPosition().z;
			}
				
			if(leftHand.getPositionConfidence() > 0.1){
				leftHandCoordinates.x = leftHand.getPosition().x;
				leftHandCoordinates.y = leftHand.getPosition().y;
				leftHandCoordinates.z = leftHand.getPosition().z;
				leftHandPositionConfidence = leftHand.getPositionConfidence();
				/* printf("%d. (%5.2f, %5.2f, %5.2f)) \n", user.getId(),
					leftHandCoordinates.x,
					leftHandCoordinates.y,
					leftHandCoordinates.z);*/
			}else leftHandPositionConfidence = 0;
			
			if(rightHand.getPositionConfidence() > 0.1){
				rightHandCoordinates.x = rightHand.getPosition().x;
				rightHandCoordinates.y = rightHand.getPosition().y;
				rightHandCoordinates.z = rightHand.getPosition().z;
				rightHandPositionConfidence = rightHand.getPositionConfidence();
				/*printf("%d. (%5.2f, %5.2f, %5.2f)) \n", user.getId(),
				rightHandCoordinates.x,
				rightHandCoordinates.y,
				rightHandCoordinates.z);*/
			}else rightHandPositionConfidence = 0;

			//detect pose
			/*nite::PoseData pose = user.getPose(nite::POSE_PSI);
			if(pose.isHeld()){
				printf("User holds PSI Pose\n");
			}*/

			nite::PoseData pose = user.getPose(nite::POSE_CROSSED_HANDS);
			if(pose.isEntered()){
				printf("User enters crossed hands pose (ESCAPE)\n");
				BYTE keys[1] = {VK_ESCAPE};
				GUIConnector::sendKeyboardInput(keys,1);
			}
	
		}

		if(outOfSync == maxOutOFSync && isTracking){
			isTracking = false;
			userTracker.stopSkeletonTracking(user.getId());
			printf("Lost User! (user out of sync)\n");

		}


		/*const nite::Array<nite::GestureData>& gestures = userTrackerFrame.getUserById(trackedUser).;
		for (int i = 0; i < gestures.getSize(); ++i)
		{
			if (gestures[i].isComplete()){
				nite::HandId newId;
				handTracker.startHandTracking(gestures[i].getCurrentPosition(), &newId);
			}
		}*/

	}


}



Point3f FullBodyTracker::getLeftHandCoordinates(void)
{
	return leftHandCoordinates;
}


Point3f FullBodyTracker::getRightHandCoordinates(void)
{
	return rightHandCoordinates;
}


Point3f FullBodyTracker::getTransformedLeftHandCoordinates(void)
{
		Point3f out;
		out.x=leftHandCoordinates.x - headCoordinates.x;
		out.y=leftHandCoordinates.y - headCoordinates.y;
		out.z=leftHandCoordinates.z - headCoordinates.z;
		return out;
}


Point3f FullBodyTracker::getTransformedRightHandCoordinates(void)
{
			Point3f out;
		out.x=rightHandCoordinates.x - headCoordinates.x;
		out.y=rightHandCoordinates.y - headCoordinates.y;
		out.z=rightHandCoordinates.z - headCoordinates.z;
		return out;
}


nite::UserTracker FullBodyTracker::getUserTracker(){
	return userTracker;
}
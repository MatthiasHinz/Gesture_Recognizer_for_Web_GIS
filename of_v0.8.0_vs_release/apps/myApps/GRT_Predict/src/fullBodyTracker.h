#pragma once

#include <NiTE.h>
using namespace nite;

class FullBodyTracker
{
public:
	FullBodyTracker(void);
	~FullBodyTracker(void);
	void update(void);
	bool isTracking;
	float leftHandPositionConfidence;
	float rightHandPositionConfidence;

private:
	nite::Status niteRc;
	nite::UserTracker userTracker;
	Point3f leftHandCoordinates;

	Point3f rightHandCoordinates;
	Point3f headCoordinates;
	nite::UserId trackedUser;

public:
	void initTracker(void);
	Point3f getLeftHandCoordinates(void);
	Point3f getRightHandCoordinates(void);
	Point3f getTransformedLeftHandCoordinates(void);
	Point3f getTransformedRightHandCoordinates(void);
	nite::UserTracker getUserTracker();
};




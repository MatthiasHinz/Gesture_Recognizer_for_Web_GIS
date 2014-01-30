#include <NiTE.h>
#include "windows.h";
#include <string.h>;
using namespace nite;

#pragma once
class Nite_HandTracker
{
public:
	Nite_HandTracker(void);
	~Nite_HandTracker(void);
	int initHandTracker(void);
	Point3f getLeftHandCoordinates(void);
	void updateHandTracker(void);
	Point3f getRightHandCoordinates(void);
//	bool isLeftHandRecognized(void);
	bool isRightHandTracked(void);
	bool isLeftHandTracked(void);
	void initLeftHand(Point3f position);
	void initRightHand(Point3f position);
	int leftLockedInCounter;
	int rightLockedInCounter;
};


#pragma once
//#include "GameGlobal.h"
#include "ObjectClass.h"

using namespace std;

class PLAYERCONTROLLER_implement :public COMPONENT_implement
{
public:
	PLAYERCONTROLLER_implement(CONTROLLER* _controller, float _speedX = 0,float _speedY = 0)
		:COMPONENT_implement("PLAYERCONTROLLER"),controller(_controller),speedX(_speedX),speedY(_speedY)
	{
		
	}
	float speedX;
	float speedY;
	wTRANSFORM transform;
	wRIGIDBODY rigidbody;
	CONTROLLER *controller;

	virtual void Start()
	{
		rigidbody = gameObject->rigidbody;
	}
	virtual void FixedUpdate()
	{
		if (controller->keyDown['a' - 'a'])
			rigidbody.lock()->velocity.x = -speedX;
		if (controller->keyDown['d' - 'a'])
			rigidbody.lock()->velocity.x = speedX;
		if (controller->keyDown['w' - 'a'])
			rigidbody.lock()->velocity.y = -speedY;
		if (controller->keyDown['s' - 'a'])
			rigidbody.lock()->velocity.y = speedY;

	}
};
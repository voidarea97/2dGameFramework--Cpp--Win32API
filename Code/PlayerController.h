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
		if (controller->mouseLeftDown)
		{
			/*for (int i = 0; i < 50; i++)
			{
				GAMEOBJECT o1 = CreateObject(VECTOR2(0, 0), "obj1");
				SPRITE sprite1 = make_shared<SPRITE_implement>(VECTOR2(50, 50), 100, 100, "2.bmp");
				o1->AddComponent(sprite1);
				RIGIDBODY r1 = make_shared<RIGIDBODY_implement>(VECTOR2(0, 50), VECTOR2(0, 0));
				o1->AddComponent(r1);
			}*/
			DestroyObject();
			return;
		}
		if (controller->keyUp['a' - 'a'])
			rigidbody.lock()->acceleration.x = 0;
		if (controller->keyUp['d' - 'a'])
			rigidbody.lock()->acceleration.x = 0;
		if (controller->keyUp['w' - 'a'])
			rigidbody.lock()->acceleration.y = 0;
		if (controller->keyUp['s' - 'a'])
			rigidbody.lock()->acceleration.y = 0;


		if (controller->keyDown['a' - 'a'])
			rigidbody.lock()->acceleration.x = -speedX;
		if (controller->keyDown['d' - 'a'])
			rigidbody.lock()->acceleration.x = speedX;
		if (controller->keyDown['w' - 'a'])
			rigidbody.lock()->acceleration.y = -speedY;
		if (controller->keyDown['s' - 'a'])
			rigidbody.lock()->acceleration.y = speedY;

		
	}
};
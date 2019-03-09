#include "ObjectClass.h"

void OBJECT_implement::Start()
{
}

void OBJECT_implement::Update()
{
}

void OBJECT_implement::FixedUpdate()
{
}

void OBJECT_implement::OnCollision(OBJECT obj)
{
}

void OBJECT_implement::OnPhysics()
{
}

void OBJECT_implement::Destroy()
{
}

void COLLIDER_implement::CollisionCall(OBJECT another)
{
	parentObject->OnCollision(another);
}

void RIGIDBODY_implement::PhysicsCall()
{
	parentObject->OnPhysics();
}

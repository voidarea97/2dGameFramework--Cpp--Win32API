#include "ObjectClass.h"

//void GAMEOBJECT_implement::AddSprite(VECTOR2 _pos,int _width,int _height,string _Name)
//{
//	sprite = make_shared<SPRITE_implement>(_pos, _width, _height, _Name);
//	onPaintingObject.insert(this->ID);
//}
//
//void GAMEOBJECT_implement::AddCollider()
//{
//}
//
//void GAMEOBJECT_implement::AddRigidBody()
//{
//}
//
//void GAMEOBJECT_implement::DeleteSprite()
//{
//}
//
//void GAMEOBJECT_implement::DeleteCollider()
//{
//}
//
//void GAMEOBJECT_implement::DeleteRigidBody()
//{
//}

bool GAMEOBJECT_implement::AddComponent(COMPONENT component)
{
	if (component != nullptr)
	{
		if (components.find(component->name) != components.end())
		{
			return false;
		}

		if (component->name == "Sprite")
		{
			sprite = dynamic_pointer_cast<SPRITE_implement>(component);
			onPaintingObject.insert(this->ID);
		}
		else if (component->name == "Rigidbody")
		{
			rigidbody = dynamic_pointer_cast<RIGIDBODY_implement>(component);
			onMovementObject.insert(ID);
		}
		else if (component->name == "Collider")
		{
			collider = dynamic_pointer_cast<COLLIDER_implement>(component);
			onCollisionDetectionObject.insert(ID);
		}
		else
		{
			components[component->name] = (component);
			//component->gameObject = static_cast<GAMEOBJECT> (this);
		}
		component->gameObject =  (this);
		return true;
	}
	else
		return false;
}

void GAMEOBJECT_implement::Start()
{
	for (auto i = components.begin(); i != components.end(); i++)
	{
		i->second->Start();
	}
}

void GAMEOBJECT_implement::Update()
{
	for (auto i = components.begin(); i != components.end(); i++)
	{
		i->second->Update();
	}
}

void GAMEOBJECT_implement::FixedUpdate()
{
	for (auto i = components.begin(); i != components.end(); i++)
	{
		i->second->FixedUpdate();
	}
}

void GAMEOBJECT_implement::OnCollision(GAMEOBJECT obj)
{
}


void GAMEOBJECT_implement::Destroy()
{
}

void COLLIDER_implement::CollisionCall(GAMEOBJECT another)
{
	//parentObject->OnCollision(another);
}

void RIGIDBODY_implement::Move()
{
	//parentObject->OnPhysics();
	gameObject->transform->position.x += velocity.x * static_cast<float>(fixedDeltaTime) / 1000;
	gameObject->transform->position.y += velocity.y * static_cast<float>(fixedDeltaTime) / 1000;

	velocity.x += acceleration.x * static_cast<float>(fixedDeltaTime) / 1000;
	velocity.y += acceleration.y * static_cast<float>(fixedDeltaTime) / 1000;
}


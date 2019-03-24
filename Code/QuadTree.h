#pragma once
#include "GameGlobal.h"
#include "BaseClass.h"
#include <list>

class Quadtree {
private:
	int MAX_OBJECTS = 10;
	int MAX_LEVELS = 5;

	int level;        // �ӽڵ����
	list<pair<int,RECTANGLE>> objects;     // ��������
	RECTANGLE bounds; // ����߽�
	shared_ptr<Quadtree> nodes[4]; // �ĸ��ӽڵ�

public:
	Quadtree(int _level, RECTANGLE _bounds) :level(_level),bounds(_bounds)
	{
		//level = pLevel;
		//objects = new ArrayList();
		//bounds = pBounds;
		//nodes = new Quadtree[4];
	}

	void Clear() {
		objects.clear();

		for (int i = 0; i < 4; i++) {
			if (nodes[i] != nullptr) {
				nodes[i]->Clear();
				//delete nodes[i];
				nodes[i] = nullptr;
			}
		}
	}

	void Split() {
		int subWidth = (int)(bounds.width / 2);
		int subHeight = (int)(bounds.height / 2);
		int x = (int)bounds.x;
		int y = (int)bounds.y;

		nodes[0] = make_shared<Quadtree>(level + 1, RECTANGLE(x + subWidth, y, subWidth, subHeight));
		nodes[1] = make_shared<Quadtree>(level + 1, RECTANGLE(x, y, subWidth, subHeight));
		nodes[2] = make_shared<Quadtree>(level + 1, RECTANGLE(x, y + subHeight, subWidth, subHeight));
		nodes[3] = make_shared<Quadtree>(level + 1, RECTANGLE(x + subWidth, y + subHeight, subWidth, subHeight));
	}


	int getIndex(RECTANGLE pRect) {
		int index = -1;
		// ����
		double verticalMidpoint = bounds.x + (bounds.width / 2);
		double horizontalMidpoint = bounds.y + (bounds.height / 2);

		// ������ȫλ�����������ڵ���������
		bool topQuadrant = (pRect.y < horizontalMidpoint && pRect.y + pRect.height < horizontalMidpoint);
		// ������ȫλ�����������ڵ���������
		bool bottomQuadrant = (pRect.y > horizontalMidpoint);

		// ������ȫλ�����������ڵ���������
		if (pRect.x < verticalMidpoint && pRect.x + pRect.width < verticalMidpoint) {
			if (topQuadrant) {
				index = 1; // �������Ͻڵ� 
			}
			else if (bottomQuadrant) {
				index = 2; // �������½ڵ�
			}
		}
		// ������ȫλ�����������ڵ���������
		else if (pRect.x > verticalMidpoint) {
			if (topQuadrant) {
				index = 0; // �������Ͻڵ�
			}
			else if (bottomQuadrant) {
				index = 3; // �������½ڵ�
			}
		}

		return index;
	}


	void insert(int ID,RECTANGLE pRect) {

		// ���뵽�ӽڵ�
		if (nodes[0] != nullptr) {
			int index = getIndex(pRect);

			if (index != -1) {
				nodes[index]->insert(ID,pRect);

				return;
			}
		}

		// ��û���ѻ��߲��뵽�ӽڵ�ʧ�ܣ�ֻ���������ڵ���
		objects.emplace_back(ID,pRect);

		// �����������û�з��������
		if (objects.size() > MAX_OBJECTS && level < MAX_LEVELS) {
			if (nodes[0] == nullptr) {
				Split();
			}
			// ���Ѻ�Ҫ�����ڵ������ָ��ӽڵ���
			//int i = 0;
			//while (i < objects.size()) {
			//	int index = getIndex(objects[i].second);
			//	if (index != -1) {
			//		nodes[index]->insert(objects.remove(i));
			//		objects
			//	}
			//	else {
			//		i++;
			//	}
			//}
			for (auto i = objects.begin();i!=objects.end();i++)
			{
				int index = getIndex(i->second);
				if (index != -1)
				{
					nodes[index]->insert(i->first, i->second);
					i=objects.erase(i);
					continue;
				}
			}
		}
	}


	bool Retrieve(vector<int> & returnObjects, RECTANGLE pRect) 
	{
		int index = getIndex(pRect);
		if (index != -1 && nodes[0] != nullptr) {
			nodes[index]->Retrieve(returnObjects, pRect);
		}

		for (auto &i : objects)
		{
			returnObjects.push_back(i.first);
		}
		//returnObjects.addAll(objects);
		if (returnObjects.empty())
			return 0;
		return 1;
	}

};
//bool CollisionDetection(RECTANGLE a, RECTANGLE b)
//{
//
//	return 0;
//}
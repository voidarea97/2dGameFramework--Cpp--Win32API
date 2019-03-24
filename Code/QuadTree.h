#pragma once
#include "GameGlobal.h"
#include "BaseClass.h"
#include <list>

class Quadtree {
private:
	int MAX_OBJECTS = 10;
	int MAX_LEVELS = 5;

	int level;        // 子节点深度
	list<pair<int,RECTANGLE>> objects;     // 物体数组
	RECTANGLE bounds; // 区域边界
	shared_ptr<Quadtree> nodes[4]; // 四个子节点

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
		// 中线
		double verticalMidpoint = bounds.x + (bounds.width / 2);
		double horizontalMidpoint = bounds.y + (bounds.height / 2);

		// 物体完全位于上面两个节点所在区域
		bool topQuadrant = (pRect.y < horizontalMidpoint && pRect.y + pRect.height < horizontalMidpoint);
		// 物体完全位于下面两个节点所在区域
		bool bottomQuadrant = (pRect.y > horizontalMidpoint);

		// 物体完全位于左面两个节点所在区域
		if (pRect.x < verticalMidpoint && pRect.x + pRect.width < verticalMidpoint) {
			if (topQuadrant) {
				index = 1; // 处于左上节点 
			}
			else if (bottomQuadrant) {
				index = 2; // 处于左下节点
			}
		}
		// 物体完全位于右面两个节点所在区域
		else if (pRect.x > verticalMidpoint) {
			if (topQuadrant) {
				index = 0; // 处于右上节点
			}
			else if (bottomQuadrant) {
				index = 3; // 处于右下节点
			}
		}

		return index;
	}


	void insert(int ID,RECTANGLE pRect) {

		// 插入到子节点
		if (nodes[0] != nullptr) {
			int index = getIndex(pRect);

			if (index != -1) {
				nodes[index]->insert(ID,pRect);

				return;
			}
		}

		// 还没分裂或者插入到子节点失败，只好留给父节点了
		objects.emplace_back(ID,pRect);

		// 超容量后如果没有分裂则分裂
		if (objects.size() > MAX_OBJECTS && level < MAX_LEVELS) {
			if (nodes[0] == nullptr) {
				Split();
			}
			// 分裂后要将父节点的物体分给子节点们
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
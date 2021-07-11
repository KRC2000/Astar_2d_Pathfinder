#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;


struct Node
{
	enum State { IDLE, OPENED, CLOSED };

	Node* parent = nullptr;
	State state = IDLE;
	Vector2i pos;
	int g_cost = 0, h_cost = 0;
	Uint8 mapValue;
	bool solver = false;

	Node(Vector2i pos) :pos(pos){};

	// Distance to start node through completed path
	int recalc_g_cost(Node* parent)
	{
		if (state == IDLE)
		{
			if (parent->pos.x == pos.x || parent->pos.y == pos.y) g_cost = parent->g_cost + 10;
			if (parent->pos.x != pos.x && parent->pos.y != pos.y) g_cost = parent->g_cost + 14;
			this->parent = parent;
		}
		if (state == OPENED)
		{
			int g_costTemp = 0;
			if (parent->pos.x == pos.x || parent->pos.y == pos.y) g_costTemp = parent->g_cost + 10;
			if (parent->pos.x != pos.x && parent->pos.y != pos.y) g_costTemp = parent->g_cost + 14;

			if (g_costTemp < g_cost)
			{
				g_cost = g_costTemp;
				this->parent = parent;
			}
		}

		//text.setString("\n" + std::to_string(g_cost) + "\n" + std::to_string(h_cost));

		return g_cost;
	};

	// Distance to finish node
	int calc_h_cost(Vector2i finishNodePos)
	{
		h_cost = 0;
		Vector2i stepPos = pos;

		while (stepPos != finishNodePos)
		{
			Vector2i vec = finishNodePos - stepPos;

			Vector2i move{ 0,0 };
			if (vec.x != 0) move.x = vec.x / abs(vec.x);
			if (vec.y != 0) move.y = vec.y / abs(vec.y);
			stepPos += move;

			if (vec.x != 0 && vec.y != 0)
				h_cost += 14;

			if (vec.x == 0 || vec.y == 0)
				h_cost += 10;
		}

		//text.setString("\n" + std::to_string(g_cost) + "\n" + std::to_string(h_cost));

		return h_cost;
	}
};
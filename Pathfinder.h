#pragma once
#include <iostream>

using namespace sf;

namespace finder
{
	struct Node
	{
		enum State { IDLE, OPENED, CLOSED };

		Node* parent = nullptr;
		State state = IDLE;
		Vector2i pos;
		int g_cost = 0, h_cost = 0;
		Uint8 mapValue;
		bool solver = false;

		Node(Vector2i pos) :pos(pos) {};

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


	class Pathfinder
	{
		const std::vector<unsigned char>* map = nullptr;
		std::vector<Node> nodes;
		std::vector<Vector2i> pathVec;
		const Vector2u* mapSize = nullptr;
		Node* startNode = nullptr, * finishNode = nullptr;
	public:
		Pathfinder() {};

		/*
		Call this before computePath
		Map must be 2d array converted to vector where
		0 - empty space
		1 - obstacle
		2 - start position
		3 - finish position
		*/
		void setUp(const std::vector<Uint8>& map, const Vector2u& mapSize)
		{
			this->map = &map;
			this->mapSize = &mapSize;

			genNodes();

			startNode->calc_h_cost(finishNode->pos);

		};

		// Call only after setUp()
		// Returns vector of node positions that are a part of solving path, from start node to finish node including
		std::vector<Vector2i> computePath()
		{
			Clock timer;

			while (finishNode->state != Node::OPENED)
			{

				Node* targetNode = nullptr;

				// Find node with smallest f_cost
				int cheapestFPrice = INT_MAX;
				int opened = 0;
				for (auto& node : nodes)
				{
					if (node.state == Node::OPENED)
					{
						if (node.g_cost + node.h_cost < cheapestFPrice)
						{
							targetNode = &node;
							cheapestFPrice = node.g_cost + node.h_cost;
						}
						opened++;
					}
				}

				// Checking for other nodes with same smallest f_cost, picking one with smallest h_cost
				for (auto& node : nodes)
				{
					if (node.state == Node::OPENED)
					{
						if (node.g_cost + node.h_cost == cheapestFPrice)
						{
							if (node.h_cost < targetNode->h_cost) targetNode = &node;
						}
					}
				}

				targetNode->state = Node::CLOSED;
				//targetNode->shape.setFillColor(Color::Red);

				// Opening and calculating all surrounding targetNode nodes
				for (auto& node : nodes)
				{
					if (node.state != Node::CLOSED)
					{
						Vector2i vec = node.pos - targetNode->pos;

						if (abs(vec.x) < 2 && abs(vec.y) < 2)
						{
							node.recalc_g_cost(targetNode);
							node.calc_h_cost(finishNode->pos);
							node.state = Node::OPENED;
						}
					}
				}

				
			}

			std::cout << "Path calculated\n";

			Node* node = finishNode;
			pathVec.clear();
			while (node->parent != nullptr)
			{
				node->solver = true;
				pathVec.push_back(node->pos);
				node = node->parent;
			}
			pathVec.push_back(startNode->pos);
			std::reverse(pathVec.begin(), pathVec.end());


			std::cout << "Calculated in " << timer.getElapsedTime().asMicroseconds() << " microseconds\n";

			return pathVec;
			
		};

		// Call this in main loop between window.clear() and window.display()
		void debugDraw(RenderTarget& target, int cellSize)
		{
			RectangleShape s{ {(float)cellSize, (float)cellSize } };



			for (auto& node : nodes)
			{
				s.setPosition({ (float)node.pos.x * cellSize, (float)node.pos.y * cellSize });
				if (node.mapValue == 0) s.setFillColor(Color::White);
				if (node.solver) s.setFillColor(Color::Yellow);

				if (node.mapValue == 2) s.setFillColor(Color::Blue);
				if (node.mapValue == 3) s.setFillColor(Color::Magenta);
				Color c = s.getFillColor(); c.a = 150;
				s.setFillColor(c);
				target.draw(s);
			}
		}

	private:

		// Fill map with nodes, get start and finish node pointers
		void genNodes()
		{
			nodes.clear();
			for (int i = 0; i < mapSize->y; i++)
			{
				for (int j = 0; j < mapSize->x; j++)
				{
					Node node({ j, i });
					if (map->at(i * mapSize->x + j) != 1)
					{
						node.mapValue = map->at(i * mapSize->x + j);
						nodes.push_back(node);
					}
				}
			}

			for (int i = 0; i < mapSize->y; i++)
			{
				for (int j = 0; j < mapSize->x; j++)
				{
					if (map->at(i * mapSize->x + j) == 2)
					{
						for (auto& node : nodes)
						{
							if (node.pos.x == j && node.pos.y == i)
							{
								node.state = Node::OPENED;
								startNode = &node;
							}
						}
					}
					if (map->at(i * mapSize->x + j) == 3)
					{
						for (auto& node : nodes)
						{
							if (node.pos.x == j && node.pos.y == i)
							{
								finishNode = &node;
							}
						}
					}
				}
			}


			std::cout << "Start node position: x:" << startNode->pos.x << " y:" << startNode->pos.y << std::endl;
			std::cout << "Finish node position: x:" << finishNode->pos.x << " y:" << finishNode->pos.y << std::endl;
		};

		
	};

};

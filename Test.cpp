#include <SFML/Graphics.hpp>
#include <iostream>

#include "Pathfinder.h"

using namespace sf;
using namespace std;

int main()
{
	RenderWindow window(VideoMode(800, 600), "SFML works!");

	// Create pathfinder object
	finder::Pathfinder p;

	// Data for pathfinder to use: vector of sf::Uint8(alias for unsigned char) and Vector2i describing map size
	vector<Uint8> map;
	Vector2u mSize;// = { 10, 10 };

	/*
		Map must contain these symbols

		0 - empty space
		1 - obstacle
		2 - start position
		3 - finish position
	*/


	map =
	{
		3, 0, 1, 0, 1, 0, 0, 0, 0, 0,
		0, 0, 1, 0, 1, 0, 0, 0, 0, 0,
		0, 0, 1, 0, 1, 0, 0, 0, 0, 0,
		0, 0, 1, 0, 1, 0, 0, 0, 0, 0,
		0, 0, 1, 0, 1, 0, 0, 0, 0, 0,
		0, 0, 1, 0, 0, 0, 1, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 1, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 1, 0, 0, 2
	};
	mSize = {10, 10};

	// Pass data to pathfinder object. Next calls of computePath() will find path for last passed map here in setUp() method
	p.setUp(map, mSize);

	// Computing path, saving path node coordinates in vector
	vector<Vector2i> vec = p.computePath();

	// Print result
	for (auto& v : vec)
		cout << v.x << "; " << v.y << endl;




	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
		}

		window.clear();

		// Call this to visualize results
		p.debugDraw(window, 30);

		window.display();
	}

	return 0;
}


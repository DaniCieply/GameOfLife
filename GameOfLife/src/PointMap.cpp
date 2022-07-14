#include "PointMap.h"

PointMap::PointMap(int size) 
{
	vertices.setPrimitiveType(sf::Points);
	vertices.resize(size);

	this->size = size;
	width = sqrt(this->size);

	// Sets all 2D coordinates of points
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < width; y++)
		{
			vertices[x * width + y].position = sf::Vector2f(x, y);
		}
	}
}

void PointMap::draw(sf::RenderTarget& target, sf::RenderStates states) const 
{
	target.draw(vertices, states);
}

void PointMap::setPointColor(int x, int y, sf::Color color) 
{
	vertices[x * width + y].color = color;
}

void PointMap::setPointColor(int index, sf::Color color)
{
	vertices[index].color = color;
}

void PointMap::setAllPointColor(sf::Color color)
{
	for (int i = 0; i < size; i++)
	{
		vertices[i].color = color;
	}
}

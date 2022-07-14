#pragma once
#include <SFML/Graphics.hpp>

class PointMap : public sf::Drawable, public sf::Transformable
{
public: 
	PointMap(int size);

	void setPointColor(int x, int y, sf::Color color);
	void setPointColor(int index, sf::Color color);
	void setAllPointColor(sf::Color color);
private:
	sf::VertexArray vertices;
	int width;
	int size;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};


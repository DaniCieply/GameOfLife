#include <SFML/Graphics.hpp>
#include <random>
#include <array>
#include <vector>
#include <iostream>
#include <memory>
#include "PointMap.h"

using std::array;
using std::vector;

const int GRID_WIDTH = 250;
const int GRID_HEIGHT = 250;

int genRandInt(int start, int end)
{
    std::random_device rd;
    std::default_random_engine ran(rd());
    return std::uniform_int_distribution<>{start, end}(ran);
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(GRID_WIDTH, GRID_HEIGHT), "Conway's Game of Life");

    sf::Clock clock;

    // Visual of game that is drawn to screen
    PointMap pointMap(GRID_WIDTH * GRID_HEIGHT);
    pointMap.setAllPointColor(sf::Color::Transparent);

    // Arrays of cell states
    array<std::unique_ptr<bool>, GRID_WIDTH * GRID_HEIGHT> frontArray;
    array<std::unique_ptr<bool>, GRID_WIDTH * GRID_HEIGHT> backArray;
    vector<bool> neighborValues;

    // Gives each cell a random starting value in the front array
    for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT; i++) 
    {
        int randNum = genRandInt(1, 13);
        if (randNum <= 3)
        {
            *frontArray[i] = true;
        }
        else 
        {
            *frontArray[i] = false;
        }
    }

    // Sets all back array cells false by default
    for (int i = 0; i < backArray.size(); i++)
    {
        *backArray[i] = false;
    }

    while (window.isOpen())
    {
        float Framerate = 1.0f / clock.getElapsedTime().asSeconds();
        clock.restart();

        // Event Handling
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Game logic here
        // We analyze the front array, then apply cell life changes to the back array
        // Once all cells are iterated through, set the front array equal to the back array

        for (int x = 0; x < GRID_WIDTH; x++)
        {
            for (int y = 0; y < GRID_HEIGHT; y++)
            {
                // Resets vars
                vector<bool> neighborValues;
                neighborValues.clear();
                int numAlive = 0;
                int numDead = 0;

                // Read the values of the front array and change the values of the back array
                bool& frontCellVal = *frontArray[x * GRID_WIDTH + y];
                bool& backCellVal = *backArray[x * GRID_WIDTH + y];
                if (x - 1 >= 0 && y + 1 <= GRID_HEIGHT - 1)
                    neighborValues.push_back(*frontArray[(x - 1) * GRID_WIDTH + (y + 1)]);
                if (y + 1 <= GRID_HEIGHT - 1)
                    neighborValues.push_back(*frontArray[x * GRID_WIDTH + (y + 1)]);
                if (x + 1 <= GRID_WIDTH - 1 && y + 1 <= GRID_HEIGHT - 1)
                    neighborValues.push_back(*frontArray[(x + 1) * GRID_WIDTH + (y + 1)]);
                if (x + 1 <= GRID_WIDTH - 1)
                    neighborValues.push_back(*frontArray[(x + 1) * GRID_WIDTH + y]);
                if (x + 1 <= GRID_WIDTH - 1 && y - 1 >= 0)
                    neighborValues.push_back(*frontArray[(x + 1) * GRID_WIDTH + (y - 1)]);
                if (y - 1 >= 0)
                    neighborValues.push_back(*frontArray[x * GRID_WIDTH + (y - 1)]);
                if (x - 1 >= 0 && y - 1 >= 0)
                    neighborValues.push_back(*frontArray[(x - 1) * GRID_WIDTH + (y - 1)]);
                if (x - 1 >= 0)
                    neighborValues.push_back(*frontArray[(x - 1) * GRID_WIDTH + y]);

                // Records how many neighbors of the cell are dead or alive
                for (bool b : neighborValues)
                {
                    if (b)
                    {
                        numAlive++;
                    }
                    else
                    {
                        numDead++;
                    }
                }

                // Determines whether the cell being analyzed should be dead or alive
                if (frontCellVal)
                {
                    if (numAlive < 2)
                    {
                        backCellVal = false;
                    }
                    else if (numAlive == 2 || numAlive == 3)
                    {
                        backCellVal = true;
                    }
                    else if (numAlive > 3)
                    {
                        backCellVal = false;
                    }
                }
                else
                {
                    if (numAlive == 3)
                    {
                        backCellVal = true;
                    }
                }
            }
        }

        for (int i = 0; i < frontArray.size(); i++)
        {
            *frontArray[i] = *backArray[i];
        }

        // Changes colors of point map based upon front array
        for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT; i++) 
        {
            if (frontArray[i])
            {
                pointMap.setPointColor(i, sf::Color::White);
            }
            else
            {
                pointMap.setPointColor(i, sf::Color::Transparent);
            }
        }

        // Draw
        window.clear();
        window.draw(pointMap);
        window.display();

        std::cout << Framerate << std::endl;
    }

    return 0;
}

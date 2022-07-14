#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <random>
#include <array>
#include <iostream>
#include <memory>
#include "PointMap.h"

using std::array;
using std::vector;

const int GRID_WIDTH = 500;
const int GRID_HEIGHT = 500;

int genRandInt(int start, int end)
{
    std::random_device rd;
    std::default_random_engine ran(rd());
    return std::uniform_int_distribution<>{start, end}(ran);
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(GRID_WIDTH * 2, GRID_HEIGHT), "Conway's Game of Life", sf::Style::Titlebar | sf::Style::Close);

    sf::Clock frameClock; // Used for calculating Framerate
    sf::Clock deltaClock; // ImGui clock
    sf::Clock gameClock; // Game-speed calculations

    ImGui::SFML::Init(window);

    window.setVerticalSyncEnabled(true);

    // Vars regarding whether sim should continue running or not
    bool shouldRun = false;
    bool enoughTimePass = false;
    float gameSpeed = 1.0f;

    // Visual of game that is drawn to screen
    PointMap pointMap(GRID_WIDTH * GRID_HEIGHT);
    pointMap.setAllPointColor(sf::Color::Transparent);

    // Arrays of cell states
    array<bool, GRID_WIDTH * GRID_HEIGHT> frontArray;
    array<bool, GRID_WIDTH * GRID_HEIGHT> backArray;

    // Gives each cell a random starting value in the front array
    for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT; i++) 
    {
        int randNum = genRandInt(1, 13);
        if (randNum <= 3)
        {
            frontArray[i] = true;
        }
        else 
        {
            frontArray[i] = false;
        }
    }

    // Sets all back array cells false by default
    for (int i = 0; i < backArray.size(); i++)
    {
        backArray[i] = false;
    }

    // Main Loop
    while (window.isOpen())
    {
        // Event Handling
        sf::Event event;
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // FPS calculation
        float Framerate = 1.0f / frameClock.getElapsedTime().asSeconds();
        frameClock.restart();

        // Limits speed of the simulation
        if (gameClock.getElapsedTime().asMilliseconds() >= gameSpeed * 1000)
        {
            enoughTimePass = true;
            gameClock.restart();
        }
        else
        {
            enoughTimePass = false;
        }

        if (shouldRun && enoughTimePass)
        {
            // Determines the which value the cell should change to
            for (int x = 0; x < GRID_WIDTH; x++)
            {
                for (int y = 0; y < GRID_HEIGHT; y++)
                {
                    int numAlive = 0;
                    int numDead = 0;

                    // Read the values of the front array and change the values of the back array
                    bool& frontCellVal = frontArray[x * GRID_WIDTH + y];
                    bool& backCellVal = backArray[x * GRID_WIDTH + y];
                    if (x - 1 >= 0 && y + 1 <= GRID_HEIGHT - 1) 
                    {
                        if (frontArray[(x - 1) * GRID_WIDTH + (y + 1)]) numAlive++;
                        else numDead++;
                    }
                    if (y + 1 <= GRID_HEIGHT - 1) 
                    {
                        if (frontArray[x * GRID_WIDTH + (y + 1)]) numAlive++;
                        else numDead++;
                    }
                    if (x + 1 <= GRID_WIDTH - 1 && y + 1 <= GRID_HEIGHT - 1)
                    {
                        if (frontArray[(x + 1) * GRID_WIDTH + (y + 1)]) numAlive++;
                        else numDead++;
                    }
                    if (x + 1 <= GRID_WIDTH - 1) 
                    {
                        if (frontArray[(x + 1) * GRID_WIDTH + y]) numAlive++;
                        else numDead++;
                    }
                    if (x + 1 <= GRID_WIDTH - 1 && y - 1 >= 0)
                    {
                        if (frontArray[(x + 1) * GRID_WIDTH + (y - 1)]) numAlive++;
                        else numDead++;
                    }
                    if (y - 1 >= 0)
                    {
                        if (frontArray[x * GRID_WIDTH + (y - 1)]) numAlive++;
                        else numDead++;
                    }
                    if (x - 1 >= 0 && y - 1 >= 0) 
                    {
                        if (frontArray[(x - 1) * GRID_WIDTH + (y - 1)]) numAlive++;
                        else numDead++;
                    }
                    if (x - 1 >= 0)
                    {
                        if (frontArray[(x - 1) * GRID_WIDTH + y]) numAlive++;
                        else numDead++;
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

            frontArray = backArray;
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

        ImGui::SFML::Update(window, deltaClock.restart());

        // Sim Options UI
        ImGui::Begin("Options");
        ImGui::SetWindowPos(ImVec2(GRID_WIDTH + 10, 10));
        ImGui::SetWindowSize(ImVec2(GRID_WIDTH / 1.1f, GRID_HEIGHT / 1.1f));
        std::string fpsString = "FPS: " + std::to_string(Framerate);
        ImGui::Text(fpsString.c_str());

        ImGui::SliderFloat("Sim Speed", &gameSpeed, 0.0f, 10.0f);
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        {
            ImGui::SetTooltip("How many seconds between game updates");
        }

        ImGui::Checkbox("Running", &shouldRun);
        // Exit Button
        bool winShouldClose = ImGui::Button("Exit");
        ImGui::End();

        if (winShouldClose)
        {
            window.close();
        }

        // Draw
        window.clear();
        window.draw(pointMap);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}

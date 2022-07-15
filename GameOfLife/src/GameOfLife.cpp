#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <ImGuiFileDialog.h>
#include <random>
#include <array>
#include <fstream>
#include "PointMap.h"

using std::array;
using std::vector;
using std::fstream;

const int GRID_WIDTH = 500;
const int GRID_HEIGHT = 500;

int genRandInt(int start, int end)
{
    std::random_device rd;
    std::default_random_engine ran(rd());
    return std::uniform_int_distribution<>{start, end}(ran);
}

void saveGrid(array<bool, GRID_WIDTH* GRID_HEIGHT>& sArray, std::string filePath)
{
    fstream file;
    file.open(filePath, std::ios::out);
    if (file.is_open())
    {
        for (bool b : sArray)
        {
            file << b;
        }
    }

    file.close();
}

void loadGrid(array<bool, GRID_WIDTH* GRID_HEIGHT>& lArray, std::string filePath)
{
    fstream file;
    file.open(filePath, std::ios::in);
    if (file.is_open())
    {
        std::string line;
        std::getline(file, line);

        int i = 0;
        for (char c : line)
        {
            if (c == '0')
            {
                lArray[i] = false;
            }
            else if (c == '1')
            {
                lArray[i] = true;
            }

            i++;
        }
    }

    file.close();
}


void randomizeArray(array<bool, GRID_WIDTH * GRID_HEIGHT>& array, int aliveChance)
{
    for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT; i++)
    {
        int randNum = genRandInt(1, 10);
        if (randNum <= aliveChance)
        {
            array[i] = true;
        }
        else
        {
            array[i] = false;
        }
    }
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

    // Generation vars
    int aliveChance = 5;
    float liveCellColor[3] = { (float)255 / 255, (float)255 / 255, (float)255 / 255 };
    float deadCellColor[3] = { (float)0 / 255, (float)0 / 255, (float)0 / 255 };

    // Visual of game that is drawn to screen
    PointMap pointMap(GRID_WIDTH * GRID_HEIGHT);
    pointMap.setAllPointColor(sf::Color::Transparent);

    // Arrays of cell states
    array<bool, GRID_WIDTH * GRID_HEIGHT> frontArray;
    array<bool, GRID_WIDTH * GRID_HEIGHT> backArray;

    randomizeArray(frontArray, aliveChance);

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
        // Main Options Window
        ImGui::Begin("Main Options");
        ImGui::SetWindowPos(ImVec2(GRID_WIDTH + 10, 10));
        ImGui::SetWindowSize(ImVec2(GRID_WIDTH / 2.25f, GRID_HEIGHT / 2.5f));
        std::string fpsString = "FPS: " + std::to_string(Framerate);
        ImGui::Text(fpsString.c_str());

        ImGui::SliderFloat("Speed", &gameSpeed, 0.0f, 10.0f);
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        {
            ImGui::SetTooltip("How many seconds between game updates");
        }

        ImGui::Checkbox("Running", &shouldRun);

        // Save current grid
        if (ImGui::Button("Save Grid"))
            ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Select file", ".clg", ".");
        if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
        {
            std::string filePath;
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                filePath = ImGuiFileDialog::Instance()->GetFilePathName();
            }

            saveGrid(frontArray, filePath);
            ImGuiFileDialog::Instance()->Close();
        }
        
        // Load saved grid
        if (ImGui::Button("Load Grid"))
            ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey2", "Select file", ".clg", ".");
        if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey2"))
        {
            std::string filePath;
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                filePath = ImGuiFileDialog::Instance()->GetFilePathName();
            }

            loadGrid(frontArray, filePath);
            ImGuiFileDialog::Instance()->Close();
        }

        // Exit Button
        if (ImGui::Button("Exit"))
        {
            window.close();
        }
        ImGui::End();

        // Generation Options Window
        ImGui::Begin("Generation Options");
        ImGui::SetWindowPos(ImVec2(GRID_WIDTH + 10, 215));
        ImGui::SetWindowSize(ImVec2(GRID_WIDTH / 1.1f, GRID_HEIGHT / 2.5f));
        ImGui::SliderInt("Alive Chance", &aliveChance, 1, 10);

        // Color Options
        ImGui::ColorEdit3("Live Cell Color", liveCellColor);
        ImGui::ColorEdit3("Dead Cell Color", deadCellColor);
        for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT; i++)
        {
            if (frontArray[i])
                pointMap.setPointColor(i, sf::Color((int)(liveCellColor[0] * 255), (int)(liveCellColor[1] * 255), (int)(liveCellColor[2] * 255)));
            else
                pointMap.setPointColor(i, sf::Color((int)(deadCellColor[0] * 255), (int)(deadCellColor[1] * 255), (int)(deadCellColor[2] * 255)));
        }

        // Randomizes the sim on click
        if (ImGui::Button("Randomize"))
        {
            randomizeArray(frontArray, aliveChance);
        }
        
        ImGui::End();
        
        // Draw
        window.clear();
        window.draw(pointMap);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}

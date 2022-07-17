#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <ImGuiFileDialog.h>
#include <random>
#include <fstream>
#include "PointMap.h"

using std::vector;
using std::fstream;

const int GRID_WIDTH = 500;
const int GRID_HEIGHT = 500;
const int GRID_SIZE = GRID_WIDTH * GRID_HEIGHT;

int genRandInt(int start, int end)
{
    std::random_device rd;
    std::default_random_engine ran(rd());
    return std::uniform_int_distribution<>{start, end}(ran);
}

void saveGrid(vector<bool>& sVect, std::string filePath)
{
    fstream file;
    file.open(filePath, std::ios::out);
    if (file.is_open())
    {
        for (bool b : sVect)
        {
            file << b;
        }
    }

    file.close();
}

void loadGrid(vector<bool>& lVect, std::string filePath)
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
                lVect[i] = false;
            }
            else if (c == '1')
            {
                lVect[i] = true;
            }

            i++;
        }
    }

    file.close();
}


void randomizeVector(vector<bool>& vect, int aliveChance)
{
    for (int i = 0; i < GRID_SIZE; i++)
    {
        int randNum = genRandInt(1, 10);
        if (randNum <= aliveChance)
        {
            vect[i] = true;
        }
        else
        {
            vect[i] = false;
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
    PointMap pointMap(GRID_SIZE);
    pointMap.setAllPointColor(sf::Color::Transparent);

    // Arrays of cell states
    vector<bool> frontVect(GRID_SIZE);
    vector<bool> backVect(GRID_SIZE);

    randomizeVector(frontVect, aliveChance);

    // Sets all back array cells false by default
    for (int i = 0; i < backVect.size(); i++)
    {
        backVect[i] = false;
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
                    bool frontCellVal = frontVect[x * GRID_WIDTH + y];
                    bool backCellVal = backVect[x * GRID_WIDTH + y];
                    if (x - 1 >= 0 && y + 1 <= GRID_HEIGHT - 1) 
                    {
                        if (frontVect[(x - 1) * GRID_WIDTH + (y + 1)]) numAlive++;
                        else numDead++;
                    }
                    if (y + 1 <= GRID_HEIGHT - 1) 
                    {
                        if (frontVect[x * GRID_WIDTH + (y + 1)]) numAlive++;
                        else numDead++;
                    }
                    if (x + 1 <= GRID_WIDTH - 1 && y + 1 <= GRID_HEIGHT - 1)
                    {
                        if (frontVect[(x + 1) * GRID_WIDTH + (y + 1)]) numAlive++;
                        else numDead++;
                    }
                    if (x + 1 <= GRID_WIDTH - 1) 
                    {
                        if (frontVect[(x + 1) * GRID_WIDTH + y]) numAlive++;
                        else numDead++;
                    }
                    if (x + 1 <= GRID_WIDTH - 1 && y - 1 >= 0)
                    {
                        if (frontVect[(x + 1) * GRID_WIDTH + (y - 1)]) numAlive++;
                        else numDead++;
                    }
                    if (y - 1 >= 0)
                    {
                        if (frontVect[x * GRID_WIDTH + (y - 1)]) numAlive++;
                        else numDead++;
                    }
                    if (x - 1 >= 0 && y - 1 >= 0) 
                    {
                        if (frontVect[(x - 1) * GRID_WIDTH + (y - 1)]) numAlive++;
                        else numDead++;
                    }
                    if (x - 1 >= 0)
                    {
                        if (frontVect[(x - 1) * GRID_WIDTH + y]) numAlive++;
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

                    // Copies updated cells back to vectors because for some dumb reason, i can't use vector values as a reference
                    frontVect[x * GRID_WIDTH + y] = frontCellVal;
                    backVect[x * GRID_WIDTH + y] = backCellVal;
                }
            }

            frontVect = backVect;
        }
        
        // Changes colors of point map based upon front array
        for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT; i++) 
        {
            if (frontVect[i])
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

            saveGrid(frontVect, filePath);
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

            loadGrid(frontVect, filePath);
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
            if (frontVect[i])
                pointMap.setPointColor(i, sf::Color((int)(liveCellColor[0] * 255), (int)(liveCellColor[1] * 255), (int)(liveCellColor[2] * 255)));
            else
                pointMap.setPointColor(i, sf::Color((int)(deadCellColor[0] * 255), (int)(deadCellColor[1] * 255), (int)(deadCellColor[2] * 255)));
        }

        // Randomizes the sim on click
        if (ImGui::Button("Randomize"))
        {
            randomizeVector(frontVect, aliveChance);
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

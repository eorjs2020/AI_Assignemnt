#include "States.h"
#include "StateManager.h" // Make sure this is NOT in "States.h" or circular reference.
#include "Engine.h"
#include <iostream>
#include <fstream>
#include "TextureManager.h"
#include "glm.hpp"
#include "DebugManager.h"
#include "EventManager.h"

// Begin State. CTRL+M+H and CTRL+M+U to turn on/off collapsed code.
void State::Render()
{
	SDL_RenderPresent(Engine::Instance().GetRenderer());
}
void State::Resume() {}
// End State.

PlayState::PlayState()
{
}

void PlayState::Enter()
{
	std::ifstream inFile("Dat/Tiledata.txt");
	if (inFile.is_open())
	{ // Create map of Tile prototypes.
		char key;
		int x, y;
		bool o, h;
		while (!inFile.eof())
		{
			inFile >> key >> x >> y >> o >> h;
			m_tiles.emplace(key, new Tile({ x * 16, y * 16, 16, 16 }, { 0,0,32,32 }, 
				Engine::Instance().GetRenderer(), TEMA::GetTexture("Tile"), o, h));
		}
	}
	inFile.close();
	inFile.open("Dat/Level1.txt");
	if (inFile.is_open())
	{ // Build the level from Tile prototypes.
		char key;
		for (int row = 0; row < ROWS; row++)
		{
			for (int col = 0; col < COLS; col++)
			{
				
				inFile >> key;
				Engine::Instance().GetLevel()[row][col] = m_tiles[key]->Clone(); // Prototype design pattern used.
				Engine::Instance().GetLevel()[row][col]->GetDstP()->x = (float)(32 * col);
				Engine::Instance().GetLevel()[row][col]->GetDstP()->y = (float)(32 * row);
				PathNode* temp = new PathNode(Engine::Instance().GetLevel()[row][col]->GetDstP()->x + 16, Engine::Instance().GetLevel()[row][col]->GetDstP()->y + 16,
					Engine::Instance().GetLevel()[row][col]->GetDstP()->w, Engine::Instance().GetLevel()[row][col]->GetDstP()->h);
				m_pGrid.push_back(temp);
				
				
			}
		}
	}
	inFile.close();
	// Final engine initialization calls.
	m_pPlayer = new Player({ 0,47,15,20 }, { 60.0f,200.0f,30.0f,40.0f },
		Engine::Instance().GetRenderer(), TEMA::GetTexture("Tile"), 0, 0, 3, 4);
	m_Enemy = new Enemy({ 0,88,14,21 }, { 400.0f,200.0f,30.0f,40.0f },
		Engine::Instance().GetRenderer(), TEMA::GetTexture("Tile"), 0, 0, 3, 4);
	std::cout << "Number of Nodes: " << m_pGrid.size() << std::endl;
}
void PlayState::RenderGrid()
{
	
	for (int row = 0; row < ROWS; ++row)
	{
		for (int col = 0; col < COLS; ++col)
		{
			auto colour = (!m_pGrid[row * COLS + col]->getLOS()) ? glm::vec4(1.0f, 0.0f, 0.0f, 1.0f) : glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);

			DEMA::DrawRect(m_pGrid[row * COLS + col]->GetPos() - glm::vec2(m_pGrid[row * COLS + col]->GetWidth() * 0.5f, 
				m_pGrid[row * COLS + col]->GetHeight() * 0.5f),
				32, 32);
			DEMA::DrawRect(m_pGrid[row * COLS + col]->GetPos(),
				5, 5, colour);
		
		}
	}
	DEMA::DrawRect(glm::vec2(m_pPlayer->GetDstP()->x, m_pPlayer->GetDstP()->y),
		m_pPlayer->GetDstP()->w, m_pPlayer->GetDstP()->h);
}
void PlayState::RenderLOS()
{

}
void PlayState::SetLOS()
{
}
void PlayState::Update()
{
	if (EVMA::KeyPressed(SDL_SCANCODE_H)) {
		std::cout << "Debug mode\n";
		m_Debugmode = !m_Debugmode;
	}
	if (m_Debugmode) {
		if (EVMA::KeyPressed(SDL_SCANCODE_K)) {
			std::cout << "Damage to Enemy\n";
		}
		if (EVMA::KeyPressed(SDL_SCANCODE_P)) {
			std::cout << "Patrol mode\n";
			m_PatrolMode = !m_PatrolMode;
		}
	}

	m_pPlayer->Update();
	m_Enemy->Update();
}

void PlayState::Render()
{
	
	SDL_RenderClear(Engine::Instance().GetRenderer()); // Clear the screen with the draw color.
	// Draw anew.
	for (int row = 0; row < ROWS; row++)
	{
		for (int col = 0; col < COLS; col++)
		{
			Engine::Instance().GetLevel()[row][col]->Render();
		}
	}
	m_pPlayer->Render();
	m_Enemy->Render();

	RenderGrid();
	
}

void PlayState::Exit()
{
	for (auto const& i : m_tiles)
		delete m_tiles[i.first];
	m_tiles.clear();
}

void PlayState::Resume()
{
}

StartState::StartState() {}

void StartState::Update()
{
	if (m_StartBtn->Update() == 1)
		return;
}

void StartState::Render()
{
	
	m_StartBtn->Render();
	State::Render();
}

void StartState::Enter()
{
	
	m_StartBtn = new PlayButton({ 0, 0, 67, 15 }, { 380.0f, 350.0f, 268, 60},
		Engine::Instance().GetRenderer(), TEMA::GetTexture("Button"));
}

void StartState::Exit()
{
}

void StartState::Resume()
{
}

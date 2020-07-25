#include "States.h"
#include "StateManager.h" // Make sure this is NOT in "States.h" or circular reference.
#include "Engine.h"
#include <iostream>
#include <fstream>
#include "TextureManager.h"

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
			m_tiles.emplace(key, new Tile({ x * 32, y * 32, 32, 32 }, { 0,0,32,32 }, 
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
			}
		}
	}
	inFile.close();
	// Final engine initialization calls.
	m_pPlayer = new Player({ 0,0,19,26 }, { 60.0f,200.0f,46.0f,64.0f },
		Engine::Instance().GetRenderer(), TEMA::GetTexture("Player"), 0, 0, 4, 4);

}
void PlayState::Update()
{
	m_pPlayer->Update();
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

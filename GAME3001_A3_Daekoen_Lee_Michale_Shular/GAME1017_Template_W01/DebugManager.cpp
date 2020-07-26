#include "DebugManager.h"
#include "MathManager.h"
#include "Engine.h"

void DebugManager::DrawLine(const SDL_Point start, const SDL_Point end, SDL_Color col)
{
	SDL_SetRenderDrawColor(Engine::Instance().GetRenderer(), col.r, col.g, col.b, col.a);
	SDL_RenderDrawLine(Engine::Instance().GetRenderer(), end.x, end.y, start.x, start.y);
}

void DebugManager::QueueLine(const SDL_Point start, const SDL_Point end, const SDL_Color col)
{
	s_points.push_back(start);
	s_points.push_back(end);
	s_colors.push_back(col);
}


void DebugManager::FlushLines()
{
	for (unsigned i = 0, j = 0; i < s_points.size(); i+=2, j++)
		DrawLine(s_points[i], s_points[i+1], s_colors[j]);
	Quit(); // Clears out vectors.
}

void DebugManager::DrawRect(glm::vec2 position, int width, int height, glm::vec4 colour)
{
	int r = floor(colour.r * 255.0f);
	int g = floor(colour.g * 255.0f);
	int b = floor(colour.b * 255.0f);
	int a = floor(colour.a * 255.0f);

	SDL_Rect rectangle;
	rectangle.x = position.x;
	rectangle.y = position.y;
	rectangle.w = width;
	rectangle.h = height;

	const auto renderer = /* TheGame::Instance()->getRenderer()*/ Engine::Instance().GetRenderer();

	SDL_SetRenderDrawColor(renderer, r, g, b, a);
	SDL_RenderDrawRect(renderer, &rectangle);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
}

void DebugManager::DrawRay(const SDL_Point start, const double angle, const double length, const SDL_Color col)
{ // Raw angle in radians required.
	double dX, dY;
	MAMA::SetDeltas(angle, dX, dY, length, length);
	//std::cout << start.x << ',' << start.y << '|' << start.x+dX << ',' << start.y+dY << std::endl;
	DrawLine({(int)start.x, (int)start.y}, { (int)(start.x + dX), (int)(start.y + dY) }, col);
}



void DebugManager::Quit()
{
	s_points.clear();
	s_colors.clear();
	s_points.shrink_to_fit();
	s_colors.shrink_to_fit();
}

int DebugManager::s_debugMode = 0;
std::vector<SDL_Point> DebugManager::s_points;
std::vector<SDL_Color> DebugManager::s_colors;
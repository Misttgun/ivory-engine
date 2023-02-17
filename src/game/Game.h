#pragma once

#include <SDL2/SDL.h>

#include "../ecs/ECS.h"

class Game
{
private:
	SDL_Window* m_window;
	SDL_Renderer* m_renderer;

	std::unique_ptr<Registry> m_registry;

	bool m_isRunning;

	int m_windowWidth;
	int m_windowHeight;

	int m_previousFrameTime;

	void Setup();
	void ProcessInputs();
	void Update();
	void Render();

public:
	Game();
	~Game() = default;

	void Init();
	void Run();
	void Destroy();
};


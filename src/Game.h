#pragma once

#include <SDL2/SDL.h>

class Game
{
private:
	SDL_Window* m_window;
	SDL_Renderer* m_renderer;

	bool m_isRunning;

	int m_windowWidth;
	int m_windowHeight;

	void Setup();
	void ProcessInputs();
	void Update();
	void Render();

public:
	Game();
	~Game();

	void Init();
	void Run();
	void Destroy();
};


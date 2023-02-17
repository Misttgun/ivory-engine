#include <iostream>
#include <SDL2/SDL_image.h>

#include "Game.h"

#include "../components/RigidBodyComponent.h"
#include "../components/TransformComponent.h"
#include "../helpers/Constants.h"
#include "../helpers/Logger.h"
#include "../systems/MovementSystem.h"
#include "../systems/RenderSystem.h"

Game::Game() : m_window{}, m_renderer{}, m_isRunning(false), m_windowWidth(0), m_windowHeight(0), m_previousFrameTime(0)
{
	m_registry = std::make_unique<Registry>();
}

void Game::Setup()
{
	m_registry->AddSystem<MovementSystem>();
	m_registry->AddSystem<RenderSystem>();

	Entity tank = m_registry->CreateEntity();
	tank.AddComponent<TransformComponent>(glm::vec2(10.0f, 30.0f), glm::vec2(1.0f, 1.0f), 0.0f);
	tank.AddComponent<RigidBodyComponent>(glm::vec2(50.0f, 0.0f));
	tank.AddComponent<SpriteComponent>(10, 10);
}

void Game::ProcessInputs()
{
	SDL_Event sdlEvent{};
	while(SDL_PollEvent(&sdlEvent))
	{
		switch(sdlEvent.type)
		{
			case SDL_QUIT:
				m_isRunning = false;
				break;
			case SDL_KEYDOWN:
				if(sdlEvent.key.keysym.sym == SDLK_ESCAPE)
					m_isRunning = false;
				break;
			default:
				break;
		}
	}
}

void Game::Update()
{
	const int currentFrameTime = static_cast<int>(SDL_GetTicks());
	const int timeToWait = TIME_PER_FRAME - (currentFrameTime - m_previousFrameTime);

	if(timeToWait > 0 && timeToWait <= TIME_PER_FRAME)
		SDL_Delay(timeToWait);

	const float deltaTime = static_cast<float>(currentFrameTime - m_previousFrameTime) / 1000.0f;

	m_previousFrameTime = currentFrameTime;

	m_registry->GetSystem<MovementSystem>().Update(deltaTime);

	m_registry->Update();
}

void Game::Render()
{
	SDL_SetRenderDrawColor(m_renderer, 21, 21, 21, 255);
	SDL_RenderClear(m_renderer);

	m_registry->GetSystem<RenderSystem>().Update(m_renderer);

	SDL_RenderPresent(m_renderer);
}

void Game::Init()
{
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		Logger::LogError("Error initializing SDL.");
		return;
	}

	SDL_DisplayMode displayMode{};
	SDL_GetCurrentDisplayMode(0, &displayMode);
	m_windowWidth = displayMode.w;
	m_windowHeight = displayMode.h;

	m_window = SDL_CreateWindow(nullptr, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_windowWidth, m_windowHeight, SDL_WINDOW_BORDERLESS);
	if(!m_window)
	{
		Logger::LogError("Error creating SDL window.");
		return;
	}

	m_renderer = SDL_CreateRenderer(m_window, -1, 0);
	if(!m_renderer)
	{
		Logger::LogError("Error creating SDL renderer.");
		return;
	}

	SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN);

	m_isRunning = true;
}

void Game::Run()
{
	Setup();
	while(m_isRunning)
	{
		ProcessInputs();
		Update();
		Render();
	}
}

void Game::Destroy()
{
	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_window);
	SDL_Quit();
}

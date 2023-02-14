#include "Game.h"
#include "../helpers/Constants.h"
#include "../helpers/Logger.h"
#include <SDL2/SDL_image.h>
#include <iostream>

Game::Game() : m_window{}, m_renderer{}, m_isRunning(false), m_windowWidth(0), m_windowHeight(0), m_previousFrameTime(0)
{
	Logger::Log("Game constructor called!");
	Logger::LogWarning("Game constructor called!");
	Logger::LogError("Game constructor called!");
}

Game::~Game()
{
}

void Game::Setup()
{
}

void Game::ProcessInputs()
{
	SDL_Event sdlEvent{};
	while (SDL_PollEvent(&sdlEvent))
	{
		switch (sdlEvent.type)
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
	auto currentFrameTime = SDL_GetTicks();
	int timeToWait = TIME_PER_FRAME - (currentFrameTime - m_previousFrameTime);

	if(timeToWait > 0 && timeToWait <= TIME_PER_FRAME)
		SDL_Delay(timeToWait);
	
	float deltaTime = (currentFrameTime - m_previousFrameTime) / 1000.0f;

	m_previousFrameTime = currentFrameTime;
}

void Game::Render()
{
	SDL_SetRenderDrawColor(m_renderer, 21, 21, 21, 255);
	SDL_RenderClear(m_renderer);

	SDL_Surface* surface = IMG_Load("./assets/images/tank-tiger-right.png");
	SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, surface);
	SDL_FreeSurface(surface);

	SDL_Rect dstRect = {10, 10, 32, 32};
	SDL_RenderCopy(m_renderer, texture, nullptr, &dstRect);

	SDL_DestroyTexture(texture);

	SDL_RenderPresent(m_renderer);
}

void Game::Init()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		Logger::LogError("Error initialiazing SDL.");
		return;
	}

	SDL_DisplayMode displayMode{};
	SDL_GetCurrentDisplayMode(0, &displayMode);
	m_windowWidth = displayMode.w;
	m_windowHeight = displayMode.h;

	m_window = SDL_CreateWindow(nullptr, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_windowWidth, m_windowHeight, SDL_WINDOW_BORDERLESS);
	if (!m_window)
	{
		Logger::LogError("Error creating SDL window.");
		return;
	}

	m_renderer = SDL_CreateRenderer(m_window, -1, 0);
	if (!m_renderer)
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
	while (m_isRunning)
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

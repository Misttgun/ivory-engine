#include "Game.h"

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_sdlrenderer.h>

#include "Helpers/LevelLoader.h"

#include "Ivory/ECS/Systems/AnimationSystem.h"
#include "Ivory/ECS/Systems/CollisionSystem.h"
#include "Ivory/ECS/Systems/RenderSystem.h"
#include "Ivory/ECS/Systems/RenderTextSystem.h"
#include "Ivory/Helpers/Log.h"
#include "Ivory/Input/Input.h"

#include "Systems/CameraMovementSystem.h"
#include "Systems/DamageSystem.h"
#include "Systems/KeyboardControlSystem.h"
#include "Systems/MovementSystem.h"
#include "Systems/ProjectileEmitSystem.h"
#include "Systems/ProjectileLifecycleSystem.h"
#include "Systems/RenderGUISystem.h"
#include "Systems/RenderHealthBarSystem.h"

namespace Ivory
{

	int32 Level::m_mapWidth = 0;
	int32 Level::m_mapHeight = 0;

	Game::Game() : m_camera{}, m_isDebug(false), m_previousFrameTime(0), m_accumulator(0)
	{
		Ivory::Log::Init();

		m_eventBus = std::make_shared<EventBus>();
		m_registry = std::make_unique<Registry>();
		m_registry->Init(m_eventBus);
		m_resourceManager = std::make_unique<ResourceManager>();
		m_window = std::make_unique<Window>();
	}

	void Game::LoadLevel() const
	{
		m_registry->RegisterSystem<MovementSystem>();
		m_registry->RegisterSystem<RenderSystem>();
		m_registry->RegisterSystem<AnimationSystem>();
		m_registry->RegisterSystem<CollisionSystem>();
		m_registry->RegisterSystem<DamageSystem>();
		m_registry->RegisterSystem<KeyboardControlSystem>();
		m_registry->RegisterSystem<CameraMovementSystem>();
		m_registry->RegisterSystem<ProjectileEmitSystem>();
		m_registry->RegisterSystem<ProjectileLifecycleSystem>();
		m_registry->RegisterSystem<RenderTextSystem>();
		m_registry->RegisterSystem<RenderHealthBarSystem>();
		m_registry->RegisterSystem<RenderGUISystem>();

		const auto renderer = m_window->Renderer();

		LevelLoader::LoadResources(m_resourceManager, renderer);
		LevelLoader::LoadMap(m_registry);
		LevelLoader::LoadEntities(m_registry);

		m_fpsCounter = m_registry->CreateEntity();
		constexpr SDL_Color green = { 0, 255, 0, 255 };
		m_registry->AddComponent(m_fpsCounter, TextLabelComponent{ glm::vec2(Window::m_width / 2 - 40, 10), m_fps, "charriot-font", green, true });
	}

	void Game::Setup() const
	{
		LoadLevel();
	}

	void Game::ProcessInputs()
	{
		m_window->ProcessEvents();

		if(Input::IsKeyDown(KeyCode::F2))
		{
			m_isDebug = !m_isDebug;
		}
	}

	void Game::Update()
	{
		constexpr float deltaTime = DELTA_TIME_S;

		const auto currentFrameTime = SDL_GetTicks64();
		auto deltaMs = currentFrameTime - m_previousFrameTime;

		// Clamp deltaMS so we don't have weird behaviour when we hit a break point
		if(deltaMs > static_cast<uint64>(FUZZY_TIME_PER_FRAME_MS))
			deltaMs = static_cast<uint64>(FUZZY_TIME_PER_FRAME_MS);

		m_previousFrameTime = currentFrameTime;

		m_accumulator += static_cast<float>(deltaMs);

		if (m_accumulator <= FUZZY_TIME_PER_FRAME_MS)
			return;

		m_accumulator -= TIME_PER_FRAME_MS;
		if (m_accumulator < 0)
			m_accumulator = 0;

		m_eventBus->Reset();

		m_registry->GetSystem<DamageSystem>().SubscribeToEvent(m_eventBus);
		m_registry->GetSystem<KeyboardControlSystem>().SubscribeToEvent(m_eventBus);
		m_registry->GetSystem<ProjectileEmitSystem>().SubscribeToEvent(m_eventBus);
		m_registry->GetSystem<MovementSystem>().SubscribeToEvent(m_eventBus);
		m_registry->GetSystem<CollisionSystem>().SubscribeToEvent(m_eventBus);

		m_registry->Update();

		m_registry->GetSystem<MovementSystem>().Update(deltaTime);
		m_registry->GetSystem<AnimationSystem>().Update(deltaTime);
		m_registry->GetSystem<CollisionSystem>().Update(m_eventBus);
		m_registry->GetSystem<CameraMovementSystem>().Update(m_camera);
		m_registry->GetSystem<ProjectileEmitSystem>().Update(deltaTime);
		m_registry->GetSystem<ProjectileLifecycleSystem>().Update(deltaTime);

		// FPS Counter
		m_totalFrames++;

		if (m_updateTimer.ElapsedSeconds() > 1)
		{
			m_fps = "FPS: " + std::to_string(static_cast<uint64>(static_cast<double>(m_totalFrames) / m_fpsTimer.ElapsedSeconds()));
			auto& comp = m_registry->GetComponent<TextLabelComponent>(m_fpsCounter);
			comp.m_text = m_fps;

			m_updateTimer.Start();
		}
	}

	void Game::Render() const
	{
		m_window->BeginDraw();

		const auto renderer = m_window->Renderer();

		m_registry->GetSystem<RenderSystem>().Draw(renderer, m_resourceManager, m_camera);
		m_registry->GetSystem<RenderTextSystem>().Draw(renderer, m_resourceManager, m_camera);
		m_registry->GetSystem<RenderHealthBarSystem>().Draw(renderer, m_resourceManager, m_camera);

		if (m_isDebug)
		{
			m_registry->GetSystem<RenderGUISystem>().Draw();
			m_registry->GetSystem<CollisionSystem>().Draw(renderer, m_camera);

			ImGui::Render();
			ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
		}

		m_window->EndDraw();
	}

	void Game::Init()
	{
		m_window->Init("Game", glm::ivec2(1280, 720));

		m_camera.x = 0;
		m_camera.y = 0;
		m_camera.w = Window::m_width;
		m_camera.h = Window::m_height;
	}

	void Game::Run()
	{
		Setup();

		m_previousFrameTime = SDL_GetTicks64();

		m_fpsTimer.Start();
		m_updateTimer.Start();

		while (m_window->ShouldClose() == false)
		{
			ProcessInputs();
			Update();
			Render();
		}
	}

	void Game::Destroy() const
	{
		m_window->Cleanup();
	}
}

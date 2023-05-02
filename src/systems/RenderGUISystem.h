#pragma once

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_sdlrenderer.h>

#include "../core/System.h"

namespace re
{
	class RenderGUISystem : public System
	{
	public:
		RenderGUISystem() = default;

		void Draw() const
		{
			// Start the Dear ImGui frame
			ImGui_ImplSDLRenderer_NewFrame();
			ImGui_ImplSDL2_NewFrame();
			ImGui::NewFrame();

			if (ImGui::Begin("SpawnEnemies"))
			{
				// Enemy sprites
				const char* sprites[] = {"tank_image_right", "truck_image_right"};
				static int32 selectedSprite = 0;
				if (ImGui::CollapsingHeader("Sprite", ImGuiTreeNodeFlags_DefaultOpen))
					ImGui::Combo("Sprite ID", &selectedSprite, sprites, IM_ARRAYSIZE(sprites));

				ImGui::Spacing();

				// Transform
				static int32 pos[2] = {0, 0};
				static int32 angle = 0;
				if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::InputInt2("Position", pos);
					ImGui::SliderInt("Angle", &angle, 0, 360);
				}

				ImGui::Spacing();

				//RigidBody
				static int32 vel[2] = {0, 0};
				if (ImGui::CollapsingHeader("RigidBody", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::InputInt2("Velocity", vel);
				}

				ImGui::Spacing();

				//Projectile
				static int32 projectileVel[2] = {0, 0};
				static float coolDown = 0;
				if (ImGui::CollapsingHeader("Projectile", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::InputInt2("P Velocity", projectileVel);
					ImGui::InputFloat("Cooldown (sec)", &coolDown);
				}

				ImGui::Spacing();

				//Health
				static int32 health = 0;
				if (ImGui::CollapsingHeader("Health", ImGuiTreeNodeFlags_DefaultOpen))
					ImGui::SliderInt("Max Health", &health, 0, 100);

				if (ImGui::Button("Create Enemy"))
				{
					const auto enemy = m_registry->CreateEntity();
					m_registry->AddComponent(enemy, EnemyTag{});
					m_registry->AddComponent(enemy, TransformComponent{glm::vec2(pos[0], pos[1]), glm::vec2(1, 1), glm::degrees(static_cast<double>(angle))});
					m_registry->AddComponent(enemy, RigidBodyComponent{glm::vec2(vel[0], vel[1])});
					m_registry->AddComponent(enemy, SpriteComponent{sprites[selectedSprite], 32, 32, 1});
					m_registry->AddComponent(enemy, BoxColliderComponent{glm::vec2(25, 20), glm::vec2(5, 5)});
					m_registry->AddComponent(enemy, ProjectileEmitterComponent{glm::vec2(projectileVel[0], projectileVel[1]), coolDown});
					m_registry->AddComponent(enemy, HealthComponent{health});
				}
			}

			ImGui::End();
		}
	};
}

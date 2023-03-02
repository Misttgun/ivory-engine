#pragma once
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_sdlrenderer.h>

#include "../ecs/ECS.h"


class RenderGUISystem : public System
{
public:
	RenderGUISystem() = default;

	void Draw(const std::unique_ptr<Registry>& registry) const
	{
		// Start the Dear ImGui frame
		ImGui_ImplSDLRenderer_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		if(ImGui::Begin("SpawnEnemies"))
		{
			// Enemy sprites
			const char* sprites[] = {"tank_image_right", "truck_image_right"};
			static int selectedSprite = 0;
			if(ImGui::CollapsingHeader("Sprite", ImGuiTreeNodeFlags_DefaultOpen))
				ImGui::Combo("Sprite ID", &selectedSprite, sprites, IM_ARRAYSIZE(sprites));

			ImGui::Spacing();

			// Transform
			static int pos[2] = {0, 0};
			static int angle = 0;
			if(ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::InputInt2("Position", pos);
				ImGui::SliderInt("Angle", &angle, 0, 360);
			}

			ImGui::Spacing();

			//RigidBody
			static int vel[2] = {0, 0};
			if(ImGui::CollapsingHeader("RigidBody", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::InputInt2("Velocity", vel);
			}

			ImGui::Spacing();

			//Projectile
			static int projectileVel[2] = {0, 0};
			static float coolDown = 0;
			if(ImGui::CollapsingHeader("Projectile", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::InputInt2("P Velocity", projectileVel);
				ImGui::InputFloat("Cooldown (sec)", &coolDown);
			}

			ImGui::Spacing();

			//Health
			static int health = 0;
			if(ImGui::CollapsingHeader("Health", ImGuiTreeNodeFlags_DefaultOpen))
				ImGui::SliderInt("Max Health", &health, 0, 100);

			if(ImGui::Button("Create Enemy"))
			{
				auto enemy = registry->CreateEntity();
				enemy.AddComponent<EnemyTag>();
				enemy.AddComponent<TransformComponent>(glm::vec2(pos[0], pos[1]), glm::vec2(1, 1),  glm::degrees(static_cast<float>(angle)));
				enemy.AddComponent<RigidBodyComponent>(glm::vec2(vel[0], vel[1]));
				enemy.AddComponent<SpriteComponent>(sprites[selectedSprite], 32, 32, 1);
				enemy.AddComponent<BoxColliderComponent>(glm::vec2(25, 20), glm::vec2(5, 5));
				enemy.AddComponent<ProjectileEmitterComponent>(glm::vec2(projectileVel[0], projectileVel[1]), coolDown);
				enemy.AddComponent<HealthComponent>(health);
			}
		}

		ImGui::End();
	}

};

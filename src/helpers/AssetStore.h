#pragma once

#include <map>
#include <string>
#include <SDL2/SDL_render.h>

class AssetStore
{
private:
	std::map<std::string, SDL_Texture*> m_textures;
	SDL_Renderer* m_renderer{};

public:
	AssetStore();
	explicit AssetStore(SDL_Renderer* renderer);
	~AssetStore();

	void ClearAssets();
	void AddTexture(const std::string& assetId, const std::string& path);
	void SetRenderer(SDL_Renderer* renderer);
	[[nodiscard]] SDL_Texture* GetTexture(const std::string& assetId) const;
};


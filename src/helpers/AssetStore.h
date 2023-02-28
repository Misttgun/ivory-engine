#pragma once

#include <map>
#include <string>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>

class AssetStore
{
private:
	std::map<std::string, SDL_Texture*> m_textures;
	std::map<std::string, TTF_Font*> m_fonts;

	SDL_Renderer* m_renderer{};

public:
	AssetStore();
	explicit AssetStore(SDL_Renderer* renderer);
	~AssetStore();

	void ClearAssets();

	void AddTexture(const std::string& assetId, const std::string& path);
	void SetRenderer(SDL_Renderer* renderer);
	[[nodiscard]] SDL_Texture* GetTexture(const std::string& assetId) const;

	void AddFont(const std::string& assetId, const std::string& path, int fontSize);
	TTF_Font* GetFont(const std::string& assetId) const;
};


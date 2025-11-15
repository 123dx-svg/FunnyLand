#include "resource_manager.h"
#include "font_manager.h"
#include "audio_manager.h"
#include "texture_manager.h"
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>


namespace engine::resource
{
    
    
    ResourceManager::ResourceManager(SDL_Renderer* renderer)
    {
        texture_manager_ = std::make_unique<TextureManager>(renderer);
        font_manager_ = std::make_unique<FontManager>();
        audio_manager_ = std::make_unique<AudioManager>();
        
        spdlog::trace("ResourceManager 构造完成");
    }

    ResourceManager::~ResourceManager() = default;

    void ResourceManager::clear()
    {
        texture_manager_->clearTextures();
        font_manager_->clearFonts();
        audio_manager_->clearAudio();
        spdlog::trace("ResourceManager 清除所有资源完成");
    }

    SDL_Texture* ResourceManager::loadTexture(const std::string& file_path)
    {
        return texture_manager_->loadTexture(file_path);
    }

    SDL_Texture* ResourceManager::getTexture(const std::string& file_path)
    {
        return texture_manager_->getTexture(file_path);
    }

    void ResourceManager::unloadTexture(const std::string& file_path)
    {
        texture_manager_->unloadTexture(file_path);
    }

    glm::vec2 ResourceManager::getTextureSize(const std::string& file_path)
    {
        return texture_manager_->getTextureSize(file_path);
    }

    void ResourceManager::clearTextures()
    {
        texture_manager_->clearTextures();
    }

    Mix_Chunk* ResourceManager::loadSound(const std::string& file_path)
    {
        return audio_manager_->loadSound(file_path);
    }

    Mix_Chunk* ResourceManager::getSound(const std::string& file_path)
    {
        return audio_manager_->getSound(file_path);
    }

    void ResourceManager::unloadSound(const std::string& file_path)
    {
        audio_manager_->unloadSound(file_path);
    }

    void ResourceManager::clearSounds()
    {
        audio_manager_->clearSounds();
    }

    Mix_Music* ResourceManager::loadMusic(const std::string& file_path)
    {
        return audio_manager_->loadMusic(file_path);
    }

    Mix_Music* ResourceManager::getMusic(const std::string& file_path)
    {
        return audio_manager_->getMusic(file_path);
    }

    void ResourceManager::unloadMusic(const std::string& file_path)
    {
        audio_manager_->unloadMusic(file_path);
    }

    void ResourceManager::clearMusics()
    {
        audio_manager_->clearMusics();
    }

    TTF_Font* ResourceManager::loadFont(const std::string& file_path, int font_size)
    {
        return font_manager_->loadFont(file_path, font_size);
    }

    TTF_Font* ResourceManager::getFont(const std::string& file_path, int font_size)
    {
        return font_manager_->getFont(file_path, font_size);
    }

    void ResourceManager::unloadFont(const std::string& file_path, int font_size)
    {
        font_manager_->unloadFont(file_path, font_size);
    }

    void ResourceManager::clearFonts()
    {
        font_manager_->clearFonts();
    }
}

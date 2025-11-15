#include "texture_manager.h"
#include <SDL3_image/SDL_image.h>
#include <stdexcept>
#include <spdlog/spdlog.h>

namespace engine::resource
{
    TextureManager::TextureManager(SDL_Renderer* renderer)
        :renderer_(renderer)
    {
        if (!renderer_)
        {
            //抛出异常 可以被GameApp中的 try-catch 捕获
            throw std::runtime_error("TextureManager 构造函数：渲染器不能为空");
        }
        //SDL3中不需要IMG_INI
        spdlog::trace("TextureManager 构造完成");
    }

    SDL_Texture* TextureManager::loadTexture(const std::string& file_path)
    {
        //检查是否已加载
        auto it = textures_.find(file_path);
        if (it != textures_.end())
            return it->second.get();
        
        //如果没加载到尝试加载纹理
        SDL_Texture* raw_texture = IMG_LoadTexture(renderer_, file_path.c_str());
        if (raw_texture == nullptr)
        {
            spdlog::debug("纹理记载失败 {} : {}", file_path, SDL_GetError());
            return nullptr;
        }
        
        //加载到了正式存储
        textures_.emplace(file_path, std::unique_ptr<SDL_Texture,SDLTextureDeleter>(raw_texture));
        spdlog::debug("成功加载并缓存纹理 {}", file_path);
        
        return raw_texture;
       
    }

    SDL_Texture* TextureManager::getTexture(const std::string& file_path)
    {
        //检查现有纹理
        auto it = textures_.find(file_path);
        if (it != textures_.end())
            return it->second.get();
        
        spdlog::warn("纹理未加载 {}", file_path);
        return loadTexture(file_path);
    }

    glm::vec2 TextureManager::getTextureSize(const std::string& file_path)
    {
        SDL_Texture* texture = getTexture(file_path);
        if (!texture)
        {
            spdlog::error("无法获取纹理 {}", file_path);
            return {0,0};
        }
        
        //获取纹理尺寸
        glm::vec2 size;
        if (!SDL_GetTextureSize(texture, &size.x, &size.y))
        {
            spdlog::error("获取纹理尺寸失败 {}", file_path);
            return {0,0};
        }
        return size;
    }

    void TextureManager::unloadTexture(const std::string& file_path)
    {
        auto it = textures_.find(file_path);
        if (it != textures_.end())
        {
            spdlog::debug("成功卸载纹理 {}", file_path);
            textures_.erase(it);//此处会走自定义删除器删除
        }
        else
        {
            spdlog::warn("尝试卸载，但纹理未加载 {}", file_path);
        }
    }

    void TextureManager::clearTextures()
    {
        if (!textures_.empty())
        {
            spdlog::debug("正在清理{}个纹理", textures_.size());
            textures_.clear();
        }
    }
}

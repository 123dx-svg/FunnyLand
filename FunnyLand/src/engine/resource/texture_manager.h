#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <glm/vec2.hpp>
#include <SDL3/SDL_render.h>

struct SDL_Renderer;
namespace engine::resource
{
    
/**
 * @brief 管理 SDL_Texture 资源的加载、存储和检索。
 *
 * 在构造时初始化。使用文件路径作为键，确保纹理只加载一次并正确释放。
 * 依赖于一个有效的 SDL_Renderer，构造失败会抛出异常。
 */
class TextureManager final
{
    friend class ResourceManager;
    
private:
    //SDL_Texture的删除器函数对象,用于智能指针管理
    struct SDLTextureDeleter
    {
        void operator()(SDL_Texture* texture) const
        {
            if (texture)
            {
                SDL_DestroyTexture(texture);
            }
        }
    };
    
    //纹理存储路径和指向纹理管理器的智能指针
    std::unordered_map<std::string, std::unique_ptr<SDL_Texture, SDLTextureDeleter>> textures_;
    
    SDL_Renderer* renderer_ = nullptr;//指向主渲染器的非拥有指针
    
    
    
public:
    explicit  TextureManager(SDL_Renderer* renderer);
    
    //只需要一个实例
    TextureManager(const TextureManager&) = delete;
    TextureManager(TextureManager&&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;
    TextureManager& operator=(TextureManager&&) = delete;
    
private://仅允许ResourceManager访问
    
    SDL_Texture* loadTexture(const std::string& file_path);
    SDL_Texture* getTexture(const std::string& file_path);
    glm::vec2 getTextureSize(const std::string& file_path);
    void unloadTexture(const std::string& file_path);
    void clearTextures();//清除所有纹理
    
    
    
};
}
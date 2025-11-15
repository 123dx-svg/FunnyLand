#pragma once
#include <memory>
#include <string>
#include <glm/glm.hpp>

struct SDL_Texture;
struct SDL_Renderer;
struct Mix_Chunk;
struct Mix_Music;
struct TTF_Font;

namespace engine::resource
{
    //前向申明资源管理器
    class FontManager;
    class TextureManager;
    class AudioManager;
    
/**
 * @brief 作为访问各种资源管理器的中央控制点（外观模式 Facade）。
 * 在构造时初始化其管理的子系统。构造失败会抛出异常。
 */
class ResourceManager final
{
 
private:
    //使用unique_ptr管理资源管理器的实例
    std::unique_ptr<TextureManager> texture_manager_;
    std::unique_ptr<FontManager> font_manager_;
    std::unique_ptr<AudioManager> audio_manager_;

public:
    explicit  ResourceManager(SDL_Renderer* renderer);//单个参数的构造函数，防止隐式转换
    
    //只有一个资源管理器实例
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager(ResourceManager&&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    ResourceManager& operator=(ResourceManager&&) = delete;
    
    //析构函数,清理所有资源
    ~ResourceManager();//析构函数,清理所有资源
    
    void clear();//清除所有资源
    
    //资源访问统一接口
    //texture
    SDL_Texture* loadTexture(const std::string& file_path); //加载纹理
    SDL_Texture* getTexture(const std::string& file_path); //尝试获取的纹理指针，如果不存在就尝试加载
    void unloadTexture(const std::string& file_path);//卸载纹理
    glm::vec2 getTextureSize(const std::string& file_path);//获取纹理大小
    void clearTextures();//清除所有纹理
    
    //sound
    Mix_Chunk* loadSound(const std::string& file_path);//加载音效
    Mix_Chunk* getSound(const std::string& file_path);//尝试获取的音效指针，如果不存在就尝试加载
    void unloadSound(const std::string& file_path);//卸载音效
    void clearSounds();//清除所有音效
    
    //music
    Mix_Music* loadMusic(const std::string& file_path);//加载音乐
    Mix_Music* getMusic(const std::string& file_path);//尝试获取的音乐指针，如果不存在就尝试加载
    void unloadMusic(const std::string& file_path);//卸载音乐
    void clearMusics();//清除所有音乐
    
    //fonts
    TTF_Font* loadFont(const std::string& file_path, int font_size);//加载字体
    TTF_Font* getFont(const std::string& file_path, int font_size);//尝试获取的字体指针，如果不存在就尝试加载
    void unloadFont(const std::string& file_path, int font_size);//卸载字体
    void clearFonts();//清除所有字体
    
    
    
    
    
};

}

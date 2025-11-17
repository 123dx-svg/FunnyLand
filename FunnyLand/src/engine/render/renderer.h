#pragma once
#include <optional>
#include <glm/glm.hpp>
#include <SDL3/SDL_render.h>

struct SDL_Renderer;
struct SDL_FRect;

namespace engine::resource {
    class ResourceManager;
}
namespace engine::render
{
    class Sprite;
    class Camera;


    /**
     * @brief 封装 SDL3 渲染操作
     *
     * 包装 SDL_Renderer 并提供清除屏幕、绘制精灵和呈现最终图像的方法。
     * 在构造时初始化。依赖于一个有效的 SDL_Renderer 和 ResourceManager。
     * 构造失败会抛出异常。
     */
    class Renderer final
    {
        
    private:
        //他们的生命周期不归该类管理
        SDL_Renderer* renderer_ = nullptr;
        engine::resource::ResourceManager* resource_manager_ = nullptr;
    public:
        /**
         * @brief 构造函数
         *
         * @param sdl_renderer 有效的 SDL_Renderer 指针
         * @param resource_manager 有效的 ResourceManager 指针
         */
        Renderer(SDL_Renderer* sdl_renderer, engine::resource::ResourceManager* resource_manager);
        
        //禁止拷贝和移动
        Renderer(const Renderer&) = delete;
        Renderer(Renderer&&) = delete;
        Renderer& operator=(const Renderer&) = delete;
        Renderer& operator=(Renderer&&) = delete;
        
        /**
        * @brief 绘制一个精灵
        * 
        * @param sprite 包含纹理ID、源矩形和翻转状态的 Sprite 对象。
        * @param position 世界坐标中的左上角位置。
        * @param scale 缩放因子。
        * @param angle 旋转角度（度）。
        */
        void drawSprite(const Camera& camera,const Sprite& sprite,const glm::vec2& position,const glm::vec2& scale = {1.0f,1.0f},float angle = 0.0f);
        
        /**
        * @brief 绘制视差滚动背景
        * 
        * @param sprite 包含纹理ID、源矩形和翻转状态的 Sprite 对象。
        * @param position 世界坐标。
        * @param scroll_factor 滚动因子。
        * @param scale 缩放因子。
        */
        void drawParallax(const Camera& camera,const Sprite& sprite,const glm::vec2& position,const glm::vec2& scroll_factor,const glm::bvec2& repeat = 
            {true,true},const glm::vec2& scale = {1.0f,1.0f});
        
        
        /**
        * @brief 在屏幕坐标中直接渲染一个用于UI的Sprite对象。
        *
        * @param sprite 包含纹理ID、源矩形和翻转状态的Sprite对象。
        * @param position 屏幕坐标中的左上角位置。
        * @param size 可选：目标矩形的大小。如果为 std::nullopt，则使用Sprite的原始大小。
        */
        void drawUISprite(const Sprite& sprite,const glm::vec2& position,const std::optional<glm::vec2>& size = std::nullopt);
        
        
        void present();  //更新屏幕 包装SDL_RenderPresent 函数
        void clearScreen();  //清除屏幕 包装SDL_RenderClear 函数
        void setDrawColor(Uint8 r,Uint8 g,Uint8 b,Uint8 a = 255);  //设置绘制颜色 包装SDL_SetRenderDrawColor 函数
        void setDrawColorFloat(float r,float g,float b,float a = 1.f);//设置绘制颜色 包装SDL_SetRenderDrawColorFloat 函数 浮点数版本
        
        
        SDL_Renderer* getRenderer() const {return renderer_;}
        
    private:
        std::optional<SDL_FRect> getSpriteSrcRect(const Sprite& sprite);  //获取精灵源矩形，用于具体绘制，如果返回std::nullopt，就跳过绘制
        bool isRectInViewport(const Camera& camera,const SDL_FRect& rect);  //判断矩形是否在相机视野内 
    };  
}



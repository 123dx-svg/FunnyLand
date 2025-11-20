#include "renderer.h"
#include "../resource/resource_manager.h"
#include <spdlog/spdlog.h>

#include "camera.h"
#include "sprite.h"

namespace engine::render
{
    Renderer::Renderer(SDL_Renderer* sdl_renderer, engine::resource::ResourceManager* resource_manager)
        : renderer_(sdl_renderer), resource_manager_(resource_manager)
    {
        spdlog::trace("构造SDL渲染器...");
        if (!renderer_)
        {
            throw std::runtime_error("Renderer构造失败，提供的SDL_Renderer为空");
        }
        if (!resource_manager_)
        {
            //绘制精灵必须要用到资源管理器
            throw std::runtime_error("Renderer构造失败，提供的ResourceManager为空");
        }
        setDrawColor(0,0,0,255);
        spdlog::trace("Renderer 构造完成");
    }

    void Renderer::drawSprite(const Camera& camera, const Sprite& sprite, const glm::vec2& position,
        const glm::vec2& scale, float angle)
    {
        auto texture = resource_manager_->getTexture(sprite.getTextureId());
        if (!texture)
        {
            spdlog::error("无法为ID{}获取纹理", sprite.getTextureId());
            return; 
        }
        auto src_rect = getSpriteSrcRect(sprite);
        if (!src_rect.has_value())
        {
            spdlog::error("无法获取精灵的源矩形，ID:{}",sprite.getTextureId());
            return;
        }
        //将相机空间转换为屏幕空间
        glm::vec2 screen_pos = camera.worldToScreen(position);
       //计算目标矩形，注意position是精灵左上角坐标
       float scaled_w = src_rect.value().w*scale.x;
       float scaled_h = src_rect.value().h*scale.y;
       SDL_FRect dest_rect = {screen_pos.x,screen_pos.y,scaled_w,scaled_h};//确定目标矩形左上角坐标

        if (!isRectInViewport(camera,dest_rect)) 
        {
            //spdlog::trace("精灵{}超出视口范围，不绘制",sprite.getTextureId());
            return;
        }
        
        //执行绘制
        if (!SDL_RenderTextureRotated(renderer_,texture,&src_rect.value(),&dest_rect,angle,nullptr,sprite.isFlipped()?SDL_FLIP_HORIZONTAL:SDL_FLIP_NONE))
        {
            spdlog::error("渲染 Sprite 失败 (ID: {}): {}", sprite.getTextureId(), SDL_GetError());
        }
        
        
    }

    void Renderer::drawParallax(const Camera& camera, const Sprite& sprite, const glm::vec2& position,
        const glm::vec2& scroll_factor, const glm::bvec2& repeat, const glm::vec2& scale)
    {
        auto texture = resource_manager_->getTexture(sprite.getTextureId());
        if (!texture)
        {
            spdlog::error("无法为ID{}获取纹理", sprite.getTextureId());
            return; 
        }
        auto src_rect = getSpriteSrcRect(sprite);
        if (!src_rect.has_value())
        {
            spdlog::error("无法获取精灵的源矩形，ID:{}",sprite.getTextureId());
            return;
        }
        //相机变换
        glm::vec2 screen_pos = camera.worldToScreenWithParallax(position,scroll_factor);
        
        //缩放后的纹理尺寸
        float scaled_tex_w = src_rect.value().w*scale.x;
        float scaled_tex_h = src_rect.value().h*scale.y;
        
        glm::vec2 start,stop;
        glm::vec2 viewport_size = camera.getViewportSize();
        if (repeat.x)//x轴有重复
        {
            start.x = glm::mod(screen_pos.x,scaled_tex_w) - scaled_tex_w;
            stop.x = viewport_size.x;
        }
        else
        {
            start.x = screen_pos.x;
            stop.x = glm::min(screen_pos.x + scaled_tex_w,viewport_size.x);
        }
        if (repeat.y)//y轴有重复
        {
            start.y = glm::mod(screen_pos.y,scaled_tex_h) - scaled_tex_h;
            stop.y = viewport_size.y;
        }
        else
        {
            start.y = screen_pos.y;
            stop.y = glm::min(screen_pos.y + scaled_tex_h,viewport_size.y);
        }

        for (float y = start.y; y < stop.y; y+=scaled_tex_h)
        {
            for (float x = start.x; x < stop.x; x+=scaled_tex_w)
            {
                SDL_FRect dest_rect = {x,y,scaled_tex_w,scaled_tex_h};//确定目标矩形左上角坐标
                //执行绘制
                if (!SDL_RenderTexture(renderer_,texture,nullptr,&dest_rect))
                {
                    spdlog::error("渲染 Sprite 失败 (ID: {}): {}", sprite.getTextureId(), SDL_GetError());
                }
            }
        }
    }

    void Renderer::drawUISprite(const Sprite& sprite, const glm::vec2& position, const std::optional<glm::vec2>& size)
    {
        auto texture = resource_manager_->getTexture(sprite.getTextureId());
        if (!texture)
        {
            spdlog::error("无法为ID{}获取纹理", sprite.getTextureId());
            return; 
        }
        
        auto src_rect = getSpriteSrcRect(sprite);
        if (!src_rect.has_value())
        {
            spdlog::error("无法获取精灵的源矩形，ID:{}",sprite.getTextureId());
            return;
        }
        
        SDL_FRect dest_rect = {position.x,position.y,0,0};//确定目标矩形左上角坐标
        if (size.has_value())//如果提供了大小，就用提供的大小
        {
            dest_rect.w = size.value().x;
            dest_rect.h = size.value().y;
        }
        else
        {
            dest_rect.w = src_rect.value().w;
            dest_rect.h = src_rect.value().h;
        }       
        
        //执行绘制
        if (!SDL_RenderTextureRotated(renderer_,texture,&src_rect.value(),&dest_rect,0.0,nullptr,sprite.isFlipped()?SDL_FLIP_HORIZONTAL:SDL_FLIP_NONE))
        {
            spdlog::error("渲染 UI Sprite 失败 (ID: {}): {}", sprite.getTextureId(), SDL_GetError());
        }
    }

    void Renderer::present()
    {
        SDL_RenderPresent(renderer_);
    }

    void Renderer::clearScreen()
    {
        if (!SDL_RenderClear(renderer_))
        {
            spdlog::error("清除渲染器失败: {}", SDL_GetError());
        }
    }

    void Renderer::setDrawColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
    {
        if (!SDL_SetRenderDrawColor(renderer_, r , g , b , a))
        {
            spdlog::error("设置渲染器绘制颜色失败: {}", SDL_GetError());
        }
    }

    void Renderer::setDrawColorFloat(float r, float g, float b, float a)
    {
        if (!SDL_SetRenderDrawColorFloat(renderer_, r , g , b , a))
        {
            spdlog::error("设置渲染器绘制颜色失败: {}", SDL_GetError());
        }
    }

    std::optional<SDL_FRect> Renderer::getSpriteSrcRect(const Sprite& sprite)
    {
        SDL_Texture* texture = resource_manager_->getTexture(sprite.getTextureId());
        if (!texture)
        {
            spdlog::error("无法为ID{}获取纹理", sprite.getTextureId());
            return std::nullopt; 
        }
        auto src_rect = sprite.getSourceRect();
        if (src_rect.has_value())//如果提供了源矩形，就用提供的源矩形
        {
            if (src_rect.value().w<=0||src_rect.value().h<=0)
            {
                spdlog::error("源尺寸无效，id:{}",sprite.getTextureId());
                return std::nullopt;
            }
            return src_rect;
        }
        else //否则获取纹理尺寸并返回整个纹理大小
        {
            SDL_FRect result = {0,0,0,0};
            if (!SDL_GetTextureSize(texture,&result.w,&result.h))
            {
                spdlog::error("获取纹理尺寸失败，id:{}",sprite.getTextureId());
                return std::nullopt;
            }
            return result;
        }
    }

    bool Renderer::isRectInViewport(const Camera& camera, const SDL_FRect& rect)
    {
        glm::vec2 viewport_size = camera.getViewportSize();
        //检查矩形是否在视口内
        return rect.x+rect.w>=0 && rect.x <= viewport_size.x &&
            rect.y+rect.h>=0 && rect.y <= viewport_size.y;
        
    }
}



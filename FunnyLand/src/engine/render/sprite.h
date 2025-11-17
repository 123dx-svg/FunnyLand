#pragma once
#include <optional>
#include <string>
#include <SDL3/SDL_render.h>


namespace engine::render
{
    
    
    /**
     * @brief 表示要绘制的视觉精灵的数据。
     *
     * 包含纹理标识符、要绘制的纹理部分（源矩形）以及翻转状态。
     * 位置、缩放和旋转由外部（例如 SpriteComponent）标识。
     * 渲染工作由 Renderer 类完成。（传入Sprite作为参数）
     */
    class Sprite final
    {
        
        
    private:
        std::string texture_id_;                      ///< @brief 纹理资源的标识符
        std::optional<SDL_FRect> source_rect_;        ///< @brief 可选：要绘制的纹理部分
        bool is_flipped_ = false;                     ///< @brief 是否水平翻转
    public:
        
        /**
         * @brief 构造一个 Sprite 对象。
         *
         * @param texture_id 纹理资源的标识符
         * @param source_rect 可选的源矩形，指定要绘制的纹理部分
         * @param is_flipped 是否水平翻转
         */
        explicit  Sprite(const std::string& texture_id, const std::optional<SDL_FRect>& source_rect = std::nullopt, bool is_flipped = false)
            : texture_id_(texture_id), source_rect_(source_rect), is_flipped_(is_flipped) {}
        
        
        //getter and setter
        const std::string& getTextureId() const { return texture_id_; }
        void setTextureId(const std::string& texture_id) { texture_id_ = texture_id; }
        
        const std::optional<SDL_FRect>& getSourceRect() const { return source_rect_; }
        void setSourceRect(const std::optional<SDL_FRect>& source_rect) { source_rect_ = source_rect; }
        
        bool isFlipped() const { return is_flipped_; }
        void setFlipped(bool is_flipped) { is_flipped_ = is_flipped; }
    };
}


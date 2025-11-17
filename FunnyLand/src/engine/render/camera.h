#pragma once
#include <optional>
#include <glm/vec2.hpp>
#include "../utils/math.h"


namespace  engine::render
{
    /**
 * @brief 相机类负责管理相机位置和视口大小，并提供坐标转换功能。
 * 它还包含限制相机移动范围的边界。
 */
    class Camera final
    {
        glm::vec2 viewport_size_;                                                ///< @brief 视口大小（屏幕大小）
        glm::vec2 position_;                                                     ///< @brief 相机左上角的世界坐标
        std::optional<engine::utils::Rect> limit_bounds_;                        ///< @brief 限制相机的移动范围，空值表示不限制

    public:
        /**
         * @brief 构造一个相机对象。
         * @param viewport_size 视口大小（屏幕大小）
         * @param position 相机左上角的世界坐标
         * @param limit_bounds 限制相机移动范围的边界，空值表示不限制
         */
        explicit Camera(const glm::vec2& viewport_size, const glm::vec2& position = glm::vec2(0.0f), const std::optional<engine::utils::Rect>& limit_bounds = std::nullopt);
        
        //相机就一个实例
        Camera(const Camera&) = delete;
        Camera(Camera&&) = delete;
        Camera& operator=(const Camera&) = delete;
        Camera& operator=(Camera&&) = delete;
        
        
        void update(float delta_time);      //更新相机位置
        void move(const glm::vec2& offset); //相机移动
        
        
        glm::vec2 worldToScreen(const glm::vec2& world_pos) const;//世界转屏幕
        glm::vec2 screenToWorld(const glm::vec2& screen_pos) const;//屏幕转世界
        glm::vec2 worldToScreenWithParallax(const glm::vec2& world_pos, const glm::vec2& scroll_factor) const;//世界转屏幕（带视差滚动）
        
        void setPosition(const glm::vec2& position);//设置相机位置
        const glm::vec2& getPosition() const;//获取相机位置
        void setViewportSize(const glm::vec2& viewport_size);//设置视口大小
        glm::vec2 getViewportSize() const;//获取视口大小
        void setLimitBounds(const engine::utils::Rect& bounds);//设置限制相机移动范围的边界
        std::optional<engine::utils::Rect> getLimitBounds() const;//获取限制相机移动范围的边界
        
        
    private:
        void clampPosition(); //限制相机在边界内
    
    };
}


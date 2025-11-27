#include "parallax_component.h"
#include "transform_component.h"
#include "../render/renderer.h"
#include "../render/camera.h"
#include "../render/sprite.h"
#include "../object/game_object.h"
#include "../core/context.h"
#include <spdlog/spdlog.h>

engine::component::ParallaxComponent::ParallaxComponent(const std::string& texture_id, const glm::vec2& scroll_factor,
    const glm::bvec2& repeat)
        :sprite_(engine::render::Sprite(texture_id))
        ,scroll_factor_(scroll_factor)
        ,repeat_(repeat)
{
    spdlog::trace("ParallaxComponent 构造完成,纹理ID:{}", texture_id);
}

void engine::component::ParallaxComponent::init()
{
    if (!owner_)
    {
        spdlog::error("ParallaxComponent 初始化失败, 没有所属对象");
        return;
    }
    transform_ = owner_->getComponent<TransformComponent>();
    if (!transform_)
    {
        spdlog::error("ParallaxComponent 初始化时，GameObject 上没有找到 TransformComponent 组件。");
    }
}

void engine::component::ParallaxComponent::render(engine::core::Context& context)
{
    if (is_hidden_||!transform_)
    {
        return;
    }
    // 直接调用视差滚动绘制函数
    context.getRenderer().drawParallax(context.getCamera(), sprite_, transform_->getPosition(), scroll_factor_, repeat_, transform_->getScale());  
}

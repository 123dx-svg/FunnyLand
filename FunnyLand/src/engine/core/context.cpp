#include "context.h"
#include <spdlog/spdlog.h>

engine::core::Context::Context(engine::input::InputManager& input_manager, engine::render::Renderer& renderer,
                               engine::render::Camera& camera, engine::resource::ResourceManager& resource_manager)
    : input_manager_(input_manager),
      renderer_(renderer),
      camera_(camera),
      resource_manager_(resource_manager)
{
    spdlog::trace("上下文已创建并初始化，包含输入管理器、渲染器、相机和资源管理器。");
}

#include "scene.h"
#include "../object/game_object.h"
#include "scene_manager.h"
#include <algorithm> // for std::remove_if
#include <spdlog/spdlog.h>

namespace engine::scene
{
    Scene::Scene(std::string scene_name, engine::core::Context& context,
        engine::scene::SceneManager& scene_manager)
        : scene_name_(std::move(scene_name)), context_(context), scene_manager_(scene_manager),
        is_initialized_(false)
    {
        spdlog::trace("构造场景 {} 完成", scene_name_);
    }

    Scene::~Scene() = default;

    void Scene::init()
    {
        is_initialized_ = true;//子类应该最后调用父类init方法
        spdlog::trace("初始化场景 {} 完成", scene_name_);
    }

    void Scene::update(float delta_time)
    {
        if (!is_initialized_) return;
        
        //更新游戏中的所有对象，并删除需要移除的对象
        for (auto it = game_objects_.begin(); it != game_objects_.end();)
        {
            if (*it&&!(*it)->isNeedRemoved())
            {
                (*it)->update(delta_time, context_);
                ++it;
            }
            else
            {
                if (*it)
                {
                    (*it)->clean();
                }
                it = game_objects_.erase(it); // 删除需要移除的对象，智能指针自动管理内存
            }
        }
        
        processPendingAdditions();// 处理待添加（延时添加）的游戏对象
    }

    void Scene::render()
    {
        if (!is_initialized_) return;
        // 渲染所有游戏对象
        for (const auto& obj : game_objects_) {
            if (obj) obj->render(context_);
        }
    }

    void Scene::handleInput()
    {
        if (!is_initialized_) return;
        
        // 遍历所有游戏对象，并删除需要移除的对象
        for (auto it = game_objects_.begin(); it != game_objects_.end();) {
            if (*it && !(*it)->isNeedRemoved()) {
                (*it)->handleInput(context_);
                ++it;
            } else {
                // 安全删除需要移除的对象
                if (*it) (*it)->clean();
                it = game_objects_.erase(it);
            }
        }
    }

    void Scene::clean()
    {
        if (!is_initialized_) return;
        
        for (const auto& obj : game_objects_) {
            if (obj) obj->clean();
        }
        game_objects_.clear();
        
        is_initialized_ = false;        // 清理完成后，设置场景为未初始化
        spdlog::trace("场景 '{}' 清理完成。", scene_name_);
    }

    void Scene::addGameObject(std::unique_ptr<engine::object::GameObject>&& game_object)
    {
        if (game_object) game_objects_.push_back(std::move(game_object));
        else spdlog::warn("尝试向场景 '{}' 添加空游戏对象指针。", scene_name_);
    }

    void Scene::safeAddGameObject(std::unique_ptr<engine::object::GameObject>&& game_object)
    {
        if (game_object) pending_additions_.push_back(std::move(game_object));
        else spdlog::warn("尝试向场景 '{}' 添加空游戏对象指针。", scene_name_);
    }

    void Scene::removeGameObject(engine::object::GameObject* game_object_ptr)
    {
        if (!game_object_ptr)
        {
            spdlog::warn("尝试从场景 '{}' 中移除一个空的游戏对象指针。", scene_name_);
            return;
        }
        
        // erase-remove 移除法不可用，因为智能指针与裸指针无法比较
        // 需要使用 std::remove_if 和 lambda 表达式自定义比较的方式
        auto it = std::remove_if(game_objects_.begin(), game_objects_.end(),
            [game_object_ptr](const std::unique_ptr<engine::object::GameObject>& obj)
            {
                return obj.get() == game_object_ptr;// 比较裸指针是否相等（自定义比较方式）
            });
        
        if (it != game_objects_.end())
        {
            (*it)->clean();
            game_objects_.erase(it,game_objects_.end()); // 删除从 it 到容器末尾的所有元素 最后一个元素
            spdlog::trace("从场景 '{}' 中成功移除游戏对象 '{}'。", scene_name_, game_object_ptr->getName());
        }
        else
        {
            spdlog::warn("从场景 '{}' 中尝试移除一个不存在的游戏对象指针 '{}'。", scene_name_, game_object_ptr->getName());
        }
        
    }

    void Scene::safeRemoveGameObject(engine::object::GameObject* game_object_ptr)
    {
        if (game_object_ptr) game_object_ptr->setNeedRemoved(true);
    }

    engine::object::GameObject* Scene::findGameObjectByName(const std::string& name) const
    {
        for (const auto& obj : game_objects_)
        {
            if (obj&& obj->getName() == name) return obj.get();
            
        }
        return nullptr;
    }

    void Scene::processPendingAdditions()
    {
        //处理待添加的游戏对象
        for (auto& obj : pending_additions_)
        {
           addGameObject(std::move(obj));
        }
        pending_additions_.clear();
    }
}

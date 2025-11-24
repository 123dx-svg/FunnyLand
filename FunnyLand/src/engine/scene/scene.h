#pragma once
#include <memory>
#include <string>
#include <vector>


namespace engine::object
{
    class GameObject;
}

namespace engine::core
{
    class Context;
}

namespace engine::scene
{
    class SceneManager;


    /**
     * @brief 场景基类，负责管理场景中的游戏对象和场景生命周期。
     *
     * 包含一组游戏对象，并提供更新、渲染、处理输入和清理的接口。
     * 派生类应实现具体的场景逻辑。
     */
    class Scene
    {
        
    protected:
        std::string scene_name_; // 场景名称
        engine::core::Context& context_; // 场景上下文
        engine::scene::SceneManager& scene_manager_; // 场景管理器引用
        bool is_initialized_ = false;  //场景是否已初始化 当前场景很可能没被删除，加个标记避免重复初始化
        std::vector<std::unique_ptr<engine::object::GameObject>> game_objects_; // 场景中的游戏对象指针
        std::vector<std::unique_ptr<engine::object::GameObject>> pending_additions_; // 待添加的游戏对象指针(延时添加)
    public:
        /**
         * @brief 构造函数，初始化场景名称、上下文和场景管理器引用。
         *
         * @param scene_name 场景名称
         * @param context 场景上下文引用
         * @param scene_manager 场景管理器引用
         */
        Scene(std::string scene_name, engine::core::Context& context, engine::scene::SceneManager& scene_manager);
        
        virtual ~Scene(); 
        
        // 禁止拷贝和移动构造
        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;
        Scene(Scene&&) = delete;
        Scene& operator=(Scene&&) = delete;
        
        // 核心循环方法
        virtual void init();                        ///< @brief 初始化场景。
        virtual void update(float delta_time);      ///< @brief 更新场景。
        virtual void render();                      ///< @brief 渲染场景。
        virtual void handleInput();                 ///< @brief 处理输入。
        virtual void clean();                       ///< @brief 清理场景。
        
        /// @brief 直接向场景中添加一个游戏对象。（初始化时可用，游戏进行中不安全） （&&表示右值引用，与std::move搭配使用，避免拷贝）
        virtual void addGameObject(std::unique_ptr<engine::object::GameObject>&& game_object);
        
        /// @brief 安全添加游戏对象到场景中。（游戏进行中安全） （&&表示右值引用，与std::move搭配使用，避免拷贝） pending_additions_
        virtual void safeAddGameObject(std::unique_ptr<engine::object::GameObject>&& game_object);
        
        /// @brief 直接从场景中移除一个游戏对象。（一般不使用，但保留实现的逻辑）
        virtual void removeGameObject(engine::object::GameObject* game_object_ptr);
    
        /// @brief 安全地移除游戏对象。（设置need_remove_标记）
        virtual void safeRemoveGameObject(engine::object::GameObject* game_object_ptr);
        
        /// @brief 获取场景中的游戏对象容器。
        const std::vector<std::unique_ptr<engine::object::GameObject>>& getGameObjects() const { return game_objects_; }
        
        /// @brief 根据名称查找游戏对象（返回找到的第一个对象）。
        engine::object::GameObject* findGameObjectByName(const std::string& name) const;
        
        // getters and setters
        void setName(const std::string& name) { scene_name_ = name; }               ///< @brief 设置场景名称
        const std::string& getName() const { return scene_name_; }                  ///< @brief 获取场景名称
        void setInitialized(bool initialized) { is_initialized_ = initialized; }    ///< @brief 设置场景是否已初始化
        bool isInitialized() const { return is_initialized_; }                      ///< @brief 获取场景是否已初始化
        
        engine::core::Context& getContext() const { return context_; }                  ///< @brief 获取上下文引用
        engine::scene::SceneManager& getSceneManager() const { return scene_manager_; } ///< @brief 获取场景管理器引用
        std::vector<std::unique_ptr<engine::object::GameObject>>& getGameObjects() { return game_objects_; } ///< @brief 获取场景中的游戏对象
        
    protected:
        void processPendingAdditions();     ///< @brief 处理待添加的游戏对象。（每轮更新的最后调用）
    };
}



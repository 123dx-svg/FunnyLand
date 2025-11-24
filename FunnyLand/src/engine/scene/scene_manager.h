#pragma once
#include <memory>
#include <vector>

// 前置声明
namespace engine::core {
    class Context;
}
namespace engine::scene {
    class Scene;
}

namespace engine::scene
{
    
    
    /**
     * @brief 管理游戏中的场景栈，处理场景切换和生命周期。
     */
    class SceneManager final
    {
    private:
        engine::core::Context& context_;   //引擎上下文
        std::vector<std::unique_ptr<engine::scene::Scene>> scene_stack_; // 场景栈

        enum class PendingAction:uint8_t{None, Push,Pop,Replace }; // 待处理操作
        PendingAction pending_action_ = PendingAction::None; // 当前待处理操作
        std::unique_ptr<engine::scene::Scene> pending_scene_;               //待处理的场景
        
    public:
        explicit SceneManager(engine::core::Context& context);
        ~SceneManager();
        
        
        //禁止拷贝和移动构造
        SceneManager(const SceneManager&) = delete;
        SceneManager& operator=(const SceneManager&) = delete;
        SceneManager(SceneManager&&) = delete;
        SceneManager& operator=(SceneManager&&) = delete;
        
        //延时切换场景
        
        void requestPushScene(std::unique_ptr<engine::scene::Scene>&& scene); //请求压入一个新场景
        void requestPopScene(); //请求弹出当前场景 
        void requestReplaceScene(std::unique_ptr<engine::scene::Scene>&& scene); //请求用新场景替换当前场景
        
        //getter
        Scene* getCurrentScene() const;       //获取当前场景指针 栈顶场景
        engine::core::Context& context() const { return context_; } // 获取引擎上下文引用
        
        //核心循环函数
        void update(float delta_time);      ///< @brief 更新场景。
        void render();                      ///< @brief 渲染场景。
        void handleInput();                 ///< @brief 处理输入。
        void close();                      ///< @brief 关闭场景管理器，清理所有场景。
        
    private:
        void processPendingActions();   //处理挂起的场景操作 (每轮更新后最后调用)
        //直接切换场景
        void pushScene(std::unique_ptr<engine::scene::Scene>&& scene);    //将一个新场景压入栈顶,使其成为当前活动场景
        void popScene();                    //弹出栈顶场景
        void replaceScene(std::unique_ptr<engine::scene::Scene>&& scene);  //清理场景栈所有场景，将此场景设为栈顶场景
        
        
        
        
     
    };
}

#pragma once
#include "../object/game_object.h"


namespace engine::object
{
    class GameObject;
}

namespace engine::component
{
    
    /**
     * @brief 组件的抽象基类。
     *
     * 所有具体组件都应从此类继承。
     * 定义了组件生命周期中可能调用的通用方法。
     */
    
    class Component
    {
    public:
        friend class engine::object::GameObject;  // 它需要调用Component的init方法
        
    protected:
        engine::object::GameObject* owner_ = nullptr;   ///< @brief 指向拥有此组件的 GameObject
        
    public:
        Component() = default;
        virtual ~Component() = default; ///< @brief 虚拟析构函数，正确清理组件
        
        //禁止拷贝和移动
        Component(const Component&) = delete;
        Component& operator=(const Component&) = delete;
        Component(Component&&) = delete;
        Component& operator=(Component&&) = delete;
        
        void setOwner(engine::object::GameObject* const owner){ owner_ = owner; } ///< @brief 设置组件的所有者 GameObject
        engine::object::GameObject* getOwner(){ return owner_; } ///< @brief 获取组件的所有者 GameObject
        
    protected:
        
        virtual void init() {}  //GameObject 添加组件时自动调用 不需要外部调用
        
        virtual void handleInput() {}
        virtual void update(float) {}
        virtual void render() {}
        virtual void clean() {}
        
    };
    
}

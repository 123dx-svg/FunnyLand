#pragma once
#include "../component/component.h" 
#include <memory>
#include <unordered_map>
#include <typeindex>        // 用于类型索引
#include <utility>          // 用于完美转发
#include <spdlog/spdlog.h>
namespace engine::core
{
    class Context;
}


namespace engine::object
{
    /**
   * @brief 游戏对象类，负责管理游戏对象的组件。
   * 
   * 该类管理游戏对象的组件，并提供添加、获取、检查和移除组件的功能。
   * 它还提供更新和渲染游戏对象的方法。
   */ 
    class GameObject final
    {
        private:
        std::string name_; ///< @brief 游戏对象的名称
        std::string tag_; ///< @brief 游戏对象的标签
        std::unordered_map<std::type_index,std::unique_ptr<engine::component::Component>> components_; ///< @brief 组件列表
        bool need_removed_ = false; ///< @brief 延迟删除的标识,将来由场景类负责删除
        
        public:
        GameObject(const std::string& name = "", const std::string& tag = ""); ///< @brief 构造函数，初始化游戏对象的名称和标签
        
        
        //禁止拷贝和移动，确保唯一性 (通常游戏对象不应随意拷贝)
        GameObject(const GameObject&) = delete;
        GameObject& operator=(const GameObject&) = delete;
        GameObject(GameObject&&) = delete;
        GameObject& operator=(GameObject&&) = delete;
        
        //get setter
        void setName(const std::string& name){name_ = name;} ///< @brief 设置游戏对象的名称
        const std::string& getName() const {return name_;} ///< @brief 获取游戏对象的名称
        void setTag(const std::string& tag){tag_ = tag;} ///< @brief 设置游戏对象的标签
        const std::string& getTag() const {return tag_;} ///< @brief 获取游戏对象的标签
        void setNeedRemoved(bool need_remove){need_removed_ = need_remove;}
        bool isNeedRemoved() const {return need_removed_;}
        
        /**
     * @brief 添加组件 (里面会完成组件的init())
     * 
     * @tparam T 组件类型
     * @tparam Args 组件构造函数参数类型
     * @param args 组件构造函数参数
     * @return 组件指针
     */
        template<typename T, typename... Args>
        T* addComponent(Args&&... args)
        {
            // 检测组件是否合法。  /*  static_assert(condition, message)：静态断言，在编译期检测，无任何性能影响 */
            /* std::is_base_of<Base, Derived>::value -- 判断 Base 类型是否是 Derived 类型的基类 */
            static_assert(std::is_base_of_v<engine::component::Component, T>, "T 必须继承自 Component");
            // 获取类型标识。     /* typeid(T) -- 用于获取一个表达式或类型的运行时类型信息 (RTTI), 返回 std::type_info& */
            /* std::type_index -- 针对std::type_info对象的包装器，主要设计用来作为关联容器（如 std::map）的键。*/
            auto type_index = std::type_index(typeid(T));
            //如果组件已经存在，则直接返回组件 （避免重复添加）
            if (hasComponent<T>()) 
            {
                return getComponent<T>();
            }
            //如果不存在 /* std::forward -- 用于实现完美转发。传递多个参数的时候使用...标识 */
            auto new_component = std::make_unique<T>(std::forward<Args>(args)...);
            T* ptr= new_component.get(); //先获取指针方便返回
            new_component->setOwner(this); //设置组件的所有者
            components_[type_index] = std::move(new_component); //将组件添加到组件列表
            ptr->init(); //初始化组件
            spdlog::debug("GameObject::addComponent: {} ;added component: {}", name_, typeid(T).name());
            return ptr; //返回组件指针
        }
        
        
        /**
     * @brief 获取组件
     * 
     * @tparam T 组件类型
     * @return 组件指针
     */
        template<typename T>
        T* getComponent() const
        {
            static_assert(std::is_base_of_v<engine::component::Component, T>, "T 必须继承自 Component");
            auto type_index = std::type_index(typeid(T));
            auto it = components_.find(type_index);
            //返回组件指针
            if (it != components_.end())
            {
                return static_cast<T*>(it->second.get());
            }
            return nullptr;
        }
        
        /**
     * @brief 检查是否存在组件
     * 
     * @tparam T 组件类型
     * @return 是否存在组件
     */
        template<typename T>
        bool hasComponent() const
        {
            static_assert(std::is_base_of_v<engine::component::Component, T>, "T 必须继承自 Component");
            // contains方法为 C++20 新增
            return components_.contains(std::type_index(typeid(T)));
        }
        
        /**
    * @brief 移除组件
    * 
    * @tparam T 组件类型
    */
        template <typename T>
        void removeComponent() {
            static_assert(std::is_base_of_v<engine::component::Component, T>, "T 必须继承自 Component");
            auto type_index = std::type_index(typeid(T));
            auto it = components_.find(type_index);
            if (it != components_.end()) {
                it->second->clean();
                components_.erase(it);
            }
        }

        
        void update(float delta_time, engine::core::Context& context);               ///< @brief 更新所有组件
        void render(engine::core::Context& context);                                ///< @brief 渲染所有组件
        void clean();                                                               ///< @brief 清理所有组件
        void handleInput(engine::core::Context& context);                           ///< @brief 处理输入
    };
}

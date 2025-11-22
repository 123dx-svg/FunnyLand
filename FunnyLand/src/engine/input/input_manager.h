#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <variant>
#include <SDL3/SDL_render.h>
#include <glm/vec2.hpp>

namespace engine::core {
    class Config;
}

namespace engine::input
{
    enum class ActionState
    {
        INACTIVE,
        PRESSED_THIS_FRAME,
        HELD_DOWN,
        RELEASED_THIS_FRAME,
    };
    
    /**
     * @brief 输入管理器类，负责处理输入事件和动作状态。
     * 
     * 该类管理输入事件，将按键转换为动作状态，并提供查询动作状态的功能。
     * 它还处理鼠标位置的逻辑坐标转换。
     */
    class InputManager final
    {
    
    private:
        SDL_Renderer* sdl_renderer_;    //用于获取逻辑坐标
        
        std::unordered_map<std::string,std::vector<std::string>> actions_to_keyname_map_; ///< @brief 存储动作名称到按键名称列表的映射
        std::unordered_map<std::variant<SDL_Scancode,Uint32>, std::vector<std::string>> input_to_actions_map_;   ///< @brief 从输入（SDL_Scancode 或 Uint32）到关联的动作名称列表
        
        std::unordered_map<std::string,ActionState> action_states_; ///< @brief 存储每个动作的当前状态
        
        bool should_quit_;//是否退出标记
        glm::vec2 mouse_position_; ///< @brief 鼠标当前位置（屏幕坐标）
        
    public:
        /**
         * @brief 构造函数
         * @param sdl_renderer 指向 SDL_Renderer 的指针
         * @param config 配置对象
         * @throws std::runtime_error 如果任一指针为 nullptr。
         */
        InputManager(SDL_Renderer* sdl_renderer, const engine::core::Config* config);

        void update();                                    ///< @brief 更新输入状态，每轮循环最先调用
        
        //动作状态检查
        bool isActionDown(const std::string& action_name) const;  ///< @brief 检查指定动作是否当前正在被按下或保持按下
        
        bool isActionPressed(const std::string& action_name) const; ///< @brief 检查指定动作是否在当前帧被按下
        bool isActionReleased(const std::string& action_name) const; ///< @brief 检查指定动作是否在当前帧被释放
        
        bool shouldQuit() const;//查询退出状态
        void setShouldQuit(bool shouldQuit); ///< @brief 设置是否退出标记
        
        glm::ivec2 getMousePosition() const;//获取鼠标当前位置（屏幕坐标）
        glm::vec2 getLogicalMousePosition() const;//获取鼠标当前位置（逻辑坐标）
        
    private:
        void processEvent(const SDL_Event& event);//处理SDL事件 将按键转化为动作状态
        
        void initializeMapping(const engine::core::Config* config);//初始化按键映射表
        
        void updateActionStates(const std::string& action_name,bool is_input_active,bool is_repeat_event);//辅助更新动作状态
        
        SDL_Scancode scancodeFromString(const std::string& key_name);//将按键名称字符串转换为SDL_Scancode

        Uint32 mouseButtonUint32FromString(const std::string& button_name);//将鼠标按钮名称字符串转换为Uint32
    };

    
}
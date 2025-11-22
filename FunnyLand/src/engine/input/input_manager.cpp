#include "input_manager.h"

#include <spdlog/spdlog.h>

#include "../core/config.h"


namespace engine::input
{
    InputManager::InputManager(SDL_Renderer* sdl_renderer, const engine::core::Config* config):
        sdl_renderer_(sdl_renderer)
    {
        if (!sdl_renderer_)
        {
            spdlog::error("输入管理器:SDL渲染器为空");
            throw std::runtime_error("输入管理器:SDL渲染器为空");
        }
        initializeMapping(config);
        
        //获取鼠标初始位置
        float x,y;
        SDL_GetMouseState( &x, &y );
        mouse_position_ = {x,y};
        spdlog::trace("初始鼠标位置:({},{})",mouse_position_.x,mouse_position_.y);
    }

    void InputManager::update()
    {
        //根据上一帧的值更新默认的动作状态
        for (auto& [action_name,state] : action_states_)
        {
            if (state == ActionState::PRESSED_THIS_FRAME)
                state = ActionState::HELD_DOWN;//当某个按键一直是按下状态时就不生成SDL_Event
            
            else if (state == ActionState::RELEASED_THIS_FRAME)
                state = ActionState::INACTIVE;
        }
        
        //处理所有待处理的SDL事件
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            processEvent(event);
        }
    }

    bool InputManager::isActionDown(const std::string& action_name) const
    {
        if (auto it = action_states_.find(action_name); it != action_states_.end())
        {
            return it->second == ActionState::PRESSED_THIS_FRAME || it->second == ActionState::HELD_DOWN;
        }
        return false;
    }

    bool InputManager::isActionPressed(const std::string& action_name) const
    {
        if (auto it = action_states_.find(action_name); it != action_states_.end())
        {
            return it->second == ActionState::PRESSED_THIS_FRAME;
        }
        return false;
    }

    bool InputManager::isActionReleased(const std::string& action_name) const
    {
        if (auto it = action_states_.find(action_name); it != action_states_.end())
        {
            return it->second == ActionState::RELEASED_THIS_FRAME;
        }
        return false;
    }

    bool InputManager::shouldQuit() const
    {
        return should_quit_;
    }

    void InputManager::setShouldQuit(bool shouldQuit)
    {
        should_quit_ = shouldQuit;
    }

    glm::ivec2 InputManager::getMousePosition() const
    {
        return mouse_position_;
    }

    glm::vec2 InputManager::getLogicalMousePosition() const
    {
        glm::vec2 logical_pos;
        // 通过窗口坐标获取渲染坐标（逻辑坐标）
        SDL_RenderCoordinatesFromWindow(sdl_renderer_,mouse_position_.x,mouse_position_.y,&logical_pos.x,&logical_pos.y);
        return logical_pos;
    }

    void InputManager::processEvent(const SDL_Event& event)
    {
        switch (event.type)
        {
        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP:
            {
                SDL_Scancode scancode = event.key.scancode; //获取按键的的scancode
                bool is_down = event.key.down;
                bool is_repeat = event.key.repeat;
                
                auto it = input_to_actions_map_.find(scancode);
                if (it != input_to_actions_map_.end())//如果按键中有对应的action
                {
                    const std::vector<std::string>& associate_actions = it->second;
                    for (const auto& action_name : associate_actions)
                    {
                        updateActionStates(action_name, is_down, is_repeat);
                    }
                }
                break;
            }
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
            {
                Uint32 button = event.button.button; //获取鼠标按钮索引
                bool is_down = event.button.down;
                auto it = input_to_actions_map_.find(button);
                if (it != input_to_actions_map_.end())//如果鼠标按钮中有对应的action
                {
                    const std::vector<std::string>& associate_actions = it->second;
                    for (const auto& action_name : associate_actions)
                    {
                        //鼠标不考虑重复点击
                        updateActionStates(action_name, is_down, false);//action状态
                    }
                }
                //点击时更新鼠标位置
                mouse_position_ = {event.button.x,event.button.y};
                break;
            }
        case SDL_EVENT_MOUSE_MOTION: //鼠标移动
            mouse_position_ = {event.motion.x,event.motion.y};
            break;
        case  SDL_EVENT_QUIT:
            should_quit_ = true;
            break;
        default:
            break;
    }
}
    
        
    

    void InputManager::initializeMapping(const engine::core::Config* config)
    {
        spdlog::trace("初始化输入映射...");
        if (!config)
        {
            spdlog::error("输入管理器:配置为空");
            throw std::runtime_error("输入管理器:配置为空");
        }
        
        actions_to_keyname_map_ = config->input_mappings_; // 获取配置中的输入映射（动作 -> 按键名称）
        input_to_actions_map_.clear();
        action_states_.clear();
        
        // 如果配置中没有定义鼠标按钮动作(通常不需要配置)，则默认映射为鼠标按钮索引 用于UI
        if (actions_to_keyname_map_.find("MouseLeftClick") == actions_to_keyname_map_.end())
        {
            spdlog::debug("配置中没有定义 'MouseLeftClick' 动作,添加默认映射到 'MouseLeft'.");
            actions_to_keyname_map_["MouseLeftClick"] = {"MouseLeft"};//缺失就补充
        }
        if (actions_to_keyname_map_.find("MouseRightClick") == actions_to_keyname_map_.end())
        {
            spdlog::debug("配置中没有定义 'MouseRightClick' 动作,添加默认映射到 'MouseRight'.");
            actions_to_keyname_map_["MouseRightClick"] = {"MouseRight"};//缺失就补充
        }
        
        //遍历 动作-> 按键名称的映射
        for (const auto& [action_name, key_names] : actions_to_keyname_map_)
        {
            //每个动作对应一个动作状态,初始化为INACTIVE
            action_states_[action_name] = ActionState::INACTIVE;
            spdlog::trace("动作映射：{}",action_name);
            
            //设置按键->动作的映射
            for (const auto& key_name : key_names)
            {
                SDL_Scancode scancode = scancodeFromString(key_name);//根据名称获取SDL_Scancode
                Uint32 mouse_button    = mouseButtonUint32FromString(key_name);//根据名称获取鼠标按钮索引
                
                //其他输入类型
                
                if (scancode!=SDL_SCANCODE_UNKNOWN)//如果有效就添加
                {
                    input_to_actions_map_[scancode].push_back(action_name);
                    spdlog::trace("  映射按键: {} (Scancode: {}) 到动作: {}", key_name, static_cast<int>(scancode), action_name);
                }
                else if (mouse_button!=0)//如果鼠标按钮索引有效就添加
                {
                    input_to_actions_map_[mouse_button].push_back(action_name);
                    spdlog::trace("  映射鼠标按键: {} (MouseButton: {}) 到动作: {}", key_name, static_cast<int>(mouse_button), action_name);
                }
                else
                {
                    spdlog::warn("输入映射警告: 未知键或按钮名称 '{}' 用于动作 '{}'.", key_name, action_name);
                }
            }
        }
        spdlog::trace("输入映射初始化完成.");
    }

    void InputManager::updateActionStates(const std::string& action_name, bool is_input_active, bool is_repeat_event)
    {
        auto it = action_states_.find(action_name);
        if (it == action_states_.end())
        {
            spdlog::warn("尝试更新未注册的动作状态: {}", action_name);
            return;
        }
        if (is_input_active)
        {
            if (is_repeat_event)
            {
                it->second = ActionState::HELD_DOWN;
            }
            else
            {
                it->second = ActionState::PRESSED_THIS_FRAME;
            }
        }
        else
        {
            it->second = ActionState::RELEASED_THIS_FRAME;
        }
    }

    SDL_Scancode InputManager::scancodeFromString(const std::string& key_name)
    {
        return SDL_GetScancodeFromName(key_name.c_str());
    }

    Uint32 InputManager::mouseButtonUint32FromString(const std::string& button_name)
    {
        if (button_name == "MouseLeft") return SDL_BUTTON_LEFT;
        if (button_name == "MouseMiddle") return SDL_BUTTON_MIDDLE;
        if (button_name == "MouseRight") return SDL_BUTTON_RIGHT;
        // SDL 还定义了 SDL_BUTTON_X1 和 SDL_BUTTON_X2
        if (button_name == "MouseX1") return SDL_BUTTON_X1;
        if (button_name == "MouseX2") return SDL_BUTTON_X2;
        return 0; // 0 不是有效的按钮值，表示无效
    }
}

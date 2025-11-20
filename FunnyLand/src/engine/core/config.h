#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <nlohmann/json_fwd.hpp>    // nlohmann_json 提供的前向声明


namespace engine::core
{
    /**
    * @brief 管理应用程序的配置设置。
    *
    * 提供配置项的默认值，并支持从 JSON 文件加载/保存配置。
    * 如果加载失败或文件不存在，将使用默认值。
    */   
    class Config final
    {
    public:
        //默认值配置 为了方便配置设置为公有
        //窗口设置
        std::string window_title_ = "FunnyLand";
        int window_width_ = 1280;
        int window_height_ = 720;
        bool window_resizable_ = true ;
        
        //图形设置
        bool vsync_enabled_ = true ;//是否启用垂直同步
        
        //性能设置
        int target_fps_ = 144;
        
        //音屏设置
        float music_volume_ = 0.5f;
        float sound_volume_ = 0.5f;
        
        //存储动作名称到SDL Scancode名称列表映射
        std::unordered_map<std::string,std::vector<std::string>> input_mappings_
        {
            // 提供一些合理的默认值，以防配置文件加载失败或缺少此部分
            {"move_left",{"Left","A"}},
            {"move_right",{"Right","D"}},
            {"move_up",{"UP","W"}},
            {"move_down",{"DOWN","S"}},
            {"jump",{"SPACE","J"}},
            {"pause",{"ESCAPE","P"}},
        {"attack",{"MouseLeft","K"}}
        };
        
        //构造函数
        explicit  Config(const std::string& config_file_path);
        
        //禁用复制构造函数和赋值运算符
        Config(const Config&) = delete;
        Config(Config&&) = delete;
        Config& operator=(const Config&) = delete;
        Config& operator=(Config&&) = delete;
        
        bool loadFromFile(const std::string& config_file_path); ///< @brief 从指定的 JSON 文件加载配置。成功返回 true，否则返回 false。
        [[nodiscard]] bool saveToFile(const std::string& config_file_path); ///< @brief 将当前配置保存到指定的 JSON 文件。成功返回 true，否则返回 false。
        
    private:
        void fromJson(const nlohmann::json& j); ///< @brief 从 JSON 对象加载配置。
        nlohmann::ordered_json toJson(); ///< @brief 将当前配置转换为 JSON 对象。 按顺序
    };

}
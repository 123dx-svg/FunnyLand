#include "config.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include "spdlog/spdlog.h"

namespace engine::core
{
    Config::Config(const std::string& config_file_path)
    {
        loadFromFile(config_file_path);
    }

    bool Config::loadFromFile(const std::string& config_file_path)
    {
        std::ifstream file(config_file_path);
        if (!file.is_open())
        {
            spdlog::warn("没找到配置文件: {}", config_file_path);
            if (!saveToFile(config_file_path))
            {
                spdlog::error("创建配置文件失败: {}", config_file_path);
                return false;
            }
            return false;//文件不存在使用默认配置
        }

        try
        {
            nlohmann::json j;
            file >> j;
            fromJson(j);
            spdlog::info("成功加载配置文件: {}", config_file_path);
            return true;
        }
        catch (const std::exception& e)
        {
            spdlog::error("加载配置文件失败: {} ({}), 使用默认配置", config_file_path, e.what());
            return false;
        }
    }

    bool Config::saveToFile(const std::string& config_file_path)
    {
        std::ofstream file(config_file_path);
        if (!file.is_open())
        {
            spdlog::error("无法打开文件: {}", config_file_path);
            return false;
        }

        try
        {
            nlohmann::ordered_json j  = toJson();
            file << j.dump(4);
            spdlog::info("成功保存配置文件: {}", config_file_path);
            return true;
        }
        catch (const std::exception& e)
        {
            spdlog::error("保存配置文件失败: {} ({}), 配置文件内容可能损坏", config_file_path, e.what());
            
        }
        return false;
    }

    void Config::fromJson(const nlohmann::json& j)
    {
        if (j.contains("window"))
        {
            const auto& window_config = j["window"];
            window_title_ = window_config.value("title", window_title_);
            window_width_ = window_config.value("width", window_width_);
            window_height_ = window_config.value("height", window_height_);
            window_resizable_ = window_config.value("resizable", window_resizable_);
        }
        if (j.contains("graphics"))
        {
            const auto& graphics_config = j["graphics"];
            vsync_enabled_ = graphics_config.value("vsync", vsync_enabled_);
        }
        if (j.contains("performance"))
        {
            const auto& performance_config = j["performance"];
            target_fps_ = performance_config.value("target_fps", target_fps_);
            if (target_fps_ < 0)
            {
                spdlog::warn("目标FPS不能小于0，已设置为默认值: {}", target_fps_);
                target_fps_ = 0;
            }
        }
        if (j.contains("audio"))
        {
            const auto& audio_config = j["audio"];
            music_volume_ = audio_config.value("music_volume", music_volume_);
            sound_volume_ = audio_config.value("sound_volume", sound_volume_);
        }
        //json加载 按键绑定
        if (j.contains("input_mappings")&& j["input_mappings"].is_object())
        {
            const auto& input_mappings_config = j["input_mappings"];
            try
            {
                //直接从json对象转换为map
                auto input_mappings = input_mappings_config.get<std::unordered_map<std::string, std::vector<std::string>>>();
                //将map赋值给成员变量
                input_mappings_ = std::move(input_mappings);
                spdlog::trace("成功加载按键绑定配置");
            }catch (const std::exception& e)
            {
                spdlog::warn("配置加载警告：解析 'input_mappings' 时发生异常。使用默认映射。错误：{}", e.what());
            }
        }
        else
        {
            spdlog::trace("配置跟踪：未找到 'input_mappings' 部分或不是对象。使用头文件中定义的默认映射。");
        }
    }

    nlohmann::ordered_json Config::toJson()
    {
        return nlohmann::ordered_json{
            {"window", {
                {"title", window_title_},
                {"width", window_width_},
                {"height", window_height_},
                {"resizable", window_resizable_}
            }},
            {"graphics", {
                {"vsync", vsync_enabled_}
            }},
            {"performance", {
                {"target_fps", target_fps_}
            }},
            {"audio", {
                {"music_volume", music_volume_},
                {"sound_volume", sound_volume_}
            }},
            {"input_mappings", input_mappings_}
        };
    }
}

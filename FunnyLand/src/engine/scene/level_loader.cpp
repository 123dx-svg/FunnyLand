#include "level_loader.h"
#include "../component/parallax_component.h"
#include "../component/transform_component.h"
#include "../object/game_object.h"
#include "../scene/scene.h"
#include "../core/context.h"
#include "../render/sprite.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <spdlog/spdlog.h>
#include <glm/vec2.hpp>
#include <filesystem>


namespace engine::scene
{
    bool LevelLoader::loadLevel(const std::string& map_path, Scene& scene)
    {
        map_path_ = map_path;
        std::ifstream map_file(map_path_);
        //1、加载json文件
        if (!map_file.is_open())
        {
            spdlog::error("无法打开地图文件: {}", map_path_);
            return false;
        }
        //2、解析json数据
        nlohmann::json json_data;
        try
        {
            map_file >> json_data;
        }
        catch (const nlohmann::json::parse_error& e)
        {
            spdlog::error("解析地图文件 {} 时出错: {}", map_path_, e.what());
            return false;
        }
        
        //3、加载图层数据
        if (!json_data.contains("layers") || !json_data["layers"].is_array()) //检查是否包含图层数组
        {
            spdlog::error("地图文件 {} 中没有找到图层数组", map_path_);
            return false;
        }

        for (const auto& layer_json : json_data["layers"])
        {
            //获取个图层对象中的类型 type 字段
            std::string layer_type = layer_json.value("type", "none");
            if (!layer_json.value("visible", true))
            {
                spdlog::info("图层 {} 不可见", layer_json.value("name", "无名称"));
                continue;
            }
            if (layer_type == "imagelayer")
            {
                loadImageLayer(layer_json, scene);
            }
            else if (layer_type == "tilelayer")
            {
                loadTileLayer(layer_json, scene);
            }
            else if (layer_type == "objectgroup")
            {
                loadObjectLayer(layer_json, scene);
            }
            else
            {
                spdlog::warn("未知图层类型 {}，跳过加载", layer_type);
            }
        }
        
        spdlog::info("关卡加载完成： {}", map_path_);
        return true;
        
    }

    void LevelLoader::loadImageLayer(const nlohmann::json& layer_json, Scene& scene)
    {
        // 获取纹理相对路径 （会自动处理'\/'符号）
        const std::string& image_path = layer_json.value("image", "");
        if (image_path.empty())
        {
            spdlog::error("图层 '{}' 缺少 'image' 属性。", layer_json.value("name", "Unnamed"));
            return;
        }
        
        auto texture_id = resolvePath(image_path);
        
        //获取图层偏移量 json中没有则代表未设置用默认值
        const glm::vec2 offset =  glm::vec2(layer_json.value("offsetx", 0.0f), layer_json.value("offsety", 0.0f));
        
        //获取视差因子极其重复标志
        const glm::vec2 scroll_factor = glm::vec2(layer_json.value("parallaxx", 1.0f), layer_json.value("parallaxy", 1.0f));
        const glm::bvec2 repeat = glm::bvec2(layer_json.value("repeatx", false), layer_json.value("repeaty", false));
        
        //获取图层名称
        const std::string& layer_name = layer_json.value("name", "Unnamed");
        /*  可用类似方法获取其它各种属性，这里我们暂时用不上 */
        
        //创建游戏对象
        auto game_obj = std::make_unique<engine::object::GameObject>(layer_name);
        
        //添加组件
        game_obj->addComponent<engine::component::TransformComponent>(offset);
        game_obj->addComponent<engine::component::ParallaxComponent>(texture_id,scroll_factor, repeat);
        
        //将创建好的 GameObject 添加到场景中 （一定要用std::move，否则传递的是左值）
        scene.addGameObject(std::move(game_obj)); 
        spdlog::info("加载图片图层 '{}' 完成。", layer_name);
    }

    void LevelLoader::loadTileLayer(const nlohmann::json& layer_json, Scene& scene)
    {
    }

    void LevelLoader::loadObjectLayer(const nlohmann::json& layer_json, Scene& scene)
    {
    }

    std::string LevelLoader::resolvePath(std::string image_path)
    {
        try
        {
            // 获取地图文件的父目录（相对于可执行文件） “assets/maps/level1.tmj” -> “assets/maps”
            auto map_dir = std::filesystem::path(map_path_).parent_path();
            // 合并路径（相对于可执行文件）并返回。 /* std::filesystem::canonical：解析路径中的当前目录（.）和上级目录（..）导航符，
            /*  得到一个干净的路径 */
            auto final_path = std::filesystem::canonical(map_dir / image_path);
            return final_path.string();
        }
        catch (const std::exception& e) {
            spdlog::error("解析路径失败: {}", e.what());
            return image_path;
        }
    }
}

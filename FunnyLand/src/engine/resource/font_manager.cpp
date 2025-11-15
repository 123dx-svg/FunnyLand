#include "font_manager.h"

#include <stdexcept>
#include <spdlog/spdlog.h>


namespace engine::resource
{
    FontManager::FontManager()
    {
        if (!TTF_WasInit()&&!TTF_Init())
        {
            throw std::runtime_error("FontManager 构造函数：初始化 SDL_ttf 失败");
        }
        spdlog::trace("FontManager 构造完成");
    }

    FontManager::~FontManager()
    {
        if (!fonts_.empty())
        {
            spdlog::debug("不为空 ，清除所有字体");
            clearFonts();
        }
        TTF_Quit();
        spdlog::trace("FontManager 析构完成");
    }

    TTF_Font* FontManager::loadFont(const std::string& file_path, int point_size)
    {
        if (point_size <= 0 )
        {
            spdlog::error("无法加载字体 {} ，点大小必须大于 0", file_path);
            return nullptr;
        }
        
        //创建映射表
        FontKey key = FontKey(file_path, point_size);
        
        //检查是否已加载
        auto it = fonts_.find(key);
        if (it != fonts_.end())
        {
            return it->second.get();
        }
        //缓存中没有，加载字体
        spdlog::debug("加载字体 {} ，点大小 {}", file_path, point_size);
        TTF_Font* raw_font = TTF_OpenFont(file_path.c_str(), static_cast<float>(point_size));
        if (!raw_font)
        {
            throw std::runtime_error("FontManager loadFont 函数：加载字体失败");
            return nullptr;
        }
        fonts_.emplace(key, std::unique_ptr<TTF_Font,SDLFontDeleter>(raw_font));
        spdlog::debug("成功加载并缓存字体: {} ，点大小 {}", file_path, point_size);
        return raw_font;
        
        
        
    }

    TTF_Font* FontManager::getFont(const std::string& file_path, int point_size)
    {
        FontKey key = FontKey(file_path, point_size);
        auto it = fonts_.find(key);
        if (it != fonts_.end())
        {
            return it->second.get();
        }
        spdlog::warn("无法获取缓存字体 {} ，点大小 {} ，未加载", file_path, point_size);
        return loadFont(file_path, point_size);
    }

    void FontManager::unloadFont(const std::string& file_path, int point_size)
    {
        FontKey key = FontKey(file_path, point_size);
        auto it = fonts_.find(key);
        if (it != fonts_.end())
        {
            spdlog::debug("成功卸载字体 {} ，点大小 {}", file_path, point_size);
            fonts_.erase(it);
        }
        else
        {
            spdlog::warn("无法卸载字体 {} ，点大小 {} ，未加载", file_path, point_size);
        }
    }

    void FontManager::clearFonts()
    {
        if (!fonts_.empty())
        {
            spdlog::debug("成功清除所有字体{}个", fonts_.size());
            fonts_.clear();//智能指针会自动调用删除器，释放字体资源
        }
    }
}

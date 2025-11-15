#pragma once
#include <memory>       // 用于 std::unique_ptr
#include <string>       // 用于 std::string
#include <unordered_map> // 用于 std::unordered_map
#include <utility>      // 用于 std::pair
#include <functional>   // 用于 std::hash

#include <SDL3_ttf/SDL_ttf.h> // SDL_ttf 主头文件


namespace engine::resource
{
  using FontKey = std::pair<std::string, int>;  //将字体路径和字号合并成一个单元
  
  //FontKey的自定义哈希函数
  struct FontKeyHash
  {
    std::size_t operator()(const FontKey& key) const
    {
        //异或运算符:将字符串和整数的哈希值合并
        return std::hash<std::string>()(key.first) ^ std::hash<int>()(key.second); //教unordered_map 如何哈希FontKey
    }
  };
  
  
  
/**
 * @brief 管理 SDL_ttf 字体资源（TTF_Font）。
 *
 * 提供字体的加载和缓存功能，通过文件路径和点大小来标识。
 * 构造失败会抛出异常。仅供 ResourceManager 内部使用。
 */
class FontManager
{
    friend class ResourceManager;
    
private:
    //SDL_Font的删除器函数对象,用于智能指针管理
    struct SDLFontDeleter
    {
        void operator()(TTF_Font* font) const
        {
            if (font)
            {
                TTF_CloseFont(font);
            }
        }
    };
    // 字体存储（FontKey -> TTF_Font）。  
    // unordered_map 的键需要能转换为哈希值，对于基础数据类型，系统会自动转换
    // 但是对于对于自定义类型（系统无法自动转化），则需要提供自定义哈希函数（第三个模版参数）
    std::unordered_map<FontKey, std::unique_ptr<TTF_Font, SDLFontDeleter>, FontKeyHash> fonts_;

public:
    FontManager();
    
    ~FontManager();//清理资源并关闭SDL_ttf
    
    //只需要一个实例
    FontManager(const FontManager&) = delete;
    FontManager(FontManager&&) = delete;
    FontManager& operator=(const FontManager&) = delete;
    FontManager& operator=(FontManager&&) = delete;
    
private://仅允许ResourceManager访问
    
    TTF_Font* loadFont(const std::string& file_path, int point_size);
    TTF_Font* getFont(const std::string& file_path, int point_size);
    void unloadFont(const std::string& file_path, int point_size);
    void clearFonts();//清除所有字体
    
    
};
}

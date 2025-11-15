#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <SDL3_mixer/SDL_mixer.h>

namespace engine::resource
{
    /**
 * @brief 管理 SDL_mixer 音效 (Mix_Chunk) 和音乐 (Mix_Music)。
 *
 * 提供音频资源的加载和缓存功能。构造失败时会抛出异常。
 * 仅供 ResourceManager 内部使用。
 */
    class AudioManager final
    {
        
        friend class ResourceManager;
        
    private:
        //自定义Mix_Chunk删除器
        struct SDLMixChunkDeleter
        {
            void operator()(Mix_Chunk* chunk) const
            {
                if (chunk)
                {
                    Mix_FreeChunk(chunk);
                }
            }
        };
        //自定义Mix_Music删除器
        struct SDLMixMusicDeleter
        {
            void operator()(Mix_Music* music) const
            {
                if (music)
                {
                    Mix_FreeMusic(music);
                }
            }
        };
        
        //音效缓存
        std::unordered_map<std::string, std::unique_ptr<Mix_Chunk, SDLMixChunkDeleter>> sounds_;
        //音乐缓存
        std::unordered_map<std::string, std::unique_ptr<Mix_Music, SDLMixMusicDeleter>> musics_;
        
    public:
        AudioManager();
        ~AudioManager();//析构函数,清理所有音效和音乐资源
        
        //只需要一个实例
        AudioManager(const AudioManager&) = delete;
        AudioManager(AudioManager&&) = delete;
        AudioManager& operator=(const AudioManager&) = delete;
        AudioManager& operator=(AudioManager&&) = delete;
    private://只有ResourceManager可以访问
        
        //chunk相关
        Mix_Chunk* loadSound(const std::string& file_path);//加载音效
        Mix_Chunk* getSound(const std::string& file_path);//尝试获取的音效指针，如果不存在就尝试加载
        void unloadSound(const std::string& file_path);//卸载音效
        void clearSounds();//清除所有音效
        
        //music相关
        Mix_Music* loadMusic(const std::string& file_path);//加载音乐
        Mix_Music* getMusic(const std::string& file_path);//尝试获取的音乐指针，如果不存在就尝试加载
        void unloadMusic(const std::string& file_path);//卸载音乐
        void clearMusics();//清除所有音乐
        
        void clearAudio();//清除所有音效和音乐资源
    };
}

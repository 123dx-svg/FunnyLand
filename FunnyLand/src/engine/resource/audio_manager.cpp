#include "audio_manager.h"
#include <stdexcept>
#include <spdlog/spdlog.h>

namespace engine::resource
{
    AudioManager::AudioManager()
    {
        //初始化SDL_mixer ogg和mp3
        MIX_InitFlags flags = MIX_INIT_OGG|MIX_INIT_MP3;
        if ((Mix_Init(flags)&flags) != flags)
        {
            throw std::runtime_error("AudioManager 构造函数：初始化 SDL_mixer 失败");
        }
        //打开音频设备
        if (!Mix_OpenAudio(0,nullptr))
        {
            Mix_Quit();//如果失效先清理再抛出异常
            throw std::runtime_error("AudioManager 构造函数：打开音频设备失败");
        }
        spdlog::trace("AudioManager 构造完成");
    }

    AudioManager::~AudioManager()
    {
        //停止播放所有音效
        Mix_HaltChannel(-1);//-1表示所有通道
        Mix_HaltMusic();
        
        //清理资源
        clearAudio();
        clearMusics();
        
        //关闭音频设备
        Mix_CloseAudio();
        //注销SDL_mixer
        Mix_Quit();
        spdlog::trace("AudioManager 析构完成");
    }

    Mix_Chunk* AudioManager::loadSound(const std::string& file_path)
    {
        auto it = sounds_.find(file_path);
        if (it != sounds_.end())
        {
            return it->second.get();
        }
        
        //加载音效
        spdlog::debug("加载音效 {}", file_path);
        Mix_Chunk* raw_chunk = Mix_LoadWAV(file_path.c_str());
        if (!raw_chunk)
        {
            throw std::runtime_error("AudioManager loadSound 函数：加载音效失败");
            return nullptr;
        }
        sounds_.emplace(file_path, std::unique_ptr<Mix_Chunk,SDLMixChunkDeleter>(raw_chunk));
        spdlog::debug("成功加载并缓存音效: {}", file_path);
        return raw_chunk;
    }

    Mix_Chunk* AudioManager::getSound(const std::string& file_path)
    {
        auto it = sounds_.find(file_path);
        if (it != sounds_.end())
        {
            return it->second.get();
        }
        spdlog::warn("未找到音效: {}", file_path);
        
        return loadSound(file_path);
    }

    void AudioManager::unloadSound(const std::string& file_path)
    {
        auto it = sounds_.find(file_path);
        if (it != sounds_.end())
        {
            spdlog::debug("成功卸载音效: {}", file_path);
            sounds_.erase(it);
        }
        else
        {
            spdlog::warn("尝试卸载不存在的音效: {}", file_path);
        }
    }

    void AudioManager::clearSounds()
    {
        if (!sounds_.empty())
        {
            spdlog::debug("正在清除所有{}个音效", sounds_.size());
            sounds_.clear();
        }
            
    }

    Mix_Music* AudioManager::loadMusic(const std::string& file_path)
    {
        auto it = musics_.find(file_path);
        if (it != musics_.end())
        {
            return it->second.get();
        }
        
        //加载音乐
        spdlog::debug("加载音乐 {}", file_path);
        Mix_Music* raw_music = Mix_LoadMUS(file_path.c_str());
        if (!raw_music)
        {
            throw std::runtime_error("AudioManager loadMusic 函数：加载音乐失败");
            return nullptr;
        }
        musics_.emplace(file_path, std::unique_ptr<Mix_Music,SDLMixMusicDeleter>(raw_music));
        spdlog::debug("成功加载并缓存音乐: {}", file_path);
        return raw_music;
    }

    Mix_Music* AudioManager::getMusic(const std::string& file_path)
    {
        auto it = musics_.find(file_path);
        if (it != musics_.end())
        {
            return it->second.get();
        }
        spdlog::warn("未找到音乐: {}", file_path);
        
        return loadMusic(file_path);
    }

    void AudioManager::unloadMusic(const std::string& file_path)
    {
        auto it = musics_.find(file_path);
        if (it != musics_.end())
        {
            spdlog::debug("成功卸载音乐: {}", file_path);
            musics_.erase(it);
        }
        else
        {
            spdlog::warn("尝试卸载不存在的音乐: {}", file_path);
        }
    }

    void AudioManager::clearMusics()
    {
        if (!musics_.empty())
        {
            spdlog::debug("正在清除所有{}个音乐", musics_.size());
            musics_.clear();
        }
    }

    void AudioManager::clearAudio()
    {
        clearSounds();
        clearMusics();
    }
}

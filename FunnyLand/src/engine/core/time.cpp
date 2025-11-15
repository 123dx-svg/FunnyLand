#include "time.h"
#include <spdlog/spdlog.h>
#include <SDL3/SDL_timer.h>  

namespace engine::core
{
    
Time::Time()
{
    //初始化last_time_和frame_start_time_为当前时间 避免第一帧的delta_time为过大
    last_time_ = SDL_GetTicksNS();
    frame_start_time_ = last_time_;
    spdlog::trace("Time::init()  last_time_: {}", last_time_);
}

void Time::update()
{
    frame_start_time_ = SDL_GetTicksNS();//当前帧的开始时间
    auto current_delta_time = static_cast<double>(frame_start_time_ - last_time_) / 1000000000.0;
    if (target_frame_time_ > 0.0)//如果设置了目标帧率 则限制帧率，否则直接使用当前帧时间
        limitFrameRate(current_delta_time);
    else
        delta_time_ = current_delta_time;
    
    last_time_ = SDL_GetTicksNS();//可以理解为上一帧的结束时间
}

float Time::getDeltaTime() const
{
    return static_cast<float>(delta_time_ * time_scale_);
}

float Time::getUnscaledDeltaTime() const
{
    return static_cast<float>(delta_time_);
}

void Time::setTargetFPS(int fps)
{
    if (fps<0)
    {
        spdlog::warn("目标帧率必须大于等于0");
        target_fps_ = 0;
    }else{
        target_fps_ = fps;
    }

    if (target_fps_>0)
    {
        target_frame_time_ = 1.0 / static_cast<double>(target_fps_);
        spdlog::info("TargetFPS: {} (frametime: {:.6f}s)", target_fps_, target_frame_time_);
    }else{
        target_frame_time_ = 0.0;
        spdlog::info("不限帧率");
    }
}

int Time::getTargetFPS() const
{
    return target_fps_;
}

double Time::getTimeScale() const
{
    return time_scale_;
}

void Time::setTimeScale(double scale)
{
    if (scale<0.0)
    {
        spdlog::warn("时间缩放必须大于等于0");
        scale = 0.0;
    }
    time_scale_ = scale;
}

void Time::limitFrameRate(double current_delta_time)
{
    //当前帧耗费时间小于目标帧时间 则等待剩余时间
    if (current_delta_time < target_frame_time_)
    {
        double time_to_wait = target_frame_time_ - current_delta_time;
        Uint64 ns_to_wait = static_cast<Uint64>(time_to_wait * 1000000000.0);
        SDL_DelayNS(ns_to_wait);
        delta_time_ = static_cast<double>(SDL_GetTicksNS() - last_time_) / 1000000000.0;
    }
    
}
}
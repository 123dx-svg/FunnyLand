#pragma once
#include <SDL3/SDL_stdinc.h>


namespace engine::core
{
    
/**
 * @brief 管理游戏循环中的时间，计算帧间时间差 (DeltaTime)。
 *
 * 使用 SDL 的高精度性能计数器来确保时间测量的准确性。
 * 提供获取缩放和未缩放 DeltaTime 的方法，以及设置时间缩放因子的能力。
 */
class Time final
{
    
private:
    Uint64 last_time_ = 0; // 上一帧的时间戳
    Uint64 frame_start_time_ = 0; // 当前帧的开始时间戳
    double delta_time_ = 0.0; // 帧间时间差
    double time_scale_ = 1.0; // 时间缩放因子
    
    //帧率限制相关
    int target_fps_ = 0; // 目标帧率(0表示不限制)
    double target_frame_time_ = 0.0; // 目标帧时间(秒)
    
public:
    Time();
    
    //禁用拷贝和移动构造函数
    Time(const Time&) = delete;
    Time(Time&&) = delete;
    Time& operator=(const Time&) = delete;
    Time& operator=(Time&&) = delete;
    
    /**
    * @brief 每帧开始时调用，更新内部时间状态并计算 DeltaTime。
    */
    void update(); // 更新时间
    
    /**
    * @brief 获取当前帧的 DeltaTime (缩放后的)。
    * @return 缩放后的帧间时间差 (秒)
    */
    float  getDeltaTime() const; // 获取当前帧的DeltaTime
    
    /**
    * @brief 获取当前帧的 DeltaTime (未缩放)。
    * @return 未缩放的帧间时间差 (秒)
    */
    float getUnscaledDeltaTime() const; // 获取当前帧的未缩放DeltaTime
    
    /**
    * @brief 设置目标帧率(0表示不限制)。
    * @param fps 目标帧率
    */
    void setTargetFPS(int fps); // 设置目标帧率
    
    int getTargetFPS() const;
    
    /**
    * @brief 获取当前时间缩放因子。
    * @return 时间缩放因子
    */
    double getTimeScale() const; // 获取当前时间缩放因子
    
    /**
    * @brief 设置时间缩放因子。
    * @param scale 时间缩放因子(1.0表示正常速度)
    */
    void setTimeScale(double scale); // 设置时间缩放因子
    

    
private:
    
    // 限制帧率
    void limitFrameRate(double current_delta_time);
};
}

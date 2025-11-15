#pragma once

struct SDL_Window;
struct SDL_Renderer;

namespace  engine::core
{
    class GameApp final // final 表示不能被继承
    {
    private:    
        //SDL 窗口和渲染器
        SDL_Window* window_ = nullptr;
        SDL_Renderer* renderer_ = nullptr;
        bool is_running_ = false;
        
    public:
        GameApp();
        ~GameApp();
        
        //游戏运行程序中间会调用init(),进入主循环，离开循环后调用close()
        void run();
        
        //禁用掉拷贝和移动构造函数
        GameApp(const GameApp&) = delete;
        GameApp(GameApp&&) = delete;
        GameApp& operator=(const GameApp&) = delete;
        GameApp& operator=(GameApp&&) = delete;
        
    private:
        [[nodiscard]] bool init(); //返回值必须被使用
        void handleEvents();
        void update(float delta_time);
        void render();
        void close();
        
    };
}


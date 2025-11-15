#include "game_app.h"
#include "time.h"
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#include <spdlog/spdlog.h>


namespace  engine::core
{
    
GameApp::GameApp()
{
    time_ = std::make_unique<Time>();
}


GameApp::~GameApp()
{
    if (is_running_)
    {
        spdlog::warn("GameApp销毁时,游戏仍在运行,可能会导致资源泄漏...");
        close();
    }
}
void GameApp::run()
{
    if (!init()) {
        spdlog::error("初始化失败,游戏无法运行");
        return;
    }
    
    time_->setTargetFPS(144);//设置目标帧率 后期从配置文件读取
    
    while (is_running_)
    {
        time_->update();
        float delta_time = time_->getDeltaTime();
        handleEvents();
        update(delta_time);
        render();
        
        //spdlog::info("delta time: {}", delta_time);
    }
    
    close();
}
    
bool GameApp::init()
{
    spdlog::trace("GameApp初始化...");
    // SDL初始化
    if (!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO)) {
        spdlog::error("SDL 初始化失败: {}", SDL_GetError());
        return false;
    }
    //创建窗口
    window_ = SDL_CreateWindow("FunnyLand", 1280, 720, SDL_WINDOW_RESIZABLE);
    if (!window_) {
        spdlog::error("SDL 创建窗口失败: {}", SDL_GetError());
        return false;
    }
    //创建渲染器
    renderer_ = SDL_CreateRenderer(window_, nullptr);
    if (!renderer_) {
        spdlog::error("SDL 创建渲染器失败: {}", SDL_GetError());
        return false;
    }
    is_running_ = true;
    return true;
}

void GameApp::handleEvents()
{
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            is_running_ = false;
        }
    }
}

void GameApp::update(float delta_time)
{
    
}

void GameApp::render()
{
    
}

void GameApp::close()
{
    spdlog::trace("GameApp关闭...");
    if (renderer_) {
        SDL_DestroyRenderer(renderer_);
        renderer_ = nullptr;
    }
    if (window_) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }
    SDL_Quit();
    is_running_ = false;
}
    
}

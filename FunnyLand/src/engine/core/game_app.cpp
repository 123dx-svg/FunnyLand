#include "game_app.h"
#include "time.h"
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_render.h>
#include <spdlog/spdlog.h>


namespace  engine::core
{
    
GameApp::GameApp() =default;


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
   spdlog::trace("初始化 GameApp...");
   if (!initSDL()) return false;
   if (!initTime()) return false;
   if (!initResourceManager()) return false;
    
    //测试资源管理器
   testResourceManager();
    
   is_running_ = true;
   spdlog::trace("GameApp初始化完成");
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
    
    //为了确保正确的销毁顺序，有些智能指针需要手动管理
    resource_manager_.reset();
    
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
    
bool GameApp::initSDL()
{
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
    spdlog::trace("SDL 初始化完成");
    return true;
}

bool GameApp::initTime()
{
    try
    {
        time_ = std::make_unique<Time>();
    }catch (const std::exception& e)
    {
        spdlog::error("初始化 Time 失败: {}", e.what());
        return false;
    }
    spdlog::trace("初始化 Time 成功");
    return true;
}

bool GameApp::initResourceManager()
{
    try
    {
        resource_manager_ = std::make_unique<engine::resource::ResourceManager>(renderer_);
        
    }catch (const std::exception& e)
    {
        spdlog::error("初始化资源管理器失败: {}", e.what());
        return false;
    }
    spdlog::trace("初始化资源管理器成功");
    return true;
}

void GameApp::testResourceManager()
{
    resource_manager_->getTexture("assets/textures/Actors/eagle-attack.png");
    resource_manager_->getFont("assets/fonts/VonwaonBitmap-16px.ttf", 16);
    resource_manager_->getSound("assets/audio/button_click.wav");

    resource_manager_->unloadTexture("assets/textures/Actors/eagle-attack.png");
    resource_manager_->unloadFont("assets/fonts/VonwaonBitmap-16px.ttf", 16);
    resource_manager_->unloadSound("assets/audio/button_click.wav");
}

}

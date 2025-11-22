#include "game_app.h"
#include "time.h"
#include "../resource/resource_manager.h"
#include "../render/sprite.h"
#include "../render/renderer.h"
#include "../render/camera.h"
#include "../input/input_manager.h"
#include "../object/game_object.h"
#include "../component/component.h"
#include <SDL3/SDL.h>
#include <spdlog/spdlog.h>

#include "config.h"

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
    
    while (is_running_)
    {
        time_->update();
        float delta_time = time_->getDeltaTime();
        input_manager_->update();
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
    if (!initConfig()) return false;
   if (!initSDL()) return false;
   if (!initTime()) return false;
   if (!initResourceManager()) return false;
   if (!initRenderer()) return false;
   if (!initCamera()) return false;
   if (!initInputManager()) return false;
    
    //测试资源管理器
   testResourceManager();
   testRenderer();
   testCamera();
    
   is_running_ = true;
   spdlog::trace("GameApp初始化完成");
   testGameObject();
   return true;
}

void GameApp::handleEvents()
{
    if (input_manager_->shouldQuit()) {
        spdlog::trace("GameApp 收到来自 InputManager 的退出请求。");
        is_running_ = false;
        return;
    }
    
    testInputManager();
}

void GameApp::update(float delta_time)
{
    testCamera();
}

void GameApp::render()
{
    renderer_->clearScreen();
    testRenderer();
    renderer_->present();
}

void GameApp::close()
{
    spdlog::trace("GameApp关闭...");
    
    //为了确保正确的销毁顺序，有些智能指针需要手动管理
    resource_manager_.reset();
    
    if (sdl_renderer_) {
        SDL_DestroyRenderer(sdl_renderer_);
        sdl_renderer_ = nullptr;
    }
    if (window_) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }
    SDL_Quit();
    is_running_ = false;
}

bool GameApp::initConfig()
{
    try
    {
        config_ = std::make_unique<Config>("assets/config.json");
    }catch (const std::exception& e)
    {
        spdlog::error("初始化配置失败: {}", e.what());
        return false;
    }
    spdlog::trace("初始化配置成功");
    return true;
}

bool GameApp::initSDL()
{
    // SDL初始化
    if (!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO)) {
        spdlog::error("SDL 初始化失败: {}", SDL_GetError());
        return false;
    }
    //创建窗口
    window_ = SDL_CreateWindow(config_->window_title_.c_str(), config_->window_width_, config_->window_height_, SDL_WINDOW_RESIZABLE);
    if (!window_) {
        spdlog::error("SDL 创建窗口失败: {}", SDL_GetError());
        return false;
    }
    //创建渲染器
    sdl_renderer_ = SDL_CreateRenderer(window_, nullptr);
    if (!sdl_renderer_) {
        spdlog::error("SDL 创建渲染器失败: {}", SDL_GetError());
        return false;
    }
    
    //设置垂直同步
    int vsync_mode = config_->vsync_enabled_ ? SDL_RENDERER_VSYNC_ADAPTIVE:SDL_RENDERER_VSYNC_DISABLED;
    SDL_SetRenderVSync(sdl_renderer_, vsync_mode);
    spdlog::trace("VSync 模式: {}", vsync_mode);
    
    //设置逻辑分辨率 视口的一半大
    SDL_SetRenderLogicalPresentation(sdl_renderer_,config_->window_width_/2,config_->window_height_/2,SDL_LOGICAL_PRESENTATION_LETTERBOX);
    
    
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
    time_->setTargetFPS(config_->target_fps_);
    spdlog::trace("初始化 Time 成功");
    return true;
}

bool GameApp::initResourceManager()
{
    try
    {
        resource_manager_ = std::make_unique<engine::resource::ResourceManager>(sdl_renderer_);
        
    }catch (const std::exception& e)
    {
        spdlog::error("初始化资源管理器失败: {}", e.what());
        return false;
    }
    spdlog::trace("初始化资源管理器成功");
    return true;
}

bool GameApp::initRenderer()
{
    try
    {
        renderer_ = std::make_unique<engine::render::Renderer>(sdl_renderer_,resource_manager_.get());
    }catch (const std::exception& e)
    {
        spdlog::error("初始化渲染器失败: {}", e.what());
        return false;
    }
    spdlog::trace("初始化渲染器成功");
    return true;
}

bool GameApp::initCamera()
{
    try
    {
        camera_ = std::make_unique<engine::render::Camera>(glm::vec2(640,360));
    }catch (const std::exception& e)
    {
        spdlog::error("初始化相机失败: {}", e.what());
        return false;
    }
    spdlog::trace("初始化相机成功");
    return true;
}

bool GameApp::initInputManager()
{
    try
    {
        input_manager_ = std::make_unique<engine::input::InputManager>(sdl_renderer_,config_.get());
    }
    catch (const std::exception& e)
    {
        spdlog::error("初始化输入管理器失败: {}", e.what());
        return false;
    }
    spdlog::trace("输入管理器初始化成功");
    return true;
}

void GameApp::testInputManager()
{
    std::vector<std::string> actions = {
        "move_up",
        "move_down",
        "move_left",
        "move_right",
        "jump",
        "attack",
        "pause",
        "MouseLeftClick",
        "MouseRightClick"
    };
    for (const auto& action : actions) {
        if (input_manager_->isActionPressed(action)) {
            spdlog::info(" {} 按下 ", action);
        }
        if (input_manager_->isActionReleased(action)) {
            spdlog::info(" {} 抬起 ", action);
        }
        if (input_manager_->isActionDown(action)) {
            spdlog::info(" {} 按下中 ", action);
        }
    }
}

void GameApp::testGameObject()
{
    engine::object::GameObject game_object("test_game_object");
    game_object.addComponent<engine::component::Component>();
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

void GameApp::testRenderer()
{
    engine::render::Sprite sprite_world("assets/textures/Actors/frog.png");
    engine::render::Sprite sprite_ui("assets/textures/UI/buttons/Start1.png");
    engine::render::Sprite sprite_parallax("assets/textures/Layers/back.png");
    
    static  float rotation = 0.0f;
    rotation += 0.1f;
    //渲染顺序
    renderer_->drawParallax(*camera_,sprite_parallax,glm::vec2(100,100),glm::vec2(0.5f,0.5f),glm::bvec2(true,false));
    renderer_->drawSprite(*camera_,sprite_world,glm::vec2(100,100),glm::vec2(1),rotation);
    renderer_->drawUISprite(sprite_ui,glm::vec2(100,100));
}

void GameApp::testCamera()
{
    auto key_state = SDL_GetKeyboardState(nullptr);
    if (key_state[SDL_SCANCODE_UP]) camera_->move(glm::vec2(0,-1));
    if (key_state[SDL_SCANCODE_DOWN]) camera_->move(glm::vec2(0,1));
    if (key_state[SDL_SCANCODE_LEFT]) camera_->move(glm::vec2(-1,0));
    if (key_state[SDL_SCANCODE_RIGHT]) camera_->move(glm::vec2(1,0));
}
}

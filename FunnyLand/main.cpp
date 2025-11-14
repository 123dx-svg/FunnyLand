#include <fstream>
#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <glm/glm.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

//如果要测试请打开main
int main() {

	// spdlog::set_level(spdlog::level::trace);//不设置的话默认是info级别  过滤日志级别
	//
	//  spdlog::trace("最低级别日志");
	//  spdlog::debug("调试日志");
	//  spdlog::info("你好！世界！");
	//  spdlog::warn("警告日志");
	//  spdlog::error("错误日志");
	//  spdlog::critical("严重错误日志");
	
	//格式化输出
	spdlog::info("格式化输出 {} {} {}", 1,"hello", 3.14f);

	try
	{
		std::ifstream file("assets/json_example.json");
		//nlohmann::ordered_json json;//这个可以输出为顺序的json 代价是性能下降
		nlohmann::json json_data = nlohmann::json::parse(file);
		file.close();
		spdlog::info("Json 成功载入");
		//1、字符串打印
		std::string name = json_data["name"].get<std::string>();
		spdlog::info("name: {}", name);
		//2、数字打印
		int age = json_data["age"].get<int>();
		spdlog::info("age: {}", age);
		//3、double打印
		double height = json_data["height_meters"].get<double>();
		spdlog::info("height_meters: {}", height);
		//4、bool打印
		bool isStudent = json_data["isStudent"].get<bool>();
		spdlog::info("isStudent: {}", isStudent);
		//5、null打印
		if (json_data["middleName"].is_null())
		{
			spdlog::info("middleName: null");
		}
		else
		{
			spdlog::info("middleName: {}", json_data.get<std::string>());
		}
		//另外一种方式  at  与 [] 不同的是，at 会检查索引是否越界，而 [] 不会。
		std::string email = json_data.at("email").get<std::string>();
		spdlog::info("email: {}", email);
		
		//value()用法 会查找optional_int  如果不存在  则返回默认值 -99
		int optional_int = json_data.value("optional_int", -99);
		spdlog::info("optional_int: {}", optional_int);
		
		//对象Objec
		nlohmann::json address_obj = json_data["address"];
		std::string street = address_obj["street"].get<std::string>();
		std::string city = address_obj["city"].get<std::string>();
		spdlog::info("Address: {} ,City: {}", street, city);
		
		//字符串数组
		spdlog::info("Hobbies");
		nlohmann::json hobbies_array = json_data["hobbies"];
		for (auto& hobby : hobbies_array)
		{
			spdlog::info("-{}", hobby.get<std::string>());
		}
		//数字数组
		spdlog::info("Scores");
		nlohmann::json scores_array = json_data["scores"];
		for (auto& score : scores_array)
		{
			if (score.is_number_integer())
			{
				spdlog::info("-{}", score.get<int>());
			}
			else if (score.is_number_float())
			{
				spdlog::info("-{}", score.get<double>());
			}
		}
		
		//对象数组
		spdlog::info("projects");
		nlohmann::json projects_array = json_data["projects"];
		for (auto& project : projects_array)
		{
			std::string name = project["projectName"].get<std::string>();
			spdlog::info("-{}", name);
		}
		spdlog::info("----------------------------------------");
		//直接访问深层嵌套的对象和数组
		double metadata_version = json_data["metadata"]["version"].get<double>();
		spdlog::info("metadata_version: {}", metadata_version);

		for (const auto& tag_json:json_data["metadata"]["tags"])
		{
			std::string tag = tag_json.get<std::string>();
			spdlog::info("-{}", tag);
		}
		//将json数据保存为文件
		std::ofstream out_file("assets/save_json.json");
		out_file<<json_data.dump(4); // 4 是缩进空格数//将json数据保存为文件
		out_file.close();
		spdlog::info("--------------------json已经保存------------------");

	}
	catch (const std::exception& e)
	{
		spdlog::error("Exception: {}", e.what());
		return EXIT_FAILURE;
	}
	

	glm::vec2 a = glm::vec2(1.0f, 2.0f);
	glm::vec2 b = glm::vec2(3.0f, 4.0f);




	std::cout << "Hello, World!" << std::endl;
	// SDL初始化
	if (!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO)) {
		std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}
	// 创建窗口
	SDL_Window* window = SDL_CreateWindow("Hello World! 好的", 800, 600, 0);
	// 创建渲染器
	SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

	// SDL3_Image不需要手动初始化

	// 加载图片
	SDL_Texture* texture = IMG_LoadTexture(renderer, "assets/textures/UI/title-screen.png");

	// SDL_Mixer初始化
	if (!Mix_OpenAudio(0, NULL)) {
		std::cerr << "Mix_OpenAudio Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	// 读取音乐
	Mix_Music* music = Mix_LoadMUS("assets/audio/hurry_up_and_run.ogg");
	// 播放音乐
	Mix_PlayMusic(music, -1);

	// SDL_TTF初始化
	if (!TTF_Init()) {
		std::cerr << "TTF_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}
	// 加载字体
	TTF_Font* font = TTF_OpenFont("assets/fonts/VonwaonBitmap-16px.ttf", 24);

	// 创建文本纹理
	SDL_Color color = { 255, 255, 255, 255 };
	SDL_Surface* surface = TTF_RenderText_Solid(font, "Hello, SDL 幽灵", 0, color);
	SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, surface);

	TTF_TextEngine* textEngine = TTF_CreateRendererTextEngine(renderer);
	TTF_Text* text = TTF_CreateText(textEngine, font, "Hello, SDL3 幽灵", 0);
	TTF_SetTextColor(text, 0, 255, 0, 255);


	// 渲染循环
	glm::vec2 MousePos = glm::vec2(0.0f, 0.0f);

	while (true) {
		SDL_Event event;
		if (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) {
				break;
			}
		}
		//鼠标位置
		auto state = SDL_GetMouseState(&MousePos.x, &MousePos.y);
		//SDL_Log("MousePosition (%f,%f)", MousePos.x, MousePos.y);
		//判断是否左键
		if (state & SDL_BUTTON_LMASK)
		{
			SDL_Log("Left button Down");
		}
		if (state & SDL_BUTTON_RMASK)
		{
			SDL_Log("Right button Down");
		}


		// 清屏
		SDL_RenderClear(renderer);
		// 画一个长方形
		SDL_FRect rect = { 100, 100, 200, 200 };
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		SDL_RenderFillRect(renderer, &rect);

		// 画图片
		SDL_FRect dstrect = { 200, 200, 200, 200 };
		SDL_RenderTexture(renderer, texture, NULL, &dstrect);

		// 画文本
		SDL_FRect textRect = { 300, 300, static_cast<float>(surface->w), static_cast<float>(surface->h) };
		SDL_RenderTexture(renderer, textTexture, NULL, &textRect);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

		//新版画文本
		TTF_DrawRendererText(text, 400, 400);

		// 更新屏幕
		SDL_RenderPresent(renderer);
	}

	// 清理图片资源
	SDL_DestroyTexture(texture);

	// 清理音乐资源
	Mix_FreeMusic(music);
	Mix_CloseAudio();
	Mix_Quit();

	// 清理字体资源
	SDL_DestroySurface(surface);
	SDL_DestroyTexture(textTexture);
	TTF_CloseFont(font);
	TTF_Quit();

	// 清理并退出
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
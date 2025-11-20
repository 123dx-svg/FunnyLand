

#include <spdlog/spdlog.h>

#include "src/engine/core/game_app.h"
//如果要测试请打开main
int main() {
	//spdlog::set_level(spdlog::level::trace);
	
    engine::core::GameApp game_app;
    game_app.run();
	
	return 0;
}
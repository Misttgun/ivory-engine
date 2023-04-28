#include <sol/sol.hpp>
#include "game/Game.h"

void TestLua()
{
	sol::state lua;
    lua.open_libraries(sol::lib::base);

    lua.script_file("./assets/scripts/myscript.lua");
    int someVariable = lua["some_variable"];

    std::cout << "Value of some_variable in C++ is: " << someVariable << std::endl;

    bool isFullScreen = lua["config"]["fullscreen"];
    sol::table config = lua["config"];
    int width = config["resolution"]["width"];
    int height = config["resolution"]["height"];
}

int main(int argc, char* argv[])
{
    /*Game game;

    game.Init();
    game.Run();

    game.Destroy();*/

    TestLua();

    return 0;
}
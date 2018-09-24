#include "System.hpp"

int WINAPI
WinMain(HINSTANCE instance, HINSTANCE old_instance, PSTR cmd_line, int cmd_show)
{
	std::unique_ptr<System> system = std::make_unique<System>();
	if (!system)
	{
		return 0;
	}

	if (system->init())
	{
		system->run();
	}

	system->stop();
	return 0;
}
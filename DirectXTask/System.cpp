#include "System.hpp"

#include <iostream>
#include <thread>

System::System()
{
	WindowSettings ws{ "RTS", 1280, 720 };
	window = std::make_unique<Window>(ws);
	input = std::make_unique<Input>();
	graphics = std::make_unique<Graphics>(ws.width, ws.height, window->window_handle);
}

void System::run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	bool quit = false;
	while (!quit)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
		{
			quit = true;
		}	
		else
		{
			quit = update();
			quit = quit || draw();
		}
		dt = frame_timer.getElapsedTime<Timer::seconds_float>();
		timeSinceLastFPSPrint += dt;
		if (timeSinceLastFPSPrint > 1.0f)
		{
			std::cout << "FPS: " << static_cast<int>(1.0f / dt) << "\n";
			timeSinceLastFPSPrint = 0;
		}
		frame_timer.restart();
	}
}

bool System::update()
{
	return (input->is_key_down(VK_ESCAPE)) || graphics->update(input.get(), dt);
}

bool System::draw()
{
	return graphics->draw();
}
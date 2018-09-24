#include <windows.h>

static constexpr bool fullscreen = false;
static constexpr bool vsync = true;
static constexpr float screen_depth = 1000.0f;
static constexpr float screen_near = 0.1f;

class Graphics
{
public:
	bool init(int width, int height, HWND window);
	void stop();
	bool frame();

private:
	bool render();
};
#pragma once

#include <array>

class Input
{
public:
	void key_down(unsigned int key);
	void key_up(unsigned int key);
	bool is_key_down(unsigned int key);

	void mouse_key_down(unsigned int key);
	void mouse_key_up(unsigned int key);
	bool is_mouse_key_down(unsigned int mouse_key);

private:
	std::array<bool, 256> keys;
	std::array<bool, 256> mouse_keys;
};
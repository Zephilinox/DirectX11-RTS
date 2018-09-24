#pragma once

#include <array>

class Input
{
public:
	void init();
	void key_down(unsigned int key);
	void key_up(unsigned int key);
	bool is_key_down(unsigned int key);

private:
	std::array<bool, 256> keys;
};
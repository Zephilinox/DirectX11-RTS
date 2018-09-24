#include "Input.hpp"

void Input::init()
{
	for (bool& key : keys)
	{
		key = false;
	}
}

void Input::key_down(unsigned int key)
{
	keys[key] = true;
}

void Input::key_up(unsigned int key)
{
	keys[key] = false;
}

bool Input::is_key_down(unsigned int key)
{
	return keys[key];
}

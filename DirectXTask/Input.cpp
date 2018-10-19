#include "Input.hpp"

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

void Input::mouse_key_down(unsigned int mouse_key)
{
	mouse_keys[mouse_key] = true;
}

void Input::mouse_key_up(unsigned int mouse_key)
{
	mouse_keys[mouse_key] = false;
}

bool Input::is_mouse_key_down(unsigned int mouse_key)
{
	return mouse_keys[mouse_key];
}

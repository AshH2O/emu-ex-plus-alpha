#pragma once

/*  This file is part of Imagine.

	Imagine is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Imagine is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Imagine.  If not, see <http://www.gnu.org/licenses/> */

#include <SDL/SDL_keysym.h>
#include <SDL/SDL_mouse.h>

namespace Input
{

namespace Keycode
{
	static const uint ESCAPE = SDLK_ESCAPE,
	ENTER = SDLK_RETURN,
	LALT = SDLK_LALT,
	RALT = SDLK_RALT,
	LSHIFT = SDLK_LSHIFT,
	RCTRL = SDLK_RCTRL,
	LEFT = SDLK_LEFT,
	RIGHT = SDLK_RIGHT,
	UP = SDLK_UP,
	DOWN = SDLK_DOWN,
	BACK_SPACE = SDLK_BACKSPACE,
	MENU = SDLK_MENU,
	// end of keys on Palm Pixi/Pre
	PGUP = SDLK_PAGEUP,
	PGDOWN = SDLK_PAGEDOWN,
	RSHIFT = SDLK_RSHIFT,
	LCTRL = SDLK_LCTRL,
	TAB = SDLK_TAB,
	HOME = SDLK_HOME,
	DELETE = SDLK_DELETE,
	END = SDLK_END,
	INSERT = SDLK_INSERT,
	SCROLL_LOCK = SDLK_SCROLLOCK,
	CAPS = SDLK_CAPSLOCK,
	PAUSE = SDLK_PAUSE,
	LMETA = SDLK_LMETA,
	RMETA = SDLK_RMETA,
	F1 = SDLK_F1,
	F2 = SDLK_F2,
	F3 = SDLK_F3,
	F4 = SDLK_F4,
	F5 = SDLK_F5,
	F6 = SDLK_F6,
	F7 = SDLK_F7,
	F8 = SDLK_F8,
	F9 = SDLK_F9,
	F10 = SDLK_F10,
	F11 = SDLK_F11,
	F12 = SDLK_F12,
	SEARCH = SDLK_WORLD_0 // dummy key, doesn't exist in SDL 1.2
	;

	static const uint COUNT = 0xfff + 1;
};

typedef uint16 Key;

namespace Pointer
{
	static const uint LBUTTON = SDL_BUTTON_LEFT,
		MBUTTON = SDL_BUTTON_MIDDLE,
		RBUTTON = SDL_BUTTON_RIGHT,
		WHEEL_UP = SDL_BUTTON_WHEELUP,
		WHEEL_DOWN = SDL_BUTTON_WHEELDOWN
		;
}

}

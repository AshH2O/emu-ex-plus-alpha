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

#include <imagine/gfx/Viewport.hh>
#include <imagine/base/baseDefs.hh>
#include <imagine/util/Interpolator.hh>

namespace Gfx
{

class Window;

class AnimatedViewport
{
public:
	constexpr AnimatedViewport() {}
	void start(Base::Window &w, Gfx::Viewport begin, Gfx::Viewport end);
	void finish();
	bool isFinished() const;
	void cancel();
	Gfx::Viewport viewport() const;

protected:
	IG::InterpolatorValue<int, IG::FrameTime, IG::InterpolatorType::EASEINOUTQUAD> animator[4]{};
	Base::OnFrameDelegate animate{};
	Base::Window *win{};
};

}

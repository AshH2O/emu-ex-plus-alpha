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

#define LOGTAG "GfxProjectionPlane"
#include <imagine/gfx/RendererCommands.hh>
#include <imagine/gfx/ProjectionPlane.hh>
#include <imagine/logger/logger.h>

namespace Gfx
{

GC ProjectionPlane::width() const
{
	return w;
}

GC ProjectionPlane::height() const
{
	return h;
}

GP ProjectionPlane::size() const
{
	return {w, h};
}

GC ProjectionPlane::focalZ() const
{
	return focal;
}

Viewport ProjectionPlane::viewport() const
{
	return viewport_;
}

void ProjectionPlane::updateMMSize(Viewport v)
{
	mmToXScale = w/(GC)v.widthMM();
	mmToYScale = h/(GC)v.heightMM();
	//logMsg("projector to mm %fx%f", (double)mmToXScale, (double)mmToYScale);
}

ProjectionPlane ProjectionPlane::makeWithMatrix(Viewport viewport, Mat4 mat)
{
	ProjectionPlane p;
	auto matInv = mat.invert();
	p.viewport_ = viewport;
	auto lowerLeft = mat.unproject(viewport.inGLFormat(), {(GC)viewport.bounds().x, (GC)viewport.bounds().y, .5}, matInv);
	//logMsg("Lower-left projection point %d,%d -> %f %f %f", viewport.bounds().x, viewport.bounds().y, (double)lowerLeft.v.x, (double)lowerLeft.v.y, (double)lowerLeft.v.z);
	auto upperRight = mat.unproject(viewport.inGLFormat(), {(GC)viewport.bounds().x2, (GC)viewport.bounds().y2, .5}, matInv);
	//logMsg("Upper-right projection point %d,%d -> %f %f %f", viewport.bounds().x2, viewport.bounds().y2, (double)upperRight.v.x, (double)upperRight.v.y, (double)upperRight.v.z);
	p.w = upperRight.x() - lowerLeft.x(), p.h = upperRight.y() - lowerLeft.y();
	p.focal = upperRight.z();
	p.rect.x = -p.w/2._gc;
	p.rect.y = -p.h/2._gc;
	p.rect.x2 = p.w/2._gc;
	p.rect.y2 = p.h/2._gc;
	p.pixToXScale = p.w / (GC)viewport.width();
	p.pixToYScale = p.h / (GC)viewport.height();
	p.xToPixScale = (GC)viewport.width() / p.w;
	p.yToPixScale = (GC)viewport.height() / p.h;
	p.updateMMSize(viewport);
	logMsg("made with size %fx%f, to pix %fx%f, to view %fx%f",
		(double)p.w, (double)p.h, (double)p.xToPixScale, (double)p.yToPixScale, (double)p.pixToXScale, (double)p.pixToYScale);
	return p;
}

Mat4 ProjectionPlane::makeTranslate(IG::Point2D<float> p) const
{
	return Mat4::makeTranslate({p.x, p.y, focal});
}

Mat4 ProjectionPlane::makeTranslate() const
{
	return Mat4::makeTranslate({0_gc, 0_gc, focal});
}

void ProjectionPlane::loadTranslate(Gfx::RendererCommands &cmds, GC x, GC y) const
{
	cmds.loadTranslate(x, y, focal);
}

void ProjectionPlane::loadTranslate(Gfx::RendererCommands &cmds, GP p) const
{
	loadTranslate(cmds, p.x, p.y);
}

void ProjectionPlane::resetTransforms(Gfx::RendererCommands &cmds) const
{
	loadTranslate(cmds, 0., 0.);
}

GC ProjectionPlane::unprojectXSize(int x) const
{
	GC s = (GC)(x) * pixToXScale;
	//logMsg("project x %d, to %f", x, r);
	return s;
}

GC ProjectionPlane::unprojectYSize(int y) const
{
	GC s = (GC)y * pixToYScale;
	//logMsg("project y %d, to %f", y, r);
	return s;
}

GC ProjectionPlane::unprojectX(int x) const
{
	return unprojectXSize(x - viewport().bounds().x) - wHalf();
}

GC ProjectionPlane::unprojectY(int y) const
{
	return -unprojectYSize(y - viewport().bounds().y) + hHalf();
}

int ProjectionPlane::projectXSize(GC x) const
{
	int s = int(std::floor(x * xToPixScale));
	//if(s) logMsg("unproject x %f, to %d", x, s);
	return s;
}

int ProjectionPlane::projectYSize(GC y) const
{
	int s = int(std::floor(y * yToPixScale));
	//if(s) logMsg("unproject y %f, to %d", y, s);
	return s;
}

int ProjectionPlane::projectX(GC x) const
{
	//logMsg("unproject x %f", x);
	return projectXSize(x + wHalf()) + viewport().bounds().x;
}

int ProjectionPlane::projectY(GC y) const
{
	//logMsg("unproject y %f", y);
	return projectYSize(-(y - hHalf())) + viewport().bounds().y;
}

GCRect ProjectionPlane::unProjectRect(int x, int y, int x2, int y2) const
{
	GCRect objRect;
	objRect.x = unprojectX(x);
	objRect.y = unprojectY(y2); // flip y-axis
	objRect.x2 = unprojectX(x2);
	objRect.y2 = unprojectY(y);
	return objRect;
}

GCRect ProjectionPlane::unProjectRect(IG::WindowRect r) const
{
	return unProjectRect(r.x, r.y, r.x2, r.y2);
}

IG::WindowRect ProjectionPlane::projectRect(GCRect r) const
{
	IG::WindowRect winRect;
	winRect.x = projectX(r.x);
	winRect.y = projectY(r.y2);
	winRect.x2 = projectX(r.x2);
	winRect.y2 = projectY(r.y);
	return winRect;
}

GC ProjectionPlane::alignXToPixel(GC x) const
{
	return unprojectX(projectX(x));
}

GC ProjectionPlane::alignYToPixel(GC y) const
{
	return unprojectY(projectY(y));
}

IG::Point2D<GC> ProjectionPlane::alignToPixel(IG::Point2D<GC> p) const
{
	return {alignXToPixel(p.x), alignYToPixel(p.y)};
}

GC ProjectionPlane::xMMSize(GC mm) const { return mm * mmToXScale; }
GC ProjectionPlane::yMMSize(GC mm) const { return mm * mmToYScale; }

}

/*  This file is part of EmuFramework.

	Imagine is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Imagine is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with EmuFramework.  If not, see <http://www.gnu.org/licenses/> */

#include <emuframework/ButtonConfigView.hh>
#include <emuframework/AppKeyCode.hh>
#include <emuframework/InputManagerView.hh>
#include <emuframework/EmuApp.hh>
#include "../privateInput.hh"
#include <imagine/gfx/RendererCommands.hh>
#include <imagine/gui/AlertView.hh>
#include <imagine/util/math/int.hh>
#include <imagine/util/variant.hh>
#include <imagine/logger/logger.h>
#include <format>

namespace EmuEx
{

bool ButtonConfigSetView::pointerUIIsInit()
{
	return unbindB.x != unbindB.x2;
}

void ButtonConfigSetView::initPointerUI()
{
	if(!pointerUIIsInit())
	{
		logMsg("init pointer UI elements");
		waitForDrawFinished();
		unbind = {"Unbind", &defaultFace()};
		cancel = {"Cancel", &defaultFace()};
		unbindB.x2 = 1;
	}
}

ButtonConfigSetView::ButtonConfigSetView(ViewAttachParams attach,
	InputManagerView &rootIMView, Input::Device &dev, std::string_view actionName,
	SetDelegate onSet):
		View{attach},
		text{&defaultFace()},
		onSetD{onSet},
		dev{dev},
		rootIMView{rootIMView},
		actionStr{actionName} {}

void ButtonConfigSetView::place()
{
	text.compile(renderer());

	if(pointerUIIsInit())
	{
		unbind.compile(renderer());
		cancel.compile(renderer());

		IG::WindowRect btnFrame;
		btnFrame.setPosRel(viewRect().pos(LB2DO), unbind.nominalHeight() * 2, LB2DO);
		unbindB = btnFrame;
		unbindB.x = (viewRect().xSize()/2)*0;
		unbindB.x2 = (viewRect().xSize()/2)*1;
		cancelB = btnFrame;
		cancelB.x = (viewRect().xSize()/2)*1;
		cancelB.x2 = (viewRect().xSize()/2)*2;
	}
}

bool ButtonConfigSetView::inputEvent(const Input::Event &e)
{
	return visit(overloaded
	{
		[&](const Input::MotionEvent &motionEv)
		{
			if(!Config::Input::POINTING_DEVICES || !motionEv.isAbsolute())
				return false;
			if(!pointerUIIsInit())
			{
				initPointerUI();
				place();
				postDraw();
				return true;
			}
			else if(pointerUIIsInit() && motionEv.released())
			{
				if(unbindB.overlaps(motionEv.pos()))
				{
					logMsg("unbinding key");
					auto onSet = onSetD;
					dismiss();
					onSet(MappedKeys{});
					return true;
				}
				else if(cancelB.overlaps(motionEv.pos()))
				{
					dismiss();
					return true;
				}
			}
			return false;
		},
		[&](const Input::KeyEvent &keyEv)
		{
			if(keyEv.pushed() && !keyEv.repeated())
			{
				auto d = keyEv.device();
				if(d != &dev)
				{
					if(d == savedDev)
					{
						app().unpostMessage();
						auto &rootIMView = this->rootIMView;
						popTo(rootIMView);
						rootIMView.pushAndShowDeviceView(*d, e);
					}
					else
					{
						savedDev = d;
						app().postMessage(7, false,
							std::format("You pushed a key from device:\n{}\nPush another from it to open its config menu",
							inputDevData(*d).displayName));
						postDraw();
					}
					return true;
				}
				pushedKeys.tryPushBack(keyEv.mapKey());
				return true;
			}
			else if(keyEv.released())
			{
				if(pushedKeys.size())
					finalize();
			}
			return false;
		}
	}, e);
}

void ButtonConfigSetView::finalize()
{
	auto onSet = onSetD;
	auto mappedKeys = pushedKeys;
	dismiss();
	onSet(mappedKeys);
}

void ButtonConfigSetView::draw(Gfx::RendererCommands &__restrict__ cmds)
{
	using namespace IG::Gfx;
	auto &basicEffect = cmds.basicEffect();
	cmds.set(BlendMode::OFF);
	basicEffect.disableTexture(cmds);
	cmds.setColor({.4, .4, .4});
	cmds.drawRect(viewRect());
	if(pointerUIIsInit())
	{
		cmds.setColor({.2, .2, .2});
		cmds.drawRect(unbindB);
		cmds.drawRect(cancelB);
	}
	basicEffect.enableAlphaTexture(cmds);
	if(pointerUIIsInit())
	{
		unbind.draw(cmds, unbindB.pos(C2DO), C2DO, ColorName::WHITE);
		cancel.draw(cmds, cancelB.pos(C2DO), C2DO, ColorName::WHITE);
	}
	text.draw(cmds, viewRect().center(), C2DO, ColorName::WHITE);
}

void ButtonConfigSetView::onAddedToController(ViewController *, const Input::Event &e)
{
	if(e.motionEvent())
		text.resetString(std::format("Push up to 3 keys, release any to set:\n{}", actionStr));
	else
		text.resetString(std::format("Push up to 3 keys, release any to set:\n{}\n\nTo unbind:\nQuickly push [Left] key twice in previous menu", actionStr));
	if(e.motionEvent())
	{
		initPointerUI();
	}
}

static std::string keyNames(MappedKeys keys, const Input::Device &dev)
{
	Input::KeyNameFlags flags{.basicModifiers = keys.size() > 1};
	std::string s{dev.keyString(keys[0], flags)};
	for(const auto &b : keys | std::ranges::views::drop(1))
	{
		s += " + ";
		s += dev.keyString(b, flags);
	}
	return s;
}

void ButtonConfigView::onSet(int catIdx, MappedKeys mapKey)
{
	if(!devConf->setKey(app(), cat.keys[catIdx], mapKey))
		return;
	devConf->buildKeyMap(app().inputManager);
	auto &b = btn[catIdx];
	b.set2ndName(keyNames(mapKey, devConf->device()));
	b.compile2nd(renderer());
}

bool ButtonConfigView::inputEvent(const Input::Event &e)
{
	if(e.keyEvent() && e.keyEvent()->pushed(Input::DefaultKey::LEFT) && selected > 0)
	{
		auto &keyEv = *e.keyEvent();
		auto durationSinceLastKeySet = hasTime(leftKeyPushTime) ? keyEv.time() - leftKeyPushTime : SteadyClockTime{};
		leftKeyPushTime = keyEv.time();
		if(durationSinceLastKeySet.count() && durationSinceLastKeySet <= IG::Milliseconds(500))
		{
			// unset key
			leftKeyPushTime = {};
			onSet(selected - 1, {});
			postDraw();
		}
		return true;
	}
	else
	{
		return TableView::inputEvent(e);
	}
}

ButtonConfigView::ButtonConfigView(ViewAttachParams attach, InputManagerView &rootIMView_, const KeyCategory &cat_, InputDeviceConfig &devConf_):
	TableView
	{
		cat_.name,
		attach,
		[this](const TableView &)
		{
			return 1 + cat.keys.size();
		},
		[this](const TableView &, size_t idx) -> MenuItem&
		{
			if(idx == 0)
				return reset;
			else
				return btn[idx-1];
		}
	},
	rootIMView{rootIMView_},
	reset
	{
		"Unbind All", &defaultFace(),
		[this](const Input::Event &e)
		{
			pushAndShowModal(makeView<YesNoAlertView>("Really unbind all keys in this category?",
				YesNoAlertView::Delegates
				{
					.onYes = [this]
					{
						auto conf = devConf->makeMutableKeyConf(app());
						if(!conf)
							return;
						conf->unbindCategory(cat);
						for(auto &&[i, key]: enumerate(cat.keys))
						{
							btn[i].set2ndName(keyNames(conf->get(key), devConf->device()));
							btn[i].compile2nd(renderer());
						}
						devConf->buildKeyMap(app().inputManager);
					}
				}), e);
		}
	},
	cat{cat_}
{
	logMsg("init button config view for %s", Input::KeyEvent::mapName(devConf_.device().map()).data());
	devConf = &devConf_;
	auto keyConfig = devConf_.keyConf(app().inputManager);
	btn = std::make_unique<DualTextMenuItem[]>(cat_.keys.size());
	for(auto &&[i, key]: enumerate(cat.keys))
	{
		btn[i] =
		{
			app().inputManager.toString(key),
			keyNames(keyConfig.get(key), devConf_.device()),
			&defaultFace(),
			[this, keyIdxToSet = i](const Input::Event &e)
			{
				auto btnSetView = makeView<ButtonConfigSetView>(rootIMView,
					devConf->device(), app().inputManager.toString(cat.keys[keyIdxToSet]),
					[this, keyIdxToSet](const MappedKeys &val)
					{
						onSet(keyIdxToSet, val);
					});
				pushAndShowModal(std::move(btnSetView), e);
			}
		};
		btn[i].text2Color = Gfx::ColorName::YELLOW;
	}
}

}

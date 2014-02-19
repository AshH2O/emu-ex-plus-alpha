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

#include <bluetooth/sys.hh>
#include <input/Input.hh>
#include <input/AxisKeyEmu.hh>
#include <util/collection/ArrayList.hh>

struct IControlPad : public BluetoothInputDevice, public Input::Device
{
public:
	static const uchar btClass[3];
	static StaticArrayList<IControlPad*, Input::MAX_BLUETOOTH_DEVS_PER_TYPE> devList;

	IControlPad(BluetoothAddr addr):
		Device {0, Input::Event::MAP_ICONTROLPAD, Input::Device::TYPE_BIT_GAMEPAD, "iControlPad"},
		addr(addr)
	{}

	CallResult open(BluetoothAdapter &adapter) override;
	void close();
	void removeFromSystem() override;
	uint joystickAxisBits() override;
	uint joystickAxisAsDpadBitsDefault() override;
	void setJoystickAxisAsDpadBits(uint axisMask) override;
	uint joystickAxisAsDpadBits() override { return joystickAxisAsDpadBits_; }
	uint statusHandler(BluetoothSocket &sock, uint status);
	bool dataHandler(const char *packet, size_t size);

	static bool isSupportedClass(const uchar devClass[3])
	{
		return mem_equal(devClass, btClass, 3);
	}

private:
	enum
	{
		FUNC_NONE,
		FUNC_SET_LED_MODE,
		FUNC_GP_REPORTS,
	};
	BluetoothSocketSys sock;
	char inputBuffer[6] = {0};
	uint inputBufferPos = 0;
	uint player = 0;
	int function = 0;
	uint joystickAxisAsDpadBits_;
	char prevBtnData[2] = {0};
	Input::AxisKeyEmu<int> axisKey[4]
	{
		{-nubDeadzone, nubDeadzone, Input::iControlPad::LNUB_LEFT, Input::iControlPad::LNUB_RIGHT}, // Left X Axis
		{-nubDeadzone, nubDeadzone, Input::iControlPad::LNUB_UP, Input::iControlPad::LNUB_DOWN},  // Left Y Axis
		{-nubDeadzone, nubDeadzone, Input::iControlPad::RNUB_LEFT, Input::iControlPad::RNUB_RIGHT}, // Right X Axis
		{-nubDeadzone, nubDeadzone, Input::iControlPad::RNUB_UP, Input::iControlPad::RNUB_DOWN}   // Right Y Axis
	};
	static constexpr int nubDeadzone = 64;
	BluetoothAddr addr;

	static uint findFreeDevId();
	void processBtnReport(const char *btnData, uint player);
};

#pragma once

#include <input/Input.hh>
#include <bluetooth/sys.hh>

namespace Bluetooth
{

bool scanForDevices(BluetoothAdapter &bta, BluetoothAdapter::OnStatusDelegate onScanStatus);
bool listenForDevices(BluetoothAdapter &bta, const BluetoothAdapter::OnStatusDelegate &onScanStatus);
void closeDevices(BluetoothAdapter *bta);
void closeBT(BluetoothAdapter *&bta);
uint devsConnected();
uint pendingDevs();
void connectPendingDevs(BluetoothAdapter *bta);

}

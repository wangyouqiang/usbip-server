#include <string.h>

#include "UsbIpDevice.h"
#include "UsbDevice.h"
#include "UsbConfiguration.h"
#include "UsbInterface.h"
#include "NetworkUtil.h"

using namespace NetworkUtil;

UsbIpDevice::UsbIpDevice(UsbDevice* d, string path, string busId,
			 int busNum, int devNum,
			 enum usb_device_speed speed) {
    this->d = d;
    memset(this->pathStrBuf, 0, sizeof(this->pathStrBuf));
    if (path.length() < sizeof(this->pathStrBuf)) {
	strcpy(this->pathStrBuf, path.c_str());
    }
    memset(this->busIdStrBuf, 0, sizeof(this->busIdStrBuf));
    if (busId.length() < sizeof(this->busIdStrBuf)) {
	strcpy(this->busIdStrBuf, busId.c_str());
    }
    this->busNum = busNum;
    this->devNum = devNum;
    this->speed = speed;    
}

int UsbIpDevice::FillDeviceData(unsigned char* buffer, int offset, bool withInterfaces) {
    int pos = offset;

    pos += CopyString(this->pathStrBuf, sizeof(this->pathStrBuf), buffer, pos);
    pos += CopyString(this->busIdStrBuf, sizeof(this->busIdStrBuf), buffer, pos);
    pos += SetUint(busNum, buffer, pos, 4); /* */
    pos += SetUint(devNum, buffer, pos, 4); /* */
    pos += SetUint(speed, buffer, pos, 4); /* */
    
    pos += SetUint(d->idVendor, buffer, pos, 2); /* */
    pos += SetUint(d->idProduct, buffer, pos, 2); /* */
    pos += SetUint(d->bcdDevice, buffer, pos, 2); /* */
    pos += SetUint(d->bDeviceClass, buffer, pos, 1); /* */
    pos += SetUint(d->bDeviceSubClass, buffer, pos, 1); /* */
    pos += SetUint(d->bDeviceProtocol, buffer, pos, 1); /* */
    pos += SetUint(d->bConfigurationValue, buffer, pos, 1); /* */
    pos += SetUint(d->bNumConfigurations, buffer, pos, 1); /* */
    pos += SetUint(d->configurationArray[0]->bNumInterfaces, buffer, pos, 1); /* */

    if (withInterfaces) {
	for (int idx = 0; idx < d->configurationArray[0]->bNumInterfaces; idx++) {
	    UsbInterface* interface = d->configurationArray[0]->interfaceArray[idx];
	    pos += SetUint(interface->bInterfaceClass,    buffer, pos, 1); /* InterfaceClass */
	    pos += SetUint(interface->bInterfaceSubClass, buffer, pos, 1); /* InterfaceSubClass*/
	    pos += SetUint(interface->bInterfaceProtocol, buffer, pos, 1); /* InterfaceProtocol */
	    pos += SetUint(0,                             buffer, pos, 1); /* Padding */
	}
    }
    
    return pos - offset;
}

int UsbIpDevice::CopyString(char* str, int length, unsigned char* buffer, int offset) {
    memcpy(&buffer[offset], str, length);
    return length;
}

int UsbIpDevice::TxRx(unsigned char* setupBuffer, unsigned char* dataBuffer, unsigned char* outBuffer, int outBufferLength) {
    return d->TxRx(setupBuffer, dataBuffer, outBuffer, outBufferLength);
}
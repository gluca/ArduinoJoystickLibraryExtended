/*
  Flystick.cpp

  Copyright (c) 2015-2017, Matthew Heironimus

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "Flystick.h"

#if defined(_USING_DYNAMIC_HID)

#define FLYSTICK_REPORT_ID_INDEX 7
#define FLYSTICK_SIMULATOR_MINIMUM -32767
#define FLYSTICK_SIMULATOR_MAXIMUM 32767

#define FLYSTICK_INCLUDE_RUDDER      B00000001
#define FLYSTICK_INCLUDE_THROTTLE    B00000010
#define FLYSTICK_INCLUDE_ACCELERATOR B00000100
#define FLYSTICK_INCLUDE_BRAKE       B00001000
#define FLYSTICK_INCLUDE_STEERING    B00010000
#define FLYSTICK_INCLUDE_TOEBRAKE    B00100000

Flystick_::Flystick_(
	uint8_t hidReportId,
	uint8_t stickType,
	bool includeRudder,
	bool includeThrottle,
	bool includeToeBrake,
	bool includeAccelerator,
	bool includeBrake,
	bool includeSteering
	)
{
    // Set the USB HID Report ID
    _hidReportId = hidReportId;

	_includeSimulatorFlags = 0;
	_includeSimulatorFlags |= (includeRudder ? FLYSTICK_INCLUDE_RUDDER : 0);
	_includeSimulatorFlags |= (includeThrottle ? FLYSTICK_INCLUDE_THROTTLE : 0);
	_includeSimulatorFlags |= (includeAccelerator ? FLYSTICK_INCLUDE_ACCELERATOR : 0);
	_includeSimulatorFlags |= (includeBrake ? FLYSTICK_INCLUDE_BRAKE : 0);
	_includeSimulatorFlags |= (includeSteering ? FLYSTICK_INCLUDE_STEERING : 0);
	_includeSimulatorFlags |= (includeToeBrake ? FLYSTICK_INCLUDE_TOEBRAKE : 0);
	
    // Build stick HID Report Description
	
	uint8_t simulationCount = (includeRudder == true)
		+ (includeThrottle == true)
		+ (includeAccelerator == true)
		+ (includeBrake == true)
		+ (includeSteering == true)
		+ (includeToeBrake == true); 
		
    uint8_t tempHidReportDescriptor[150];
    int hidReportDescriptorSize = 0;

    // USAGE_PAGE (Generic Desktop)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x05;
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x02;

    // USAGE (Flight - 0x01; Spaceship - 0x04; Airplane - 0x09)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
    tempHidReportDescriptor[hidReportDescriptorSize++] = stickType;

    // COLLECTION (Application)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0xa1;
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

    // REPORT_ID (Default: 3)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x85;
    tempHidReportDescriptor[hidReportDescriptorSize++] = _hidReportId;

    // USAGE_PAGE (Generic Desktop)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x05;
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x02;

	// LOGICAL_MINIMUM (-32767)
	tempHidReportDescriptor[hidReportDescriptorSize++] = 0x16;
	tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;
	tempHidReportDescriptor[hidReportDescriptorSize++] = 0x80;

	// LOGICAL_MAXIMUM (+32767)
	tempHidReportDescriptor[hidReportDescriptorSize++] = 0x26;
	tempHidReportDescriptor[hidReportDescriptorSize++] = 0xFF;
	tempHidReportDescriptor[hidReportDescriptorSize++] = 0x7F;

	// REPORT_SIZE (16)
	tempHidReportDescriptor[hidReportDescriptorSize++] = 0x75;
	tempHidReportDescriptor[hidReportDescriptorSize++] = 0x10;

	// REPORT_COUNT (simulationCount)
	tempHidReportDescriptor[hidReportDescriptorSize++] = 0x95;
	tempHidReportDescriptor[hidReportDescriptorSize++] = simulationCount;

	// COLLECTION (Physical)
	tempHidReportDescriptor[hidReportDescriptorSize++] = 0xA1;
	tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;

	if (includeRudder == true) {
		// USAGE (Rudder)
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0xBA;
	}

	if (includeThrottle == true) {
		// USAGE (Throttle)
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0xBB;
	}
	if (includeToeBrake == true) {
		// USAGE (ToeBrake)
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09; // Left Toe Brake
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0xBF; 
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09; // Right Toe Brake
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0xBF;
	}
	
	if (includeAccelerator == true) {
		// USAGE (Accelerator)
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0xC4;
	}

	if (includeBrake == true) {
		// USAGE (Brake)
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0xC5;
	}

	if (includeSteering == true) {
		// USAGE (Steering)
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
		tempHidReportDescriptor[hidReportDescriptorSize++] = 0xC8;
	}

		// INPUT (Data,Var,Abs)
	tempHidReportDescriptor[hidReportDescriptorSize++] = 0x81;
	tempHidReportDescriptor[hidReportDescriptorSize++] = 0x02;
	
	// END_COLLECTION (Physical)
	tempHidReportDescriptor[hidReportDescriptorSize++] = 0xc0;
	
    // END_COLLECTION
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0xc0;

	// Create a copy of the HID Report Descriptor template that is just the right size
	uint8_t *customHidReportDescriptor = new uint8_t[hidReportDescriptorSize];
	memcpy(customHidReportDescriptor, tempHidReportDescriptor, hidReportDescriptorSize);
	
	// Register HID Report Description
	DynamicHIDSubDescriptor *node = new DynamicHIDSubDescriptor(customHidReportDescriptor, hidReportDescriptorSize, false);
	DynamicHID().AppendDescriptor(node);

	// Initalize Joystick State
	_throttle = 0;
	_rudder = 0;
	_accelerator = 0;
	_brake = 0;
	_toeBrakeL = 0;
	_toeBrakeR = 0;
	_steering = 0;
}

void Flystick_::begin(bool initAutoSendState)
{
	_autoSendState = initAutoSendState;
	sendState();
}

void Flystick_::end()
{
}

void Flystick_::setRudder(int16_t value)
{
	_rudder = value;
	if (_autoSendState) sendState();
}
void Flystick_::setThrottle(int16_t value)
{
	_throttle = value;
	if (_autoSendState) sendState();
}
void Flystick_::setAccelerator(int16_t value)
{
	_accelerator = value;
	if (_autoSendState) sendState();
}
void Flystick_::setBrake(int16_t value)
{
	_brake = value;
	if (_autoSendState) sendState();
}
void Flystick_::setSteering(int16_t value)
{
	_steering = value;
	if (_autoSendState) sendState();
}

void Flystick_::setToeBrake(int16_t valueL, int16_t valueR)
{
	_toeBrakeL = valueL;
	_toeBrakeR = valueR;
	if (_autoSendState) sendState();
}


int Flystick_::buildAndSet16BitValue(bool includeValue, int16_t value, int16_t valueMinimum, int16_t valueMaximum, int16_t actualMinimum, int16_t actualMaximum, uint8_t dataLocation[]) 
{
	int16_t convertedValue;
	uint8_t highByte;
	uint8_t lowByte;
	int16_t realMinimum = min(valueMinimum, valueMaximum);
	int16_t realMaximum = max(valueMinimum, valueMaximum);

	if (includeValue == false) return 0;

	if (value < realMinimum) {
		value = realMinimum;
	}
	if (value > realMaximum) {
		value = realMaximum;
	}

	if (valueMinimum > valueMaximum) {
		// Values go from a larger number to a smaller number (e.g. 1024 to 0)
		value = realMaximum - value + realMinimum;
	}

	convertedValue = map(value, realMinimum, realMaximum, actualMinimum, actualMaximum);

	highByte = (uint8_t)(convertedValue >> 8);
	lowByte = (uint8_t)(convertedValue & 0x00FF);
	
	dataLocation[0] = lowByte;
	dataLocation[1] = highByte;
	
	return 2;
}

int Flystick_::buildAndSetSimulationValue(bool includeValue, int16_t value, int16_t valueMinimum, int16_t valueMaximum, uint8_t dataLocation[]) 
{
	return buildAndSet16BitValue(includeValue, value, valueMinimum, valueMaximum, FLYSTICK_SIMULATOR_MINIMUM, FLYSTICK_SIMULATOR_MAXIMUM, dataLocation);
}

void Flystick_::sendState()
{
	uint8_t data[_hidReportSize];
	int index = 0;
		
	// Set Simulation Values
	index += buildAndSetSimulationValue(_includeSimulatorFlags & FLYSTICK_INCLUDE_RUDDER, _rudder, _rudderMinimum, _rudderMaximum, &(data[index]));
	index += buildAndSetSimulationValue(_includeSimulatorFlags & FLYSTICK_INCLUDE_THROTTLE, _throttle, _throttleMinimum, _throttleMaximum, &(data[index]));
	index += buildAndSetSimulationValue(_includeSimulatorFlags & FLYSTICK_INCLUDE_TOEBRAKE, _toeBrakeL, _toeBrakeMinimumL, _toeBrakeMaximumL, &(data[index]));
	index += buildAndSetSimulationValue(_includeSimulatorFlags & FLYSTICK_INCLUDE_TOEBRAKE, _toeBrakeR, _toeBrakeMinimumR, _toeBrakeMaximumR, &(data[index]));
	index += buildAndSetSimulationValue(_includeSimulatorFlags & FLYSTICK_INCLUDE_ACCELERATOR, _accelerator, _acceleratorMinimum, _acceleratorMaximum, &(data[index]));
	index += buildAndSetSimulationValue(_includeSimulatorFlags & FLYSTICK_INCLUDE_BRAKE, _brake, _brakeMinimum, _brakeMaximum, &(data[index]));
	index += buildAndSetSimulationValue(_includeSimulatorFlags & FLYSTICK_INCLUDE_STEERING, _steering, _steeringMinimum, _steeringMaximum, &(data[index]));

	DynamicHID().SendReport(_hidReportId, data, _hidReportSize);
}

#endif

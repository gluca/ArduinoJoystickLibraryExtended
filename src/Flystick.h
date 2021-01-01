/*
  Flystick.h

  Copyright (c) 2015-2017, Matthew Heironimus
  Copyright (c) 2020-    , Gianluca Magalotti

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

#ifndef FLYSTICK_h
#define FLYSTICK_h

#include "DynamicHID/DynamicHID.h"

#if ARDUINO < 10606
#error The Flystick library requires Arduino IDE 1.6.6 or greater. Please update your IDE.
#endif // ARDUINO < 10606

#if ARDUINO > 10606
#if !defined(USBCON)
#error The Flystick library can only be used with a USB MCU (e.g. Arduino Leonardo, Arduino Micro, etc.).
#endif // !defined(USBCON)
#endif // ARDUINO > 10606

#if !defined(_USING_DYNAMIC_HID)

#warning "Using legacy HID core (non pluggable)"

#else // !defined(_USING_DYNAMIC_HID)

//================================================================================
//  Flystick (Flight Simulators Devices)

#define FLYSTICK_DEFAULT_REPORT_ID         0x03
#define FLYSTICK_DEFAULT_SIMULATOR_MINIMUM    0
#define FLYSTICK_DEFAULT_SIMULATOR_MAXIMUM 1023

#define FLYSTICK_TYPE_FLIGHT_DEVICE        0x01
#define FLYSTICK_TYPE_SPACESHIP_DEVICE     0x04
#define FLYSTICK_TYPE_AIRPLANE_DEVICE      0x09

class Flystick_
{
private:

    // Flystick State
	int16_t                  _throttle;    //BB
	int16_t                  _rudder;      //BA
	int16_t					 _accelerator; //C4
	int16_t					 _brake;       //C5
	int16_t					 _steering;    //C8
	int16_t                  _toeBrakeL;   //BF
	int16_t                  _toeBrakeR;   //BF

    // Joystick Settings
    bool                     _autoSendState;
	uint8_t					 _includeSimulatorFlags;
	int16_t                  _rudderMinimum = FLYSTICK_DEFAULT_SIMULATOR_MINIMUM;
	int16_t                  _rudderMaximum = FLYSTICK_DEFAULT_SIMULATOR_MAXIMUM;
	int16_t                  _throttleMinimum = FLYSTICK_DEFAULT_SIMULATOR_MINIMUM;
	int16_t                  _throttleMaximum = FLYSTICK_DEFAULT_SIMULATOR_MAXIMUM;
	int16_t                  _acceleratorMinimum = FLYSTICK_DEFAULT_SIMULATOR_MINIMUM;
	int16_t                  _acceleratorMaximum = FLYSTICK_DEFAULT_SIMULATOR_MAXIMUM;
	int16_t                  _brakeMinimum = FLYSTICK_DEFAULT_SIMULATOR_MINIMUM;
	int16_t                  _brakeMaximum = FLYSTICK_DEFAULT_SIMULATOR_MAXIMUM;
	int16_t                  _toeBrakeMinimumL = FLYSTICK_DEFAULT_SIMULATOR_MINIMUM;
	int16_t                  _toeBrakeMaximumL = FLYSTICK_DEFAULT_SIMULATOR_MAXIMUM;
	int16_t                  _toeBrakeMinimumR = FLYSTICK_DEFAULT_SIMULATOR_MINIMUM;
	int16_t                  _toeBrakeMaximumR = FLYSTICK_DEFAULT_SIMULATOR_MAXIMUM;
	int16_t                  _steeringMinimum = FLYSTICK_DEFAULT_SIMULATOR_MINIMUM;
	int16_t                  _steeringMaximum = FLYSTICK_DEFAULT_SIMULATOR_MAXIMUM;

	uint8_t                  _hidReportId;
	uint8_t                  _hidReportSize; 

protected:
	int buildAndSet16BitValue(bool includeValue, int16_t value, int16_t valueMinimum, int16_t valueMaximum, int16_t actualMinimum, int16_t actualMaximum, uint8_t dataLocation[]);
	int buildAndSetAxisValue(bool includeAxis, int16_t axisValue, int16_t axisMinimum, int16_t axisMaximum, uint8_t dataLocation[]);
	int buildAndSetSimulationValue(bool includeValue, int16_t value, int16_t valueMinimum, int16_t valueMaximum, uint8_t dataLocation[]);

public:
	Flystick_(
		uint8_t hidReportId = FLYSTICK_DEFAULT_REPORT_ID,
		uint8_t stickType = FLYSTICK_TYPE_FLIGHT_DEVICE,
		bool includeRudder = true,
		bool includeThrottle = true,
		bool includeToeBrake = true,
		bool includeAccelerator = true,
		bool includeBrake = true,
		bool includeSteering = true
		);

	void begin(bool initAutoSendState = true);
	void end();
	
	inline void setRudderRange(int16_t minimum, int16_t maximum)
	{
		_rudderMinimum = minimum;
		_rudderMaximum = maximum;
	}
	inline void setThrottleRange(int16_t minimum, int16_t maximum)
	{
		_throttleMinimum = minimum;
		_throttleMaximum = maximum;
	}
	inline void setAcceleratorRange(int16_t minimum, int16_t maximum)
	{
		_acceleratorMinimum = minimum;
		_acceleratorMaximum = maximum;
	}
	inline void setBrakeRange(int16_t minimum, int16_t maximum)
	{
		_brakeMinimum = minimum;
		_brakeMaximum = maximum;
	}
	inline void setSteeringRange(int16_t minimum, int16_t maximum)
	{
		_steeringMinimum = minimum;
		_steeringMaximum = maximum;
	}

	inline void setToeBrakeRange(int16_t minimumLeft, int16_t maximumLeft,int16_t minimumRight, int16_t maximumRight )
	{
		_toeBrakeMinimumL = minimumLeft;
		_toeBrakeMaximumL = maximumLeft;
		_toeBrakeMinimumR = minimumRight;
		_toeBrakeMaximumR = maximumRight;
	}

	// Set Simuation Values
	void setRudder(int16_t value);
	void setThrottle(int16_t value);
	void setAccelerator(int16_t value);
	void setBrake(int16_t value);
	void setToeBrake(int16_t valueL,int16_t valueR);
	void setSteering(int16_t value);
	
	void sendState();
};

#endif // !defined(_USING_DYNAMIC_HID)
#endif // FLYSTICK_h

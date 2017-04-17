/*
Fochica™ - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// 
// 
// 

#include "CalibratedSensorTester.h"
#include "DebugStream.h"
#include "SensorMock.h"
#include "RNGUtils.h"

bool CalibratedSensorTester::runTests()
{
	if (DebugStream == NULL)
		return false; // assume we have a debug stream

	DebugStream->println(F("CalibratedSensorTester start"));

	// setup randomness
	RNGUtils::seedWithAnalogInputs();

	// test mock sources
	DebugStream->println(F("Uniform source [100,200]"));
	SensorMock s1("Uniform", SensorMockType::Uniform, 100, 200, 0);
	for (int i = 0; i < 100; i++)
		DebugStream->println(s1.getValueInt());

	DebugStream->println(F("Normal source [100,200]"));
	SensorMock s2("Normal", SensorMockType::Normal, 100, 200, 0);
	for (int i = 0; i < 200; i++)
		DebugStream->println(s2.getValueInt());

	DebugStream->println(F("Saw source [100,200]"));
	SensorMock s3("Saw", SensorMockType::Saw, 100, 200, 8);
	for (int i = 0; i < 100; i++)
		DebugStream->println(s3.getValueInt());

	DebugStream->println(F("Sine source [100,200]"));
	SensorMock s4("Sine", SensorMockType::Sine, 100, 200, 8);
	for (int i = 0; i < 100; i++)
		DebugStream->println(s4.getValueInt());

	// test calibrated sensor
	DebugStream->println(F("Calibration [a=0.5, 50, 150] Uniform source [100,200]"));
	s1.resetState();
	CalibratedSensor cs1(&s1, CalibratedSensor::MAX_EXP_ALPHA/2, 50, 150);
	for (int i = 0; i < 100; i++)
		getAndPrintSample(cs1);

	DebugStream->println(F("Calibration [a=0.5, 120, 180] Normal source [100,200]"));
	s2.resetState();
	CalibratedSensor cs2(&s2, CalibratedSensor::MAX_EXP_ALPHA / 2, 120, 180);
	for (int i = 0; i < 100; i++)
		getAndPrintSample(cs2);

	DebugStream->println(F("Calibration [a=0.5, 110, 150] Saw source [100,200]"));
	s3.resetState();
	CalibratedSensor cs3(&s3, CalibratedSensor::MAX_EXP_ALPHA / 2, 110, 150);
	for (int i = 0; i < 100; i++)
		getAndPrintSample(cs3);

	DebugStream->println(F("Calibration [a=0.5, 110, 150] Sine source [100,200]"));
	s4.resetState();
	CalibratedSensor cs4(&s4, CalibratedSensor::MAX_EXP_ALPHA / 2, 110, 150);
	for (int i = 0; i < 100; i++)
		getAndPrintSample(cs4);

	// test calibration
	DebugStream->println(F("Non intersecting"));
	cs2.resetCalibrationData();
	s2.setParams(SensorMockType::Normal, 100, 200, 0);
	cs2.calibrate(CalibratedSensorState::A);
	cs2.debugCalibrationState();
	s2.setParams(SensorMockType::Normal, 300, 400, 0);
	cs2.calibrate(CalibratedSensorState::B);
	cs2.debugCalibrationState();

	DebugStream->println(F("Intersecting"));
	cs2.resetCalibrationData();
	s2.setParams(SensorMockType::Normal, 100, 200, 0);
	cs2.calibrate(CalibratedSensorState::A);
	cs2.debugCalibrationState();
	s2.setParams(SensorMockType::Normal, 150, 250, 0);
	cs2.calibrate(CalibratedSensorState::B);
	cs2.debugCalibrationState();

	DebugStream->println(F("Contained"));
	cs2.resetCalibrationData();
	s2.setParams(SensorMockType::Normal, 100, 200, 0);
	cs2.calibrate(CalibratedSensorState::A);
	cs2.debugCalibrationState();
	s2.setParams(SensorMockType::Normal, 120, 150, 0);
	cs2.calibrate(CalibratedSensorState::B);
	cs2.debugCalibrationState();

	DebugStream->println(F("CalibratedSensorTester end"));
	return true;
}

void CalibratedSensorTester::getAndPrintSample(CalibratedSensor & cs) {
	sensorVal_t raw, filtered;
	CalibratedSensorState::e state;
	state = cs.getValue(&raw, &filtered);

	DebugStream->print(raw);
	DebugStream->print('\t');
	DebugStream->print(filtered);
	DebugStream->print('\t');
	DebugStream->println(state);
}

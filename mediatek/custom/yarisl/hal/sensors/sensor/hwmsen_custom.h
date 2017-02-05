/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#ifndef __HWMSEN_CUSTOM_H__ 
#define __HWMSEN_CUSTOM_H__

#ifdef CUSTOM_KERNEL_ACCELEROMETER
    #define ACCELEROMETER           "bma222 3-axis Accelerometer"
    #define ACCELEROMETER_VENDER    "The Android Open Source Project"
#endif

#ifdef CUSTOM_KERNEL_ALSPS
	#define PROXIMITY 		"stk3x1x/tmd2771 Proximity Sensor"
	#define PROXIMITY_VENDER 	"Sitronix/Capella"
	#define LIGHT 			"stk3x1x/tmd2771 Light Sensor"
	#define LIGHT_VENDER 		"Sitronix/Capella"
#endif

#ifdef CUSTOM_KERNEL_MAGNETOMETER
	#define MAGNETOMETER 		"mmc3416 3-axis Magnetic Field sensor"
	#define MAGNETOMETER_VENDER 	"Memsic"
	#define ORIENTATION 		"mmc3416 3-axis Orientation sensor"
	#define ORIENTATION_VENDER 	"Memsic"
#endif

#ifdef CUSTOM_KERNEL_GYROSCOPE
	#define GYROSCOPE 		"MPU3050c gyroscope Sensor"
	#define GYROSCOPE_VENDER 	"Invensensor"
#endif

#ifdef CUSTOM_KERNEL_BAROMETER
	#define PRESSURE 		"BMA050"
	#define PRESSURE_VENDER		"BMA050"
#endif

#ifdef CUSTOM_KERNEL_TEMPURATURE
	#define TEMPURATURE 		"BMA050"
	#define TEMPURATURE_VENDER	"BMA050"
#endif

#endif


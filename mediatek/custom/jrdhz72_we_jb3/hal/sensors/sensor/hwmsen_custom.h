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

//FR 437100 and 437101 llf 20130427
#define MAX_NUM_SENSORS                 3
#define MSENSOR_AMI_LIB


#ifdef CUSTOM_KERNEL_ACCELEROMETER
    #define ACCELEROMETER           "bma222e/kxtj2 3-axis Accelerometer"
    #define ACCELEROMETER_VENDER    "Bma222e"
#endif

#ifdef CUSTOM_KERNEL_MAGNETOMETER
	#define MAGNETOMETER 		"AKM8963 3-axis Magnetic Field sensor"
	#define MAGNETOMETER_VENDER 	"AKM"
	#define ORIENTATION 		"AKM8963 Orientation sensor"
	#define ORIENTATION_VENDER 	"AKM"
#endif

#endif


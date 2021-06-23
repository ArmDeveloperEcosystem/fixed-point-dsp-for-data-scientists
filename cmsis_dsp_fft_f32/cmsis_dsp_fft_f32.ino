//
// Copyright (c) 2021 Arm Limited and Contributors. All rights reserved.
//
// SPDX-License-Identifier: Apache-2.0
//

#ifdef ARDUINO_ARCH_SAMD
#define ARM_MATH_CM0PLUS
#endif

#include <arm_math.h>

// constants
const int WINDOW_SIZE = 256;
const int STEP_SIZE   = 128;

// global variables
float32_t input_f32[WINDOW_SIZE];
float32_t hanning_window_f32[WINDOW_SIZE];
float32_t processed_window_f32[WINDOW_SIZE];

arm_rfft_fast_instance_f32 S_f32;

// this is twice the size because each FFT output has a real and imaginary part
float32_t fft_f32[WINDOW_SIZE * 2];

// this is half the size of WINDOW_SIZE becase we just need the magnitude from
// the first half of the FFT output
float32_t fft_mag_f32[WINDOW_SIZE / 2];


void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("Hello from CMSIS DSP F32 FFT example");

  for (int i = 0; i < WINDOW_SIZE; i++) {
    input_f32[i] = sin((2 * PI * 440) / 16000 * i);
  }

  // pipeline initialization
  hanning_window_init_f32(hanning_window_f32, WINDOW_SIZE);
  arm_rfft_fast_init_f32(&S_f32, WINDOW_SIZE);
}

void loop() {
  unsigned long pipeline_start_us = micros();

  // equivalent to: processed_window_f32 = input_f32 * hanning_window_f32
  arm_mult_f32(input_f32, hanning_window_f32, processed_window_f32, WINDOW_SIZE);

  // calculate the FFT and FFT magnitude
  arm_rfft_fast_f32(&S_f32, processed_window_f32, fft_f32, 0);
  arm_cmplx_mag_f32(fft_f32, fft_mag_f32, WINDOW_SIZE / 2);

  unsigned long pipeline_end_us = micros();

  Serial.print("Pipeline run time = ");
  Serial.print(pipeline_end_us - pipeline_start_us);
  Serial.println(" microseconds");


  // all done loop forever ...
  while (1);
}


void hanning_window_init_f32(float32_t* hanning_window_f32, size_t size) {
  for (size_t i = 0; i < size; i++) {
    // calculate the Hanning Window value for i as a float32_t
    hanning_window_f32[i] = 0.5 * (1.0 - arm_cos_f32(2 * PI * i / size ));
  }
}

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
q15_t input_q15[WINDOW_SIZE];
q15_t hanning_window_q15[WINDOW_SIZE];
q15_t processed_window_q15[WINDOW_SIZE];

arm_rfft_instance_q15 S_q15;

// this is twice the size because each FFT output has a real and imaginary part
q15_t fft_q15[WINDOW_SIZE * 2];

// this is half the size of WINDOW_SIZE becase we just need the magnitude from
// the first half of the FFT output
q15_t fft_mag_q15[WINDOW_SIZE / 2];


void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("Hello from CMSIS DSP Q15 FFT example");

  for (int i = 0; i < WINDOW_SIZE; i++) {
    float32_t f = sin((2 * PI * 440) / 16000 * i);

    arm_float_to_q15(&f, &input_q15[i], 1);
  }

  // pipeline initialization
  hanning_window_init_q15(hanning_window_q15, WINDOW_SIZE);
  arm_rfft_init_q15(&S_q15, WINDOW_SIZE, 0, 1);
}

void loop() {
  unsigned long pipeline_start_us = micros();

  // equivalent to: processed_window_q15 = input_q15 * hanning_window_q15
  arm_mult_q15(input_q15, hanning_window_q15, processed_window_q15, WINDOW_SIZE);

  // calculate the FFT and FFT magnitude
  arm_rfft_q15(&S_q15, processed_window_q15, fft_q15);
  arm_cmplx_mag_q15(fft_q15, fft_mag_q15, WINDOW_SIZE / 2);

  unsigned long pipeline_end_us = micros();

  Serial.print("Pipeline run time = ");
  Serial.print(pipeline_end_us - pipeline_start_us);
  Serial.println(" microseconds");


  // all done loop forever ...
  while (1);
}


void hanning_window_init_q15(q15_t* hanning_window_q15, size_t size) {
  for (size_t i = 0; i < size; i++) {
    // calculate the Hanning Window value for i as a float32_t
    float32_t f = 0.5 * (1.0 - arm_cos_f32(2 * PI * i / size ));

    // convert value for index i from float32_t to q15_t and store
    // in window at position i
    arm_float_to_q15(&f, &hanning_window_q15[i], 1);
  }
}

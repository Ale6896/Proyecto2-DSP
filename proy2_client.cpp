/**
 * proy2_client.cpp
 *
 * Copyright (C) 2023  Pablo Alvarado
 * EL5802 Procesamiento Digital de Señales
 * Escuela de Ingeniería Electrónica
 * Tecnológico de Costa Rica
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the authors nor the names of its contributors may be
 *    used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "proy2_client.h"

#include <cstring>
#include <cmath>
#include <iostream>
#include <fstream>
#define REAL 0
#define IMAG 1

proy2_client::proy2_client() : jack::client() {
  y1 = 0.0;
  y2 = 0.0;
  x1 = 0.0;
  x2 = 0.0;
  a0 = 1.0;
  a1 = 0.0;
  a2 = 0.0;
  b0 = 1.0;
  b1 = 0.0;
  b2 = 0.0;
  alpha = 0.5;
  polo = 0.9;
  modo = time_filter;
  modo_reverb = no_reverb;
  _power = 0.0f;
  _volume = 1.0f;
  L = 800;
  pb0 = 0.9;
  pb1 = -0.9;
  pa1 = -0.8;
  int nframes=1024;
  // Define the input and output buffers
  px = boost::circular_buffer<float>(L, 0.0);
  py = boost::circular_buffer<float>(L, 0.0);
  yr = boost::circular_buffer<float>(1024*10 + 2, 0.0);
  intermediate_1 = std::vector<sample_t>(nframes, 0.0);
  intermediate_2 = std::vector<sample_t>(nframes, 0.0);

  
  /*
   * X values mapped from 0Hz to 24kHz to a 1024 buffer size
   * This values correspond to frecuencies 93.842, 140.76,
   * 164.22, 234.6, 304.99, 398.83, 516.13, 680.35, 891.5,
   * 1173, 1548.4, 2041.1, 2674.5, 3519.1, 4645.2, 6099.7,
   * 8023.5, 10557, 13865, 18252, 24000
   * Also, 0 Hz value is needed just to get the interpolation done
   */
  n = 22; 
  x_equalizer = {0,6,8,11,15,19,25,33,44,57,75,99,130,171,225,297,390,513,675,887,1167,1535};
  y_equalizer = {1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0,1.0};
    
  // Initialize the interpolator
  acc = gsl_interp_accel_alloc();
  spline = gsl_spline_alloc(gsl_interp_akima, n);
  
}

proy2_client::~proy2_client() {
// Free the interpolator
  gsl_spline_free(spline);
  gsl_interp_accel_free(acc);
  
  // Free the pointers
  delete[] x_interp;
  delete[] y_interp;
}
  
/**
 * The process callback for this JACK application is called in a
 * special realtime thread once for each audio cycle.
 *
 * This client does nothing more than copy data from its input
 * port to its output port. It will exit when stopped by 
   * the user (e.g. using Ctrl-C on a unix-ish operating system)
   */

void proy2_client::set_mode(const float *const A, const float *const B) {
  a0 = *A;
  b0 = *B;

  a1 = *(A + 1);
  b1 = *(B + 1);

  a2 = *(A + 2);
  b2 = *(B + 2);
}

void proy2_client::set_props() {
  std::filesystem::path ruta_al_archivo2 = "../filters_octave/hn_filtro_interesante.txt";
  std::vector<float> valores2;
  float valor2;

  if (std::filesystem::exists(ruta_al_archivo2)) {
    std::ifstream archivo(ruta_al_archivo2);

    while (archivo >> valor2) {
      valores2.push_back(valor2);
    }
  }
  else {
    std::cout << "El archivo no existe" << std::endl;
  }

  interesante = std::unique_ptr<float[]>(new float[buffer_size()]);
  for (size_t i = 0; i < valores2.size(); ++i) {
    interesante[i] = valores2[i];
  }
}

void proy2_client::set_size_block(const std::size_t block_size) {
  _ffilter.set_block_size(block_size);
}

void proy2_client::set_filter(float* hn, std::size_t hn_size, std::size_t Hw_size) {
  _ffilter.set_filter(hn, hn_size, Hw_size);
}

void proy2_client::set_filter_h(const fftwf_complex* Hw,
                             std::size_t Hw_size,
                             std::size_t hn_size) {
  _ffilter.set_filter_h(Hw, Hw_size, hn_size);
}

void proy2_client::set_equalizer() {
  if(eq_is_on){
    _ffilter.set_filter_h(equ_Hw, _Hw_size, _hn_size);
  }
}

void proy2_client::update_equalizer(int index, double value) {
  
  y_equalizer[index] = pow(10, value/20); // Logarithmic scale
  
  // Fit the interpolator to the data
  gsl_spline_init(spline, x_equalizer.data(), y_equalizer.data(), n);
  
  for(int i = 0; i < (3*_hn_size/2); i++) {
    equ_Hw[i][REAL] = gsl_spline_eval(spline, double(i), acc);
  }

  proy2_client::set_equalizer();

  return;

}

void proy2_client::set_buffers() {
  
  _hn_size = buffer_size();
  _Hw_size = 2*_hn_size;
  eq_is_on = false;
  
  //std::vector<double> x_interp(_hn_size);
  //std::vector<double> y_interp(_hn_size);
  //y_interp = new double[_hn_size];
  equ_Hw = new fftwf_complex[_Hw_size];

  for(int i = 0; i < (3*_hn_size/2); i++) {
    equ_Hw[i][REAL] = 1.0f;
  }

  return;
}

bool proy2_client::process(jack_nframes_t nframes,
                                 const sample_t *const in,
                                 sample_t *const out) {
  if (modo_reverb == simple_reverb) {
    const sample_t *in_ptr = in;
    sample_t *out_ptr = intermediate_1.data();

    for (; in_ptr < in + nframes; ++out_ptr, ++in_ptr) {
      *out_ptr = *in_ptr + alpha * yr.front();
      yr.push_back(*out_ptr);
    }
  }

  else if (modo_reverb == filtered_reverb) {

    const sample_t *in_ptr = in;
    sample_t *out_ptr = intermediate_1.data();
    const float *yr_ptr_begin = &yr.front();
    float temp;

    for (; in_ptr < in + nframes; ++out_ptr, ++in_ptr) {

      temp = 1/2.0 * (4.0*polo*y1 - 2.0*polo*polo*y2 + (1-polo)*(1-polo)* *(yr_ptr_begin - 1) + (1-polo)*(1-polo)* *(yr_ptr_begin));

      *out_ptr = *in_ptr + alpha * temp;

      y2 = y1;
      y1 = temp;
      x2 = x1;
      x1 = *in_ptr;

      yr.push_back(*out_ptr);

      yr_ptr_begin = &yr.front();

    }
  }
  else {
    // If no reverb mode runs, we have to copy input values to intermediate_1 
    memcpy(intermediate_1.data(), in, nframes * sizeof(sample_t));
  }

  if (modo == over_save && eq_is_on) {
    _ffilter.overlap_save(intermediate_1.data(), intermediate_2.data());
  }
  else if (modo == over_add && eq_is_on) {
    _ffilter.overlap_add(intermediate_1.data(), intermediate_2.data());
  }
  else {
   // If eq_is_on is false, eq is no executed, then we copy intermediate_1 values to intermediate_2
   memcpy(intermediate_2.data(), intermediate_1.data(), nframes * sizeof(sample_t));
  }
 
  if (peine_is_on) {
    for (unsigned int i = 0; i < nframes; ++i) {
        // Compute the filter output
        out[i] = pb0*intermediate_2[i]+ pb1 * px.front() - pa1*py.front();

        // Push the output into the output buffer
        py.push_back(out[i]);

        // Push the new sample into the input buffer
        px.push_back(intermediate_2[i]);
  }
  }
  else {
    // If peine_is_on is false, then we have to copy values from intermediate_2 to out
    memcpy(out, intermediate_2.data(), nframes * sizeof(sample_t));
  }
  //}  
  /*
   * Calculo de energia
   */
   
  sample_t* ptr = out;
  sample_t *const endptr = out + nframes;

  float block_energy=0.0f;
  
  for (;ptr!=endptr;++ptr) {
    *ptr *= _volume;
    block_energy += (*ptr)*(*ptr);
  }

  block_energy/=nframes;

  constexpr float c = 0.6f;
  _power = std::max(block_energy*c + (1.0f-c)*_power,block_energy);
  
  return true;
}

float proy2_client::power() const {
  return _power;
}

/**
 * Set the volume
 */
void proy2_client::set_volume(float vol) {
  _volume=vol*vol;
}

/**
 * Get current volumen setting
 */
float proy2_client::volume() const {
  return std::sqrt(_volume);
}


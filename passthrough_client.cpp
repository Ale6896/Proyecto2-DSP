/**
 * passthrough_client.cpp
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

#include "passthrough_client.h"

#include <cstring>
#include <cmath>

passthrough_client::passthrough_client() : jack::client(),_ffilter() {
}

passthrough_client::~passthrough_client() {
}

jack::client_state passthrough_client::init() {
  auto ret = jack::client::init();

  // This code is just an example (to be removed) of how to setup the
  // freq_filter:
  
  // Let's compute the delta[n] as impulse response
  // response of the passthrough filter:
  std::unique_ptr<float[]> delta(new float[this->buffer_size()]);
  memset(delta.get(),0,this->buffer_size()*sizeof(float));
  delta[0]=1.0f;
  
  // let the freq_filter know the length of the block_size
  _ffilter.set_block_size(buffer_size());
  
  // and tell it the impulse response of the filter
  _ffilter.set_filter(delta.get(),buffer_size(),2*buffer_size()-1);

  _power = 0.0f;
  
  return ret;
}
  
/**
 * The process callback for this JACK application is called in a
 * special realtime thread once for each audio cycle.
 *
 * This client does nothing more than copy data from its input
 * port to its output port. It will exit when stopped by 
   * the user (e.g. using Ctrl-C on a unix-ish operating system)
   */
bool passthrough_client::process(jack_nframes_t nframes,
                                 const sample_t *const in,
                                 sample_t *const out) {

  _ffilter.overlap_save(in,out);

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
  
float passthrough_client::power() const {
  return _power;
}

/**
 * Set the volume
 */
void passthrough_client::set_volume(float vol) {
  _volume=vol*vol;
}

/**
 * Get current volumen setting
 */
float passthrough_client::volume() const {
  return std::sqrt(_volume);
}

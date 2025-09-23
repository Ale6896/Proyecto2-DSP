/**
 * tarea4_client.h
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

#ifndef _TAREA4_CLIENT_H
#define _TAREA4_CLIENT_H


#include "jack_client.h"
#include "freq_filter.h"
#include <boost/circular_buffer.hpp>

/**
 * Jack client class
 *
 * This class wraps some basic jack functionality.
 */
class tarea4_client : public jack::client {
private:
  freq_filter _ffilter;

public:
  // typedef jack::client::sample_t sample_t;

  enum Mode {
    over_add,
    over_save,
    time_filter,
    reverb
  };

  float y1;
  float y2;
  float x1;
  float x2;
  float a0;
  float a1;
  float a2;
  float b0;
  float b1;
  float b2;
  boost::circular_buffer<float> yr;

  Mode modo;

  void set_size_block(const std::size_t block_size);

  void set_filter(float* hn,
                      std::size_t hn_size,
                      std::size_t Hw_size);

  /**
   * The default constructor performs some basic connections.
   */
  tarea4_client();
  ~tarea4_client();

  /**
   * Tarea3 functionality
   */
  virtual bool process(jack_nframes_t nframes,
                       const sample_t *const in,
                       sample_t *const out) override;

  void set_mode(const float *const A, const float *const B);

};


#endif

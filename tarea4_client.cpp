/**
 * tarea4_client.cpp
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

#include "tarea4_client.h"

#include <cstring>

tarea4_client::tarea4_client() : jack::client() {
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
  modo = time_filter;

  yr = boost::circular_buffer<float>(1024, 0.0);
}

tarea4_client::~tarea4_client() {
}
  
/**
 * The process callback for this JACK application is called in a
 * special realtime thread once for each audio cycle.
 *
 * This client does nothing more than copy data from its input
 * port to its output port. It will exit when stopped by 
   * the user (e.g. using Ctrl-C on a unix-ish operating system)
   */

void tarea4_client::set_mode(const float *const A, const float *const B) {
  a0 = *A;
  b0 = *B;

  a1 = *(A + 1);
  b1 = *(B + 1);

  a2 = *(A + 2);
  b2 = *(B + 2);
}

void tarea4_client::set_size_block(const std::size_t block_size) {
  _ffilter.set_block_size(block_size);
}

void tarea4_client::set_filter(float* hn, std::size_t hn_size, std::size_t Hw_size) {
  _ffilter.set_filter(hn, hn_size, Hw_size);
}

bool tarea4_client::process(jack_nframes_t nframes,
                                 const sample_t *const in,
                                 sample_t *const out) {

  if (modo == time_filter) {
    const sample_t *in_ptr = in;
    sample_t *out_ptr = out;

    *out_ptr = 1/a0 * (-(a1*y1 + a2*y2) + (b0 * *in_ptr + b1 * x1 + b2 * x2));
    ++out_ptr, ++in_ptr;
    *out_ptr = 1/a0 * (-(a1* *(out_ptr - 1) + a2*y1) + (b0 * *in_ptr + b1 * *(in_ptr - 1) + b2 * x1));
    ++out_ptr, ++in_ptr;

    for (; in_ptr < in + nframes; ++out_ptr, ++in_ptr) {
      *out_ptr = 1/a0 * (-(a1* *(out_ptr - 1) + a2* *(out_ptr - 2)) + (b0 * *in_ptr + b1 * *(in_ptr - 1) + b2 * *(in_ptr - 2)));

    }

    y1 = *(out_ptr - 1);
    y2 = *(out_ptr - 2);
    x1 = *(in_ptr - 1);
    x2 = *(in_ptr - 2);
  }
  else if (modo == reverb) {
    const sample_t *in_ptr = in;
    sample_t *out_ptr = out;
    float *last_ptr = &yr[0];

    for (; in_ptr < in + nframes; ++out_ptr, ++in_ptr, ++last_ptr) {
      *out_ptr = *in_ptr + a0 * *last_ptr;
    }

    out_ptr = out;
    for (; out_ptr != out + nframes; ++out_ptr) {
      yr.push_back(*out_ptr);
    }
  }
  else if (modo == over_save) {
    _ffilter.overlap_save(in, out);
  }
  else if (modo == over_add) {
    _ffilter.overlap_add(in, out);
  }

  return true;
}


/**
 * main.cpp
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

/** @file main.cpp
 *
 * @brief This is a C++ version of a simple jack client, to serve as
 * a simple framework to test basic digital signal processing algorithms,
 * applied to audio.
 */

#include <cstdlib>

#include <iostream>
#include <stdexcept>
#include <filesystem>
#include <vector>
#include <fstream>

#include <csignal>

#include <boost/program_options.hpp>

#include "waitkey.h"
#include "proy2_client.h"

#include "mainwindow.h"

#include <QtWidgets/QApplication>


namespace po=boost::program_options;

void signal_handler(int signal) {
  if (signal == SIGINT) {
    std::cout << "Ctrl-C caught, cleaning up and exiting" << std::endl;

    // Let RAII do the clean-up
    exit(EXIT_SUCCESS);
  }
}

int main (int argc, char *argv[])
{
  std::signal(SIGINT,signal_handler);

  
  try {
  
    QApplication a(argc, argv);
    
    static proy2_client client;

    po::options_description desc("Allowed options");

    desc.add_options()
      ("help,h","show usage information")
      ("files,f",
       po::value<std::vector<std::filesystem::path> >()->multitoken(),
       "List of audio files to be played");

    po::variables_map vm;
    po::store(po::parse_command_line(argc,argv,desc),vm);

    if (vm.count("help")) {
      std::cout << desc << std::endl;
      return EXIT_SUCCESS;
    }
    
    MainWindow w;

    if (client.init() != jack::client_state::Running) {
      throw std::runtime_error("Could not initialize the JACK client");
    }

    if (vm.count("files")) {
      const std::vector< std::filesystem::path >&
        audio_files = vm["files"].as< std::vector<std::filesystem::path> >();
    
      for (const auto& f : audio_files) {
        bool ok =client.add_file(f);
        std::cout << "Adding file '" << f.c_str() << "' "
                  << (ok ? "succedded" : "failed") << std::endl;
      }
    }
    
    // keep running until stopped by the user
    std::cout << "Press x key to exit" << std::endl;

    std::vector<float> A_l = {1.0, -1.9257, 0.9270};
    std::vector<float> B_l = {0.0, 0.00069095, 0.00069095};

    std::vector<float> A_m = {1.0, -1.9797, 0.9801};
    std::vector<float> B_m = {0.99, -1.9798, 0.99};

    std::vector<float> A_b = {1.0, -1.9950, 0.9953};
    std::vector<float> B_b = {-0.0023462, 0.0, 0.0023462};

    std::vector<float> A_h = {1.0, -0.9860, 0.0};
    std::vector<float> B_h = {0.9930, -0.9930, 0.0};

    std::vector<float> A_p = {1.0, 0.0, 0.0};
    std::vector<float> B_p = {1.0, 0.0, 0.0};

    client.set_size_block(client.buffer_size());

    std::unique_ptr<float[]> delta(new float[client.buffer_size()]);
    memset(delta.get(),0,client.buffer_size()*sizeof(float));
    delta[0]=1.0f;

    std::filesystem::path ruta_al_archivo = "../filters_octave/hn_filtro_muesca.txt";
    std::vector<float> valores;
    float valor;

    if (std::filesystem::exists(ruta_al_archivo)) {
      std::ifstream archivo(ruta_al_archivo);

      while (archivo >> valor) {
        valores.push_back(valor);
      }
    }
    else {
      std::cout << "El archivo no existe" << std::endl;
    }

    std::unique_ptr<float[]> notch(new float[valores.size()]);
    for (size_t i = 0; i < valores.size(); ++i) {
      notch[i] = valores[i];
    }

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

    std::unique_ptr<float[]> interesante(new float[client.buffer_size()]);
    for (size_t i = 0; i < valores2.size(); ++i) {
      interesante[i] = valores2[i];
    }
    
    w.show();
    return a.exec();

    
    /*

    int key = -1;
    bool go_away=false;
    while (!go_away) {
      key = waitkey(100);
      if (key>0) {
        switch(key) {
        case 'x': {
          go_away=true;
          std::cout << "Finishing..." << std::endl;
        } break;
        case 'r': {

          if (vm.count("files")) {
            const std::vector< std::filesystem::path >&
              audio_files =
              vm["files"].as< std::vector<std::filesystem::path> >();
            
            for (const auto& f : audio_files) {
              bool ok =client.add_file(f);
              std::cout << "  Re-adding file '" << f.c_str() << "' "
                        << (ok ? "succedded" : "failed") << std::endl;
            }
          }
          
          std::cout << "Repeat playing files" << std::endl;
        } break;
        case 'z': {
          if (client.modo == proy2_client::over_save) {
            std::cout << "Se cambió a modo de filtrado solapamiento suma" << std::endl;
            client.modo = proy2_client::over_add;
          }
          else if (client.modo == proy2_client::over_add) {
            std::cout << "Se cambió a modo de filtrado solapamiento almacenamiento" << std::endl;
            client.modo = proy2_client::over_save;
          }
          else {
            std::cout << "No se encuentra en un modo de filtrado en frecuencia" << std::endl;
          }
        } break;
        case 'l': {
          std::cout << "Se cambió a filtro pasa bajas filtrando en el tiempo" << std::endl;
          client.set_mode(&A_l[0], &B_l[0]);
          client.modo = proy2_client::time_filter;
        } break;
        case 'm': {
          std::cout << "Se cambió a filtro de muesca filtrando en el tiempo" << std::endl;
          client.set_mode(&A_m[0], &B_m[0]);
          client.modo = proy2_client::time_filter;
        } break;
        case 'b': {
          std::cout << "Se cambió a filtro pasa banda filtrando en el tiempo" << std::endl;
          client.set_mode(&A_b[0], &B_b[0]);
          client.modo = proy2_client::time_filter;
        } break;
        case 'h': {
          std::cout << "Se cambió a filtro pasa altas filtrando en el tiempo" << std::endl;
          client.set_mode(&A_h[0], &B_h[0]);
          client.modo = proy2_client::time_filter;
        } break;
        case 'p': {
          std::cout << "Se cambió a modo passthrough filtrando en el tiempo" << std::endl;
          client.set_mode(&A_p[0], &B_p[0]);
          client.modo = proy2_client::time_filter;
        } break;
        case 'A': {
          std::cout << "Se cambió a modo passthrough filtrando en frecuencia" << std::endl;
          client.set_filter(delta.get(), client.buffer_size(), 2*client.buffer_size()-1);
          client.modo = proy2_client::over_save;
        } break;
        case 'S': {
          std::cout << "Se cambió a filtro de muesca filtrando en frecuencia" << std::endl;
          client.set_filter(notch.get(), client.buffer_size(), 2*client.buffer_size()-1);
          client.modo = proy2_client::over_save;
        } break;
        case 'M': {
          std::cout << "Se cambió a filtro interesante filtrando en frecuencia" << std::endl;
          client.set_filter(interesante.get(), client.buffer_size(), 2*client.buffer_size()-1);
          client.modo = proy2_client::over_save;
        } break;
        case 'i': {
          if (client.modo == proy2_client::time_filter) {
            std::cout << "Filtrado en el tiempo, actualmente se están usando estos coeficientes:\n" << std::endl;
            std::cout << "Coeficiente a0: " << client.a0 << " " << "Coeficiente a1: " << client.a1 << " " << "Coeficiente a2: " << client.a2  << std::endl;
            std::cout << "Coeficiente b0: " << client.b0 << " " << "Coeficiente b1: " << client.b1 << " " << "Coeficiente b2: " << client.b2 << " " << std::endl;
            std::cout << "Última salida guardada: " << client.y1 << " " << "Penúltima salida guardada: " << client.y2 << std::endl;
            std::cout << "Última entrada guardada: " << client.x1 << " " << "Penúltima entrada guardada: " << client.x2 << std::endl << std::endl;
          }
          else if (client.modo == proy2_client::over_save) {
            std::cout << "Procesado en frecuencia usando overlap save" << std::endl;
          }
          else {
            std::cout << "Procesado en frecuencia usando overlap add" << std::endl;
          }
        } break;
        case '?': {
          std::cout << "Following keys are enabled:\n"
                    << "x: exit program\n"
                    << "z: cambia el método de filtrado en frecuencia\n"
                    << "l: uso de filtro en el tiempo pasabajas\n"
                    << "m: uso de filtro en el tiempo muesca\n"
                    << "b: uso de filtro en el tiempo pasabanda\n"
                    << "h: uso de filtro en el tiempo pasaaltas\n"
                    << "p: uso de filtro en el tiempo pasatodo\n"
                    << "i: muestra los coeficientes usados en caso de usar filtrado en el tiempo\n"
                    << "S: uso de filtro en la frecuencia muesca\n"
                    << "A: uso de filtro en la frecuencia pasatodo\n"
                    << "M: uso de filtro en la frecuencia interesante\n"
                    << "?: show this help message\n"
                    << "r: repeat file playback"
                    << std::endl;
        } break;
        default: {
          if (key>32) {
            std::cout << "Key " << char(key) << " pressed" << std::endl;
          } else {
            std::cout << "Key " << key << " pressed" << std::endl;
          }
          key=-1;
        }
        } // switch key
      } // if (key>0)
    } // end while

    client.stop(); */
  }
  catch (std::exception& exc) {
    std::cout << argv[0] << ": Error: " << exc.what() << std::endl;
    exit(EXIT_FAILURE);
  }
  

	exit(EXIT_SUCCESS);
}

/**
 * mainwindow.h
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>

#include <QTimer>
#include <QFileDialog>
#include <QMainWindow>
#include <QSlider>
#include <QDebug>
#include <QString>

#include "proy2_client.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
  Q_OBJECT
  
public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  /**
   * Add file to playlist
   */
  bool add_file(const std::filesystem::path& file);
  
private:
  Ui::MainWindow *ui;

  /**
   * Jack client
   */
  static proy2_client _client;
  
  /**
   * Timer used to recompute the filter once the user changes the
   * values
   */
  std::unique_ptr<QTimer> _timer;

  /**
   * List of selected files so far
   */
  QStringList selectedFiles_;

  // Hw pointer for set_filter function
  std::unique_ptr<fftwf_complex[]> Hw;
  
  QSlider *sliders[20];
  

private slots:
  void on_fileEdit_returnPressed();
  void on_fileButton_clicked();

  void on_stopButton_clicked();


  void on_volumeDial_sliderMoved(int value);
  void on_volumeSpin_valueChanged(int value);

  void on_update_timer();
  void on_pushButton2_clicked();
  void on_pushButton3_clicked(); // Declare the slot here
  void handleValueChanged();
  void on_equalizer_stateChanged(bool eq_is_on);
  void on_peine_stateChanged(bool peine_is_on);
  
  void on_reverberador1_stateChanged();
  void on_alphaValue_sliderMoved(int position);
  void on_tauValue_sliderMoved(int position);

  void on_reverberador2_stateChanged(int arg1);
  void on_poloValue_sliderMoved(int position);
  void on_reverberador2_clicked();
  void on_reverberador1_clicked();
};
#endif // MAINWINDOW_H

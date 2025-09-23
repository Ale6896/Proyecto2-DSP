/**
 * mainwindow.cpp
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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <cstdlib>
#include <cmath>
#include <iostream>

#define pass (void)0

proy2_client MainWindow::_client;

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
{
  
  if (_client.init() != jack::client_state::Running) {
    throw std::runtime_error("Could not initialize the JACK client");
  }
  _client.set_size_block(_client.buffer_size());
  
  _client.set_buffers();
  
  ui->setupUi(this);
  
  for (int i = 0; i < 22; ++i) {
    QSlider *slider = this->findChild<QSlider *>("verticalSlider_" + QString::number(i));
    if (slider) {
      connect(slider, SIGNAL(sliderReleased()), this, SLOT(handleValueChanged()));
    } // sliderReleased()  valueChanged(int)
  }
 _client.set_props();

  /*
   * Set up a timer 4 times in a second to check if the user
   * changed the equalizer values, and if so, then create a new
   * filter response
   */
  _timer = std::make_unique<QTimer>(this);
  connect(_timer.get(), SIGNAL(timeout()), this, SLOT(on_update_timer()));
  _timer->start(100); // 10 times per second

  //connect(ui->pushButton3, SIGNAL(clicked()), this, SLOT(on_pushButton3_clicked()));

  connect(ui->ecualizador, SIGNAL(toggled(bool)), this, SLOT(on_equalizer_stateChanged(bool)));
  connect(ui->peine, SIGNAL(toggled(bool)), this, SLOT(on_peine_stateChanged(bool)));
  
  // definición filtro paso todo en freq
  // Create a frequency response array with all values set to 1
  Hw = std::make_unique<fftwf_complex[]>(2*_client.buffer_size());
  for (std::size_t i = 0; i < 2*_client.buffer_size(); ++i) {
    Hw[i][0] = 1.0f; // Real part
    Hw[i][1] = 0.0f; // Imaginary part
  }
}

MainWindow::~MainWindow()
{
    _client.stop();
}
void MainWindow::on_pushButton2_clicked()
{
  std::cout << "You pushed button 2" << std::endl;
  //_client.set_filter_h(Hw.get(), 2*_client.buffer_size()-1, _client.buffer_size());
  _client.modo = proy2_client::peine;
   // Code to execute when the button is clicked
}
void MainWindow::on_peine_stateChanged(bool peine_is_on)
{
    if (peine_is_on) {
        _client.peine_is_on = true;
        //_client.modo = proy2_client::peine;
        std::cout << "Se activó filtro peine" << std::endl;
    }
    else {
        _client.peine_is_on = false;
        //MainWindow::on_pushButton3_clicked();
    }

}
/**
 * Add file to playlist
 */
bool MainWindow::add_file(const std::filesystem::path& file) {
  ui->fileEdit->setText(file.c_str());
  return _client.add_file(file);
}

void MainWindow::on_fileButton_clicked() {
  selectedFiles_ =
      QFileDialog::getOpenFileNames(this,
                                   "Select one or more audio files to open",
                                   ui->fileEdit->text(),
                                   "WAV Files (*.wav)");

  if (!selectedFiles_.empty()) {
    ui->fileEdit->setText(*selectedFiles_.begin());

    _client.stop_files();
    QStringList::iterator it;
    for (it=selectedFiles_.begin();it!=selectedFiles_.end();++it) {
      std::filesystem::path tmp(qPrintable(*it));
      _client.add_file(tmp.c_str());
    }
  }
}

void MainWindow::on_fileEdit_returnPressed() {
  _client.stop_files();
  std::filesystem::path tmp(qPrintable(ui->fileEdit->text()));
  if (!tmp.empty()) {
    _client.add_file(tmp.c_str());
  }
}

void MainWindow::on_stopButton_clicked() {
  _client.stop_files();
}

void MainWindow::on_volumeDial_sliderMoved(int value) {
  ui->volumeSpin->setValue(value);
 
  _client.set_volume(float(value)/100.0f);
}

void MainWindow::on_volumeSpin_valueChanged(int value) { 
  ui->volumeDial->setValue(value);  
  
  _client.set_volume(float(value)/100.0f);
}

void MainWindow::on_update_timer() {
  float p = std::sqrt(_client.power());
  ui->powerBar->setValue(static_cast<int>(p*100));
}

void MainWindow::on_pushButton3_clicked()
{
  std::cout << "Se cambió a filtro paso todo definido en la frecuencia" << std::endl;
  _client.set_filter_h(Hw.get(), 2*_client.buffer_size(), _client.buffer_size());
  _client.modo = proy2_client::over_save;
   // Code to execute when the button is clicked
}

void MainWindow::handleValueChanged() {
    QSlider *slider = qobject_cast<QSlider *>(sender());
    if (slider) {
        int value = slider->sliderPosition();
        QString name = slider->objectName();
        int index = name.mid(15).toInt();  // Get the index from the object name
        if (index >= 0 && index < 22) {
            //y[index] = value;
            _client.update_equalizer(index,value);
            qDebug() << "Slider" << index << "value changed to:" << value;
        }
    }
}

void MainWindow::on_equalizer_stateChanged(bool eq_is_on)
{
    if (eq_is_on) {
        _client.eq_is_on = true;
        _client.set_equalizer();
        _client.modo = proy2_client::over_save;
        std::cout << "Se cambió a ecualizador" << std::endl;
    }
    else {
        _client.eq_is_on = false;
        //MainWindow::on_pushButton3_clicked();
    }

}

void MainWindow::on_reverberador1_stateChanged()
{
  bool is_working = ui->reverberador1->isChecked();
  if (is_working) {
    _client.modo_reverb = proy2_client::simple_reverb;
    ui->reverberador2->setEnabled(false);
  }
  else {
    _client.modo_reverb = proy2_client::no_reverb;
    ui->reverberador2->setEnabled(true);
  }
}

void MainWindow::on_alphaValue_sliderMoved(int position)
{
  float new_alpha = float(position)/100.0f;
  _client.alpha = new_alpha;
  ui->alpha_print->setText(QString::number(new_alpha));
}

void MainWindow::on_tauValue_sliderMoved(int position)
{
  bool is_working1 = ui->reverberador1->isChecked();
  bool is_working2 = ui->reverberador2->isChecked();
  if (is_working1) {
    _client.modo_reverb = proy2_client::no_reverb;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    _client.yr.set_capacity(position + 2);
    _client.modo_reverb = proy2_client::simple_reverb;
    ui->tau_print->setText(QString::number(position));
  }
  else if (is_working2) {
    _client.modo_reverb = proy2_client::no_reverb;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    _client.yr.set_capacity(position + 2);
    _client.modo_reverb = proy2_client::filtered_reverb;
    ui->tau_print->setText(QString::number(position));
  }
  else {
    _client.yr.set_capacity(position + 2);
    ui->tau_print->setText(QString::number(position));
  }
}

void MainWindow::on_reverberador2_stateChanged(int arg1)
{
  bool is_working = ui->reverberador2->isChecked();
  if (is_working) {
    _client.modo_reverb = proy2_client::filtered_reverb;
    ui->reverberador1->setEnabled(false);
  }
  else {
    _client.modo_reverb = proy2_client::no_reverb;
    ui->reverberador1->setEnabled(true);
  }  
}

void MainWindow::on_poloValue_sliderMoved(int position)
{
  float new_polo = float(position)/100.0f;
  _client.polo = new_polo;
  ui->polo_print->setText(QString::number(new_polo));
}


void MainWindow::on_reverberador2_clicked()
{
  bool is_working = ui->reverberador1->isChecked();
  if (is_working) {
    ui->reverberador2->setChecked(false);
  }
}


void MainWindow::on_reverberador1_clicked()
{
  bool is_working = ui->reverberador2->isChecked();
  if (is_working) {
    ui->reverberador1->setChecked(false);
  }
}


project('proy2','cpp',
        default_options : ['cpp_std=c++20'],  
        version : '0.0.5')

qt6 = import('qt6')
qt6_dep = dependency('qt6', modules: ['Core', 'Gui', 'Widgets'])

# Add the following lines for each .h file that contains Q_OBJECT
moc_headers = ['mainwindow.h']

# Add the user interface compilable files
uic_sources = ['mainwindow.ui']

moc_files = qt6.compile_moc(headers: moc_headers,
                            dependencies: qt6_dep)
uic_files = qt6.compile_ui(sources: uic_sources)
                          
# Find jack, soundfile, boost and fftw
jack_dep = dependency('jack')
sndfile_dep = dependency('sndfile')
boost_dep = dependency('boost', modules : ['program_options'])
fftw_dep = dependency('fftw3f')
gsl_dep = dependency('gsl','gslblas','stdc++')

all_deps = [jack_dep,sndfile_dep,boost_dep,fftw_dep,qt6_dep,gsl_dep]

sources = files('main.cpp', 'jack_client.cpp','passthrough_client.cpp',
                'waitkey.cpp','sndfile_thread.cpp','freq_filter.cpp',
                'mainwindow.cpp', 'proy2_client.cpp')

# Add the Qt stuff to the sources
sources += moc_files + uic_files


executable('proy2',sources,dependencies:all_deps)

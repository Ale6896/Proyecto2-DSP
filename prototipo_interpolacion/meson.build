project('prueba_sciplot','cpp',
        default_options : ['cpp_std=c++20'],  
        version : '0.0.2')

# Find PipeWire dependency
gsl_dep = dependency('gsl', 'gslblas', 'm', 'stdc++')

all_deps = [gsl_dep]

incdir = include_directories('sciplot')

sources = files('interp.cpp')

executable('interp',sources, dependencies:all_deps, include_directories : incdir)

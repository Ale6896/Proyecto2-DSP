#include <iostream>
#include <stdio.h>
#include <cmath>
#include <gsl/gsl_spline.h>
#include <sciplot/sciplot.hpp>
using namespace sciplot;

/*
 * Instalacion GSL: sudo apt-get install libgsl-dev
 */

int main(int argc, char** argv)
{

    // Define your x and y values
    int n = 21;

    std::vector<double> x = {0,	6, 9, 12, 16, 22, 29, 40, 54, 73, 99, 134, 181, 245, 332, 450, 609, 824, 1116, 1511, 2046};
    
    std::vector<double> y = {0.2038, 0.48061, 0.78178, 0.83832, 0.3194, 0.94085, 0.78144, 0.44144, 0.46238, 0.66396, 0.87462, 0.80573, 0.14939, 0.78808, 0.023311, 0.14613, 0.86621, 0.90805, 0.955, 0.1872, 0.3594};

    // Initialize the interpolator
    gsl_interp_accel *acc = gsl_interp_accel_alloc();
    gsl_spline *spline = gsl_spline_alloc(gsl_interp_akima, n);

    // Fit the interpolator to the data
    gsl_spline_init(spline, x.data(), y.data(), n);

    // Define the points where you want to interpolate
    std::vector<double> x_interp(2047);
    for (int i = 0; i <= 2046; i++) {
        x_interp[i] = i;
    }
    
    std::vector<double> y_interp;

    // Interpolate at each z and print the interpolated value
    for (double xi : x_interp) {
        double yi = gsl_spline_eval(spline, xi, acc);
     
        y_interp.push_back(yi);
    }

    // Free the interpolator
    gsl_spline_free(spline);
    gsl_interp_accel_free(acc);

    // Create a Plot object
    Plot2D plot;

    plot.xtics().logscale(10);

    plot.drawCurve(x_interp, y_interp).label("");
    

    // Create figure to hold plot
    Figure fig = {{plot}};

    // Create canvas to hold figure
    Canvas canvas = {{fig}};
    // Set canvas output size
    canvas.size(900,600);

    // Show the plot in a pop-up window
    canvas.show();
}

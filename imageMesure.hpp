#ifndef imageMesure_h
#define imageMesure_h

#include "ImageRGB.hpp"
#include "ImageLab.hpp"
#include <vector>


void averageColor(const kn::ImageRGB8u& image, double *average);

void contrastColor(const kn::ImageRGB8u& image, double &contrast, double & variance);
void contrastColor(const ImageLab& image, double &contrast);

void saturation(const kn::ImageRGB8u& image, double &saturation);

void histogramColor(const kn::ImageRGB8u& image, double *histo, const unsigned int nbBins);

//void skewKurt(const kn::ImageRGB8u& image, const double *average,double variance, double & skewness, double & kurtosis);

#endif



#ifndef __IMAGE_LAB_HPP__
#define __IMAGE_LAB_HPP__

#include "MatricesAndVectorsDefinitions.hpp"
#include "Image.hpp"

typedef kn::Image<double> ImageLab;

inline double labFunc(double t);

inline double labtorgb_fy(const Vector3d& lab);
inline double labtorgb_fx(const Vector3d& lab);
inline double labtorgb_fz(const Vector3d& lab);

void RGBtoLAB(const unsigned char rgb[], double lab[]);
void RGBtoLABimage(const kn::Image<unsigned char> & src, ImageLab & dest);
void LABtoXYZ(const Vector3d & src, Vector3d & dest);

void LABtoRGBimage(const ImageLab & src, kn::Image<unsigned char> & dest);

void LABtoRGB(const double lab[], unsigned char rgb[]);

void averageLuminance(const ImageLab & image, double & avg);

void averageTeinte(const ImageLab & image, double & a, double & b);

void varianceLuminance(const ImageLab& image, const double lumAvg, double & variance);

#endif //_IMAGE_LAB_HPP_

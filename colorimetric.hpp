#ifndef colorimetric_h
#define colorimetric_h

#include <surflib.h>

#include "LUT3D.hpp"
#include "DefaultLUT3D.hpp"
#include "ChebyshevPolynomialLUT3D.hpp"
#include "ChebyshevPolynomialLAB.hpp"

#include "ImageLab.hpp"

#include "MatricesAndVectorsDefinitions.hpp"

#include "ImageRGB.hpp"
#include "Image3D.hpp"


// compute a 3D lut to make image2 match image1
//void colorEqualization(const kn::ImageRGB8u &image1,
//                       const kn::ImageRGB8u &image2,
//                       DefaultLUT3D &lut3d);
//
//void colorEqualization(const kn::ImageRGB8u &image1,
//                       const kn::ImageRGB8u &image2,
//                       ChebyLUT &lut3d);
//
void colorEqualization(const kn::ImageRGB8u &image1,
                       const kn::ImageRGB8u &image2,
                       LUT3D &lut3d);

void colorEqualization(const kn::ImageRGB8u &image1,
                       const kn::ImageRGB8u &image2,
                       ChebyLUT_LAB &lut3d);

// the error function given to levenbergMarquardt
//VectorXd error(const VectorXd &a, const VectorXd &b);
VectorXd error(const VectorXd &a, const VectorXd &b, LUT3D& lut);
VectorXd errora(const VectorXd &a, const VectorXd &b, ChebyLUT_LAB& lut);

// build variable vector
VectorXd build_a(const unsigned int lutNbBins, LUT3D& lut);
//VectorXd build_a(const unsigned int lutNbBins, ChebyLUT& lut);

// build constant vector
VectorXd build_b(const kn::ImageRGB8u &image1,
                           const kn::ImageRGB8u &image2,
                           const unsigned int lutNbBins,
                           unsigned int div);
VectorXd build_b(const kn::ImageRGB8u &image1,
                           const kn::ImageRGB8u &image2,
                           const unsigned int lutNbBins,
                           const IpPairVec& matches,
                           unsigned int div);

VectorXd build_b(const ImageLab &image1,
                           const ImageLab &image2,
                           const unsigned int lutNbBins);

// extract data from vector a
void read_a(const VectorXd &a,
            kn::Image3D<double> &lut3d);

void read_a(const VectorXd &a,
            LUT3D& lut);

// extract data from vector b
void read_b(const VectorXd &b,
            kn::ImageRGB8u &image1,
            kn::ImageRGB8u &image2,
            unsigned int &lutNbBins);

void read_b(const VectorXd &b,
            kn::ImageRGB8u &image1,
            kn::ImageRGB8u &image2,
            IpPairVec& matches,
            unsigned int &lutNbBins);

void read_b(const VectorXd &b,
            ImageLab &image1,
            ImageLab &image2,
            unsigned int &lutNbBins);

// extract data from vector b
void read_b_wh(const VectorXd &b,
              unsigned int &image1Width,
              unsigned int &image1Height,
              unsigned int &image2Width,
              unsigned int &image2Height);


#endif


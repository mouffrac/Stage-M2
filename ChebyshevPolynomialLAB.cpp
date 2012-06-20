#include "ChebyshevPolynomialLAB.hpp"

#include "ImageLab.hpp"
#include <typeinfo>
#include <unsupported/Eigen/Polynomials>

void ChebyLUT_LAB::exportLUT3D(kn::Image3D<unsigned char>& lut, unsigned size) const
{

    double norm(1./(size-1));
    double tmp[3] = {0};

    /* for all voxels in LUT, evaluate their values */
    for(unsigned z(0); z< size; ++z)
    for(unsigned y(0); y< size; ++y)
    for(unsigned x(0); x< size; ++x)
    {
        double lab[3];
        unsigned char rgb[3];
        unsigned char resultRGB[3];


        rgb[0] = static_cast<unsigned char>(x*norm*255);
        rgb[1] = static_cast<unsigned char>(y*norm*255);
        rgb[2] = static_cast<unsigned char>(z*norm*255);

        RGBtoLAB(rgb, lab);

//        std::cout << "(" << (int)rgb[0] << " " << (int)rgb[1] << " " << (int)rgb[2] << ") --> ";
//        unsigned char testRGB[3];
//        LABtoRGB(lab, testRGB);
//        std::cout << "(" << (int)testRGB[0] << " " << (int)testRGB[1] << " " << (int)testRGB[2] << ")" << std::endl;

        eval(lab[0]/100., (lab[1]+128)/255., (lab[2]+128)/255., tmp);

        tmp[0] *= 100.;
        tmp[1] *=255.;
        tmp[1] -= 128;

        tmp[2] *=255.;
        tmp[2] -=128;


        LABtoRGB(tmp, resultRGB);
        std::cout << (int)rgb[0] << " " << (int)rgb[1] << " " << (int)rgb[2] << " --> ";
        std::cout << (int)resultRGB[0] << " " << (int)resultRGB[1] << " " << (int)resultRGB[2] << std::endl;
//        unsigned char bonjour = std::max(0, std::min(255, (int) resultRGB[0]*255));
        lut(x,y,z)[0] = std::max(0, std::min(255, (int) resultRGB[0]));
        lut(x,y,z)[1] = std::max(0, std::min(255, (int) resultRGB[1]));
        lut(x,y,z)[2] = std::max(0, std::min(255, (int) resultRGB[2]));
    }
}

void ChebyLUT_LAB::exportLUT3D(kn::Image3D<double>& lut, unsigned size) const
{

    double norm(1./(size-1));
    double tmp[3] = {0};

    /* for all voxels in LUT, evaluate their values */
    for(unsigned z(0); z< size; ++z)
    for(unsigned y(0); y< size; ++y)
    for(unsigned x(0); x< size; ++x)
    {
        double lab[3];
        unsigned char rgb[3];
        unsigned char resultRGB[3];

        rgb[0] = static_cast<unsigned char>(x*norm*255.);
        rgb[1] = static_cast<unsigned char>(y*norm*255.);
        rgb[2] = static_cast<unsigned char>(z*norm*255.);

        RGBtoLAB(rgb, lab);
        eval(lab[0]/100., (lab[1]+128)/255., (lab[2]+128)/255., tmp);
        LABtoRGB(tmp, resultRGB);



        lut(x,y,z)[0] = std::max(0., std::min(1.,  static_cast<double>(resultRGB[0])/255.));
        lut(x,y,z)[1] = std::max(0., std::min(1.,  static_cast<double>(resultRGB[1])/255.));
        lut(x,y,z)[2] = std::max(0., std::min(1.,  static_cast<double>(resultRGB[2])/255.));
    }


}

void ChebyLUT_LAB::exportLUT3DLAB(kn::Image3D<double>& lut, unsigned size) const
{
    double norm(1./(size-1));

    for(unsigned z(0); z< size; ++z)
    for(unsigned y(0); y< size; ++y)
    for(unsigned x(0); x< size; ++x)
    {
        eval(x*norm, y*norm, z*norm, lut(x,y,z));

        lut(x,y,z)[0] = std::max(0., std::min(1., lut(x,y,z)[0]));
        lut(x,y,z)[1] = std::max(0., std::min(1., lut(x,y,z)[1]));
        lut(x,y,z)[2] = std::max(0., std::min(1., lut(x,y,z)[2]));
    }
}


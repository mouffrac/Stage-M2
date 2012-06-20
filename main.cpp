#include <iostream>
#include <cassert>
#include <sys/times.h>
#include <cstdlib>
#include <algorithm>


#include "ImageRGB.hpp"
#include "ioPPM.hpp"
#include "ioJPG.hpp"

#include "DefaultLUT3D.hpp"
#include "colorimetric.hpp"
#include "imageMesure.hpp"
#include "HistoEqualization.hpp"

#include "ChebyshevPolynomialLUT3D.hpp"
#include "ChebyshevPolynomialLAB.hpp"



const unsigned int EXPORT_SIZE  (32);

struct Test
{
    unsigned char data[3];

    Test(unsigned char r, unsigned char g, unsigned char b)
    {
        data[0] = r;
        data[1] = g;
        data[2] = b;
    }

    unsigned char operator[](unsigned  i)
    {
        return data[i];
    }

    bool operator==(const Test& t)
    {
        return data[0] == t.data[0] && data[1] == t.data[1] && data[2] == t.data[2];
    }
};

bool compareColors(Test col1, Test col2)
{
    for(int i = 0; i < 3; ++i) {
        if(col1[i] < col2[i]) {
            return true;
        } else if(col1[i] > col2[i]) {
            return false;
        }
    }
    return false;
}



void sortImage(const kn::ImageRGB8u & image, kn::ImageRGB8u& result)
{
    std::vector<Test> im;
    std::vector<Test>::iterator it;

    for(unsigned i(0); i<image.size(); i+=3)
    {
//        std::cout << i << " --> " << image.size() << std::endl;
        im.push_back(Test(image.begin()[i],image.begin()[i+1],image.begin()[i+2]));
    }

    std::sort(im.begin(), im.end(), compareColors);

    std::vector<Test> imu;

    {
        unsigned i(0);
        while(i<im.size())
        {
            Test tmp = im[i];
            imu.push_back(im[i]);
            while(i<im.size() && tmp == im[i]) ++i;

        }
    }
    unsigned int w((int) std::sqrt(imu.size()));
    result = kn::ImageRGB8u(w,w);
    for(unsigned i(0); i<w*w; ++i)
    {
        result.begin()[3*i] = imu[i][0];
        result.begin()[3*i+1] = imu[i][1];
        result.begin()[3*i+2] = imu[i][2];
    }

}

int main()
{
    // input data
    kn::ImageRGB8u image1;
    kn::ImageRGB8u image2;

    // load images
    kn::loadJPG(image1, "input/double/image0.jpg");
    kn::loadJPG(image2, "input/double/image1.jpg");
//    kn::loadJPG(image1, "input/MonsieurPatate/MonsieurPatate_00.jpg");
//    kn::loadJPG(image2, "input/MonsieurPatate/MonsieurPatate_04.jpg");

    //getApproxImage(image1, image2);

    ImageLab test(image1.width(), image1.height(), 3);

    RGBtoLABimage(image1, test);

    kn::ImageRGB8u image3(image1.width(), image1.height());

    LABtoRGBimage(test, image3);



    // difference between original image and RGBtoLAB -> LABtoRGB image
//    for(unsigned j(0); j<image1.height(); ++j)
//    for(unsigned i(0); i<image1.width(); ++i)
//    for(unsigned c(0); c<3; ++c)
//        image3(i,j)[c] = fabs(image3(i,j)[c] - image1(i,j)[c]);

    kn::savePPM(image3, "output/shouldbeblack.ppm");

    //exit(0);
#if 0
    double average[3];
    averageColor(image1,average);
    std::cout << "average color : " << average[0] << " " << average[1] << " " << average[2] << std::endl;

    double contrast = 0.0;
    contrastColor(image1, contrast);
    std::cout << "contrast " << contrast << std::endl;

    contrastColor(image2, contrast);
    std::cout << "contrast " << contrast << std::endl;

    double saturationVal = 0.0;
    saturation(image1,saturationVal);
    std::cout << "saturation " << saturationVal << std::endl;

    double histo[20*3];
    histogramColor(image1, histo, 20);
    std::cout << "histogramColor " << std::endl;
    for(unsigned int i=0; i<20; i++)
        std::cout << histo[i] << " ";
    std::cout << std::endl;
#endif

    // non linear process
    ChebyLUT lut(5);

//    kn::ImageRGB8u Identity(32,32*32);
//
//    for(unsigned k(0) ; k < 32; ++k)
//    for(unsigned j(0) ; j < 32; ++j)
//    for(unsigned i(0) ; i < 32; ++i)
//    {
//        Identity(i, j + k*32)[0] = (unsigned char)i*255/31;
//        Identity(i, j + k*32)[1] = (unsigned char)j*255/31;
//        Identity(i, j + k*32)[2] = (unsigned char)k*255/31;
//    }
//
//    ImageLab IdentityLAB(32,32*32, 3);
//    RGBtoLABimage(Identity, IdentityLAB);
//    LABtoRGBimage(IdentityLAB,Identity);
//
//    kn::savePPM(Identity, "output/lutId.ppm");
//    exit(0);


//    kn::ImageRGB8u im1r, im2r;
//    sortImage(image1imp, im1r);
//    kn::saveJPG(im1r, "output/sortedImage1.jpg");
//
//    sortImage(image2imp, im2r);
//    kn::saveJPG(im2r, "output/sortedImage2.jpg");

    std::cout << "non linear process ..." << std::endl;
    clock_t now = times(0);
    colorEqualization(image1, image2, lut);
    std::cout << "colorEqualization : elapsed time = " << (times(0) - now)*0.01/60.0  << " min" << std::endl;

    // colorimetric correction
    std::cout << "colorimetric correction ..." << std::endl;
    kn::ImageRGB8u image2Corrected(image2);
    kn::Image3D<unsigned char> lut3d(EXPORT_SIZE,EXPORT_SIZE,EXPORT_SIZE,3);
    lut.exportLUT3D(lut3d, EXPORT_SIZE);

    for(unsigned int i=0; i<image2Corrected.width(); i++)
        for(unsigned int j=0; j<image2Corrected.height(); j++)
        {
            double red = static_cast<double>(image2(i,j)[0]) * (EXPORT_SIZE-1.0)/255.0;
            double green = static_cast<double>(image2(i,j)[1]) * (EXPORT_SIZE-1.0)/255.0;
            double blue = static_cast<double>(image2(i,j)[2]) * (EXPORT_SIZE-1.0)/255.0;

            unsigned char color[3];
            lut3d.atBilinear(red,green,blue,color);

            for(unsigned int c=0; c<3; c++)
                image2Corrected(i,j)[c] = color[c];
        }

    // save results
    kn::ImageRGB8u lut3dImage(EXPORT_SIZE,EXPORT_SIZE*EXPORT_SIZE,lut3d.begin());
    kn::savePPM(lut3dImage,"output/lut3d.ppm"); // très volumineux
    kn::saveJPG(image2Corrected,"output/MonsieurPatate_00_.jpg");
    kn::saveJPG(image2,"output/input.jpg");
    kn::saveJPG(image1,"output/target.jpg");

//    for(unsigned i(0); i<lut.getDataSize(); ++i)
//    std::cout << lut(i) << std::endl;

    std::cout << "all : elapsed time = " << (times(0) - now)*0.01/(60.0)  << " minutes" << std::endl;

    return 0;
}

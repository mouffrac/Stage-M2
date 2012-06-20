#include "colorimetric.hpp"
#include "LevenbergMarquardt.hpp"
#include "imageMesure.hpp"

#include <cmath>
#include <cassert>
#include <typeinfo>
#include <cstdlib>
#include <algorithm>

/* Correspondance points */
#include <surflib.h>
#include <sstream>
#include <string>

#include <OpenKN/math/Vector.hpp>

#include "DefaultLUT3D.hpp"
#include "ChebyshevPolynomialLUT3D.hpp"

#include "MatricesAndVectorsDefinitions.hpp"
#include "RANSAC_epipolar.hpp"


#include "ImageRGB.hpp"
#include "ioJPG.hpp"

#include "ChebyshevPolynomialLAB.hpp"
#include "ioPPM.hpp"

const unsigned int nbBins  = 0; // histogram

template<typename T>
inline T clamp(const T& l_a, const T& l_min, const T& l_max)
{
    return std::max(std::min(l_a, l_max), l_min);
}

////////////////////////////////////////////////////////////////////////////////////
void errorStat(const VectorXd &vecError)
{
  double max = 0.0;
  double average = 0.0;
  double mean = 0.0;

  for(unsigned int i=0; i< vecError.size(); ++i)
    {
      if(fabs(vecError[i]) > max) max = fabs(vecError[i]);

      average += fabs(vecError[i]);
    }

  average = average / vecError.size();
  for(unsigned int i=0; i< vecError.size(); ++i)
    mean +=  fabs(fabs(vecError[i])-average);

  mean = mean / vecError.size();

  std::cout << "max     : " << max << std::endl;
  std::cout << "average : " << average << std::endl;
  std::cout << "mean    : " << mean << std::endl;
  std::cout << std::endl;
}


bool isEqual(const kn::Vector3d& c1, const kn::Vector3d& c2, const std::pair<Ipoint, Ipoint>& p)
{
    return (c1[0] == p.first.x) && (c1[1] == p.first.y) && (c2[0] == p.second.x) && (c2[1] == p.second.y);
}
void colorEqualization(const kn::ImageRGB8u &image1,
                       const kn::ImageRGB8u &image2,
                       LUT3D &lut)
{

    unsigned int lutCoeff = lut.getSize();
    unsigned int lutBins = lut.getNbBins();

  // levenberg marquardt data: variable
  VectorXd a = build_a(lutCoeff, lut); // 3D lut
  std::cout << "a = " << a << std::endl;



  // levenberg marquardt data: constant
//    unsigned int w1,w2,h1,h2,n;
//    read_b_wh(b, w1,h1,w2,h2);
//    kn::ImageRGB8u im1(w1,h1);
//    kn::ImageRGB8u im2(w2,h2);
//
//
//    read_b(b, im1, im2, n);
//    kn::saveJPG(im1, "output/lol1.jpg");
//    kn::saveJPG(im2, "output/lol2.jpg");
//
//    std::cout << im1.width() << " " << im1.height() << std::endl;
//
//    exit(0);


    IpVec ipts1, ipts2;
    //surfDetDes(image1,ipts1,false,4,4,2,0.0001f);
    //surfDetDes(image2,ipts2,false,4,4,2,0.0001f);
    surfDetDes(image1,ipts1,false,4,4,10,1.0e-20f);
    surfDetDes(image2,ipts2,false,4,4,10,1.0e-20f);

    // make pair
    IpPairVec matches;
    getMatches(ipts1,ipts2,matches);

    std::vector<kn::Vector3d> correspondences1, correspondences2;

    for(unsigned int i=0; i<matches.size(); i++)
    {
       kn::Vector3d first;
       first[0] = matches[i].first.x;
       first[1] = matches[i].first.y;
       first[2] = 1.0;

       kn::Vector3d second;
       second[0] = matches[i].second.x;
       second[1] = matches[i].second.y;
       second[2] = 1.0;

       correspondences1.push_back(first);
       correspondences2.push_back(second);
    }

    ransacEpipolar(correspondences1, correspondences2, 5, 200);

    IpPairVec realmatches;
    for(unsigned i(0); i<correspondences1.size(); ++i)
    {
        bool notFound(true);
        int j(0);
        while(notFound)
        {
            if(isEqual(correspondences1[i], correspondences2[i], matches[j]) )
            {
                notFound = false;
                realmatches.push_back(matches[j]);
            }
            ++j;
        }
    }
  // levenberg marquardt data: error
  VectorXd X(nbBins*3+5*realmatches.size());
  X.fill(0.0);  // expected error = 0
  // levenberg marquardt process

  VectorXd b = build_b(image1, image2, lutBins, realmatches,1); // image1, image2

  error(a,b,lut);
  errorStat(X-error(a,b,lut));
//  levenbergMarquardt(a, b, X, error, lut, 5); // iterations
//  b = build_b(image1, image2, lutBins, 1);
  levenbergMarquardt(a, b, X, error, lut, 50); // iterations
  errorStat(X-error(a,b,lut));
 // std::cout << "a = " << a << std::endl;

  read_a(a,lut);

  // profiler end
}



void colorEqualization(const kn::ImageRGB8u &image1,
                       const kn::ImageRGB8u &image2,
                       ChebyLUT_LAB &lut)
{
    std::cout << "EQUALIZATION USING L*a*b* COLOR SPACE" << std::endl;
    unsigned int lutCoeff = lut.getSize();
    unsigned int lutBins = lut.getNbBins();

    ImageLab image1lab(image1.width(), image1.height(), 3);
    ImageLab image2lab(image2.width(), image2.height(), 3);

    RGBtoLABimage(image1, image1lab);
    RGBtoLABimage(image2, image2lab);


  // levenberg marquardt data: variable
  VectorXd a = build_a(lutCoeff, lut); // 3D lut
  std::cout << "a = " << a << std::endl;

  // levenberg marquardt data: constant
  VectorXd b = build_b(image1lab, image2lab, lutBins); // image1, image2

  // levenberg marquardt data: error
  VectorXd X(4);
  X.fill(0.0);  // expected error = 0
  // levenberg marquardt process
  errorStat(X-errora(a,b,lut));
  levenbergMarquardt(a, b, X, errora, lut, 50); // iterations
  errorStat(X-errora(a,b,lut));
 // std::cout << "a = " << a << std::endl;

  read_a(a,lut);

  // profiler end
}



//VectorXd error(const VectorXd &a, const VectorXd &b, LUT3D& lut)
//{
//
//    unsigned int w1,h1,w2,h2,lutNbBins;
//    unsigned char maxColor(255), minColor(0);
//    read_b_wh(b,w1,h1,w2,h2);
//
//    kn::ImageRGB8u image1(w1,h1);
//    kn::ImageRGB8u image2(w2,h2);
//    read_b(b,image1,image2,lutNbBins);
//
//    kn::Image3D<unsigned char> lut3d(32,32,32,3);
//
//    read_a(a,lut);
//
//    lut.exportLUT3D(lut3d, 32);
//
//
//
////    kn::ImageRGB8u lut3dImage(32,32*32,lut3d.begin());
////    kn::savePPM(lut3dImage,"output/lut3d.ppm"); // très volumineux
//
//    // apply the lut on image2
//    for(unsigned int i=0; i<image2.width(); i++)
//        for(unsigned int j=0; j<image2.height(); j++)
//        {
//            double red = static_cast<double>(image2(i,j)[0]) * (32-1.0)/255.0;
//            double green = static_cast<double>(image2(i,j)[1]) * (32-1.0)/255.0;
//            double blue = static_cast<double>(image2(i,j)[2]) * (32-1.0)/255.0;
//
//            unsigned char color[3];
//            lut3d.atBilinear(red,green,blue,color);
//
//            for(unsigned int c=0; c<3; c++)
//                image2(i,j)[c] = color[c];
//        }
//
//       kn::savePPM(image2,"output/prout.ppm");
//
//    // error
//    VectorXd descriptorImage1(nbBins*3+5);
//    VectorXd descriptorImage2(nbBins*3+5);
//
//    // image1
//    unsigned int index =0;
//    double tab[nbBins*3];
//    double var, sk, kurt;
//    averageColor(image1, &descriptorImage1[index]); index += 3;
//    contrastColor(image1, descriptorImage1[index++], var);
//    saturation(image1, descriptorImage1[index++]);
////    skewKurt(image1, &descriptorImage1[0], var, sk, kurt);
//    //histogramColor(image1, descriptorImage1.begin()+index, nbBins);
//    /*
//    histogramColor(image1, tab, nbBins);
//    for(unsigned int i=0; i<nbBins*3; i++)
//        descriptorImage1[index++] = tab[i];
//    */
//
//    // image2
//    index =0;
//    averageColor(image2, &descriptorImage2[index]);  index += 3;
//    contrastColor(image2, descriptorImage2[index++], var);
//    saturation(image2, descriptorImage2[index++]);
//    //histogramColor(image2, descriptorImage2.begin()+index, nbBins);
///*
//    histogramColor(image2, tab, nbBins);
//    for(unsigned int i=0; i<nbBins*3; i++)
//        descriptorImage2[index++] = tab[i];
//*/
//    // print
//    //std::cout << "image1 : \n" << descriptorImage1 << std::endl;
//    //std::cout << "image2 : \n" << descriptorImage2 << std::endl;
//    //std::cout << std::endl << std::endl;
//
//    // error
//    VectorXd error(descriptorImage1-descriptorImage2);
//    //std::cout << error << std::endl;
//
//    return error;
//}

VectorXd error(const VectorXd &a, const VectorXd &b, LUT3D& lut)
{

    unsigned int w1,h1,w2,h2,lutNbBins;
    unsigned char maxColor(255), minColor(0);
    read_b_wh(b,w1,h1,w2,h2);

    kn::ImageRGB8u image1(w1,h1);
    kn::ImageRGB8u image2(w2,h2);
    IpPairVec matches;
    read_b(b,image1,image2,matches,lutNbBins);

    kn::Image3D<unsigned char> lut3d(32,32,32,3);

    read_a(a,lut);

    lut.exportLUT3D(lut3d, 32);



//    kn::ImageRGB8u lut3dImage(32,32*32,lut3d.begin());
//    kn::savePPM(lut3dImage,"output/lut3d.ppm"); // très volumineux

    // apply the lut on image2
    for(unsigned int i=0; i<image2.width(); i++)
        for(unsigned int j=0; j<image2.height(); j++)
        {
            double red = static_cast<double>(image2(i,j)[0]) * (32-1.0)/255.0;
            double green = static_cast<double>(image2(i,j)[1]) * (32-1.0)/255.0;
            double blue = static_cast<double>(image2(i,j)[2]) * (32-1.0)/255.0;

            unsigned char color[3];
            lut3d.atBilinear(red,green,blue,color);

            for(unsigned int c=0; c<3; c++)
                image2(i,j)[c] = color[c];
        }

       kn::savePPM(image2,"output/prout.ppm");

    // error
    VectorXd descriptorImage1(nbBins*3+5*matches.size());
    VectorXd descriptorImage2(nbBins*3+5*matches.size());

    // image1
    unsigned int index =0;
    double tab[nbBins*3];
    double var, sk, kurt;


    for(unsigned i(0); i<matches.size(); ++i)
    {

        unsigned sizeP1 = ((int) matches[i].first.scale)*2 +1;
        kn::ImageRGB8u patche1(sizeP1, sizeP1);

        unsigned sizeP2 = ((int) matches[i].second.scale)*2 +1;
        kn::ImageRGB8u patche2(sizeP2, sizeP2);

//        for(unsigned y(0); y<sizeP1; ++y)
//        {
//            for(unsigned x(0); x<sizeP1; ++x)
//            {
//
//                patche1(x,y)[0] = image1(((unsigned)matches[i].first.x)+(x-sizeP1/2), ((unsigned)matches[i].first.y)+(y-sizeP1/2))[0];
//                patche1(x,y)[1] = image1(((unsigned)matches[i].first.x)+(x-sizeP1/2), ((unsigned)matches[i].first.y)+(y-sizeP1/2))[1];
//                patche1(x,y)[2] = image1(((unsigned)matches[i].first.x)+(x-sizeP1/2), ((unsigned)matches[i].first.y)+(y-sizeP1/2))[2];
//            }
//        }



        image1.getSubImage(((unsigned)matches[i].first.x) -sizeP1/2, ((unsigned)matches[i].first.y) -sizeP1/2 , patche1);
        image2.getSubImage(((unsigned)matches[i].second.x) -sizeP2/2, ((unsigned)matches[i].second.y) -sizeP2/2 , patche2);

//        std::stringstream name1;
//        std::stringstream name2;
//        name1 << "output/patches/patch_" << (i+1) << "_1.jpg";
//        name2 << "output/patches/patch_" << (i+1) << "_2.jpg";
//        kn::saveJPG(patche1, name1.str());
//        kn::saveJPG(patche2, name2.str());

//        for(unsigned y(0); y<sizeP2; ++y)
//        {
//            for(unsigned x(0); x<sizeP2; ++x)
//            {
//                patche2(x,y)[0] = image2(((unsigned)matches[i].second.x)+(x-sizeP2/2), ((unsigned)matches[i].second.y)+(y-sizeP2/2))[0];
//                patche2(x,y)[1] = image2(((unsigned)matches[i].second.x)+(x-sizeP2/2), ((unsigned)matches[i].second.y)+(y-sizeP2/2))[1];
//                patche2(x,y)[2] = image2(((unsigned)matches[i].second.x)+(x-sizeP2/2), ((unsigned)matches[i].second.y)+(y-sizeP2/2))[2];
//            }
//        }

        averageColor(patche1, &descriptorImage1[index]);
        averageColor(patche2, &descriptorImage2[index]);
        index += 3;

        contrastColor(patche1, descriptorImage1[index], var);
        contrastColor(patche2, descriptorImage2[index], var);
        ++index;

        saturation(patche1, descriptorImage1[index]);
        saturation(patche2, descriptorImage2[index]);
        ++index;


//        for(unsigned y(0); y<sizeP1; ++y)
//        {
//            for(unsigned x(0); x<sizeP1; ++x)
//            {
//                double r,g,b;
//                r = image1(((unsigned)matches[i].first.x)+(x-sizeP1/2), ((unsigned)matches[i].first.y)+(y-sizeP1/2))[0];
//                g = image1(((unsigned)matches[i].first.x)+(x-sizeP1/2), ((unsigned)matches[i].first.y)+(y-sizeP1/2))[1];
//                b = image1(((unsigned)matches[i].first.x)+(x-sizeP1/2), ((unsigned)matches[i].first.y)+(y-sizeP1/2))[2];
//
////                r = std::max(0., r-50);
//                g = std::max(0., g-50);
//                b = std::max(0., b-50);
//
//                image1(((unsigned)matches[i].first.x)+(x-sizeP1/2), ((unsigned)matches[i].first.y)+(y-sizeP1/2))[0] = r;
//                image1(((unsigned)matches[i].first.x)+(x-sizeP1/2), ((unsigned)matches[i].first.y)+(y-sizeP1/2))[1] = g;
//                image1(((unsigned)matches[i].first.x)+(x-sizeP1/2), ((unsigned)matches[i].first.y)+(y-sizeP1/2))[2] = b;
//            }
//        }
//
//        for(unsigned y(0); y<sizeP2; ++y)
//        {
//            for(unsigned x(0); x<sizeP2; ++x)
//            {
//                double r,g,b;
//                r = image2(((unsigned)matches[i].second.x)+(x-sizeP2/2), ((unsigned)matches[i].second.y)+(y-sizeP2/2))[0];
//                g = image2(((unsigned)matches[i].second.x)+(x-sizeP2/2), ((unsigned)matches[i].second.y)+(y-sizeP2/2))[1];
//                b = image2(((unsigned)matches[i].second.x)+(x-sizeP2/2), ((unsigned)matches[i].second.y)+(y-sizeP2/2))[2];
//
////                r = std::max(0., r-50);
//                g = std::max(0., g-50);
//                b = std::max(0., b-50);
//
//                image2(((unsigned)matches[i].second.x)+(x-sizeP2/2), ((unsigned)matches[i].second.y)+(y-sizeP2/2))[0] = r;
//                image2(((unsigned)matches[i].second.x)+(x-sizeP2/2), ((unsigned)matches[i].second.y)+(y-sizeP2/2))[1] = g;
//                image2(((unsigned)matches[i].second.x)+(x-sizeP2/2), ((unsigned)matches[i].second.y)+(y-sizeP2/2))[2] = b;
//            }
//        }

//        for(unsigned y(0); y<sizeP2; ++y)
//        {
//            for(unsigned x(0); x<sizeP2; ++x)
//            {
//                patche2(x,y)[0] = image2(((unsigned)matches[i].second.x)+(x-sizeP2/2), ((unsigned)matches[i].second.y)+(y-sizeP2/2))[0];
//                patche2(x,y)[1] = image2(((unsigned)matches[i].second.x)+(x-sizeP2/2), ((unsigned)matches[i].second.y)+(y-sizeP2/2))[1];
//                patche2(x,y)[2] = image2(((unsigned)matches[i].second.x)+(x-sizeP2/2), ((unsigned)matches[i].second.y)+(y-sizeP2/2))[2];
//            }
//        }

    //    skewKurt(image1, &descriptorImage1[0], var, sk, kurt);
        //histogramColor(image1, descriptorImage1.begin()+index, nbBins);
        /*
        histogramColor(image1, tab, nbBins);
        for(unsigned int i=0; i<nbBins*3; i++)
            descriptorImage1[index++] = tab[i];
        */

        // image2


    }

//    kn::saveJPG(image1, "output/image1_patches.jpg");
//    kn::saveJPG(image2, "output/image2_patches.jpg");
//    exit(0);


    //histogramColor(image2, descriptorImage2.begin()+index, nbBins);
/*
    histogramColor(image2, tab, nbBins);
    for(unsigned int i=0; i<nbBins*3; i++)
        descriptorImage2[index++] = tab[i];
*/
    // print
    //std::cout << "image1 : \n" << descriptorImage1 << std::endl;
    //std::cout << "image2 : \n" << descriptorImage2 << std::endl;
    //std::cout << std::endl << std::endl;

    // error
    VectorXd error(descriptorImage1-descriptorImage2);
    //std::cout << error << std::endl;

    return error;
}

VectorXd errora(const VectorXd &a, const VectorXd &b, ChebyLUT_LAB& lut)
{

    unsigned int w1,h1,w2,h2,lutNbBins;
    unsigned char maxColor(255), minColor(0);
    read_b_wh(b,w1,h1,w2,h2);

    ImageLab image1(w1,h1,3);
    ImageLab image2(w2,h2,3);
    read_b(b,image1,image2,lutNbBins);

    kn::Image3D<double> lut3d(32,32,32,3);

    read_a(a,lut);

    lut.exportLUT3DLAB(lut3d, 32);



//    kn::ImageRGB8u lut3dImage(32,32*32,lut3d.begin());
//    kn::savePPM(lut3dImage,"output/lut3d.ppm"); // très volumineux

    // apply the lut on image2
    for(unsigned int i=0; i<image2.width(); i++)
        for(unsigned int j=0; j<image2.height(); j++)
        {
            double _lum = image2(i,j)[0]/100.*31;
            double _a = (image2(i,j)[1]+128)/255.*31;
            double _b = (image2(i,j)[2]+128)/255.*31;

            double color[3];
            lut3d.atBilinear(_lum,_a,_b,color);

            image2(i,j)[0] = color[0]*100.;
            image2(i,j)[1] = color[1]*255.-128;
            image2(i,j)[2] = color[2]*255.-128;
        }

//    kn::savePPM(image2,"output/prout.ppm");

    // error
    VectorXd descriptorImage1(4);
    VectorXd descriptorImage2(4);

    // image1
    unsigned int index =0;
    double tab[nbBins*3];
    double var, sk, kurt;
    averageLuminance(image1, descriptorImage1(0));
    averageTeinte(image1, descriptorImage1(1), descriptorImage1(2));
    varianceLuminance(image1, descriptorImage1(0), descriptorImage1(3));
    //contrastColor(image1, descriptorImage1(3));

//    averageColor(image1, &descriptorImage1[index]); index += 3;
//    contrastColor(image1, descriptorImage1[index++], var);
//    saturation(image1, descriptorImage1[index++]);
//    skewKurt(image1, &descriptorImage1[0], var, sk, kurt);
    //histogramColor(image1, descriptorImage1.begin()+index, nbBins);
    /*
    histogramColor(image1, tab, nbBins);
    for(unsigned int i=0; i<nbBins*3; i++)
        descriptorImage1[index++] = tab[i];
    */

    // image2
    index =0;
    averageLuminance(image2, descriptorImage2(0));
    averageTeinte(image2, descriptorImage2(1), descriptorImage2(2));
    varianceLuminance(image2, descriptorImage2(0), descriptorImage2(3));
    //contrastColor(image2, descriptorImage2(3));
//    averageColor(image2, &descriptorImage2[index]);  index += 3;
//    contrastColor(image2, descriptorImage2[index++], var);
//    saturation(image2, descriptorImage2[index++]);
    //histogramColor(image2, descriptorImage2.begin()+index, nbBins);
/*
    histogramColor(image2, tab, nbBins);
    for(unsigned int i=0; i<nbBins*3; i++)
        descriptorImage2[index++] = tab[i];
*/
    // print
    //std::cout << "image1 : \n" << descriptorImage1 << std::endl;
    //std::cout << "image2 : \n" << descriptorImage2 << std::endl;
    //std::cout << std::endl << std::endl;

    // error
    VectorXd error(descriptorImage1-descriptorImage2);
    //std::cout << error << std::endl;

    return error;
}


VectorXd build_a(const unsigned int lutNbBins, LUT3D& lut)
{
    unsigned int lutSize(lut.getDataSize());
    VectorXd a(lutSize);
    for(unsigned int i=0; i<lutSize; i++)
    {
        a[i] = lut(i);
        //std::cout << "b_a: " <<lut(i) << std::endl;
    }
    return a;
}

void read_a(const VectorXd &a,
            LUT3D& lut)
{
    for(unsigned int i=0; i<lut.getDataSize(); i++)
        lut(i) = a[i];
}


////////////////////////////////////////////////////////////////////////////////////
// constant
VectorXd build_b(const kn::ImageRGB8u &image1,
                           const kn::ImageRGB8u &image2,
                           const unsigned int lutNbBins,
                           const IpPairVec &matches,
                           unsigned int div = 1)
{
    unsigned int d_x1(image1.width()/(image1.width()/div));
    unsigned int d_y1(image1.height()/(image1.height()/div));
    unsigned int d_x2(image2.width()/(image2.width()/div));
    unsigned int d_y2(image2.height()/(image2.height()/div));

    unsigned int im1Size = (image1.width()/d_x1)*(image1.height()/d_y1);
    unsigned int im2Size = (image2.width()/d_x2)*(image2.height()/d_y2);
    unsigned int bSize = 2+ im1Size*3 + 2+ im2Size*3 + 1 + 1 +2*(2+1)*matches.size(); // w1,h1,image1, w2,h2,image2, lutNbBins, lutNbCoeff, matchesSize, matches

    //std::cout << "b size " << bSize << std::endl;
    VectorXd b(bSize);
    unsigned int index = 0;

    // image1 w,h
    b[index++] = (image1.width()/d_x1);
    b[index++] = (image1.height()/d_y1);

    // image1
    for(unsigned int j(0); j+d_y1-1<image1.height(); j+= d_y1)
    for(unsigned int i(0); i+d_x1-1<image1.width(); i+=d_x1)
    {
        double avgR(0.0);
        double avgG(0.0);
        double avgB(0.0);

        for(unsigned h(0); h<d_y1; ++h)
        for(unsigned g(0); g<d_x1; ++g)
        {
            avgR += image1(i+g,j+h)[0];
            avgG += image1(i+g,j+h)[1];
            avgB += image1(i+g,j+h)[2];
        }
        avgR /= (d_y1*d_x1);
        avgG /= (d_y1*d_x1);
        avgB /= (d_y1*d_x1);

        b[index++] = avgR;
        b[index++] = avgG;
        b[index++] = avgB;
    }

    // image2 w,h
    b[index++] = (image2.width()/d_x2);
    b[index++] = (image2.height()/d_y2);

    // image2
//    for(unsigned int j(0); j<image2.height(); j+= div)
//    for(unsigned int i(0); i<image2.width(); i+=div){
//        b[index++] = image2(i,j)[0];
//        b[index++] = image2(i,j)[1];
//        b[index++] = image2(i,j)[2];
//    }

    for(unsigned int j(0); j+d_y2-1<image2.height(); j+= d_y2)
    for(unsigned int i(0); i+d_x2-1<image2.width(); i+=d_x2)
    {
        double avgR(0.0);
        double avgG(0.0);
        double avgB(0.0);

        for(unsigned h(0); h<d_y2; ++h)
        for(unsigned g(0); g<d_x2; ++g)
        {
            avgR += image2(i+g,j+h)[0];
            avgG += image2(i+g,j+h)[1];
            avgB += image2(i+g,j+h)[2];
        }
        avgR /= (d_y2*d_x2);
        avgG /= (d_y2*d_x2);
        avgB /= (d_y2*d_x2);

        b[index++] = avgR;
        b[index++] = avgG;
        b[index++] = avgB;
    }
    // lutNbBins
    b[index++] = lutNbBins;

    b[index++] = matches.size();

    for(unsigned i(0); i< matches.size(); ++i)
    {
        b[index++] = (double) matches[i].first.x;
        b[index++] = (double) matches[i].first.y;
        b[index++] = (double) matches[i].first.scale;
        b[index++] = (double) matches[i].second.x;
        b[index++] = (double) matches[i].second.y;
        b[index++] = (double) matches[i].second.scale;
    }

    return b;
}

VectorXd build_b(const kn::ImageRGB8u &image1,
                           const kn::ImageRGB8u &image2,
                           const unsigned int lutNbBins,
                           unsigned int div = 1)
{
    unsigned int d_x1(image1.width()/(image1.width()/div));
    unsigned int d_y1(image1.height()/(image1.height()/div));
    unsigned int d_x2(image2.width()/(image2.width()/div));
    unsigned int d_y2(image2.height()/(image2.height()/div));

    unsigned int im1Size = (image1.width()/d_x1)*(image1.height()/d_y1);
    unsigned int im2Size = (image2.width()/d_x2)*(image2.height()/d_y2);
    unsigned int bSize = 2+ im1Size*3 + 2+ im2Size*3 + 1; // w1,h1,image1, w2,h2,image2, lutNbBins, lutNbCoeff

    //std::cout << "b size " << bSize << std::endl;
    VectorXd b(bSize);
    unsigned int index = 0;

    // image1 w,h
    b[index++] = (image1.width()/d_x1);
    b[index++] = (image1.height()/d_y1);

    // image1
    for(unsigned int j(0); j+d_y1-1<image1.height(); j+= d_y1)
    for(unsigned int i(0); i+d_x1-1<image1.width(); i+=d_x1)
    {
        double avgR(0.0);
        double avgG(0.0);
        double avgB(0.0);

        for(unsigned h(0); h<d_y1; ++h)
        for(unsigned g(0); g<d_x1; ++g)
        {
            avgR += image1(i+g,j+h)[0];
            avgG += image1(i+g,j+h)[1];
            avgB += image1(i+g,j+h)[2];
        }
        avgR /= (d_y1*d_x1);
        avgG /= (d_y1*d_x1);
        avgB /= (d_y1*d_x1);

        b[index++] = avgR;
        b[index++] = avgG;
        b[index++] = avgB;
    }

    // image2 w,h
    b[index++] = (image2.width()/d_x2);
    b[index++] = (image2.height()/d_y2);

    // image2
//    for(unsigned int j(0); j<image2.height(); j+= div)
//    for(unsigned int i(0); i<image2.width(); i+=div){
//        b[index++] = image2(i,j)[0];
//        b[index++] = image2(i,j)[1];
//        b[index++] = image2(i,j)[2];
//    }

    for(unsigned int j(0); j+d_y2-1<image2.height(); j+= d_y2)
    for(unsigned int i(0); i+d_x2-1<image2.width(); i+=d_x2)
    {
        double avgR(0.0);
        double avgG(0.0);
        double avgB(0.0);

        for(unsigned h(0); h<d_y2; ++h)
        for(unsigned g(0); g<d_x2; ++g)
        {
            avgR += image2(i+g,j+h)[0];
            avgG += image2(i+g,j+h)[1];
            avgB += image2(i+g,j+h)[2];
        }
        avgR /= (d_y2*d_x2);
        avgG /= (d_y2*d_x2);
        avgB /= (d_y2*d_x2);

        b[index++] = avgR;
        b[index++] = avgG;
        b[index++] = avgB;
    }
    // lutNbBins
    b[index++] = lutNbBins;

    return b;
}

VectorXd build_b(const ImageLab &image1,
                           const ImageLab &image2,
                           const unsigned int lutNbBins)
{
    unsigned int bSize = 2+image1.width()*image1.height()*3 + 2+image2.width()*image2.height()*3 + 1; // w1,h1,image1, w2,h2,image2, lutNbBins, lutNbCoeff
    //std::cout << "b size " << bSize << std::endl;
    VectorXd b(bSize);
    unsigned int index = 0;

    // image1 w,h
    b[index++] = image1.width();
    b[index++] = image1.height();

    // image1
    for(unsigned int i=0; i<image1.size();i++){
        b[index++] = image1.begin()[i];
    }

    // image2 w,h
    b[index++] = image2.width();
    b[index++] = image2.height();

    // image2
    for(unsigned int i=0; i<image2.size();i++){
        b[index++] = image2.begin()[i];
    }

    // lutNbBins
    b[index++] = lutNbBins;

    return b;
}

////////////////////////////////////////////////////////////////////////////////////
void read_b(const VectorXd &b,
            kn::ImageRGB8u &image1,
            kn::ImageRGB8u &image2,
            unsigned int &lutNbBins)
{
    unsigned int index = 0;

    // image1
    unsigned int w = b[index++];
    unsigned int h = b[index++];

    image1 = kn::ImageRGB8u(w,h);
    for(unsigned int i=0; i<w*h*3; i++)
        image1.begin()[i] = b[index++];

    // image2
    w = b[index++];
    h = b[index++];
    image2 = kn::ImageRGB8u(w,h);
    for(unsigned int i=0; i<w*h*3; i++)
        image2.begin()[i] = b[index++];

    // lutNbBins
    lutNbBins = b[index++];
}

void read_b(const VectorXd &b,
            kn::ImageRGB8u &image1,
            kn::ImageRGB8u &image2,
            IpPairVec& matches,
            unsigned int &lutNbBins)
{
    unsigned int index = 0;

    matches.clear();
    // image1
    unsigned int w = b[index++];
    unsigned int h = b[index++];

    image1 = kn::ImageRGB8u(w,h);
    for(unsigned int i=0; i<w*h*3; i++)
        image1.begin()[i] = b[index++];

    // image2
    w = b[index++];
    h = b[index++];
    image2 = kn::ImageRGB8u(w,h);
    for(unsigned int i=0; i<w*h*3; i++)
        image2.begin()[i] = b[index++];

    // lutNbBins
    lutNbBins = b[index++];

    unsigned nbmatches = b[index++];
    Ipoint pt1, pt2;
    for(unsigned i(0); i<nbmatches; ++i)
    {
        pt1.x = b[index++];
        pt1.y = b[index++];
        pt1.scale = b[index++];
        pt2.x = b[index++];
        pt2.y = b[index++];
        pt2.scale = b[index++];

        matches.push_back(std::make_pair(pt1, pt2));
    }
}

void read_b(const VectorXd &b,
            ImageLab &image1,
            ImageLab &image2,
            unsigned int &lutNbBins)
{
    unsigned int index = 0;

    // image1
    unsigned int w = b[index++];
    unsigned int h = b[index++];

    image1 = ImageLab(w,h,3);
    for(unsigned int i=0; i<w*h*3; i++)
        image1.begin()[i] = b[index++];

    // image2
    w = b[index++];
    h = b[index++];
    image2 = ImageLab(w,h,3);
    for(unsigned int i=0; i<w*h*3; i++)
        image2.begin()[i] = b[index++];

    // lutNbBins
    lutNbBins = b[index++];
}

////////////////////////////////////////////////////////////////////////////////////
void read_b_wh(const VectorXd &b,
              unsigned int &image1Width,
              unsigned int &image1Height,
              unsigned int &image2Width,
              unsigned int &image2Height)
{
    unsigned int index = 0;

    // image1
    image1Width  = b[index++];
    image1Height = b[index++];
    index += image1Width*image1Height*3;

    // image2
    image2Width  = b[index++];
    image2Height = b[index++];
    index += image1Width*image1Height*3;
}

void read_b_wh(const VectorXd &b,
              unsigned int &image1Width,
              unsigned int &image1Height,
              unsigned int &image2Width,
              unsigned int &image2Height,
              unsigned int &nbmatches)
{
    unsigned int index = 0;

    // image1
    image1Width  = b[index++];
    image1Height = b[index++];
    index += image1Width*image1Height*3;

    // image2
    image2Width  = b[index++];
    image2Height = b[index++];
    index += image1Width*image1Height*3;
    nbmatches = b[index++];
    index += 2*(2+1)*nbmatches;
}




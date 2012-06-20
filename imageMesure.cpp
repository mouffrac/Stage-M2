#include "imageMesure.hpp"


#include <cmath>
#include "Image3D.hpp"
#include "ImageLab.hpp"
#include "MatricesAndVectorsDefinitions.hpp"


////////////////////////////////////////////////////////////////////////////////////
/// normalized average color
void averageColor(const kn::ImageRGB8u& image, double *average)
{
    average[0] = average[1] = average[2] = 0.0;

    for(unsigned int i=0; i<image.width(); i++)
        for(unsigned int j=0; j<image.height(); j++)
            for(unsigned int c=0; c<3; c++)
                average[c] += (double)image(i,j)[c];

    for(unsigned int c=0; c<3; c++)
        average[c] /= image.width()*image.height()*255.0; // normalized average color
}

//void skewKurt(const kn::ImageRGB8u& image, const double *average, double variance, double & skewness, double & kurtosis)
//{
//
//    skewness = kurtosis = 0.0;
//
//    for(unsigned j(0); j< image.height(); ++j)
//    {
//        for(unsigned i(0); i< image.width(); ++i)
//        {
//            for(unsigned c(0); c<3; ++c)
//            {
//                //std::cout << pow(((image(i,j)[c]-(average[c]*255.))/sqrt(variance)), 4) << std::endl;
//                kurtosis += pow(((image(i,j)[c]-(average[c]*255.))/sqrt(variance)), 4);
//            }
//        }
//    }
//
////    kurtosis /= (image.width()*image.height()*3);
////    kurtosis -= 3;
//
//
//    std::cout << kurtosis << std::endl;
//}


////////////////////////////////////////////////////////////////////////////////////
void saturation(const kn::ImageRGB8u& image, double &saturationValue)
{
    saturationValue = 0.0;

    for(unsigned int i=0; i<image.width(); i++)
        for(unsigned int j=0; j<image.height(); j++)
        {
            // compute the grey scale color of the pixel
            double averageColor = 0.0;
            for(unsigned int c=0; c<3; c++)
                averageColor += ((double)image(i,j)[c])/3.0;

            // pixel contrast
            for(unsigned int c=0; c<3; c++)
                saturationValue += fabs((((double)image(i,j)[c]))-averageColor)/3.0;
        }
    const double saturationNormalizationTerm = 10.0;
    saturationValue /= image.width()*image.height()*saturationNormalizationTerm;
}

////////////////////////////////////////////////////////////////////////////////////
void contrastColor(const kn::ImageRGB8u& image, double &contrast, double & variance)
{
    // 3D histogram
    const unsigned int nbBins = 10;
    kn::Image3D<unsigned int> histogram3D(nbBins,nbBins,nbBins,1);
    histogram3D.fill(0);

    // fill the histogram
    int step = 255/ ((int)nbBins-1);
    for(unsigned int i=0; i<image.width(); i++)
        for(unsigned int j=0; j<image.height(); j++)
        {
            int x = image(i,j)[0]/step;// +1;
            int y = image(i,j)[1]/step;// +1;
            int z = image(i,j)[2]/step;// +1;

            // increments the value x,y,z
            histogram3D(x,y,z)[0]++;
        }

    // compute the 3D average of the histogram
    Vector3d histogramPos;
    Vector3d average;
    average.fill(0.0);
    for(unsigned int i=0; i<nbBins; i++)
        for(unsigned int j=0; j<nbBins; j++)
            for(unsigned int k=0; k<nbBins; k++)
                {
                    histogramPos[0] = (double)i;
                    histogramPos[1] = (double)j;
                    histogramPos[2] = (double)k;
                    average += histogramPos * histogram3D(i,j,k)[0];
                }
    average /= image.width()*image.height();

    // compute the variance of the 3D histogram
    variance = 0.0;
    for(unsigned int i=0; i<nbBins; i++)
        for(unsigned int j=0; j<nbBins; j++)
            for(unsigned int k=0; k<nbBins; k++)
            {
                // distance to average
                double distance = sqrt(pow(i-average[0],2)+pow(j-average[1],2)+pow(k-average[2],2));
                variance += pow(histogram3D(i,j,k)[0]*distance,2);
            }

    contrast = sqrt(variance) / (image.width()*image.height());
}

void contrastColor(const ImageLab& image, double &contrast)
{
    // 3D histogram
    const unsigned int nbBins = 10;
    kn::Image3D<unsigned int> histogram3D(nbBins,nbBins,nbBins,1);
    histogram3D.fill(0);

    double min = 0.;
    double max = 0.;
    // fill the histogram
    int step = 255/ ((int)nbBins-1);
    for(unsigned int i=0; i<image.width(); i++)
        for(unsigned int j=0; j<image.height(); j++)
        {
            int x = (image(i,j)[0]*(nbBins-1))/100;// +1;
            int y = ((image(i,j)[1]+128)*(nbBins-1))/255;// +1;
            int z = ((image(i,j)[2]+128)*(nbBins-1))/255;// +1;
            // increments the value x,y,z
            histogram3D(x,y,z)[0]++;
        }


    // compute the 3D average of the histogram
    Vector3d histogramPos;
    Vector3d average;
    average.fill(0.0);
    for(unsigned int i=0; i<nbBins; i++)
        for(unsigned int j=0; j<nbBins; j++)
            for(unsigned int k=0; k<nbBins; k++)
                {
                    histogramPos[0] = (double)i;
                    histogramPos[1] = (double)j;
                    histogramPos[2] = (double)k;
                    average += histogramPos * histogram3D(i,j,k)[0];
                }
    average /= image.width()*image.height();

    // compute the variance of the 3D histogram
    double variance = 0.0;
    for(unsigned int i=0; i<nbBins; i++)
        for(unsigned int j=0; j<nbBins; j++)
            for(unsigned int k=0; k<nbBins; k++)
            {
                // distance to average
                double distance = sqrt(pow(i-average[0],2)+pow(j-average[1],2)+pow(k-average[2],2));
                variance += pow(histogram3D(i,j,k)[0]*distance,2);
            }

    contrast = sqrt(variance) / (image.width()*image.height());
}

////////////////////////////////////////////////////////////////////////////////////
void histogramColor(const kn::ImageRGB8u& image, double *histo, const unsigned int nbBins)
{
    // init
    VectorXd histoR(nbBins);
    VectorXd histoG(nbBins);
    VectorXd histoB(nbBins);
    histoR.fill(0.0);
    histoG.fill(0.0);
    histoB.fill(0.0);
    int step = 255 / ((int)nbBins-1);

    // compute the 3 histograms
    for(unsigned int i=0; i<image.width(); i++)
        for(unsigned int j=0; j<image.height(); j++)
        {
            unsigned int valR = image(i,j)[0] / step;
            unsigned int valG = image(i,j)[1] / step;
            unsigned int valB = image(i,j)[2] / step;

            histoR[valR]++;
            histoG[valG]++;
            histoB[valB]++;
        }

    // normalize the histograms
    histoR *= 1.0/(double)(image.width()*image.height());
    histoG *= 1.0/(double)(image.width()*image.height());
    histoB *= 1.0/(double)(image.width()*image.height());

    // concat the 3 histograms
    for(unsigned int i=0; i<nbBins;i++) histo[i]          = histoR[i];
    for(unsigned int i=0; i<nbBins;i++) histo[i+nbBins]   = histoG[i];
    for(unsigned int i=0; i<nbBins;i++) histo[i+2*nbBins] = histoB[i];
}

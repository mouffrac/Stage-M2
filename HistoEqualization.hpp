#ifndef _HISTO_EQUA_HPP_
#define _HISTO_EQUA_HPP_

#include <iostream>
#include <cstdlib>
#include "ImageRGB.hpp"
#include "ioPNG.hpp"
#include "ioJPG.hpp"

#include <Eigen/Dense>

#include <sys/time.h>

typedef Eigen::Matrix<double, Eigen::Dynamic, 1> VectorDouble;

void histogramColorz(const kn::ImageRGB8u& image, double *histo, const unsigned int nbBins)
{
    // init
    VectorDouble histoR(nbBins);
    VectorDouble histoG(nbBins);
    VectorDouble histoB(nbBins);

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

    // concat the 3 histograms
    for(unsigned int i=0; i<nbBins;i++) histo[i]          = histoR[i];
    for(unsigned int i=0; i<nbBins;i++) histo[i+nbBins]   = histoG[i];
    for(unsigned int i=0; i<nbBins;i++) histo[i+2*nbBins] = histoB[i];
}


/**
 * Compute cumulated histogram for each canals and concat them.
 * */
void histogramCumulatedColor(const double* histo, double *histoC)
{
	for(int c(0); c<3; ++c)
	{
		histoC[c*256] = histo[c*256];
		for(unsigned int i=1; i<256;i++) histoC[i+c*256]       = histoC[i-1+c*256] + histo[i+c*256];
	}
}


/** Computation of the LUT to correct the original image based on the model image. */
void equalizeHistograms(const kn::ImageRGB8u& model, const kn::ImageRGB8u& original, double* LUT)
{
	/* Declaration and initialization of the arrays*/

	double histoModel[3*256];
	double histoCModel[3*256];
	double histoOriginal[3*256];
	double histoCOriginal[3*256];

	/* Compute stats about both images */
	int sizeModel = model.size();
	int sizeOriginal = original.size();

	histogramColorz(model, histoModel, 256);
	histogramCumulatedColor(histoModel, histoCModel);
	histogramColorz(original, histoOriginal, 256);
	histogramCumulatedColor(histoOriginal, histoCOriginal);

    /* equalization of the histograms for each canals (creation of the Look-Up Table) */
    for(int c(0); c<3; ++c)
    {
		int j = 0;
		for(int i(0); i<256; ++i)
		{
			while(j<=256 && (histoCModel[j+c*256]/sizeModel < histoCOriginal[i+c*256]/sizeOriginal))
			{
				++j;
			}
			LUT[i+c*256] = j;
		}
	}

	/* Free Memory */
}

void getApproxImage(const kn::ImageRGB8u & src, kn::ImageRGB8u & dest)
{
	double LUT[3*256];
	equalizeHistograms(src, dest, LUT);
	for(unsigned int j(0); j<dest.height(); ++j)
		for(unsigned int i(0); i<dest.width(); ++i)
		{
			for(int c(0); c<3; ++c)
				dest(i,j)[c] = LUT[dest(i,j)[c]+c*256];
		}
}

#endif //_HISTO_EQUA_HPP_

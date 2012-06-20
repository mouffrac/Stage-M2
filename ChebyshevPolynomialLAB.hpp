//! \file ChebyshevPolynomialsLUT3D.hpp
//! \author C.Mouffranc
//! \date 05/07/2012

#ifndef _CHEBY_POLY_LAB_HPP_
#define _CHEBY_POLY_LAB_HPP_

#include "ChebyshevPolynomialLUT3D.hpp"
#include "Matrix3D.hpp"

//!
//! \class ChebyLUT ChebyLUT.hpp
//! \brief Look-Up Table using Chebyshev Polynomials Basis
//! \see LUT3D
//!
class ChebyLUT_LAB : public ChebyLUT
{
    public:

    ChebyLUT_LAB(unsigned int k = 3, unsigned int nbBins = 32): ChebyLUT(k, nbBins)
    {
        if(k > maxSize) // verification of number of polynomials asked.
        {
            m_nbCoeff = maxSize;
            coeff = Matrix3D<double, 3>(maxSize);
        }

        for(unsigned i(0); i<maxSize; ++i) // Allocation of the polynomials containers
        {
            polynomials(i) = Eigen::VectorXd(i+1);
        }


        // creation of Chebyshev Polynomials
        polynomials(0) << 1.;
        polynomials(1) << 0., 1.;
        polynomials(2) << -1., 0., 2.;
        polynomials(3) << 0., -3., 0., 4.;
        polynomials(4) << 1., 0., -8., 0., 8.;
        polynomials(5) << 0., 5., 0., -20., 0., 16.;
        polynomials(6) << -1., 0., 18., 0., -48., 0., 32.;
        polynomials(7) << 0., -7., 0., 56., 0., -112., 0., 64.;
        polynomials(8) << 1., 0., -32., 0., 160., 0., -256., 0., 128.;
        polynomials(9) << 0., 9., 0, -120., 0., 432., 0., -576., 0, 256.;

        // identity LUT (Red: width, Green: height, Blue: depth)
        coeff(1,0,0,0) = coeff(0,1,0,1) = coeff(0,0,1,2) = m_nbCoeff*m_nbCoeff*m_nbCoeff;

    }
  void exportLUT3D(kn::Image3D<double>& lut, unsigned size) const;
  void exportLUT3D(kn::Image3D<unsigned char>& lut, unsigned size) const;

  void exportLUT3DLAB(kn::Image3D<double>& lut, unsigned size) const;

};

#endif //_CHEBY_POLY_LAB_HPP_


//! \file ChebyshevPolynomialsLUT3D.hpp
//! \author C.Mouffranc
//! \date 05/07/2012

#ifndef _CHEBY_LUT3D_HPP_
#define _CHEBY_LUT3D_HPP_

#include "LUT3D.hpp"
#include "Matrix3D.hpp"

// Number of Chebyshev polynomials implemented
const unsigned maxSize = 10;

//!
//! \class ChebyLUT ChebyLUT.hpp
//! \brief Look-Up Table using Chebyshev Polynomials Basis
//! \see LUT3D
//!
class ChebyLUT : public LUT3D
{
    protected:
    unsigned m_nbCoeff;
    unsigned m_defaultNbBins;

    // coefficients used to evaluate colors
    Matrix3D<double, 3> coeff;

    // list of Chebyshev polynomials
    Eigen::Matrix<Eigen::VectorXd, Eigen::Dynamic, 1> polynomials;

    public:
    //!
    //! \brief Constructor
    //!
    //! Creation of an identity Look-Up Table using Chebyshev Polynomials.
    //! \param[in] k number of polynomials used to approximate the LUT.<br/>
    //! \param[in] nbBins default number of bins used to compute an image<br/>
    //! The more polynomials, the better the results will be.
    //! \bug Cannot use more than 10 polynomials for the moment.
    //!
    ChebyLUT(unsigned int k = 3, unsigned int nbBins = 32): m_nbCoeff(k), m_defaultNbBins(nbBins), coeff(k), polynomials(maxSize)
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

    //!
    //! \brief Debug Display
    //!
    //!  displays the coefficients in the standard output. (used for debug)
    //!
    void show();

    // export the LUT in an Image3D with double values
    void exportLUT3D(kn::Image3D<double>& lut, unsigned size) const;

    // export the LUT in an Image3D with unsigned char values
    void exportLUT3D(kn::Image3D<unsigned char>& lut, unsigned size) const;

    // compute the new color with unsigned char values through the LUT
    void eval(double x, double y, double z, unsigned char* val)  const;

    // compute the new color with double values through the LUT
    void eval(double x, double y, double z, double* val)  const;

    // return the number of coefficients in 'coeff'
    unsigned int getDataSize() const;

    //!
    //! \brief number of polynomials used
    //!
    //! \return number of polynomials which are used to compute the 3D LUT.
    //!
    unsigned int getSize() const;

    unsigned int getNbBins() const
    {
        return m_defaultNbBins;
    }

    // return the coeff, x, y, z, c
    double& operator()(unsigned x, unsigned y, unsigned z, unsigned c);

    // return the coeff i
    double&  operator()(unsigned i);


};

#endif //_CHEBYSHEV_POLYNOMIAL_LUT3D_HPP_

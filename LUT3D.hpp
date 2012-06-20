
//! \file LUT3D.hpp
//! \author C.Mouffranc
//! \date 05/07/2012


#ifndef _LUT3D_HPP_
#define _LUT3D_HPP_

#include "ImageRGB.hpp"
#include "Image3D.hpp"

class LUT3DDisplayer;


//!
//! \class LUT3D LUT3D.hpp
//! \brief Interface of 3D Look-Up Tables
//!
//!
class LUT3D
{

    public:

    //!
    //! \brief <b>Export of LUT</b>
    //! \param[in] size size of the Image3D given in parameter.
    //! \param[out] lut exported Look-Up Table of size <i>size</i>, with double values
    //!
    virtual void exportLUT3D(kn::Image3D<double>& lut, unsigned size) const = 0;

    //!
    //! \brief <b>Export of LUT</b>
    //! \param[in] size size of the Image3D given in parameter.
    //! \param[out] lut exported Look-Up Table of size <i>size</i>, with unsigned char values
    //!
    virtual void exportLUT3D(kn::Image3D<unsigned char>& lut, unsigned size) const = 0;

    //!
    //! \brief <b>Evaluation of a color through the LUT</b>
    //!
    //! Values have to be inside the interval [0. 1.],<br/>
    //! the result is put in <i>val</i> and contains values between 0 and 255
    //! \param[in] x width coordinate
    //! \param[in] y height coordinate
    //! \param[in] z depth coordinate
    //! \param[out] val unsigned char array containing the RGB resulting value
    //!
    virtual void eval(double x, double y, double z, unsigned char* val) const = 0;

    //!
    //! \brief <b>Evaluation of a color through the LUT</b>
    //!
    //! Values have to be inside the interval [0. 1.],<br/>
    //! the result is put in <i>val</i> and contains values between 0. and 1.
    //! \param[in] x width coordinate
    //! \param[in] y height coordinate
    //! \param[in] z depth coordinate
    //! \param[out] val double array containing the RGB resulting value. Values between 0. and 1.
    //!
    virtual void eval(double x, double y, double z, double* val) const = 0;

    //!
    //! \brief <b>Data Size Getter</b>
    //!
    //! Getting the number of coefficients in the LUT.
    //! \return data size
    virtual unsigned int getDataSize() const = 0;

    virtual unsigned int getSize() const = 0;

    virtual unsigned int getNbBins() const = 0;

    // return the coeff, x, y, z, c
    virtual double& operator()(unsigned x, unsigned y, unsigned z, unsigned c) = 0;

    // return the coeff i
    virtual double&  operator()(unsigned i) = 0;

    //!
    //! \brief Interface of a 3D Look-Up Table Displayer
    //!
    friend class LUT3DDisplayer;
};

#endif //_LUT3D_HPP_

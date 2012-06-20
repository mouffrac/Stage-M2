//! \file DefaultLUT3D.hpp
//! \author C.Mouffranc
//! \date 05/07/2012

#ifndef _DEFAULT_LUT3D_HPP_
#define _DEFAULT_LUT3D_HPP_

#include "Image3D.hpp"
#include "LUT3D.hpp"

//!
//! \class DefaultLUT3D DefaultLUT3D.hpp
//! \brief Default Look-Up Table
//! \see LUT3D
//!
class DefaultLUT3D : public LUT3D
{
    kn::Image3D<double> m_image3d;
    unsigned int m_size;

    public:

    //!
    //! \brief <b>Identity Constructor</b>
    //!
    //! Creates the Look-Up Table identity of size <i>size</i>
    //! \param[in] size size of the Look-Up Table wanted.<br/>
    //! The bigger the LUT is, the better the results are, but the longer the computation time is.<br/>
    //! Choose the size wisely.
    //!
    DefaultLUT3D(unsigned int size)
    : m_image3d(size, size, size, 3), m_size(size)
    {
        double norm(1.0/(size-1));


        /* Creates Identity LUT */
        for(unsigned z(0); z < size; ++z)
            for(unsigned y(0); y < size; ++y)
                for(unsigned x(0); x < size; ++x)
                {
                    m_image3d(x,y,z)[0] = x * norm;
                    m_image3d(x,y,z)[1] = y * norm;
                    m_image3d(x,y,z)[2] = z * norm;
                }
    }
    //!
    //! \brief <b>Constructor</b>
    //!
    //! Creates a Look-Up Table with a kn::Image3D<double> containing an existing LUT.<br/>
    //! The values inside the existing LUT must be between 0. and 1.
    //!
    //! \param[in] imageLUT Image3D representing an existing Look-Up Table.
    //!
    DefaultLUT3D(const kn::Image3D<double> & imageLUT)
    : m_image3d(imageLUT.width(), imageLUT.height(), imageLUT.depth(), 3), m_size(imageLUT.width())
    {
        for(unsigned z(0); z < imageLUT.depth(); ++z)
            for(unsigned y(0); y < imageLUT.height(); ++y)
                for(unsigned x(0); x < imageLUT.width(); ++x)
                {
                    m_image3d(x,y,z)[0] = imageLUT(x,y,z)[0];
                    m_image3d(x,y,z)[1] = imageLUT(x,y,z)[1];
                    m_image3d(x,y,z)[2] = imageLUT(x,y,z)[2];
                }
    }

    //!
    //! \brief <b>Constructor</b>
    //!
    //! Creates a Look-Up Table with a kn::Image3D<unsigned char> containing an existing LUT.<br/>
    //! The values inside the existing LUT must be between 0 and 255
    //!
    //! \param[in] imageLUT Image3D representing an existing Look-Up Table.
    //!
    DefaultLUT3D(const kn::Image3D<unsigned char> & imageLUT)
    : m_image3d(imageLUT.width(), imageLUT.height(), imageLUT.depth(), 3), m_size(imageLUT.width())
    {
        for(unsigned z(0); z < imageLUT.depth(); ++z)
            for(unsigned y(0); y < imageLUT.height(); ++y)
                for(unsigned x(0); x < imageLUT.width(); ++x)
                {
                    m_image3d(x,y,z)[0] = static_cast<double>(imageLUT(x,y,z)[0]/255.0);
                    m_image3d(x,y,z)[1] = static_cast<double>(imageLUT(x,y,z)[1]/255.0);
                    m_image3d(x,y,z)[2] = static_cast<double>(imageLUT(x,y,z)[2]/255.0);
                }
    }

    //!
    //! \brief Export of the Look-Up Table
    //!
    //! Computes a 3D Image of this Look-Up Table.
    //!
    //! \param[out] lut resulting Image3D with double values between 0. and 1.
    //! \param[in] size size of the LUT wanted.
    //!
    void exportLUT3D(kn::Image3D<double>& lut, unsigned size) const;

    //!
    //! \brief Export of the Look-Up Table
    //!
    //! Computes a 3D Image of this Look-Up Table.
    //!
    //! \param[out] lut resulting Image3D with unsigned char values between 0 and 255
    //! \param[in] size size of the LUT wanted.
    //!
    void exportLUT3D(kn::Image3D<unsigned char>& lut, unsigned size) const;

    //!
    //! \brief Evaluation
    //!
    //! Computes the resulting color through the Look-Up Table
    //!
    //! \param[in] x width coordinate
    //! \param[in] y height coordinate
    //! \param[in] z depth coordinate
    //! \param[out] val unsigned char array containing the resulting color (values between 0 and 255)
    //!
    void eval(double x, double y, double z, unsigned char* val)  const;

    //!
    //! \brief Evaluation
    //!
    //! Computes the resulting color through the Look-Up Table
    //!
    //! \param[in] x width coordinate
    //! \param[in] y height coordinate
    //! \param[in] z depth coordinate
    //! \param[out] val unsigned char array containing the resulting color (values between 0. and 1.)
    //!
    void eval(double x, double y, double z, double* val)  const;


    //!
    //! \brief Size Getter
    //!
    //! Gets the number of bins inside the LUT times the number of canals.
    //! \param[out] the return parameter correspond to the number of all the coefficients stored in the LUT.
    //!
    unsigned int getDataSize() const {return m_size*m_size*m_size*3;}


    //!
    //! \brief number of bins Getter
    //!
    //! Gets the number of bins used in the 3D LUT. (i.e. for a 3*3*3 LUT, this will return 3)
    //! \param[out] the return parameter correspond to the number of bins in the LUT.
    //!
    unsigned int getSize() const {return m_size;}

    unsigned int getNbBins() const {return m_size;}

    //!
    //! \brief Operator() redefinition
    //!
    //! Accesses directly the x, y, z coefficients (with interpolation)
    //!
    //! \param[in] x [double] width coordinate
    //! \param[in] y [double] height coordinate
    //! \param[in] z [double] depth coordinate
    //! \return a direct access to the coefficients x,y,z. Possibility to modify the coefficients stored inside the LUT
    //!
    double* operator() (double x, double y, double z);

    // return the coeff, x, y, z, c
    double& operator()(unsigned x, unsigned y, unsigned z, unsigned c);

    // return the coeff i
    double&  operator()(unsigned i);

};

#endif //_DEFAULT_LUT3D_HPP_

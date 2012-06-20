//! \file Matrix3D.hpp
//! \author C.Mouffranc
//! \date 05/07/2012

#ifndef _MATRIX3D_HPP_
#define _MATRIX3D_HPP_

#include <Eigen/Dense>

//!
//! \class Matrix3D Matrix3D.hpp
//! \brief 3D storage
//!
template <typename T, unsigned N>
class Matrix3D
{
    private:
    Eigen::Matrix<Eigen::Matrix<T, Eigen::Dynamic, N >, Eigen::Dynamic, Eigen::Dynamic > data;

    public:

    //!
    //! \brief Constructor
    //!
    //! \param[in] size size of a side of the cubical matrix.
    //!
    Matrix3D(unsigned int size)
    : data(size, size)
    {
        for(unsigned y(0); y<size; ++y)
        for(unsigned x(0); x<size; ++x)
        {
            data(x,y) = Eigen::Matrix<T, Eigen::Dynamic, N>(size,N);
        }

        // initialization with zeros.
        for(unsigned z(0); z<size; ++z)
            for(unsigned y(0); y<size; ++y)
                for(unsigned x(0); x<size; ++x)
                    for(unsigned c(0); c<N; ++c)
                        data(x,y)(z,c) = 0;
    }

    //!
    //! \brief Const Accessor
    //! \param[in] x width coordinate
    //! \param[in] y height coordinate
    //! \param[in] z depth coordinate
    //! \param[in] c canal coordinate
    //! \return a copy of the element stored in (x,y,z,c)
    //!
    T operator()(unsigned int x,unsigned int y = 0,unsigned int z = 0, unsigned int c = 0) const
    {
        return data(x,y)(z,c);
    }

    //!
    //! \brief Accessor
    //! \param[in] x width coordinate
    //! \param[in] y height coordinate
    //! \param[in] z depth coordinate
    //! \param[in] c canal coordinate
    //! \return a reference of the element stored in (x,y,z,c)
    //!
    T& operator()(unsigned int x,unsigned int y = 0,unsigned int z = 0, unsigned int c = 0)
    {
        return data(x,y)(z,c);
    }

};


#endif

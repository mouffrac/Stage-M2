#include "DefaultLUT3D.hpp"

void DefaultLUT3D::exportLUT3D(kn::Image3D<double>& lut, unsigned size) const
{
    double norm((m_size-1)*1.0/(size-1));

    for(unsigned z(0); z < size; ++z)
        for(unsigned y(0); y < size; ++y)
            for(unsigned x(0); x < size; ++x)
            {
                lut(x,y,z)[0] = m_image3d(x*norm,y*norm,z*norm)[0];
                lut(x,y,z)[1] = m_image3d(x*norm,y*norm,z*norm)[1];
                lut(x,y,z)[2] = m_image3d(x*norm,y*norm,z*norm)[2];
            }
}

void DefaultLUT3D::exportLUT3D(kn::Image3D<unsigned char>& lut, unsigned size) const
{
    double norm((m_size-1)*1.0/(size-1.));

    for(unsigned z(0); z < size; ++z)
        for(unsigned y(0); y < size; ++y)
            for(unsigned x(0); x < size; ++x)
            {
                double col[3];

                m_image3d.atBilinear(x*norm,y*norm,z*norm,col);

                unsigned char r(static_cast<unsigned char>(col[0] * 255));
                unsigned char g(static_cast<unsigned char>(col[1] * 255));
                unsigned char b(static_cast<unsigned char>(col[2] * 255));
                lut(x,y,z)[0] = r;
                lut(x,y,z)[1] = g;
                lut(x,y,z)[2] = b;

            }
}

void DefaultLUT3D::eval(double x, double y, double z, unsigned char* val)  const
{
    double tmp[3];
    m_image3d.atBilinear(x*(m_image3d.width()-1), y*(m_image3d.height()-1), z*(m_image3d.depth()-1),tmp);
    val[0] = static_cast<unsigned char>(std::max(0., std::min(255., tmp[0]*255.)));
    val[1] = static_cast<unsigned char>(std::max(0., std::min(255., tmp[1]*255.)));
    val[2] = static_cast<unsigned char>(std::max(0., std::min(255., tmp[2]*255.)));
}

void DefaultLUT3D::eval(double x, double y, double z, double* val)  const
{
    m_image3d.atBilinear(x,y,z,val);
}

double* DefaultLUT3D::operator()(double x, double y, double z)
{
    return m_image3d(x, y,z);
}

// return the coeff, x, y, z, c
double& DefaultLUT3D::operator()(unsigned x, unsigned y, unsigned z, unsigned c)
{
    return m_image3d(x, y,z)[c];
}

// return the coeff i
double& DefaultLUT3D::operator()(unsigned i)
{
    unsigned x,y,z;
    z= i/(m_size*m_size*3);
    i %= (m_size*m_size*3);

    y = i/(m_size*3);
    i %= (m_size*3);

    x = i/3;
    i %= 3;

    return m_image3d(x,y,z)[i];
}

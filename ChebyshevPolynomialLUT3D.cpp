#include "ChebyshevPolynomialLUT3D.hpp"


#include <typeinfo>
#include <unsupported/Eigen/Polynomials>
#include "MatricesAndVectorsDefinitions.hpp"

void ChebyLUT::show()
{
    for(unsigned z(0); z<m_nbCoeff; ++z)
    {
        for(unsigned y(0); y<m_nbCoeff; ++y)
        {
            for(unsigned x(0); x<m_nbCoeff; ++x)
            {
                std::cout << "(" <<coeff(x,y,z,0) << ", "<< coeff(x,y,z,1) << ", " << coeff(x,y,z,2) << ")\t";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}

void ChebyLUT::exportLUT3D(kn::Image3D<double>& lut, unsigned size) const
{
    double norm(1./(size-1));

    for(unsigned z(0); z< size; ++z)
    for(unsigned y(0); y< size; ++y)
    for(unsigned x(0); x< size; ++x)
    {
        eval(x*norm, y*norm, z*norm, lut(x,y,z));
    }
}

void ChebyLUT::exportLUT3D(kn::Image3D<unsigned char>& lut, unsigned size) const
{

    double norm(1./(size-1));
    double tmp[3] = {0};

    /* for all voxels in LUT, evaluate their values */
    for(unsigned z(0); z < size; ++z)
    for(unsigned y(0); y < size; ++y)
    for(unsigned x(0); x < size; ++x)
    {
        eval(x*norm, y*norm, z*norm, tmp);

        lut(x,y,z)[0] = static_cast<unsigned char>(std::max(0., std::min(255., tmp[0]*255.)));
        lut(x,y,z)[1] = static_cast<unsigned char>(std::max(0., std::min(255., tmp[1]*255.)));
        lut(x,y,z)[2] = static_cast<unsigned char>(std::max(0., std::min(255., tmp[2]*255.)));
    }


}

void ChebyLUT::eval(double x, double y, double z, unsigned char* val)  const
{
    double tmp[3];

    eval(x,y,z,tmp);

    //std::cout << "before cast: " << tmp[0] * 255 << " ";

    /** Transforms the double value into unsigned char **/
    val[0] = static_cast<unsigned char>(std::max(0., std::min(255., (tmp[0] * 255))));
    val[1] = static_cast<unsigned char>(std::max(0., std::min(255., (tmp[1] * 255))));
    val[2] = static_cast<unsigned char>(std::max(0., std::min(255., (tmp[2] * 255))));
//        std::cout << "after cast: " << (int) val[0] << std::endl;
}

void ChebyLUT::eval(double x, double y, double z, double* val)  const
{
    val[0] = val[1] = val[2] = 0.;

    VectorXf evalX(m_nbCoeff);
    VectorXf evalY(m_nbCoeff);
    VectorXf evalZ(m_nbCoeff);

    for(unsigned int i(0); i < m_nbCoeff; ++i)
    {
        evalX[i] = Eigen::poly_eval(polynomials(i), x);
        evalY[i] = Eigen::poly_eval(polynomials(i), y);
        evalZ[i] = Eigen::poly_eval(polynomials(i), z);
    }

    for(unsigned k(0); k<m_nbCoeff; ++k)
    for(unsigned j(0); j<m_nbCoeff; ++j)
    for(unsigned i(0); i<m_nbCoeff; ++i)
    {
        for(unsigned c(0); c<3; ++c)
        {
//            double evalX(Eigen::poly_eval(polynomials(i), x));
//            double evalY(Eigen::poly_eval(polynomials(j), y));
//            double evalZ(Eigen::poly_eval(polynomials(k), z));

            val[c] += coeff(i,j,k,c) * evalX[i] * evalY[j] * evalZ[k];
        }
    }

    for(unsigned c(0); c<3; ++c)
    {
        val[c] /= m_nbCoeff*m_nbCoeff*m_nbCoeff;
    }
}

unsigned int ChebyLUT::getDataSize() const
{
    return m_nbCoeff*m_nbCoeff*m_nbCoeff*3;
}

unsigned int ChebyLUT::getSize() const
{
    return m_nbCoeff;
}

double& ChebyLUT::operator()(unsigned x, unsigned y, unsigned z, unsigned c)
{
    return coeff(x,y,z,c);
}

double&  ChebyLUT::operator()(unsigned i)
{
    unsigned x,y,z,c;
    z= i/(m_nbCoeff*m_nbCoeff*3);
    i %= (m_nbCoeff*m_nbCoeff*3);

    y = i/(m_nbCoeff*3);
    i %= (m_nbCoeff*3);

    x = i/3;
    i %= 3;

    c = i;

    return coeff(x,y,z,c);
}


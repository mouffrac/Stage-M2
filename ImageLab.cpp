#include "ImageLab.hpp"

const double lab_epsilon(216./24389.);
const double lab_k(24389./27.);

inline double labFunc(double t)
{
    return (t > lab_epsilon)? pow( t, 1./3.): (lab_k * t + 16.)/116.;
}

inline double labtorgb_fy(const Vector3d& lab)
{
    return (lab[0]+16.)/116.;
}

inline double labtorgb_fx(const Vector3d& lab)
{
    return (lab[1]/500.) + labtorgb_fy(lab);
}

inline double labtorgb_fz(const Vector3d& lab)
{
    return labtorgb_fy(lab) - (lab[2]/200.);
}


void RGBtoLAB(const unsigned char rgb[], double lab[])
{
    MatrixXd coeff(3,3);
    coeff <<    0.4306190, 0.3415419, 0.1783091,
                0.2220379, 0.7066384, 0.0713236,
                0.0201853, 0.1295504, 0.9390944;

//    double xn(0.95045), yn(1.0), zn(1.088754);

    Vector3d _rgb;
    _rgb << rgb[0]/255., rgb[1]/255., rgb[2]/255.;
    Vector3d xyz;
    xyz = coeff*_rgb;

    lab[0] = 116*labFunc(xyz[1]) - 16;
    lab[1] = 500*( labFunc(xyz[0]) - labFunc(xyz[1]));
    lab[2] = 200*( labFunc(xyz[1]) - labFunc(xyz[2]));
}

void RGBtoLABimage(const kn::Image<unsigned char> & src, ImageLab & dest)
{
    MatrixXd coeff(3,3);
    coeff <<    0.4306190, 0.3415419, 0.1783091,
                0.2220379, 0.7066384, 0.0713236,
                0.0201853, 0.1295504, 0.9390944;

//    double xn(0.95045), yn(1.0), zn(1.088754);

    for(unsigned j(0); j<src.height(); ++j)
    {
        for(unsigned i(0); i<src.width(); ++i)
        {
            Vector3d rgb;
            rgb << src(i,j)[0]/255., src(i,j)[1]/255., src(i,j)[2]/255.;
            Vector3d xyz;
            xyz = coeff*rgb;

            dest(i,j)[0] = 116*labFunc(xyz[1]) - 16;
            dest(i,j)[1] = 500*( labFunc(xyz[0]) - labFunc(xyz[1]));
            dest(i,j)[2] = 200*( labFunc(xyz[1]) - labFunc(xyz[2]));
        }
    }
}

void LABtoXYZ(const Vector3d & src, Vector3d & dest)
{
    double fx3(pow(labtorgb_fx(src),3));
    double fz3(pow(labtorgb_fz(src),3));

    dest[0] = (fx3 > lab_epsilon)? fx3: ((116.*labtorgb_fx(src) -16.)/lab_k);                 // x
    dest[1] = (src[0] > (lab_epsilon*lab_k))? pow(((src[0] + 16.)/116.),3): (src[0]/lab_k);   // y
    dest[2] = (fz3 > lab_epsilon)? fz3: ((116.*labtorgb_fz(src) -16.)/lab_k);                 // z
}

void LABtoRGBimage(const ImageLab & src, kn::Image<unsigned char> & dest)
{
    MatrixXd coeff(3,3);
    coeff <<    3.0628971, -1.3931791, -0.4757517,
                -0.9692660, 1.8760108, 0.0415560,
                0.0678775, -0.2288548, 1.0693490;

    for(unsigned j(0); j<src.height(); ++j)
    {
        for(unsigned i(0); i<src.width(); ++i)
        {

            Vector3d lab;
            lab << src(i,j)[0], src(i,j)[1], src(i,j)[2];

            Vector3d xyz;
            LABtoXYZ(lab, xyz);

            Vector3d rgb;
            rgb = coeff*xyz;
            rgb *= 255.;

            dest(i,j)[0] = rgb[0];
            dest(i,j)[1] = rgb[1];
            dest(i,j)[2] = rgb[2];
        }
    }
}


void LABtoRGB(const double lab[], unsigned char rgb[])
{
    MatrixXd coeff(3,3);
    coeff <<    3.0628971, -1.3931791, -0.4757517,
                -0.9692660, 1.8760108, 0.0415560,
                0.0678775, -0.2288548, 1.0693490;

    Vector3d _lab;
    _lab << lab[0], lab[1], lab[2];

    Vector3d xyz;
    LABtoXYZ(_lab, xyz);

    Vector3d _rgb;
    _rgb = coeff*xyz;
    _rgb *= 255.;

    rgb[0] = _rgb[0];
    rgb[1] = _rgb[1];
    rgb[2] = _rgb[2];
}

void averageLuminance(const ImageLab & image, double & avg)
{
    avg = 0.0;
    double norm(1./(image.height()*image.width()));
    for(unsigned j(0); j< image.height(); ++j)
    {
        for(unsigned i(0); i< image.width(); ++i)
        {
            avg += image(i,j)[0];
        }
    }
    avg *= norm/100.;
}

void averageTeinte(const ImageLab & image, double & a, double & b)
{
    a = b = 0.0;
    double norm(1./(image.height()*image.width()));
    for(unsigned j(0); j< image.height(); ++j)
    {
        for(unsigned i(0); i< image.width(); ++i)
        {
            a += (image(i,j)[1]+128);
            b += (image(i,j)[2]+128);
        }
    }

    a *= norm/255.;
    b *= norm/255.;
}

void varianceLuminance(const ImageLab& image, const double lumAvg, double & variance)
{
    variance = 0.0;

    double lumAvg2 = std::pow(lumAvg*100., 2);

    double norm(1./(image.height()*image.width()));
    for(unsigned j(0); j< image.height(); ++j)
    {
        for(unsigned i(0); i< image.width(); ++i)
        {
            variance += (std::pow(image(i,j)[0],2) - lumAvg2);
        }
    }
    variance *= norm/std::pow(100.,2);
}


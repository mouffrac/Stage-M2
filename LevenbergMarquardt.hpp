#ifndef LKlevenbergMarquardt_h
#define LKlevenbergMarquardt_h

#include "MatricesAndVectorsDefinitions.hpp"
#include "ChebyshevPolynomialLAB.hpp"
#include "LUT3D.hpp"


///////////////////////////////////////////////////////////////////////
// \brief newton algorithm : solves non linear system.
// We use in this implementation the recommandation of the "Multiple View Geometry", Hartley & Zisserman, 2nd edition, page 597



///////////////////////////////////////////////////////////////////////
/// This function solves a non-linear system.
/// the a vector is the unknown
/// the b vector is the const data
/// The vector X corresponds to the state we want to reach : we want X=f(a,b).
/// This function returns 1 if an acceptable solution has been found before the nbMaxIteration iterations.
/// This algorithm is detailed in the "Multiple View Geometry", Hartley & Zisserman, 2nd edition, page 605.
int levenbergMarquardt(VectorXd &a,
                       const VectorXd &b,
                       const VectorXd &X,
                       VectorXd (*costFunctionPtr)(const VectorXd&,const VectorXd&,LUT3D& lut),
                       LUT3D& lut,
                       const unsigned int nbMaxIteration = 100);
int levenbergMarquardt(VectorXd &a,
                       const VectorXd &b,
                       const VectorXd &X,
                       VectorXd (*costFunctionPtr)(const VectorXd&,const VectorXd&,ChebyLUT_LAB& lut),
                       ChebyLUT_LAB& lut,
                       const unsigned int nbMaxIteration = 100);


///////////////////////////////////////////////////////////////////////
/// This function computes the jacobian of 'a'.
/// This function is not optimized (a lot of 0 are computed for nothing) but can be adapted to every partitioned data.
/// This algorithm is detailed in the "Multiple View Geometry", Hartley & Zisserman, 2nd edition, page 602-605.
void levenbergMarquardtJacobian_A(const VectorXd &a,
                                  const VectorXd &b,
                                  MatrixXd &jacobian_A,
                                  VectorXd (*f)(const VectorXd&,const VectorXd&,LUT3D& lut),
                                  LUT3D& lut);

void levenbergMarquardtJacobian_A(const VectorXd &a,
                                  const VectorXd &b,
                                  MatrixXd &jacobian_A,
                                  VectorXd (*f)(const VectorXd&,const VectorXd&,ChebyLUT_LAB& lut),
                                  ChebyLUT_LAB& lut);


///////////////////////////////////////////////////////////////////////
/// return true if the error has decreased, false if the error has increased
bool levenbergMarquardtCheckImprovement(const VectorXd &epsilonBefore, const VectorXd &epsilonAfter);

#endif

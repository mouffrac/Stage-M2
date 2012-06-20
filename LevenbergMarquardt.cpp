#include "LevenbergMarquardt.hpp"

#include<cmath>
#include<iostream>
#include <Eigen/Dense>
#include "MatricesAndVectorsDefinitions.hpp"


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
		               VectorXd (*f)(const VectorXd&,const VectorXd&, LUT3D & lut),
		               LUT3D& lut,
		               const unsigned int nbMaxIteration)
{
  // Jacobian matrix for 'a'
  MatrixXd J(X.size(),a.size());
  // compute the jacobian
  levenbergMarquardtJacobian_A(a,b,J,f,lut);
  //std::cout << "J\n" << J << std::endl;
  // lambda parameter : decides if LM is a Gauss-Newton method or a gradient descent method.
  // lambda = 1.0e-3 x average of diag(N=J^T.J)  is a typical starting value.
  MatrixXd N(J.transpose()*J);
  double average = 0.0;
  for(unsigned int i=0; i<N.rows(); ++i)
    average += N(i,i);
  double lambda = (double) 1.0e-3 * average/(double)N.rows();
  //double lambda = (double) 1.0e-15;

    std::cout << "lambda: "<<lambda << std::endl;

  // start the iterative process
  for(unsigned int iter=0; iter<nbMaxIteration; ++iter)
    {
      std::cout << "iteration " << iter << std::endl;

      // compute the jacobian
      if(iter != 0) levenbergMarquardtJacobian_A(a,b,J,f,lut);

      // error vector
      VectorXd epsilon(X-f(a,b,lut));

      // iteration to update lambda if not adapted
      unsigned int counter = 0;
      bool accepted = false; // true if an iteration makes improvement
      do
        {
 	        // find delta
	        VectorXd delta_a(a.size());

			MatrixXd Id(J.transpose()*J);
	        Id.setIdentity();
			delta_a = (J.transpose()*J + lambda*Id).jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(J.transpose()*epsilon);

            // update parameters
            VectorXd a2(a + delta_a);

            // compute new error vector
            VectorXd epsilon2(X-f(a2,b,lut));

            // compare error
            if(levenbergMarquardtCheckImprovement(epsilon, epsilon2) == true) // if better
            {
              lambda /= 10.0;
              a = a2;
              accepted = true;
            }
            else
            {
              lambda *= 10.0;
              accepted = false;
            }

            // counter
            counter++;
            if(counter > 100)return 1;

        }while(accepted == false); // end of an iteration

     } // end of all iteration

std::cout << "iter = " << nbMaxIteration << std::endl;

  return 0;
}

int levenbergMarquardt(VectorXd &a,
		               const VectorXd &b,
		               const VectorXd &X,
		               VectorXd (*f)(const VectorXd&,const VectorXd&, ChebyLUT_LAB & lut),
		               ChebyLUT_LAB& lut,
		               const unsigned int nbMaxIteration)
{
  // Jacobian matrix for 'a'
  MatrixXd J(X.size(),a.size());
  // compute the jacobian
  levenbergMarquardtJacobian_A(a,b,J,f,lut);
  //std::cout << "J\n" << J << std::endl;
  // lambda parameter : decides if LM is a Gauss-Newton method or a gradient descent method.
  // lambda = 1.0e-3 x average of diag(N=J^T.J)  is a typical starting value.
  MatrixXd N(J.transpose()*J);
  double average = 0.0;
  for(unsigned int i=0; i<N.rows(); ++i)
    average += N(i,i);
  double lambda = (double) 1.0e-3 * average/(double)N.rows();


  // start the iterative process
  for(unsigned int iter=0; iter<nbMaxIteration; ++iter)
    {
      std::cout << "iteration " << iter << std::endl;

      // compute the jacobian
      if(iter != 0) levenbergMarquardtJacobian_A(a,b,J,f,lut);

      // error vector
      VectorXd epsilon(X-f(a,b,lut));

      // iteration to update lambda if not adapted
      unsigned int counter = 0;
      bool accepted = false; // true if an iteration makes improvement
      do
        {
 	        // find delta
	        VectorXd delta_a(a.size());

			MatrixXd Id(J.transpose()*J);
	        Id.setIdentity();
			delta_a = (J.transpose()*J + lambda*Id).jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(J.transpose()*epsilon);

            // update parameters
            VectorXd a2(a + delta_a);

            // compute new error vector
            VectorXd epsilon2(X-f(a2,b,lut));

            // compare error
            if(levenbergMarquardtCheckImprovement(epsilon, epsilon2) == true) // if better
            {
              lambda /= 10.0;
              a = a2;
              accepted = true;
            }
            else
            {
              lambda *= 10.0;
              accepted = false;
            }

            // counter
            counter++;
            if(counter > 100)return 1;

        }while(accepted == false); // end of an iteration

     } // end of all iteration

std::cout << "iter = " << nbMaxIteration << std::endl;

  return 0;
}




///////////////////////////////////////////////////////////////////////
/// This function computes the jacobian of 'a'.
/// This function is not optimized (a lot of 0 are computed for nothing) but can be adapted to every partitioned data.
/// This algorithm is detailed in the "Multiple View Geometry", Hartley & Zisserman, 2nd edition, page 602-605.
void levenbergMarquardtJacobian_A(const VectorXd &a,
			                      const VectorXd &b,
			                      MatrixXd &jacobian_A,
			                      VectorXd (*f)(const VectorXd&,const VectorXd&, LUT3D& lut),
			                      LUT3D& lut)
{
  VectorXd aTmp(a);
  VectorXd Xcurrent = f(a,b,lut);

  // can be computed column per column
  std::cout << a.size() << std::endl;
  for(unsigned int j=0; j<a.size(); ++j)
    {
      double delta = std::min(std::fabs(a[j])*1.0e-1,1.0); // cf multiple view geometry, 2nd ed., page 602
//      double delta = .01;
//std::cout << "delta: " << delta << std::endl;
      delta = std::max(delta,(double) 1.e-1);

      //double delta = std::min(std::fabs(a[j])*1.0e-3,1.0e-5); // cf multiple view geometry, 2nd ed., page 602
      //delta = std::max(delta,1.0e-13);

      aTmp = a;
      aTmp[j] += delta;               // P'[j] = P[j] + delta

      VectorXd Xtmp = f(aTmp,b,lut);  // f(P')
      Xtmp = (Xtmp - Xcurrent) / delta;     // for every line i : dX_i/da_j

      //jacobian_A.setColumn(j,Xtmp);
      for(int i(0); i<Xtmp.rows(); ++i)
		jacobian_A(i,j) = Xtmp(i);
    }
}

void levenbergMarquardtJacobian_A(const VectorXd &a,
			                      const VectorXd &b,
			                      MatrixXd &jacobian_A,
			                      VectorXd (*f)(const VectorXd&,const VectorXd&, ChebyLUT_LAB& lut),
			                      ChebyLUT_LAB& lut)
{
  VectorXd aTmp(a);
  VectorXd Xcurrent = f(a,b,lut);

  // can be computed column per column
  std::cout << a.size() << std::endl;
  for(unsigned int j=0; j<a.size(); ++j)
    {
      //double delta = std::min(std::fabs(a[j])*1.0e-4,1.0e-6); // cf multiple view geometry, 2nd ed., page 602
      double delta = .001;
      delta = std::max(delta,(double) 1.e-13);

      //double delta = std::min(std::fabs(a[j])*1.0e-3,1.0e-5); // cf multiple view geometry, 2nd ed., page 602
      //delta = std::max(delta,1.0e-13);

      aTmp = a;
      aTmp[j] += delta;               // P'[j] = P[j] + delta

      VectorXd Xtmp = f(aTmp,b,lut);  // f(P')
      Xtmp = (Xtmp - Xcurrent) / delta;     // for every line i : dX_i/da_j

      //jacobian_A.setColumn(j,Xtmp);
      for(int i(0); i<Xtmp.rows(); ++i)
		jacobian_A(i,j) = Xtmp(i);
    }
}

///////////////////////////////////////////////////////////////////////
/// return true if the error has decreased, false if the error has increased
bool levenbergMarquardtCheckImprovement(const VectorXd &epsilonBefore, const VectorXd &epsilonAfter)
{
  double sumBefore = 0.0;
  double sumAfter  = 0.0;

  for(unsigned int i=0; i<epsilonBefore.size(); ++i)
   {
     sumBefore += epsilonBefore[i] * epsilonBefore[i];
     sumAfter  += epsilonAfter[i]  * epsilonAfter[i];
   }

  return sumBefore > sumAfter;
}

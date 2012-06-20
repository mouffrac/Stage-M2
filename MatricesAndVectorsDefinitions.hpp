#ifndef _MATRICES_AND_VECTORS_DEFINITIONS_H_
#define _MATRICES_AND_VECTORS_DEFINITIONS_H_

#include <Eigen/Dense>

typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> MatrixXd;
typedef Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic> MatrixXf;
typedef Eigen::Matrix<double, Eigen::Dynamic, 1> VectorXd;
typedef Eigen::Matrix<float, Eigen::Dynamic, 1> VectorXf;
typedef Eigen::Matrix<double, 3, 1> Vector3d;
typedef Eigen::Matrix<float, 3, 1> Vector3f;


#endif //_MATRICES_AND_VECTORS_DEFINITIONS_H_

#ifndef _RANSAC_EPI_HPP_
#define _RANSAC_EPI_HPP_

#include "MatricesAndVectorsDefinitions.hpp"
#include <vector>
#include <OpenKN/vision/EpipolarGeometry.hpp>
#include <OpenKN/math/Vector.hpp>
#include <OpenKN/math/Matrix3x3.hpp>

void ransacEpipolar(std::vector<kn::Vector3d> &myList1,
                    std::vector<kn::Vector3d> &myList2,
                    const double minDistance,
                    const unsigned int nbIterations);

#endif //_RANSAC_EPI_HPP_

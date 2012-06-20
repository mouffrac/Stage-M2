
#include "RANSAC_epipolar.hpp"

void ransacEpipolar(std::vector<kn::Vector3d> &myList1,
                    std::vector<kn::Vector3d> &myList2,
                    const double minDistance,
                    const unsigned int nbIterations)
{
  // init
  srandom(0);
  unsigned int bestNbInliers = 0;
  std::vector<kn::Vector3d> finalInlierList1, finalInlierList2;

  // ransac iterations
  for(unsigned int iteration=0; iteration<nbIterations; iteration++)
  {
    // choose randomly 8 correspondance points from the correspondance set
    std::vector< std::pair< kn::Vector3d, kn::Vector3d > > modelList;
    for(unsigned int i=0; i<8; i++) // at least 8 points for epipolar geometry
    {
      // ON PEUT FAIRE MIEUX ICI POUR EVITER LES DOUBLETS
      unsigned int number = random()%myList1.size();
      modelList.push_back( std::pair< kn::Vector3d, kn::Vector3d >(myList1[number],myList2[number]));
    }

    // compute the relative homography
    kn::Matrix3x3d F = kn::computeFundamentalMatrixNormalized(modelList);

    // compute inliers
    unsigned int currentNbInliers = 0;
    std::vector<kn::Vector3d> currentInlierList1, currentInlierList2;
    for(unsigned int i=0; i<myList1.size(); i++)
    {
        // compute the epipolar line
        kn::Vector3d line = F*myList1[i];
        line.setHomogeneousNormalForm();

        // points in homogeneous coordinates
        kn::Vector3d secondPt(myList2[i]);
        secondPt.setHomogeneousNormalForm();

        // compute a distance
        double denomiator = std::sqrt(line[0]*line[0] + line[1]*line[1]);
        double distance = std::fabs((line[0]*secondPt[0] + line[1]*secondPt[1] + line[2]) / denomiator);

        // update the current inlier list
        if(distance < minDistance)
        {
          currentInlierList1.push_back(myList1[i]);
          currentInlierList2.push_back(myList2[i]);
          currentNbInliers++;
        }
    }

    // if the list contains more inliers than with the previsous iterations, we select this list
    if(currentNbInliers > bestNbInliers)
    {
       bestNbInliers = currentNbInliers;
       finalInlierList1.clear();
       finalInlierList2.clear();
       finalInlierList1 = currentInlierList1;
       finalInlierList2 = currentInlierList2;
    }
  }

  // update the input data
  myList1.clear();
  myList1 = finalInlierList1;
  myList2.clear();
  myList2 = finalInlierList2;
}

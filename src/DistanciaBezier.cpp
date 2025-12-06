#ifndef DISTANCIABEZIERCUBICA
#define DISTANCIABEZIERCUBICA


#include <cstdio>
#include <glm/vec4.hpp>
#include <numeric>
#include <set>
#include "matrices.h"
#include "mathUtils.cpp"

double distanciaBezierCubica(glm::vec4 p1, glm::vec4 v1, glm::vec4 p2, glm::vec4 v2, glm::vec4 origin){

    std::array<double, 4> curvax, curvay, curvaz;

    const double pascal[4][4] = {1.0, -3.0, 3.0, -1.0,
                          0.0, 3.0, -6.0, 3.0,
                          0.0, 0.0, 3.0, -3.0,
                          0.0, 0.0, 0.0, 1.0};

    curvax[0] = static_cast<double>(p1.x) - static_cast<double>(origin.x);
    curvax[3] = static_cast<double>(p2.x) - static_cast<double>(origin.x);
    curvax[1] = curvax[0] + (static_cast<double>(v1.x) / 3.0);
    curvax[2] = curvax[3] - (static_cast<double>(v2.x) / 3.0);

    curvay[0] = static_cast<double>(p1.y) - static_cast<double>(origin.y);
    curvay[3] = static_cast<double>(p2.y) - static_cast<double>(origin.y);
    curvay[1] = curvay[0] + (static_cast<double>(v1.y) / 3.0);
    curvay[2] = curvay[3] - (static_cast<double>(v2.y) / 3.0);

    curvaz[0] = static_cast<double>(p1.z) - static_cast<double>(origin.z);
    curvaz[3] = static_cast<double>(p2.z) - static_cast<double>(origin.z);
    curvaz[1] = curvaz[0] + (static_cast<double>(v1.z) / 3.0);
    curvaz[2] = curvaz[3] - (static_cast<double>(v2.z) / 3.0);

    std::vector<double> px(4, 0.0), py(4, 0.0), pz(4, 0.0);

    for(long long i = 0; i < 4; i++){

        for(long long j = 0; j < 4; j++){
            px[j] += curvax[i] * pascal[i][j];
            py[j] += curvay[i] * pascal[i][j];
            pz[j] += curvaz[i] * pascal[i][j];
        }

    }

    px = multiplyPolinomials(px, px);
    py = multiplyPolinomials(py, py);
    pz = multiplyPolinomials(pz, pz);

    std::vector<double> distanciaQuadrada(7);

    for(long long i = 0; i < 7; i++){

      distanciaQuadrada[i] = px[i] + py[i] + pz[i];

    }

    std::vector<double> derivada = deriva(distanciaQuadrada);

    std::multiset<double> rootsd = raizes(derivada);

    double minDist2 = std::min(evaluatePolynomial(distanciaQuadrada, 0.0), evaluatePolynomial(distanciaQuadrada, 1.0));

    for(double r : rootsd) if(r >= 0.0 && r <= 1.0) minDist2 = std::min(evaluatePolynomial(distanciaQuadrada, r), minDist2);

    if(minDist2 < 0.0) {printf("ERRO: Distancia menor que zero -> %f\n", minDist2); fflush(stdout); minDist2 = 0.0;}

    return std::sqrt(minDist2);

}

#endif // DISTANCIABEZIERCUBICA

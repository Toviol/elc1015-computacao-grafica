#ifndef COLLISION_UTILS_H_INCLUDED
#define COLLISION_UTILS_H_INCLUDED

#include "Vector2.h"
#include <vector>
#include <limits>    // Para std::numeric_limits
#include <algorithm> // Para std::min/max
#include <cmath>     // Para fabs

namespace CollisionUtils
{
    // Estrutura para armazenar o resultado da projeção de uma forma em um eixo
    struct Projection
    {
        float min;
        float max;
    };

    // Projeta um polígono (lista de vértices) em um eixo
    Projection projectPolygon(const Vector2 &axis, const std::vector<Vector2> &vertices);

    // Projeta um círculo em um eixo
    Projection projectCircle(const Vector2 &axis, const Vector2 &center, float radius);

    // Verifica se duas projeções (intervalos 1D) se sobrepõem
    bool doProjectionsOverlap(const Projection &p1, const Projection &p2);

    // Obtém os eixos de projeção (normais às arestas) de um polígono
    std::vector<Vector2> getPolygonAxes(const std::vector<Vector2> &vertices);

    // Verifica a colisão entre um polígono e um círculo usando o Teorema do Eixo Separador (SAT)
    bool checkPolygonCircleCollision(const std::vector<Vector2> &polygonVertices, const Vector2 &circleCenter, float circleRadius);

    // Verifica a colisão entre dois polígonos usando o Teorema do Eixo Separador (SAT)
    bool checkPolygonPolygonCollision(const std::vector<Vector2>& polygonAVertices, const std::vector<Vector2>& polygonBVertices);
} // namespace CollisionUtils

#endif // COLLISION_UTILS_H_INCLUDED
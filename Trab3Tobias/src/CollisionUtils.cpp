// Este arquivo implementa as funções do namespace CollisionUtils,
// fornecendo a lógica para a detecção de colisão baseada no Teorema do Eixo Separador (SAT).
// As implementações detalham como projetar formas em eixos,
// como obter os eixos de um polígono e como verificar a sobreposição
// dessas projeções para determinar colisões entre polígonos e círculos.
#include "CollisionUtils.h"

namespace CollisionUtils
{

    // Projeta os vértices de um polígono sobre um determinado eixo e retorna o intervalo (min, max) da projeção.
    Projection projectPolygon(const Vector2 &axis, const std::vector<Vector2> &vertices)
    {
        if (vertices.empty())
        {
            return {0, 0};
        }
        float min = axis.dot(vertices[0]);
        float max = min;
        for (size_t i = 1; i < vertices.size(); ++i)
        {
            float p = axis.dot(vertices[i]);
            if (p < min)
            {
                min = p;
            }
            else if (p > max)
            {
                max = p;
            }
        }
        return {min, max};
    }

    // Projeta um círculo sobre um determinado eixo e retorna o intervalo (min, max) da projeção.
    Projection projectCircle(const Vector2 &axis, const Vector2 &center, float radius)
    {
        float centerProjection = axis.dot(center);
        return {centerProjection - radius, centerProjection + radius};
    }

    // Verifica se dois intervalos 1D (projeções) se sobrepõem.
    bool doProjectionsOverlap(const Projection &p1, const Projection &p2)
    {
        
        if (p1.max < p2.min || p2.max < p1.min)
        {
            return false;
        }
        return true;
    }

    // Calcula e retorna os vetores normais a cada aresta de um polígono.
    // Estes são os eixos potenciais de separação para o SAT.
    std::vector<Vector2> getPolygonAxes(const std::vector<Vector2> &vertices)
    {
        std::vector<Vector2> axes;
        if (vertices.size() < 2)
        {
            return axes;
        }
        for (size_t i = 0; i < vertices.size(); ++i)
        {
            Vector2 p1 = vertices[i];
            Vector2 p2 = vertices[(i + 1) % vertices.size()]; 
            Vector2 edge = p2 - p1;
            Vector2 normal = edge.perpendicular().normalized(); 
            axes.push_back(normal);
        }
        return axes;
    }

    // Verifica se um polígono e um círculo colidem usando o Teorema do Eixo Separador.
    // Testa os eixos normais do polígono e o eixo do centro do círculo ao vértice mais próximo do polígono.
    bool checkPolygonCircleCollision(const std::vector<Vector2> &polygonVertices, const Vector2 &circleCenter, float circleRadius)
    {
        if (polygonVertices.empty())
            return false;

        std::vector<Vector2> axes = getPolygonAxes(polygonVertices);

        
        Vector2 closestVertex = polygonVertices[0];
        float minDistSq = (circleCenter - closestVertex).lengthSquared();

        for (size_t i = 1; i < polygonVertices.size(); ++i)
        {
            float distSq = (circleCenter - polygonVertices[i]).lengthSquared();
            if (distSq < minDistSq)
            {
                minDistSq = distSq;
                closestVertex = polygonVertices[i];
            }
        }
        
        Vector2 circleToClosestVertexAxis = (closestVertex - circleCenter);
        if (circleToClosestVertexAxis.lengthSquared() > 0.0001f) 
        {
            axes.push_back(circleToClosestVertexAxis.normalized());
        } 

        
        for (const auto &axis : axes)
        {
            Projection polyProjection = projectPolygon(axis, polygonVertices);
            Projection circleProjection = projectCircle(axis, circleCenter, circleRadius);

            if (!doProjectionsOverlap(polyProjection, circleProjection))
            {
                return false; 
            }
        }

        return true; 
    }

    // Verifica se dois polígonos colidem usando o Teorema do Eixo Separador.
    // Testa os eixos normais de ambos os polígonos.
    bool checkPolygonPolygonCollision(const std::vector<Vector2>& polygonAVertices, const std::vector<Vector2>& polygonBVertices)
    {
        if (polygonAVertices.empty() || polygonBVertices.empty())
            return false;

        std::vector<Vector2> axesA = getPolygonAxes(polygonAVertices);
        std::vector<Vector2> axesB = getPolygonAxes(polygonBVertices);

        
        for (const auto& axis : axesA)
        {
            Projection pA = projectPolygon(axis, polygonAVertices);
            Projection pB = projectPolygon(axis, polygonBVertices);
            if (!doProjectionsOverlap(pA, pB))
            {
                return false; 
            }
        }

        
        for (const auto& axis : axesB)
        {
            Projection pA = projectPolygon(axis, polygonAVertices);
            Projection pB = projectPolygon(axis, polygonBVertices);
            if (!doProjectionsOverlap(pA, pB))
            {
                return false; 
            }
        }

        return true; 
    }
}
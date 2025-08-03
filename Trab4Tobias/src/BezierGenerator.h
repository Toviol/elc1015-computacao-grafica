#ifndef ___BEZIER_GENERATOR__H___
#define ___BEZIER_GENERATOR__H___

#include "Vector2.h"
#include <vector>
#include <cmath>

class BezierGenerator {
public:
    static long combinations(int n, int k) {
        if (k < 0 || k > n) return 0;
        if (k == 0 || k == n) return 1;
        if (k > n / 2) k = n - k;
        long res = 1;
        for (int i = 1; i <= k; ++i) 
            res = res * (n - i + 1) / i;
        return res;
    }

    static Vector2 calculateBezierPoint(float t, const std::vector<Vector2>& points) {
        int n = points.size() - 1;
        if (n < 0) return Vector2(0,0);
        
        Vector2 p(0.0f, 0.0f);
        for (int i = 0; i <= n; ++i) {
            double bernsteinCoeff = combinations(n, i) * std::pow(t, i) * std::pow(1.0f - t, n - i);
            p.x += points[i].x * (float)bernsteinCoeff;
            p.y += points[i].y * (float)bernsteinCoeff;
        }
        return p;
    }
    
    static std::vector<Vector2> generateBezierProfile(const std::vector<Vector2>& controlPoints, int numSamples) {
        std::vector<Vector2> bezierPoints;
        if (controlPoints.size() < 2) return bezierPoints;

        for (int i = 0; i <= numSamples; ++i) {
            float t = (float)i / numSamples;
            bezierPoints.push_back(calculateBezierPoint(t, controlPoints));
        }
        return bezierPoints;
    }
};

#endif // ___BEZIER_GENERATOR__H___
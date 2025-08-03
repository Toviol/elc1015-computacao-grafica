#ifndef ___SUN__H___
#define ___SUN__H___

#include "Vector3.h"
#include <GL/glut.h>
#include <cmath>

class Sun {
public:
    static void render(const Vector3& lightPosition, float deltaTime);

private:
    // Variável estática para rotação contínua
    static float sunRotation;
};

#endif // ___SUN__H___

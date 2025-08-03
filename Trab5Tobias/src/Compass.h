/*
DESCRIÇÃO:
Este arquivo contém a implementação da bússola de orientação 3D do HUD.
A bússola mostra a orientação atual da câmera através de vetores direcionais
coloridos (X=vermelho, Y=verde, Z=azul) em um círculo no canto superior direito.

FUNCIONALIDADES:
- Renderização de bússola circular com fundo semi-transparente
- Vetores direcionais baseados na orientação da câmera
- Labels coloridos para cada eixo (X, Y, Z)
- Título descritivo
- Posicionamento automático no canto superior direito

*/

#ifndef ___COMPASS__H___
#define ___COMPASS__H___

#include "Vector3.h"
#include <GL/glut.h>
#include <cmath>

class Compass {
public:
    static void render(int screenWidth, int screenHeight, 
                      const Vector3& cameraDirection, const Vector3& cameraUp);

private:
    static void renderText(int x, int y, const char* text, void* font);
};

#endif // ___COMPASS__H___

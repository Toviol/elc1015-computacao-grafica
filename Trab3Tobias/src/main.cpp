#include <GL/glut.h>
#include <GL/freeglut_ext.h> //callback da wheel do mouse.

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "gl_canvas2d.h"
#include "Frames.h"
#include "Tela.h"


float fps = 60;
int mouseX, mouseY, _key = -1; // _key initialized to -1
Frames frameCounter;
Tela *tela;
int isPressed = 0; // 0 = nenhum, 1 = esquerdo, 2 = direito

// largura e altura inicial da tela . Alteram com o redimensionamento de tela.
int screenWidth = 1280, screenHeight = 720;



void render()
{
    fps = frameCounter.getFrames(); // Atualiza o FPS
    tela->updateInputsAndState(fps, mouseX, mouseY, isPressed, _key); // Updated method call
    tela->render(); // Updated method call
}

// funcao chamada toda vez que uma tecla for pressionada.
void keyboard(int key)
{
    _key = key;
    printf("\nkey %d", key);
}

// funcao chamada toda vez que uma tecla for liberada
void keyboardUp(int key)
{
    if (_key == key)
    {
        _key = -1;
    }
    
}

// funcao para tratamento de mouse: cliques, movimentos e arrastos
void mouse(int button, int state, int wheel, int direction, int x, int y)
{

    mouseX = x; // guarda as coordenadas do mouse para exibir dentro da render()
    mouseY = y;

    //printf("\nmouse %d %d %d %d %d %d", button, state, wheel, direction, x, y);
    if (state == 0)
    {
        if (button == 0)
        {
            //Esquerdo
            isPressed = 1;
        }
        else if (button == 2)
        {
            //Direito
            isPressed = 2;
        }
        else
        {
            //Botao desconhecido
        }
    }
    if (state == 1)
    {
        isPressed = 0;
    }
    
}

int main(void)
{
    tela = new Tela(screenWidth, screenHeight);

    CV::run();
}

#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "Tela.h"
#include "Frames.h"


float fps = 60;
Frames frameController;  // Controlador de FPS usando a classe Frames
int mouseX, mouseY, _key = -1;
Tela *tela;
int isPressed = 0; // 0 = nenhum, 1 = esquerdo, 2 = direito

// largura e altura inicial da tela
int screenWidth = 1280, screenHeight = 720;

void display()
{
    // Calcula FPS real usando a classe Frames
    fps = frameController.getFrames();
    
    if (tela) {
        tela->updateInputsAndState(fps, mouseX, mouseY, isPressed, _key);
        tela->render();
    }
    glutSwapBuffers();
}

void reshape(int w, int h)
{
    screenWidth = w;
    screenHeight = h;
    glViewport(0, 0, w, h);
}

void keyboard(unsigned char key, int x, int y)
{
    _key = key;
}

void keyboardUp(unsigned char key, int x, int y)
{
    if (_key == key) {
        _key = -1;
    }
}

void specialKeys(int key, int x, int y)
{
    _key = key + 100; // Offset para teclas especiais
}

void specialKeysUp(int key, int x, int y)
{
    if (_key == key + 100) {
        _key = -1;
    }
}

void mouse(int button, int state, int x, int y)
{
    mouseX = x;
    mouseY = y;

    if (state == GLUT_DOWN) {
        if (button == GLUT_LEFT_BUTTON) {
            isPressed = 1;
        } else if (button == GLUT_RIGHT_BUTTON) {
            isPressed = 2;
        }
    } else if (state == GLUT_UP) {
        isPressed = 0;
    }
}

void motion(int x, int y)
{
    mouseX = x;
    mouseY = y;
}

void idle()
{
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    

    // Inicializa GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(screenWidth, screenHeight);
    glutCreateWindow("Space Flight Simulator");

    // Configura callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(specialKeys);
    glutSpecialUpFunc(specialKeysUp);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutPassiveMotionFunc(motion);
    glutIdleFunc(idle);

    // Cria instância da tela
    tela = new Tela(screenWidth, screenHeight);

    // Configurações iniciais OpenGL
    glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    // Inicia loop principal
    glutMainLoop();

    return 0;
}

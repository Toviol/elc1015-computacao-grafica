/**
*   Programa para ilustrar:
*   - Uso da Material
*   - Gouraud Shading

*   Autor: Cesar Tadeu Pozzer
*   UFSM - 23/07/2024
*
*   pozzer@inf.ufsm.br
*   pozzer3@gmail.com
*
**/

#include "GL/glut.h"
#include <math.h>
#include <stdio.h>

GLfloat mat_diffuse_1[] = {1, 0 ,0}; //definicao do material para esfera 1
GLfloat mat_diffuse_2[] = {0, 1 ,0}; //definicao do material para esfera 2

GLfloat light_0_position[] = { 6, 5, 0, 0};
GLfloat light_0_difuse[]   = { 1, 1, 1 };  //luz branca
GLfloat light_0_ambient_on[]  = { 0.4, 0.4, 0.4 }; //branco
GLfloat light_0_ambient_off[]  = { 0, 0, 0 }; //sem luz ambiente

bool luzAmbiente = true;


void init(void)
{
   glClearColor (0.0, 0.0, 0.0, 0.0);

   glShadeModel (GL_SMOOTH);

   //seta os parametros fixos da luz. A posicao eh atualizada a cada frame.
   glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_0_difuse);
   glLightfv(GL_LIGHT0, GL_AMBIENT,  light_0_ambient_on);
   glLightfv(GL_LIGHT0, GL_POSITION, light_0_position);


   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_DEPTH_TEST);
}

void display(void)
{
   Sleep(3);
   glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   //ATENCAO ATENCAO ATENCAO
   //**********************************************************
   glColor3f(0,1,1); //Cores nao fazem nada quando se usa iluminacao. Soh o que interessa eh o material.


   glLoadIdentity();

   //esfera 1
   glPushMatrix();
   glTranslated(4,0,0);
   glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  mat_diffuse_1);
   glutSolidSphere (3.0, 20, 16);
   glPopMatrix();


   //esfera 2
   glPushMatrix();
   glTranslated(-4,0,0);
   glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  mat_diffuse_2);
   glutSolidSphere (3.0, 20, 16);
   glPopMatrix();


   glutSwapBuffers();
}

void reshape (int w, int h)
{
   glViewport (0, 0, (GLsizei) w, (GLsizei) h);
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity();
   glOrtho (-10, 10,  -10,10,  -10.0, 10.0);  //projecao ortografica

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y)
{
   if( key == 'w')
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   else
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if( key == 'a')
    {
        if( luzAmbiente == true )
        {
            luzAmbiente = false;
            glLightfv(GL_LIGHT0, GL_AMBIENT,  light_0_ambient_off);

        }
        else
        {
            luzAmbiente = true;
            glLightfv(GL_LIGHT0, GL_AMBIENT,  light_0_ambient_on);
        }
    }

}

int main(int argc, char** argv)
{
   printf("Digite: w, a");
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize (600, 600);
   glutInitWindowPosition (100, 100);
   glutCreateWindow("Iluminação e Materiais. Digite: w, a");
   init ();
   glutDisplayFunc(display);
   glutKeyboardFunc(keyboard);
   glutIdleFunc(display);
   glutReshapeFunc(reshape);
   glutMainLoop();
   return 0;
}


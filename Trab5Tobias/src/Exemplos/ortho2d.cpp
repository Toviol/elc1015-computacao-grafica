/**
*   Programa para ilustrar:
*   - Projecao ortográfica 2D
*   - Gouraud Shading

*   Autor: Cesar Tadeu Pozzer
*   UFSM - 15/06/2007
*
*   pozzer@inf.ufsm.br
*   pozzer3@gmail.com
*
**/

#include <GL/glut.h>
//#include <GL/freeglut_ext.h> //callback da wheel do mouse.

int cont = 0;

void init()
{
   glClearColor(1,1,1,1);
   glShadeModel(GL_SMOOTH);
}

void ortho()
{
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity ();

   if( cont<=100 )
      gluOrtho2D (0, 400, 0, 400);
   else if( cont<=200 )
      gluOrtho2D (0, 200, 0, 400);
   else if( cont<=300 )
      gluOrtho2D (0, 800, 0, 400);
   else if( cont<=400 )
      gluOrtho2D (0, 400, 400, 0);

   if( cont==500 )
      cont=0;

   glMatrixMode(GL_MODELVIEW);
}

void display (void)
{
   glClear(GL_COLOR_BUFFER_BIT );

   ortho();

   glBegin(GL_TRIANGLES);
      glColor3f(1,0,0);
      glVertex2f(0, 0);
      glColor3f(0,1,0);
      glVertex2f(400, 0);
      glColor3f(0,0,1);
      glVertex2f(200, 400);
   glEnd();

   cont++;
   glutSwapBuffers();
}

int main (int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize (400, 400);
	glutInitWindowPosition (250, 100);
	glutCreateWindow ("Teste de ViewPort" );
	init();
	glutDisplayFunc(display);
	glutIdleFunc(display);
	glutMainLoop();
	return 0;
}

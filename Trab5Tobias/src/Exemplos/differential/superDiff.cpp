////////////////////////////////////////////
////////////////////////////////////////////

// Programa gráfico 3D desenvolvido em OpenGL para simular o diferencial de um veículo.
// Possui vários recursos de interação que permitem compreender o funcionamento desta
// magnífica invenção humana.O codigo nao esta nada legivel.
// ©2005 - Cesar Tadeu Pozzer

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/glut.h>
//#include <GL/freeglut_ext.h> //callback da wheel do mouse.

#include <math.h>
#include <stdio.h>
#include <ctype.h>

#include "cilindro.h"
#include "gear.h"


void display (void);
static void CreateLight(void);
static void CreateMaterial(void);


//definicao das velocidades de rotacao e translacao
double VEL_COROA    = 0.2;
double VEL_PINHAO   = -VEL_COROA*(30.0/11.0);//-1.363636
double VEL_SATELITE = VEL_COROA;


//indicam se o item deve ou nao girar
bool gearLeft=true, gearRight=true, motor=true;
bool wireFrameMode = false;

cilindro *eixo1, *eixo2, *eixoCoroa, *eixoPinhao;
gear     *satelite1, *satelite2, *g2, *g1, *coroa, *pinhao;
float    mousex, mousey;

void init()
{
	 g2 = new gear();
	 g2->setNumDentes(20);
	 g2->setAnguloDentes(0);
	 g2->setRaios_1(2, 9, 10);
	 g2->setRaios_2(2, 4, 5);
	 g2->setLarguras_1(3, 10);
	 g2->setLarguras_2(3, 10);
	 g2->setLargura(5,5);
	 g2->setEixoRotacao(0, 0, 1);
	 g2->setRotacao(0, 0, 9);
	 g2->setVelocidRotacao(VEL_SATELITE);
	 g2->setPosicao(0, 0, 7);
	 g2->Gera(2);

	 g1 = new gear();
	 g1->setNumDentes(20);
	 g1->setAnguloDentes(0);
	 g1->setRaios_1(2, 4, 5);
	 g1->setRaios_2(2, 9, 10);
	 g1->setLarguras_1(3, 10);
	 g1->setLarguras_2(3, 10);
	 g1->setLargura(5,5);
	 g1->setEixoRotacao(0, 0, 1);
	 g1->setRotacao(0, 0, 9);
	 g1->setVelocidRotacao(VEL_SATELITE);
	 g1->setPosicao(0, 0, -7);
	 g1->Gera(2);

    satelite1 = new gear();
	 satelite1->setNumDentes(20);
	 satelite1->setAnguloDentes(0);
	 satelite1->setRaios_1(2, 4, 5);
	 satelite1->setRaios_2(2, 9, 10);
	 satelite1->setLarguras_1(3, 10);
	 satelite1->setLarguras_2(3, 10);
	 satelite1->setLargura(5,5);
	 satelite1->setEixoRotacao(0, 0, 1);
	 satelite1->setEixoTranslacao(0, 0, 1);
	 satelite1->setRotacao(90, 0, 0);
	 satelite1->setVelocidRotacao(0);
	 satelite1->setVelocidTranslacao(VEL_COROA);
	 satelite1->setPosicao(0, 7, 0);
	 satelite1->Gera(1);

	 satelite2 = new gear();
	 satelite2->setNumDentes(20);
	 satelite2->setAnguloDentes(0);
	 satelite2->setRaios_1(2, 9, 10);
	 satelite2->setRaios_2(2, 4, 5);
	 satelite2->setLarguras_1(3, 10);
	 satelite2->setLarguras_2(3, 10);
	 satelite2->setLargura(5,5);
	 satelite2->setEixoRotacao(0, 0, 1);
	 satelite2->setEixoTranslacao(0, 0, 1);
	 satelite2->setRotacao(90, 0, 0);
	 satelite2->setVelocidRotacao(0);
	 satelite2->setVelocidTranslacao(VEL_COROA);
	 satelite2->setPosicao(0, -7, 0);
	 satelite2->Gera(3);

	 coroa = new gear();
	 coroa->setNumDentes(30);
	 coroa->setAnguloDentes(-0.125);
	 coroa->setRaios_1(14, 17, 19);
	 coroa->setRaios_2(14, 15, 16);
	 coroa->setLarguras_1(3, 10);
	 coroa->setLarguras_2(3, 10);
	 coroa->setLargura(3,3);
	 coroa->setEixoRotacao(0, 0, 1);
	 coroa->setRotacao(0, 0, 0);
	 coroa->setVelocidRotacao(VEL_COROA);
	 coroa->setPosicao(0, 0, 0);
	 coroa->Gera(5);

	 pinhao = new gear();
	 pinhao->setNumDentes(11);
	 pinhao->setAnguloDentes(0.3);
	 pinhao->setRaios_1(2, 6, 8);
	 pinhao->setRaios_2(2, 4, 5);
	 pinhao->setLarguras_1(3, 10);
	 pinhao->setLarguras_2(3, 10);
	 pinhao->setLargura(3,3);
	 pinhao->setEixoRotacao(0, 0, 1);
	 pinhao->setRotacao(0, -90, 0);
	 pinhao->setVelocidRotacao(VEL_PINHAO);
    pinhao->setEixoTranslacao(0, 0, 1);
	 pinhao->setVelocidTranslacao(VEL_COROA);
    pinhao->setPosicao(-17, 0, -6);
    pinhao->setRotacaoInicial(15);
	 pinhao->Gera(6);

	 eixoPinhao = new cilindro(); //preso no pinhao
	 eixoPinhao->setResolucao(17,10);
	 eixoPinhao->setRaios_1(0.7, 1.5);
	 eixoPinhao->setRaios_2(0.7, 1.5);
	 eixoPinhao->setLargura(9);
	 eixoPinhao->setEixoRotacao(0, 0, 1);
	 eixoPinhao->setRotacao(0, -90, 0);
	 eixoPinhao->setVelocidRotacao(VEL_COROA);
    eixoPinhao->setEixoTranslacao(0, 0, 1);
	 eixoPinhao->setPosicao(-21, 0, -6);
	 eixoPinhao->Gera(1);

	 eixoCoroa = new cilindro(); //preso na coroa
	 eixoCoroa->setResolucao(17,10);
	 eixoCoroa->setRaios_1(0.7, 1.5);
	 eixoCoroa->setRaios_2(0.7, 1.5);
	 eixoCoroa->setLargura(29);
	 eixoCoroa->setEixoRotacao(1, 0, 0);
	 eixoCoroa->setRotacao(90, 0, 90);
	 eixoCoroa->setVelocidRotacao(VEL_COROA);
	 eixoCoroa->setPosicao(0, 0, 0);
	 eixoCoroa->Gera(1);

	 eixo1 = new cilindro();
	 eixo1->setResolucao(4,4);
	 eixo1->setRaios_1(1.1, 2.0);
	 eixo1->setRaios_2(1.1, 2.0);
	 eixo1->setLargura(18);
	 eixo1->setEixoRotacao(0, 0, 1);
	 eixo1->setRotacao(0, 0, 90);
	 eixo1->setVelocidRotacao(VEL_SATELITE);
	 eixo1->setPosicao(0, 0, -14);
	 eixo1->Gera(3);

    eixo2 = new cilindro();
	 eixo2->setResolucao(4,4);
	 eixo2->setRaios_1(1.1, 2.0);
	 eixo2->setRaios_2(1.1, 2.0);
	 eixo2->setLargura(18);
	 eixo2->setEixoRotacao(0, 0, 1);
	 eixo2->setRotacao(0, 0, 90);
	 eixo2->setVelocidRotacao(-VEL_SATELITE);
	 eixo2->setPosicao(0, 0, 14);
	 eixo2->Gera(2);

	 glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	 CreateMaterial();
	 CreateLight();

  	 glClearColor(0,0,0,1);
  	 //glClearColor(1,1,1,1);
	 glEnable(GL_DEPTH_TEST);

	 /* enable lighting */
	 glEnable(GL_LIGHTING);
	 glEnable(GL_LIGHT0);    /* representing the sun light */
}


////////////////////////////////////////////////////////////////////////////////////////
void display (void)
{
	 /* clear buffers */
	 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	 /* set projection */
	 glMatrixMode(GL_PROJECTION);
	 glLoadIdentity( );
	 gluPerspective(65,1,10,1000);

	 /* position camera */
	 glMatrixMode(GL_MODELVIEW);
	 glLoadIdentity( );
	 gluLookAt(0,0,40, 0,0,0, 0,1,0);

	 glColor3f(0.5, 0.1, 0.9);

 	 glRotatef(mousex*300, 0.0f, 1.0f, 0.0f);
	 glRotatef(mousey*300, 1.0f, 0.0f, 0.0f);

    pinhao->setVelocidRotacao(VEL_PINHAO);
	 pinhao->setVelocidTranslacao(0);
    eixoPinhao->setVelocidRotacao(VEL_PINHAO);
    eixoPinhao->setVelocidTranslacao(0);

	 coroa->setVelocidRotacao(VEL_COROA);
    eixoCoroa->setVelocidRotacao(VEL_COROA);

    satelite1->setVelocidTranslacao(VEL_COROA);
	 satelite2->setVelocidTranslacao(VEL_COROA);

    if( motor == true && gearLeft == true && gearRight == true)
    {
	    g1->setVelocidRotacao(VEL_SATELITE);
	    g2->setVelocidRotacao(VEL_SATELITE);
	    eixo1->setVelocidRotacao(VEL_SATELITE);
	    eixo2->setVelocidRotacao(VEL_SATELITE);

       satelite1->setVelocidRotacao(0);
	    satelite2->setVelocidRotacao(0);
    }
    if( motor == true && gearLeft == true && gearRight == false)
    {
	    g1->setVelocidRotacao(0);
	    g2->setVelocidRotacao(2*VEL_SATELITE);
	    eixo1->setVelocidRotacao(0);
	    eixo2->setVelocidRotacao(2*VEL_SATELITE);

       satelite1->setVelocidRotacao(VEL_SATELITE);
	    satelite2->setVelocidRotacao(-VEL_SATELITE);
    }
    if( motor == true && gearLeft == false && gearRight == true)
    {
	    g1->setVelocidRotacao(2*VEL_SATELITE);
	    g2->setVelocidRotacao(0);
	    eixo1->setVelocidRotacao(2*VEL_SATELITE);
	    eixo2->setVelocidRotacao(0);

       satelite1->setVelocidRotacao(-VEL_SATELITE);
	    satelite2->setVelocidRotacao(VEL_SATELITE);
    }
    if( motor == true && gearLeft == false && gearRight == false)
    {
	    g1->setVelocidRotacao(0);
	    g2->setVelocidRotacao(0);
	    eixo1->setVelocidRotacao(0);
	    eixo2->setVelocidRotacao(0);

       satelite1->setVelocidRotacao(0);
	    satelite2->setVelocidRotacao(0);
	    satelite1->setVelocidTranslacao(0);
	    satelite2->setVelocidTranslacao(0);

	    coroa->setVelocidRotacao(0);
       pinhao->setVelocidTranslacao(-VEL_COROA);
       eixoPinhao->setVelocidTranslacao(-VEL_COROA);
   	 eixoCoroa->setVelocidRotacao(0);
    }
    if( motor == false )
    {
	    g1->setVelocidRotacao(VEL_SATELITE);
	    g2->setVelocidRotacao(-VEL_SATELITE);
	    eixo1->setVelocidRotacao(VEL_SATELITE);
	    eixo2->setVelocidRotacao(-VEL_SATELITE);

       satelite1->setVelocidRotacao(-VEL_SATELITE);
	    satelite2->setVelocidRotacao(VEL_SATELITE);

	    satelite1->setVelocidTranslacao(0);
	    satelite2->setVelocidTranslacao(0);

	    coroa->setVelocidRotacao(0);
	    pinhao->setVelocidRotacao(0);
   	 eixoCoroa->setVelocidRotacao(0);
       eixoPinhao->setVelocidRotacao(0);
   }

	 satelite1->draw();
	 satelite2->draw();
	 g2->draw();
	 g1->draw();
	 coroa->draw();
	 pinhao->draw();

	 eixoCoroa->draw();
    eixoPinhao->draw();
    eixo2->draw();
	 eixo1->draw();

	 glFlush();
	 glutSwapBuffers();
}


static void CreateLight(void)
{
   GLfloat pos[4], amb[4], dif[4], esp[4], shi[1];

   pos[0] = 10.0f;  pos[1] = 10.0f,  pos[2] = 0.0f;   pos[3] = 1.0f;
   amb[0] = 0.5f;   amb[1] = 0.5f,   amb[2] = 0.5f;   amb[3] = 1.0f;
   dif[0] = 0.5f;   dif[1] = 0.4f,   dif[2] = 0.4f;   dif[3] = 1.0f;
   esp[0] = 0.3f;   esp[1] = 0.3f,   esp[2] = 0.3f;   esp[3] = 1.0f;
	shi[0] = 10;

   glLightfv (GL_LIGHT0, GL_POSITION, pos);
	glLightfv (GL_LIGHT0, GL_AMBIENT,  amb);
	glLightfv (GL_LIGHT0, GL_DIFFUSE,  dif);
	glLightfv (GL_LIGHT0, GL_SPECULAR, esp);
	glLightfv (GL_LIGHT0, GL_SHININESS,  shi);
}

static void CreateMaterial(void)
{
  	float _mat_amb[4], _mat_dif[4], _mat_esp[4], _mat_shi[1];

    _mat_amb[0] = 0.5f;   _mat_amb[1] = 0.5f;   _mat_amb[2] = 0.5f;   _mat_amb[3] = 0.2f;
    _mat_dif[0] = 0.6f;   _mat_dif[1] = 0.6f;   _mat_dif[2] = 0.1f;   _mat_dif[3] = 0.2f;
    _mat_esp[0] = 0.4f;   _mat_esp[1] = 0.4f;   _mat_esp[2] = 0.4f;   _mat_esp[3] = 0.2f;
    _mat_shi[0] = 25.0f;

    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, _mat_amb);
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, _mat_dif);
	glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, _mat_esp);
	glMaterialfv( GL_FRONT_AND_BACK, GL_SHININESS, _mat_shi);
}


//funcao para leitura do mouse e rotacao de toda cena.
//void mouseFunc(int button, int state, int x, int y)
void mouseFunc(int x, int y)
{
	mousex = (x-300) / 300.0;
	mousey = (y-300) / 300.0;
}

void keyboard(unsigned char key, int x, int y)
{
	key = tolower(key);
	switch(key)
	{
		case 27:
			exit(0);
		break;
		case 'r':
         if( gearRight==true )
            gearRight = false;
         else
            gearRight = true;
		break;

      case 'l':
         if( gearLeft==true )
            gearLeft = false;
         else
            gearLeft = true;
		break;
      case 'e':
         if( motor==true )
            motor = false;
         else
            motor = true;
		break;
      case 'i':
         VEL_COROA+=0.1;
         VEL_PINHAO   = -VEL_COROA*(30.0/11.0);//-1.363636
         VEL_SATELITE = VEL_COROA;
		break;
      case 'd':
         VEL_COROA-=0.1;
         VEL_PINHAO   = -VEL_COROA*(30.0/11.0);//-1.363636
         VEL_SATELITE = VEL_COROA;
		break;

      case 'w':
         if( wireFrameMode==false)
         {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            wireFrameMode = true;
         }
         else
         {
      	   glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            wireFrameMode = false;
         }
		break;
   }
}

void menu(void)
{
   printf("               ***  SuperDIFF-3D  *** \n");
   printf("\n The Ultimate Differential 3D Simulating Interactive Program");
   printf("\n Developed by Cesar Tadeu Pozzer");
   printf("\n Contanct: pozzer@inf.puc-rio.br");
   printf("\n           pozzer@tecgraf.puc-rio.br");
   printf("\n           pozzer@icad.puc-rio.br");
   printf("\n           pozzer3@hotmail.com");
   printf("\n           http://www.icad.puc-rio.br/~pozzer");
   printf("\n All Right Reserved - 2003\n");
   printf("\n Ops: What is a differential? Differential is a set of gears that makes");
   printf("\n      it possible for a vehicle's wheels to turn at different speeds when");
   printf("\n      going round corners. If you couldn't understand, please enjoy this ");
   printf("\n      graphical program");
   printf("\n\n Usage: ");
   printf("\n L - Stop Left wheel");
   printf("\n R - Stop Rigth wheel");
   printf("\n E - Stop the Engine");
   printf("\n I - Increase engine speed");
   printf("\n D - Decrease engine speed");
   printf("\n W - Toggle wireframe mode");
   printf("\n Mouse - Change view angle");
}

////////////////////////////////////////////////////////////////////////////////////////
int main (int argc, char* argv[])
{
	/* openg GLUT */
	glutInit(&argc, argv);
	//
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize (700, 700);
	glutInitWindowPosition (150, 100);
	/* create window */
	glutCreateWindow ("superDIFF-3D");
	glutDisplayFunc(display);
	glutPassiveMotionFunc(mouseFunc);
	glutIdleFunc(display);
	glutKeyboardFunc(keyboard);

   init();
   menu();

	/* interact " */
	glutMainLoop();
	return 0;
}

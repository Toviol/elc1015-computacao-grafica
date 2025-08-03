#include <gl/glut.h> //gl utility toolkit
#include <gl/gl.h>  //gl utility
#include <math.h>
//#include "vetor.h"

//#include "cores.h"


class gear{
	int n_ele;
	int n_dentes;
	int id;
	double velocidade_rotacao, velocidadeTranslacao;
	double angulo_rotacao_atual, anguloTranslacao;
	double angulo_dentes;
	double raio_d1,   raio_m1, raio_f1; //raio de dentro, meio e fora do lado 1
	double raio_d2,   raio_m2, raio_f2; //raio de dentro, meio e fora do lado 2
	double larg_d1,   larg_f1; //largura do dente na base e topo da engrenagem
	double larg_d2,   larg_f2; //largura do dente na base e topo da engrenagem
	double largura_d, largura_f;    //largura da engrenagem
	vetor  coord_d1[1000]; //coordenada do eixo da engrenagem
	vetor  coord_d2[1000];
	vetor  coord_f1[1000]; //coordenada dos dentes da engrenagem
 	vetor  coord_f2[1000];
	double eixo_x,  eixo_y,  eixo_z;
	double rotac_x, rotac_y, rotac_z;
	double pos_x,   pos_y,   pos_z;
   double eixoTransl_x, eixoTransl_y, eixoTransl_z;

   //Cores  *c;
public:
	gear()
	{
		n_dentes = 5;
		angulo_dentes = 0.0;
		largura_d = largura_f = 10;
		eixo_x  = eixo_y  = eixo_z  = 0.0;
		rotac_x = rotac_y = rotac_z = 0.0; //angulo inicial de rotacao
		pos_x   = pos_y   = pos_z   = 0.0;
		raio_d1 = raio_m1 = raio_f1 = 4;
		raio_d2 = raio_m2 = raio_f2 = 4;
		angulo_rotacao_atual = 0.0;
      anguloTranslacao = 0.0;
		velocidade_rotacao   = 0.0;
      velocidadeTranslacao = 0.0;
      eixoTransl_x= eixoTransl_y = eixoTransl_z=0;
		//c = new Cores(1000);
	}

	void setNumDentes(int num)
	{
		if(num<3)
			n_dentes = 3;
		else
			n_dentes = num;
	}

   void setRotacaoInicial(double ang)
   {
      angulo_rotacao_atual = ang;
   }

	void setAnguloDentes(double ang)
	{
		angulo_dentes = ang;
	}

	void setRaios_1(int dentro, int meio, int fora)
	{
		raio_d1 = dentro;
		raio_m1 = meio;
		raio_f1 = fora;
	}

	void setRaios_2(int dentro, int meio, int fora)
	{
		raio_d2 = dentro;
		raio_m2 = meio;
		raio_f2 = fora;
	}

	void setLarguras_1(double base, double topo)
	{
		larg_d1 = base;
		larg_f1 = topo;
	}

	void setLargura(double larg_d, double larg_f)
	{
		largura_d = larg_d;
		largura_f = larg_f;
	}


	void setLarguras_2(double base, double topo)
	{
		larg_d2 = base;
		larg_f2 = topo;
	}

	void setEixoRotacao(double x, double y, double z)
	{
		eixo_x = x;
		eixo_y = y;
		eixo_z = z;
	}

   void setEixoTranslacao(double x, double y, double z)
   {
      eixoTransl_x = x;
      eixoTransl_y = y;
      eixoTransl_z = z;
   }

   void setRotacao(double x, double y, double z)
	{
		rotac_x = x;
		rotac_y = y;
		rotac_z = z;
	}

	void setVelocidRotacao(double velocidade)
	{
		velocidade_rotacao = velocidade;
	}

   void setVelocidTranslacao(double velocidade)
   {
      velocidadeTranslacao = velocidade;
   }

	void setPosicao(double x, double y, double z)
	{
		pos_x = x;
		pos_y = y;
		pos_z = z;
	}

	void Rotaciona(double ang)
	{

	}

	vetor CalculaPosicaoDente(double ang, double raio, double z)
	{
		vetor coord;
		coord.x = cos(ang)*raio;
		coord.y = sin(ang)*raio;
		coord.z = z;
		return coord;
	}

	//acha um ponto 3D central a uma face quadrada de 4 lados
	vetor PontoCentral(vetor vert1, vetor vert2)
	{
		vetor central;
		central.x = (vert1.x + vert2.x)/2;
		central.y = (vert1.y + vert2.y)/2;
		central.z = (vert1.z + vert2.z)/2;
		return central;
	}

	void Gera(int identificador)
	{
		int    face=0;
		double ang, inc;
		double inc_d1, inc_f1; //incremento de dentro e fora do lado 1
		double inc_d2, inc_f2; //incremento de dentro e fora do lado 2
		double z_d1, z_f1, z_d2, z_f2;

		id = identificador;
		inc = 2.0*PI/(double)n_dentes;

		inc_d1 = inc/larg_d1;
		inc_f1 = inc/larg_f1;
		inc_d2 = inc/larg_d2;
		inc_f2 = inc/larg_f2;

		z_d1 = largura_d/2;
		z_f1 = largura_f/2;
		z_d2 = -largura_d/2;
		z_f2 = -largura_f/2;

		face = 0;
		for(ang=0; ang< (2*PI-0.00001); ang+=inc)
		{
			coord_d1[face] = CalculaPosicaoDente(ang-inc_d1, raio_d1, z_d1);
			coord_f1[face] = CalculaPosicaoDente(ang-inc_d1, raio_m1, z_f1);
			coord_d2[face] = CalculaPosicaoDente(ang-inc_d2+angulo_dentes, raio_d2, z_d2);
			coord_f2[face] = CalculaPosicaoDente(ang-inc_d2+angulo_dentes, raio_m2, z_f2);
			face++;

			coord_d1[face] = CalculaPosicaoDente(ang-inc_f1, raio_d1, z_d1);
			coord_f1[face] = CalculaPosicaoDente(ang-inc_f1, raio_f1, z_f1);
			coord_d2[face] = CalculaPosicaoDente(ang-inc_f2+angulo_dentes, raio_d2, z_d2);
			coord_f2[face] = CalculaPosicaoDente(ang-inc_f2+angulo_dentes, raio_f2, z_f2);
			face++;


			coord_d1[face] = CalculaPosicaoDente(ang+inc_f1, raio_d1, z_d1);
			coord_f1[face] = CalculaPosicaoDente(ang+inc_f1, raio_f1, z_f1);
			coord_d2[face] = CalculaPosicaoDente(ang+inc_f2+angulo_dentes, raio_d2, z_d2);
			coord_f2[face] = CalculaPosicaoDente(ang+inc_f2+angulo_dentes, raio_f2, z_f2);
			face++;

			coord_d1[face] = CalculaPosicaoDente(ang+inc_d1, raio_d1, z_d1);
			coord_f1[face] = CalculaPosicaoDente(ang+inc_d1, raio_m1, z_f1);
			coord_d2[face] = CalculaPosicaoDente(ang+inc_d2+angulo_dentes, raio_d2, z_d2);
			coord_f2[face] = CalculaPosicaoDente(ang+inc_d2+angulo_dentes, raio_m2, z_f2);
			face++;
		}
		n_ele = face;
	}

	//funcao que simula uma lista circular
	int idx(int face)
	{
		if(face==n_ele)
			return 0;
		else
			return face;
	}

	void draw()
	{
		int face=0;
		vetor v0, v1, v2, normal, centro;
		double ang;

		//glShadeModel(GL_SMOOTH);
		//glShadeModel(GL_FLAT);
		 /* set projection */
		glPushMatrix( );

      anguloTranslacao += velocidadeTranslacao;
      if( eixoTransl_x > 0 || eixoTransl_y > 0 || eixoTransl_z > 0)
         glRotated(anguloTranslacao, eixoTransl_x, eixoTransl_y, eixoTransl_z);

		//posicao e rotacao original do objeto
		glTranslated(pos_x,pos_y, pos_z);

      glRotated(rotac_x, 1,0,0);
		glRotated(rotac_y, 0,1,0);
		glRotated(rotac_z, 0,0,1);

		//rotaciona o objeto em torno de seu eixo segundo sua velocidade
		ang = angulo_rotacao_atual+=velocidade_rotacao;
		glRotated(ang, eixo_x, eixo_y, eixo_z);

		//------------------------------------------------------------------
		//desenha os dentes da engrenagem
		//------------------------------------------------------------------
		glBegin(GL_QUAD_STRIP);
			for(face=0; face <= n_ele; face++)
			{
				//glColor3f(c->getCorR(face), c->getCorG(face), c->getCorB(face));
				//glColor3f(0.5,0.5,0.5);
				glVertex3d(coord_f1[idx(face)].x, coord_f1[idx(face)].y, coord_f1[idx(face)].z);
				glVertex3d(coord_f2[idx(face)].x, coord_f2[idx(face)].y, coord_f2[idx(face)].z);

				v0.setValor(coord_f1[idx(face)]);
				v1.setValor(coord_f2[idx(face)]);
				v2.setValor(coord_f1[idx(face+1)]);
				normal = (v1 - v0) ^ (v2 - v1);
				normal = normal.normalize();
				glNormal3f(normal.x, normal.y, normal.z);
			}
		glEnd();


		//------------------------------------------------------------------
		//desenha as normais
		//------------------------------------------------------------------
		for(face=0; face <= n_ele; face++)
		{
			v0.setValor(coord_f1[idx(face)]);
			v1.setValor(coord_f2[idx(face)]);
			v2.setValor(coord_f1[idx(face+1)]);
			normal = (v1 - v0) ^ (v2 - v1);
			normal = normal.normalize();
			normal = normal.extende(3.0);
			centro = PontoCentral(coord_f1[idx(face)], coord_f2[idx(face+1)]);

			glBegin(GL_LINES);
			//glVertex3d(centro.x,          centro.y,          centro.z);
			//glVertex3d(centro.x+normal.x, centro.y+normal.y, centro.z+normal.z);
			glEnd();
		}


		/**/
		//------------------------------------------------------------------
		//desenha os lados da engrenagem
		//------------------------------------------------------------------
		glBegin(GL_QUAD_STRIP);
			for(face=0; face <= n_ele; face++)
			{
				//glColor3f(c->getCorR(face), c->getCorG(face), c->getCorB(face));

				glVertex3d(coord_f1[idx(face)].x, coord_f1[idx(face)].y, coord_f1[idx(face)].z);
				glVertex3d(coord_d1[idx(face)].x, coord_d1[idx(face)].y, coord_d1[idx(face)].z);

				v0.setValor(coord_f1[idx(face)]);
				v2.setValor(coord_d1[idx(face)]);
				v1.setValor(coord_f1[idx(face+1)]);
				normal = (v1 - v0) ^ (v2 - v1);
				normal = normal.normalize();
				glNormal3f(normal.x, normal.y, normal.z);
			}
		glEnd();

		//------------------------------------------------------------------
		//desenha as normais dos lados da engrenagem
		//------------------------------------------------------------------
		for(face=0; face < n_ele; face++)
		{
			v0.setValor(coord_f1[face]);
			v2.setValor(coord_d1[face]);
			v1.setValor(coord_f1[face+1]);
			normal = (v1 - v0) ^ (v2 - v1);
			normal = normal.normalize();
			normal = normal.extende(3.0);
			centro = PontoCentral(coord_f1[face], coord_d1[face+1]);

			glBegin(GL_LINES);
			//glVertex3d(centro.x,          centro.y,          centro.z);
			//glVertex3d(centro.x+normal.x, centro.y+normal.y, centro.z+normal.z);
			glEnd();
		}
		/**/
		glBegin(GL_QUAD_STRIP);
			for(face=0; face <= n_ele; face++)
			{
				//Color3f(c->getCorR(face), c->getCorG(face+10), c->getCorB(face));
				//glColor3d(100,100,100);

				glVertex3d(coord_f2[idx(face)].x, coord_f2[idx(face)].y, coord_f2[idx(face)].z);
				glVertex3d(coord_d2[idx(face)].x, coord_d2[idx(face)].y, coord_d2[idx(face)].z);

				v0.setValor(coord_f2[idx(face)]);
				v1.setValor(coord_d2[idx(face)]);
				v2.setValor(coord_f2[idx(face+1)]);
				normal = (v1 - v0) ^ (v2 - v1);
				normal = normal.normalize();
				glNormal3f(normal.x, normal.y, normal.z);
			}
		glEnd();

		//------------------------------------------------------------------
		//desenha as normais dos lados da engrenagem
		//------------------------------------------------------------------
		for(face=0; face < n_ele; face++)
		{
			v0.setValor(coord_f2[face]);
			v1.setValor(coord_d2[face]);
			v2.setValor(coord_f2[face+1]);
			normal = (v1 - v0) ^ (v2 - v1);
			normal = normal.normalize();
			normal = normal.extende(3.0);
			centro = PontoCentral(coord_f2[face], coord_d2[face+1]);

			glBegin(GL_LINES);
			//glVertex3d(centro.x,          centro.y,          centro.z);
			//glVertex3d(centro.x+normal.x, centro.y+normal.y, centro.z+normal.z);
			glEnd();
		}


		//------------------------------------------------------------------
		//desenha o furo do eixo da engrenagem
		//------------------------------------------------------------------
		glBegin(GL_QUAD_STRIP);
			for(face=0; face <= n_ele; face++)
			{
				//glColor3f(c->getCorR(face), c->getCorG(face), c->getCorB(face+10));

				glVertex3d(coord_d1[idx(face)].x, coord_d1[idx(face)].y, coord_d1[idx(face)].z);
				glVertex3d(coord_d2[idx(face)].x, coord_d2[idx(face)].y, coord_d2[idx(face)].z);

				v0.setValor(coord_d1[idx(face)]);
				v2.setValor(coord_d2[idx(face)]);
				v1.setValor(coord_d1[idx(face+1)]);
				normal = (v1 - v0) ^ (v2 - v1);
				normal = normal.normalize();
				glNormal3f(normal.x, normal.y, normal.z);
			}
		glEnd();
		/**/
		glPopMatrix( );
	}
};


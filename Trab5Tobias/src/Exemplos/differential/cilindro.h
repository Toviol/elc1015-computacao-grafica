#include <gl/glut.h> //gl utility toolkit
#include <gl/gl.h>  //gl utility
#include <math.h>
#include "vetor.h"

#define PI   3.1415926535897932


class cilindro{
	int    n_lados, resolucao;
	int    id;
	int    tampa; //se e' tampado=1 ou oco=2
	double velocidade_rotacao, velocidadeTranslacao;
	double angulo_rotacao_atual, anguloTranslacao;  //posicao de rotacao
	double raio_d1, raio_f1; //raio de dentro e fora do lado 1
	double raio_d2, raio_f2; //raio de dentro e fora do lado 2
	vetor  coord_d1[100]; //coordenada do eixo da engrenagem
	vetor  coord_d2[100];
	vetor  coord_f1[100]; //coordenada dos dentes da engrenagem
 	vetor  coord_f2[100];
	double eixo_x, eixo_y, eixo_z;
	double rotac_x,rotac_y,rotac_z;
	double pos_x,  pos_y,  pos_z;
	double largura;
   double eixoTransl_x, eixoTransl_y, eixoTransl_z;

public:
	cilindro()
	{
		n_lados = resolucao = 4;
		eixo_x  = eixo_y  = eixo_z  = 0.0;
		rotac_x = rotac_y = rotac_z = 0.0; //angulo inicial de rotacao
		pos_x   = pos_y   = pos_z   = 0.0;
		velocidade_rotacao = 0;
      velocidadeTranslacao = 0.0;
		angulo_rotacao_atual = 0;
      anguloTranslacao = 0.0;
      eixoTransl_x= eixoTransl_y = eixoTransl_z=0;
	}

	void setResolucao(int lados, int resoluc)
	{
		if(lados<3 || lados>99)
			n_lados = 4;
		else
			n_lados = lados;
		if(resoluc<1)
			resolucao = 1;
		else
			resolucao = resoluc;
	}

	void setRaios_1(double dentro, double fora)
	{
		raio_d1 = dentro;
		raio_f1 = fora;
	}

	void setRaios_2(double dentro, double fora)
	{
		raio_d2 = dentro;
		raio_f2 = fora;
	}

	void setLargura(double larg)
	{
		largura = larg;
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

	void setVelocidRotacao(double velocidade)
	{
		velocidade_rotacao = velocidade;
	}

   void setVelocidTranslacao(double velocidade)
   {
      velocidadeTranslacao = velocidade;
   }

	void setRotacao(double x, double y, double z)
	{
		rotac_x = x;
		rotac_y = y;
		rotac_z = z;
	}

	void setPosicao(double x, double y, double z)
	{
		pos_x = x;
		pos_y = y;
		pos_z = z;
	}

	vetor CalculaPosicaoFace(double ang, double raio, double z)
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
		//double inc_d1, inc_f1; //incremento de dentro e fora do lado 1
		//double inc_d2, inc_f2; //incremento de dentro e fora do lado 2
		double z_1, z_2;

		id = identificador;
		inc = 2.0*PI/(double)n_lados;

		z_1 = largura/2;
		z_2 = -largura/2;

		face = 0;
		for(ang=0; ang<2*PI; ang+=inc)
		{
			coord_d1[face] = CalculaPosicaoFace(ang, raio_d1, z_1);
			coord_f1[face] = CalculaPosicaoFace(ang, raio_f1, z_1);
			coord_d2[face] = CalculaPosicaoFace(ang, raio_d2, z_2);
			coord_f2[face] = CalculaPosicaoFace(ang, raio_f2, z_2);
			face++;
		}
	}

	//funcao que simula uma lista circular
	int idx(int face)
	{
		if(face==n_lados)
			return 0;
		else
			return face;
	}

	void draw(void)
	{
		int face=0;
		vetor v0, v1, v2, normal, centro, aux1, aux2;
		double ang, inc_divisoes, z_ini, z_fim;

		//glShadeModel(GL_SMOOTH);
		glShadeModel(GL_FLAT);
		// set projection
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
		//desenha a face externa
		//------------------------------------------------------------------
		inc_divisoes = largura/(float)resolucao;
		z_fim = largura/2;
		for(face=0; face < n_lados; face++)
		{
			v0.setValor(coord_f1[idx(face)]);
			v1.setValor(coord_f2[idx(face)]);
			v2.setValor(coord_f1[idx(face+1)]);
			normal = (v1 - v0) ^ (v2 - v1);
			normal = normal.normalize();
			glNormal3f(normal.x, normal.y, normal.z);

			glBegin(GL_QUAD_STRIP);
			z_ini = -largura/2;
			while(z_ini <= z_fim+0.001)
			{
				glVertex3d(coord_f1[idx(face)].x,   coord_f1[idx(face)].y,   z_ini);
				glVertex3d(coord_f1[idx(face+1)].x, coord_f2[idx(face+1)].y, z_ini);
				z_ini+=inc_divisoes;
			}
			glEnd();
		}

		//normais do cilindro
		for(face=0; face < n_lados; face++)
		{
			v0.setValor(coord_f1[face]);
			v1.setValor(coord_f2[face]);
			v2.setValor(coord_f1[face+1]);
			normal = (v1 - v0) ^ (v2 - v1);
			normal = normal.normalize();
			normal = normal.extende(3.0);
			centro = PontoCentral(coord_f1[face], coord_f2[face+1]);

			glBegin(GL_LINES);
			//glVertex3d(centro.x,          centro.y,          centro.z);
			//glVertex3d(centro.x+normal.x, centro.y+normal.y, centro.z+normal.z);
			glEnd();
		}


		//------------------------------------------------------------------
		//desenha os lados do cilindro
		//------------------------------------------------------------------
		glBegin(GL_QUAD_STRIP);
			for(face=0; face <= n_lados; face++)
			{
				//glColor3f(c->getCorR(face), c->getCorG(face), c->getCorB(face));
				glColor3d(100,100,100);

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

		//normais do cilindro
		for(face=0; face < n_lados; face++)
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




		glBegin(GL_QUAD_STRIP);
			for(face=0; face <= n_lados; face++)
			{
				//Color3f(c->getCorR(face), c->getCorG(face+10), c->getCorB(face));
				glColor3d(100,100,100);

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

		//normais do cilindro
		for(face=0; face < n_lados; face++)
		{
			v0.setValor(coord_f2[face]);
			v1.setValor(coord_d2[face]);
			v2.setValor(coord_f2[face+1]);
			normal = (v1 - v0) ^ (v2 - v1);
			normal = normal.normalize();
			normal = normal.extende(3.0);
			centro = PontoCentral(coord_f2[idx(face)], coord_d2[idx(face+1)]);

			glBegin(GL_LINES);
			//glVertex3d(centro.x,          centro.y,          centro.z);
			//glVertex3d(centro.x+normal.x, centro.y+normal.y, centro.z+normal.z);
			glEnd();
		}

		//------------------------------------------------------------------
		//desenha a face de dentro do cilindro
		//------------------------------------------------------------------
		glBegin(GL_QUAD_STRIP);
			for(face=0; face <= n_lados; face++)
			{
				//glColor3f(c->getCorR(face), c->getCorG(face), c->getCorB(face+10));
				//glColor3f(0.9, 0.2, .9);

				glVertex3d(coord_d1[idx(face)].x, coord_d1[idx(face)].y, coord_d1[idx(face)].z);
				glVertex3d(coord_d2[idx(face)].x, coord_d2[idx(face)].y, coord_d2[idx(face)].z);

				v0.setValor(coord_d1[idx(face)]);
				v1.setValor(coord_d2[idx(face)]);
				v2.setValor(coord_d1[idx(face+1)]);
				normal = (v1 - v0) ^ (v2 - v1);
				normal = normal.normalize();
				glNormal3f(normal.x, normal.y, normal.z);
			}
		glEnd();

		glPopMatrix( );
		/**/
	}
};

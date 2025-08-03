#ifndef ___CAMADA__H___
#define ___CAMADA__H___

#include "gl_canvas2d.h"
#include "Bmp.h"

class Camada
{

    int **matrizDesenho;
    bool hasImage;
    int ativa;
    int visivel;
    Bmp *img;
    int imgX0, imgY0, imgWidth, imgHeight;
    int brilho;

    unsigned char *data;
    //char *imagePath;


    int x0, y0, x1, y1;
    int idCamada;
    int idOriginalCamada;

public:
    // Getters
    int getX0() const { return x0; }
    int getY0() const { return y0; }
    int getX1() const { return x1; }
    int getY1() const { return y1; }
    int getAtiva() const { return ativa; }
    int getIdCamada() const { return idCamada; }
    int getIdOriginalCamada() const { return idOriginalCamada; }
    int** getMatrizDesenho() const { return matrizDesenho;}
    int getVisivel() const { return visivel; }
    Bmp* getImage() const { return img; }
    bool getHasImage() const { return hasImage; }
    int getImgWidth() const { return imgWidth; }
    int getImgHeight() const { return imgHeight; }
    int getImgX0() const { return imgX0; }
    int getImgY0() const { return imgY0; }
    int getBrilho() const { return brilho; }



    // Setters
    void setAtiva(int valor) { ativa = valor; }
    void setIdCamada(int valor) { idCamada = valor; }
    void setIdOriginalCamada(int valor) { idOriginalCamada = valor; }
    void setX0(int valor) { x0 = valor; }
    void setY0(int valor) { y0 = valor; }
    void setX1(int valor) { x1 = valor; }
    void setY1(int valor) { y1 = valor; }
    void setImgWidth(int valor) { imgWidth = valor; }
    void setImgHeight(int valor) { imgHeight = valor; }
    void setImgX0(int valor) { imgX0 = valor; }
    void setImgY0(int valor) { imgY0 = valor; }
    void setBrilho(int valor)  { brilho = valor; }




    Camada(char *_imagePath, int _imgX0, int _imgY0, int _x0, int _y0, int _x1, int _y1, int _idCamada, int _visible)
    {
        imgX0 = _imgX0;
        imgY0 = _imgY0;
        x0 = _x0;
        y0 = _y0;
        x1 = _x1;
        y1 = _y1;
        idCamada = _idCamada;
        idOriginalCamada = _idCamada;
        ativa = 0;

        hasImage = true;

        img = new Bmp(_imagePath);
        img->convertBGRtoRGB();
        imgWidth=img->getWidth();
        imgHeight=img->getHeight();

        visivel=_visible;
    }

    Camada(int _x0, int _y0, int _x1, int _y1, int _idCamada)
    {
        x0 = _x0;
        y0 = _y0;
        x1 = _x1;
        y1 = _y1;
        idCamada = _idCamada;
        idOriginalCamada = _idCamada;
        ativa = 0;
        visivel = 0;

        hasImage = false;
    }
    void insereImagem(char *_imagePath)
    {
        hasImage = true;

        imgX0 = 400;
        imgY0 = 200;
        img = new Bmp(_imagePath);
        img->convertBGRtoRGB();
        imgWidth=img->getWidth();
        imgHeight=img->getHeight();

    }


    // Dentro da classe Camada
    void inicializaMatriz(int largura, int altura)
    {
        matrizDesenho = (int **)malloc(altura * sizeof(int *));
        for (int i = 0; i < altura; i++)
            matrizDesenho[i] = (int *)malloc(largura * sizeof(int));

        for (int i = 0; i < altura; i++)
            for (int j = 0; j < largura; j++)
                    matrizDesenho[i][j] = -1;
    }




    void desenhaMenuCamadas()
    {
        CV::color(0.8, 0.8, 0.8);
        CV::rectFill(x0, y0, x1, y1);
        if(ativa == 1)
        {
            CV::color(0.7, 0.7, 0.7);
            CV::rectFill(x0 + 5, y0 +5, x1 -5, y1 -5);

        }
        CV::color(0, 0, 0);
        CV::text(x0+3, y0+10, "Camada:");
        char id[100];
        sprintf(id, "%d", idOriginalCamada);
        //Teste
        if(ativa == 1)
        {
            //printf("\nAtiva: %d\n", idCamada);
        }
        CV::text(x0+80, y0+10, id);

    }

    void desenhaIsVisible()
    {
        CV::color(0.8, 0.8, 0.8);
        CV::rect(20, 360, 40, 380);
        if(visivel==1)
        {
            CV::color(0, 0, 0);
            CV::circleFill(30, 370, 3, 20);
        }

    }
    void isVisible(int isPressed, int click, int mouseX, int mouseY)
    {

        if (isPressed == 1 && click == 1 && mouseX >= 20 && mouseX < 40 && mouseY>=360 && mouseY<=380)
        {
           if(visivel==0)
           {
               visivel = 1;
               //Sleep(100);
               //return;
           }
           else{
            visivel = 0;
            //Sleep(100);
            //return;
           }
        }

    }




    void renderCamada()
    {
        desenhaMenuCamadas();
    }

};

#endif

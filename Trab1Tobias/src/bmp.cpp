//*********************************************************
//
// classe para fazer o carregamento de arquivos no formato BMP
// Autor: Cesar Tadeu Pozzer
//        pozzer@inf.ufsm.br
//  Versao 09/2010
//
//**********************************************************

#include "Bmp.h"
#include <string.h>

Bmp::Bmp(const char *fileName)
{
   width = height = 0;
   data = NULL;
   if( fileName != NULL && strlen(fileName) > 0 )
   {
      load(fileName);
   }
   else
   {
      printf("Error: Invalid BMP filename");
   }
}

uchar* Bmp::getImage()
{
  return data;
}

int Bmp::getWidth(void)
{
  return width;
}

int Bmp::getHeight(void)
{
  return height;
}





void Bmp::convertBGRtoRGB()
{
  unsigned char tmp;
  if( data != NULL )
  {
     for(int y=0; y<height; y++)
     for(int x=0; x<width*3; x+=3)
     {
        //int pos = y*bytesPerLine + x;
        int pos = y*width*3+x;
        tmp = data[pos];
        data[pos] = data[pos+2];
        data[pos+2] = tmp;
     }
  }
}

//OBS - colocando dentro da classe BMP, fica muito mais facil e organizado.
void Bmp::render()
{
  unsigned char tmp;

  if( data != NULL )
  {
     for(int y=0; y<height; y++)
     for(int x=0; x<width*3; x+=3)  //cada 3 coordenadas x representa 1 pixel na imagem (RGB).
     {
        int pos = y*width*3 + x;


        //Dividir pos 3 pois a funcao CV::color() recebe valores entre [0, 1]
        CV::color(data[pos] / 255.0, data[pos+1] / 255.0, data[pos+2]/ 255.0);
        CV::point(x/3, y);
        //CV::circleFill(x/3, y, 3, 3);
     }
  }
}
void Bmp::renderLocalizado(int x0, int y0)
{
    if (data != NULL)
    {
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width * 3; x += 3)
            {
                int pos = y * width*3 + x;

                CV::color(
                    data[pos] / 255.0,
                    data[pos + 1] / 255.0,
                    data[pos + 2] / 255.0
                );

                // Inverter a altura: y0 + (height - 1 - y)
                CV::point(x / 3 + x0, y0 + y);
            }
        }
    }
}


void Bmp::flipH()
{
  unsigned char tmp;
  if( data != NULL )
  {
     for(int y=0; y<height; y++)
     for(int x=0; x<width*3/2; x+=3)
     {
        int pos1 = y * width * 3 + x;
        int pos2 = y * width * 3 + (width*3 - x - 3);
        tmp = data[pos1];
        data[pos1] = data[pos2];
        data[pos2] = tmp;
        tmp = data[pos1+1];
        data[pos1+1] = data[pos2+1];
        data[pos2+1] = tmp;
        tmp = data[pos1+2];
        data[pos1+2] = data[pos2+2];
        data[pos2+2] = tmp;
     }
  }
}

void Bmp::flipV()
{
  unsigned char tmp;
  if( data != NULL )
  {
     for(int y=0; y<height/2; y++)
     for(int x=0; x<width*3; x+=3)
     {
        int pos1 = y * width * 3 + x;
        int pos2 = (height - y - 1) * width * 3 + x;
        tmp = data[pos1];
        data[pos1] = data[pos2];
        data[pos2] = tmp;
        tmp = data[pos1+1];
        data[pos1+1] = data[pos2+1];
        data[pos2+1] = tmp;
        tmp = data[pos1+2];
        data[pos1+2] = data[pos2+2];
        data[pos2+2] = tmp;
     }
  }
}
void Bmp::aumentaBrilho(int fator)
{
  if( data != NULL )
  {
     for(int y=0; y<height; y++)
     for(int x=0; x<width*3; x+=3)
     {
        int pos = y * width * 3 + x;
        data[pos]   = (data[pos]   + fator > 255) ? 255 : data[pos]   + fator;
        data[pos+1] = (data[pos+1] + fator > 255) ? 255 : data[pos+1] + fator;
        data[pos+2] = (data[pos+2] + fator > 255) ? 255 : data[pos+2] + fator;
     }
  }
}

void Bmp::diminuiBrilho(int fator)
{
    if (data != NULL)
    {
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width * 3; x += 3)
            {
                int pos = y * width * 3 + x;
                data[pos]   = (data[pos]   - fator < 0) ? 0 : data[pos]   - fator;
                data[pos+1] = (data[pos+1] - fator < 0) ? 0 : data[pos+1] - fator;
                data[pos+2] = (data[pos+2] - fator < 0) ? 0 : data[pos+2] - fator;
            }
        }
    }
}






void Bmp::load(const char *fileName)
{
  FILE *fp = fopen(fileName, "rb");
  if( fp == NULL )
  {
     printf("\nErro ao abrir arquivo %s para leitura", fileName);
     return;
  }

  printf("\n\nCarregando arquivo %s", fileName);

  //le o HEADER componente a componente devido ao problema de alinhamento de bytes. Usando
  //o comando fread(header, sizeof(HEADER),1,fp) sao lidos 16 bytes ao inves de 14
  fread(&header.type,      sizeof(unsigned short int), 1, fp);
  fread(&header.size,      sizeof(unsigned int),       1, fp);
  fread(&header.reserved1, sizeof(unsigned short int), 1, fp);
  fread(&header.reserved2, sizeof(unsigned short int), 1, fp);
  fread(&header.offset,    sizeof(unsigned int),       1, fp); //indica inicio do bloco de pixels

  //le o INFOHEADER componente a componente devido ao problema de alinhamento de bytes
  fread(&info.size,        sizeof(unsigned int),       1, fp);
  fread(&info.width,       sizeof(int),                1, fp);
  fread(&info.height,      sizeof(int),                1, fp);
  fread(&info.planes,      sizeof(unsigned short int), 1, fp);
  fread(&info.bits,        sizeof(unsigned short int), 1, fp);
  fread(&info.compression, sizeof(unsigned int),       1, fp);
  fread(&info.imagesize,   sizeof(unsigned int),       1, fp);
  fread(&info.xresolution, sizeof(int),                1, fp);
  fread(&info.yresolution, sizeof(int),                1, fp);
  fread(&info.ncolours,    sizeof(unsigned int),       1, fp);
  fread(&info.impcolours,  sizeof(unsigned int),       1, fp);

  width  = info.width;
  height = info.height;
  bits   = info.bits;

  int bytesPerPixel = bits/8;
  int linesSize = ((width* bytesPerPixel+3)/4)*4;
  imagesize = linesSize*height;

  int delta    = bytesPerLine - (3 * width);

  printf("\nImagem: %dx%d - Bits: %d", width, height, bits);
  printf("\nbytesPerLine: %d", bytesPerLine);
  printf("\nbytesPerLine: %d", width * 3);
  printf("\ndelta: %d", delta);
  printf("\nimagesize: %d %d", imagesize, info.imagesize);

  //realiza diversas verificacoes de erro e compatibilidade
  if( header.type != 19778 )
  {
     printf("\nError: Arquivo BMP invalido");
     getchar();
     exit(0);
  }

  /*if( width*height*3 != imagesize )
  {
     printf("\nWarning: Arquivo BMP nao tem largura multipla de 4");
     getchar();
  }*/

  if( info.compression != 0 )
  {
     printf("\nError: Formato BMP comprimido nao suportado");
     getchar();
     return;
  }
  if( bits != 24 )
  {
     printf("\nError: Formato BMP com %d bits/pixel nao suportado", bits);
     getchar();
     return;
  }

  if( info.planes != 1 )
  {
     printf("\nError: Numero de Planes nao suportado: %d", info.planes);
     getchar();
     return;
  }

  data = new unsigned char[imagesize];
  fseek(fp, header.offset, SEEK_SET);
  fread(data, sizeof(unsigned char), imagesize, fp);

  fclose(fp);
}

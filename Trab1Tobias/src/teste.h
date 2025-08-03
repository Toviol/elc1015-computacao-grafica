#ifndef ___TELA__H___
#define ___TELA__H___

#include "gl_canvas2d.h"
#include "bmp.h"

class Tela
{
    int screenWidth, screenHeight;
    int menuWidth = 100;
    int matrizWidth, matrizHeight;

    int **matrizDesenho;

    int corSelecionada = 0;
    int modo = 0;       // 0 = pincel, 1 = borracha
    int tipoPincel = 0; // 0 = redondo, 1 = quadrado, 2 = triangulo
    int tamanhoPincel = 4;

    int sliderX, sliderY, sliderWidth = 80, sliderHeight = 10;

    // Ícones
    Bmp *iconePincel = nullptr;
    Bmp *iconeBorracha = nullptr;

public:
    Tela(int larguraDesenho, int alturaDesenho, int larguraMenu = 100)
    {
        matrizWidth = larguraDesenho;
        matrizHeight = alturaDesenho;
        menuWidth = larguraMenu;

        screenWidth = menuWidth + matrizWidth;
        screenHeight = matrizHeight;

        inicializaMatrizDesenho(&matrizDesenho, matrizWidth, matrizHeight);

        iconePincel = new Bmp(".\\__LAB2\\images\\lapis.bmp");
        iconeBorracha = new Bmp(".\\__LAB2\\images\\borracha8.bmp");
        iconePincel->convertBGRtoRGB();
        iconeBorracha->convertBGRtoRGB();
    }

    void initTela()
    {
        CV::init(&screenWidth, &screenHeight, "imagem com menu");
    }


    void renderTela(int mouseX, int mouseY, int isPressed)
    {
        desenhaMenu();

        if (isPressed == 1)
        {
            if (mouseX >= 0 && mouseX < menuWidth)
            {
                // Paleta de cores
                int cols = 4, espacamento = 5, tamQuadrado = 30;
                int startY = 40;
                int startX = (menuWidth - (cols * tamQuadrado + (cols - 1) * espacamento)) / 2;

                for (int i = 0; i < 14; i++)
                {
                    int linha = i / cols;
                    int coluna = i % cols;

                    int x = startX + coluna * (tamQuadrado + espacamento);
                    int y = startY + linha * (tamQuadrado + espacamento);

                    if (mouseX >= x && mouseX <= x + tamQuadrado &&
                        mouseY >= y - tamQuadrado && mouseY <= y)
                    {
                        corSelecionada = i;
                    }
                }

                int numLinhasCores = (14 + cols - 1) / cols;
                int btnY = startY + numLinhasCores * (30 + 5) + 20;

                if (mouseX >= 10 && mouseX <= menuWidth - 10 && mouseY >= btnY && mouseY <= btnY + 38)
                {
                    modo = 0;
                }

                btnY += 40;
                if (mouseX >= 10 && mouseX <= menuWidth - 10 && mouseY >= btnY && mouseY <= btnY + 38)
                {
                    modo = 1;
                }

                btnY += 50;
                sliderX = 20;
                sliderY = btnY;

                if (mouseX >= sliderX && mouseX <= sliderX + sliderWidth &&
                    mouseY >= sliderY && mouseY <= sliderY + sliderHeight + 10)
                {
                    float pct = (float)(mouseX - sliderX) / (float)sliderWidth;
                    tamanhoPincel = 1 + pct * 29;
                }

                // Botões forma
                int btnFormaY = sliderY + 30;
                int btnFormaX = 10;
                int larguraBotao = 24;
                int espacamentoForma = 8;

                for (int i = 0; i < 3; i++)
                {
                    int xMin = btnFormaX;
                    int xMax = btnFormaX + larguraBotao;
                    int yMin = btnFormaY;
                    int yMax = btnFormaY + 20;

                    if (mouseX >= xMin && mouseX <= xMax && mouseY >= yMin && mouseY <= yMax)
                    {
                        tipoPincel = i;
                    }

                    btnFormaX += larguraBotao + espacamentoForma;
                }

                // Botões Flip
                int flipBtnY = btnFormaY + 35;
                int btnW = 80;
                int btnH = 25;

                if (mouseX >= 10 && mouseX <= 10 + btnW && mouseY >= flipBtnY && mouseY <= flipBtnY + btnH)
                {
                    flipHorizontal();
                    return;
                }

                flipBtnY += 35;
                if (mouseX >= 10 && mouseX <= 10 + btnW && mouseY >= flipBtnY && mouseY <= flipBtnY + btnH)
                {
                    flipVertical();
                    return;
                }
            }
        }

        int xRel = mouseX - menuWidth;
        int yRel = mouseY;

        if (mouseX >= menuWidth && mouseX < screenWidth && mouseY >= 0 && mouseY < screenHeight)
        {
            if (isPressed == 1)
            {
                if (modo == 0)
                    desenhaNaMatriz(yRel, xRel, tamanhoPincel, corSelecionada, tipoPincel);
                else
                    desenhaNaMatriz(yRel, xRel, tamanhoPincel, -1, tipoPincel);
            }
        }

        desenhaMatrizNaTela();
    }

    void desenhaMenu()
    {
        CV::color(0.7, 0.7, 0.7);
        CV::rectFill(0, 0, menuWidth, screenHeight);

        // Paleta de cores
        int cols = 4, espacamento = 5, tamQuadrado = 30;
        int startX = (menuWidth - (cols * tamQuadrado + (cols - 1) * espacamento)) / 2;
        int startY = 40;

        for (int i = 0; i < 14; i++)
        {
            int linha = i / cols;
            int coluna = i % cols;

            int x = startX + coluna * (tamQuadrado + espacamento);
            int y = startY + linha * (tamQuadrado + espacamento);

            CV::color(i);
            CV::rectFill(x, y - tamQuadrado, x + tamQuadrado, y);

            if (i == corSelecionada)
            {
                CV::color(13);
                CV::rect(x - 2, y - tamQuadrado - 2, x + tamQuadrado + 2, y + 2);
            }
        }

        int numLinhasCores = (14 + cols - 1) / cols;
        int btnY = startY + numLinhasCores * (tamQuadrado + espacamento) + 20;

        // ícone pincel
        iconePincel->renderLocalizado(10, btnY);
        if (modo == 0)
        {
            CV::color(0);
            CV::rect(8, btnY - 2, menuWidth - 8, btnY + 40);
        }

        btnY += 40;
        iconeBorracha->renderLocalizado(10, btnY);
        if (modo == 1)
        {
            CV::color(0);
            CV::rect(8, btnY - 2, menuWidth - 8, btnY + 40);
        }

        btnY += 50;
        sliderX = 20;
        sliderY = btnY;

        CV::color(0.5, 0.5, 0.5);
        CV::rectFill(sliderX, sliderY, sliderX + sliderWidth, sliderY + sliderHeight);

        float percent = (float)tamanhoPincel / 30.0f;
        int knobX = sliderX + percent * sliderWidth;

        CV::color(0);
        CV::rectFill(knobX - 4, sliderY - 4, knobX + 4, sliderY + sliderHeight + 4);

        char label[20];
        sprintf(label, "Tam: %d", tamanhoPincel);
        CV::text(sliderX, sliderY + 18, label);

        int btnFormaY = sliderY + 30;
        int btnFormaX = 10;
        int larguraBotao = 24;
        int espacamentoForma = 8;

        for (int i = 0; i < 3; i++)
        {
            CV::color(1.0, 1.0, 1.0);
            CV::rectFill(btnFormaX, btnFormaY, btnFormaX + larguraBotao, btnFormaY + 20);

            if (tipoPincel == i)
            {
                CV::color(0);
                CV::rect(btnFormaX - 2, btnFormaY - 2, btnFormaX + larguraBotao + 2, btnFormaY + 22);
            }

            CV::color(0);
            if (i == 0) CV::text(btnFormaX + 3, btnFormaY + 6, "Red");
            if (i == 1) CV::text(btnFormaX + 3, btnFormaY + 6, "Qua");
            if (i == 2) CV::text(btnFormaX + 3, btnFormaY + 6, "Tri");

            btnFormaX += larguraBotao + espacamentoForma;
        }

        int flipBtnY = btnFormaY + 35;
        int btnW = 80;
        int btnH = 25;

        CV::color(0.8, 0.8, 0.8);
        CV::rectFill(10, flipBtnY, 10 + btnW, flipBtnY + btnH);
        CV::color(0);
        CV::text(20, flipBtnY + 7, "Flip H");

        flipBtnY += 35;
        CV::color(0.8, 0.8, 0.8);
        CV::rectFill(10, flipBtnY, 10 + btnW, flipBtnY + btnH);
        CV::color(0);
        CV::text(20, flipBtnY + 7, "Flip V");
    }


    void flipHorizontal()
    {
        for (int i = 0; i < matrizHeight; i++)
        {
            for (int j = 0; j < matrizWidth / 2; j++)
            {
                int tmp = matrizDesenho[i][j];
                matrizDesenho[i][j] = matrizDesenho[i][matrizWidth - 1 - j];
                matrizDesenho[i][matrizWidth - 1 - j] = tmp;
            }
        }
    }

    void flipVertical()
    {
        for (int i = 0; i < matrizHeight / 2; i++)
        {
            for (int j = 0; j < matrizWidth; j++)
            {
                int tmp = matrizDesenho[i][j];
                matrizDesenho[i][j] = matrizDesenho[matrizHeight - 1 - i][j];
                matrizDesenho[matrizHeight - 1 - i][j] = tmp;
            }
        }
    }

    void inicializaMatrizDesenho(int ***matrizDesenho, int x, int y)
    {
        *matrizDesenho = (int **)malloc(y * sizeof(int *));
        for (int i = 0; i < y; i++)
            (*matrizDesenho)[i] = (int *)malloc(x * sizeof(int));

        for (int i = 0; i < y; i++)
            for (int j = 0; j < x; j++)
                (*matrizDesenho)[i][j] = -1;
    }

    void desenhaNaMatriz(int i, int j, int r, int cor, int tipoPincel)
    {
        switch (tipoPincel)
        {
        case 0:
            for (int _x = -r; _x <= r; _x++)
                for (int _y = -r; _y <= r; _y++)
                {
                    int yi = i + _y;
                    int xj = j + _x;
                    if (yi >= 0 && yi < matrizHeight && xj >= 0 && xj < matrizWidth && _x * _x + _y * _y <= r * r)
                        matrizDesenho[yi][xj] = cor;
                }
            break;
        case 1:
            for (int yi = i - r; yi <= i + r; yi++)
                for (int xj = j - r; xj <= j + r; xj++)
                    if (yi >= 0 && yi < matrizHeight && xj >= 0 && xj < matrizWidth)
                        matrizDesenho[yi][xj] = cor;
            break;
        case 2:
            for (int h = 0; h <= r; h++)
            {
                int inicio = j - h;
                int fim = j + h;
                for (int xj = inicio; xj <= fim; xj++)
                {
                    int yi = i - h;
                    if (yi >= 0 && yi < matrizHeight && xj >= 0 && xj < matrizWidth)
                        matrizDesenho[yi][xj] = cor;
                }
            }
            break;
        }
    }

    void desenhaMatrizNaTela()
    {
        for (int i = 0; i < matrizHeight; i++)
            for (int j = 0; j < matrizWidth; j++)
                if (matrizDesenho[i][j] != -1)
                {
                    CV::color(matrizDesenho[i][j]);
                    CV::point(j + menuWidth, i);
                }
    }
};

#endif
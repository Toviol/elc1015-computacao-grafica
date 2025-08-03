#ifndef ___TELA__H___
#define ___TELA__H___

#include <Windows.h>

#include "Camada.h"
#include "gl_canvas2d.h"

#define M_PI 3.14159265358979323846

class Tela
{
    // --- CONFIGURAÇÃO VISUAL ---
    const int colorBoxSize = 30;
    const int colorBoxSpacing = 5;
    const int colorCols = 4;
    const int controlBtnHeight = 30;
    const int controlBtnSpacing = 10;
    const int controlBtnPadding = 10;
    const int sliderWidth = 80;
    const int sliderHeight = 10;
    const int shapeBtnWidth = 24;
    const int shapeBtnHeight = 20;
    const int shapeBtnSpacing = 8;
    const int flipBtnWidth = 80;
    const int flipBtnHeight = 25;
    const int maxYmenuLayer = 600;
    const int minYmenuLayer = 400;
    const int distanceBetweenMenuLayer = 55;

    // --- Dimensões Gerais ---
    int menuWidth;
    int screenWidth, screenHeight;
    int matrizWidth, matrizHeight;

    // --- Estado ---
    int corSelecionada = 0;
    int modo = 0;       // 0 = pincel, 1 = borracha
    int tipoPincel = 0; // 0 = redondo, 1 = quadrado, 2 = triangulo, 3 = ....
    int tamanhoPincel = 4; // Tamanho inicial do pincel

    // --- Slider ---
    int sliderX, sliderY;

    int brilhoSliderValue = 0;
    const int brilhoSliderMin = 0;
    const int brilhoSliderMax = 100;
    int brilhoAplicadoAnterior = 0;
    bool brilhoAlterado = false;


    // --- Ícones ---
    Bmp *iconePincel = nullptr;
    Bmp *iconeBorracha = nullptr;
    Bmp *iconeFlipH = nullptr;
    Bmp *iconeFlipV = nullptr;
    Bmp *iconeMais = nullptr;
    Bmp *iconeMenos = nullptr;
    Bmp *iconeSetaCima = nullptr;
    Bmp *iconeSetaBaixo = nullptr;

    Camada **camadas;
    Camada *camada1;
    Camada *camada2;
    Camada *camada3;
    int idCamadaAtiva = 0;
    int idMaximo=0;
    int numeroCamadas=3;
    int **grade;

    // Falta implementar a dinamicidade do eixo x e y na inicialização das camadas
    int camadaZeroX0 = 40;
    int camadaZeroY0 = 400;
    int camadaZeroX1 = 160;
    int camadaZeroY1 = 450;

    bool movendoImagem = false;
    int offsetMouseX, offsetMouseY;
    int brilhoImagem;


public:
    Tela(int larguraDesenho, int alturaDesenho, int larguraMenu = 200)
    {
        matrizWidth = larguraDesenho;
        matrizHeight = alturaDesenho;
        menuWidth = larguraMenu;

        screenWidth = menuWidth + matrizWidth;
        screenHeight = matrizHeight;


        iconePincel = new Bmp(".\\__LAB2\\images\\pincel.bmp");
        iconeBorracha = new Bmp(".\\__LAB2\\images\\borracha.bmp");
        iconeFlipH = new Bmp(".\\__LAB2\\images\\flipH.bmp");
        iconeFlipV = new Bmp(".\\__LAB2\\images\\flipV.bmp");
        iconeMais = new Bmp(".\\__LAB2\\images\\mais.bmp");
        iconeMenos = new Bmp(".\\__LAB2\\images\\menos.bmp");
        iconeSetaCima = new Bmp(".\\__LAB2\\images\\setacima.bmp");
        iconeSetaBaixo = new Bmp(".\\__LAB2\\images\\setabaixo.bmp");
        iconePincel->convertBGRtoRGB();
        iconeBorracha->convertBGRtoRGB();
        iconeFlipH->convertBGRtoRGB();
        iconeFlipV->convertBGRtoRGB();
        iconeMais->convertBGRtoRGB();
        iconeMenos->convertBGRtoRGB();
        iconeSetaCima->convertBGRtoRGB();
        iconeSetaBaixo->convertBGRtoRGB();

        camada1 = new Camada(".\\__LAB2\\images\\img1.bmp", 500, 300, 40, 400, 160, 450, 0, 1);
        camada2 = new Camada(".\\__LAB2\\images\\img2.bmp", 500, 300, 40, 455, 160, 505, 1, 0);
        camada3 = new Camada(".\\__LAB2\\images\\img3.bmp", 500, 300, 40, 510, 160, 560, 2, 0);
        camadas = (Camada**)malloc(numeroCamadas * sizeof(Camada*));
        camadas[0]=camada1;
        camadas[1]=camada2;
        camadas[2]=camada3;
        camadas[0]->setAtiva(1);
        inicializaGrade(&grade, matrizWidth, matrizHeight);

        for (int i = 0; i < numeroCamadas; i++)
        {
            camadas[i]->inicializaMatriz(matrizWidth, matrizHeight);
            idMaximo++;
        }



    }
    void novaCamada()
    {

        int X0 = camadaZeroX0;
        int Y0 = camadas[0]->getY0() + ((numeroCamadas)*55);
        int X1 = camadaZeroX1;
        int Y1 = camadas[0]->getY1() + ((numeroCamadas)*55);

        numeroCamadas++;

        camadas = (Camada**)realloc(camadas, numeroCamadas * sizeof(Camada*));
        if (camadas == NULL) {
            printf("Erro ao realocar memoria para nva camadas.\n");
            exit(1);
        }
        camadas[numeroCamadas - 1] = new Camada(X0, Y0, X1, Y1, idMaximo);
        camadas[numeroCamadas - 1]->inicializaMatriz(matrizWidth, matrizHeight);

        idMaximo++;
    }

    void removeCamada(int index)
    {
        if (index < 0 || index >= numeroCamadas) {
            printf("indice inválido para remoção de camada.\n");
            return;
        }

        delete camadas[index];

        int i;
        for (i = index; i < numeroCamadas - 1; i++) {
            camadas[i] = camadas[i + 1];
            camadas[i]->setIdCamada(i);
            camadas[i]->setY0(camadas[i]->getY0() - distanceBetweenMenuLayer);
            camadas[i]->setY1(camadas[i]->getY1() - distanceBetweenMenuLayer);
        }
        if(i!=index)
        {
            camadas[index]->setAtiva(1);
            idCamadaAtiva = index;
        }
        else
        {
            camadas[index-1]->setAtiva(1);
            idCamadaAtiva = index-1;
        }

        numeroCamadas--;

        camadas = (Camada**)realloc(camadas, numeroCamadas * sizeof(Camada*));
        if (camadas == NULL && numeroCamadas > 0) {
            printf("Erro ao realocar memoria ao remover camada.\n");
            exit(1);
        }
    }



    void inicializaGrade(int ***grade, int x, int y)
    {
        *grade = (int **)malloc(y * sizeof(int *));
        for (int i = 0; i < y; i++)
            (*grade)[i] = (int *)malloc(x * sizeof(int));

        for (int i = 0; i < y; i++) {
            for (int j = 0; j < x; j++) {
                (*grade)[i][j] = -1;

                // Adiciona linhas da grade a cada 20 pixels, da pra mudar
                if (i % 20 == 0 || j % 20 == 0) {
                    (*grade)[i][j] = 14;  // Cor criada pra grade, um pouco mais fraca que o cinza existente
                }
            }
        }
    }

    void initTela()
    {
        CV::init(&screenWidth, &screenHeight, "imagem com menu");
    }

    int qualCamadaFoiClicada(int mouseX, int mouseY)
    {
        for (int i = 0; i < numeroCamadas; i++)
        {
            if (mouseX >= camadas[i]->getX0() && mouseX <= camadas[i]->getX1() &&
                mouseY >= camadas[i]->getY0() && mouseY <= camadas[i]->getY1())
            {
                return i;
            }
        }
        return -1;
    }

    int mudaCamadaAtiva(int novaCamadaAtiva, int idCamadaAtiva)
    {
        camadas[idCamadaAtiva]->setAtiva(0);
        camadas[novaCamadaAtiva]->setAtiva(1);
        return novaCamadaAtiva;
    }

    void desenhaMenuTodasCamadas()
    {
        for(int i = 0; i < numeroCamadas; i++)
        {
            int y0 = camadas[i]->getY0();
            if(y0>=minYmenuLayer && y0<=maxYmenuLayer)
            {
                camadas[i]->desenhaMenuCamadas();
            }
        }

    }

    void moverCamadaAtivaParaBaixo()
    {
        if (idCamadaAtiva > 0)
        {
            // Troca ID de camada
            int idTemp = camadas[idCamadaAtiva]->getIdCamada();
            //idTemp = 1;
            camadas[idCamadaAtiva]->setIdCamada(camadas[idCamadaAtiva - 1]->getIdCamada());
            //camada[1]->id=0;
            camadas[idCamadaAtiva - 1]->setIdCamada(idTemp);

            // Troca posições visuais no menu
            int x0 = camadas[idCamadaAtiva]->getX0();
            int y0 = camadas[idCamadaAtiva]->getY0();
            int x1 = camadas[idCamadaAtiva]->getX1();
            int y1 = camadas[idCamadaAtiva]->getY1();

            camadas[idCamadaAtiva]->setX0(camadas[idCamadaAtiva - 1]->getX0());
            camadas[idCamadaAtiva]->setY0(camadas[idCamadaAtiva - 1]->getY0());
            camadas[idCamadaAtiva]->setX1(camadas[idCamadaAtiva - 1]->getX1());
            camadas[idCamadaAtiva]->setY1(camadas[idCamadaAtiva - 1]->getY1());

            camadas[idCamadaAtiva - 1]->setX0(x0);
            camadas[idCamadaAtiva - 1]->setY0(y0);
            camadas[idCamadaAtiva - 1]->setX1(x1);
            camadas[idCamadaAtiva - 1]->setY1(y1);

            // Troca as posições do vetor
            Camada *temp = camadas[idCamadaAtiva];
            camadas[idCamadaAtiva] = camadas[idCamadaAtiva - 1];
            camadas[idCamadaAtiva - 1] = temp;

            idCamadaAtiva--;
        }
    }
    void moverCamadaAtivaParaCima()
    {
        if (idCamadaAtiva < numeroCamadas - 1)
        {
            // Troca ID de camada
            int idTemp = camadas[idCamadaAtiva]->getIdCamada();
            camadas[idCamadaAtiva]->setIdCamada(camadas[idCamadaAtiva + 1]->getIdCamada());
            camadas[idCamadaAtiva + 1]->setIdCamada(idTemp);

            // Troca posições visuais no menuu
            int x0 = camadas[idCamadaAtiva]->getX0();
            int y0 = camadas[idCamadaAtiva]->getY0();
            int x1 = camadas[idCamadaAtiva]->getX1();
            int y1 = camadas[idCamadaAtiva]->getY1();

            camadas[idCamadaAtiva]->setX0(camadas[idCamadaAtiva + 1]->getX0());
            camadas[idCamadaAtiva]->setY0(camadas[idCamadaAtiva + 1]->getY0());
            camadas[idCamadaAtiva]->setX1(camadas[idCamadaAtiva + 1]->getX1());
            camadas[idCamadaAtiva]->setY1(camadas[idCamadaAtiva + 1]->getY1());

            camadas[idCamadaAtiva + 1]->setX0(x0);
            camadas[idCamadaAtiva + 1]->setY0(y0);
            camadas[idCamadaAtiva + 1]->setX1(x1);
            camadas[idCamadaAtiva + 1]->setY1(y1);

            // Troca as posições do vetor
            Camada *temp = camadas[idCamadaAtiva];
            camadas[idCamadaAtiva] = camadas[idCamadaAtiva + 1];
            camadas[idCamadaAtiva + 1] = temp;

            idCamadaAtiva++;
        }
    }

    void desenhaMaisMenos()
    {
        iconeMenos->renderLocalizado(70, 370);
        iconeMais->renderLocalizado(115, 370);
    }

    void acaoAdicionaRemoveCamada(int mouseX, int mouseY, int click)
    {

        if(mouseX>=70&&mouseX<=70+28 && mouseY>=370 && mouseY<=370+28 && click == 1)
        {
            if(numeroCamadas == 1)
            {
                idMaximo=0;
                novaCamada();
                removeCamada(idCamadaAtiva);
            }
            else{
                removeCamada(idCamadaAtiva);
            }
        }
        if(mouseX>=115&&mouseX<=115+28 && mouseY>=370 && mouseY<=370+28 && click == 1)
        {
            novaCamada();
        }
    }
    void acaoPreferenciaCamada(int mouseX, int mouseY, int click)
    {

        if(mouseX>=200&&mouseX<=200+24 && mouseY>=450 && mouseY<=450+24 && click == 1)
        {
            moverCamadaAtivaParaBaixo();
        }
        if(mouseX>=200&&mouseX<=200+24 && mouseY>=480 && mouseY<=480+24 && click == 1)
        {
            moverCamadaAtivaParaCima();
        }
    }

    void desenhaPreferenciaCamada()
    {

        iconeSetaBaixo->renderLocalizado(200, 450);
        iconeSetaCima->renderLocalizado(200, 480);
    }


    void acaoScrollCamada(int mouseX, int mouseY, int click)
    {

        if(mouseX>=170&&mouseX<=170+24 && mouseY>=450 && mouseY<=450+24 && click == 1)
        {
            for(int i=0; i<numeroCamadas; i++)
            {

                camadas[i]->setY0(camadas[i]->getY0()+distanceBetweenMenuLayer);
                camadas[i]->setY1(camadas[i]->getY1()+distanceBetweenMenuLayer);
            }
        }
        if(mouseX>=170&&mouseX<=170+24 && mouseY>=480 && mouseY<=480+24 && click == 1)
        {
            for(int i=0; i<numeroCamadas; i++)
            {

                camadas[i]->setY0(camadas[i]->getY0()-distanceBetweenMenuLayer);
                camadas[i]->setY1(camadas[i]->getY1()-distanceBetweenMenuLayer);
            }
        }
    }
    void desenhaScrollCamada()
    {

        iconeSetaBaixo->renderLocalizado(170, 450);
        iconeSetaCima->renderLocalizado(170, 480);
    }

    void desenhaInserirImagem()
    {
        int shapeBtnHeight = 25;
        int shapeBtnWidth = 30;
        int botoes = 3;
        int espacamento = 20;
        int btnFormaY = 310;

        int totalWidth = botoes * shapeBtnWidth + (botoes - 1) * espacamento;
        int startX = (menuWidth - totalWidth) / 2;

        for (int i = 0; i < botoes; i++)
        {
            int btnFormaX = startX + i * (shapeBtnWidth + espacamento);

            CV::color(0);
            CV::rectFill(btnFormaX, btnFormaY, btnFormaX + shapeBtnWidth, btnFormaY + shapeBtnHeight);

            char id[10];
            sprintf(id, "%d", i + 1);

            int textX = btnFormaX + shapeBtnWidth / 2 - 4;
            int textY = btnFormaY + shapeBtnHeight/4;

            CV::color(255, 255, 255);
            CV::text(textX, textY, id);
        }
    }




    void acaoInserirImagem(int mouseX, int mouseY, int click)
    {
        int shapeBtnHeight = 25;
        int shapeBtnWidth = 30;
        int botoes = 3;
        int espacamento = 20;
        int btnFormaY = 310;

        int totalWidth = botoes * shapeBtnWidth + (botoes - 1) * espacamento;
        int startX = (menuWidth - totalWidth) / 2;

        for (int i = 0; i < botoes; i++)
        {
            int btnFormaX = startX + i * (shapeBtnWidth + espacamento);

            int xMin = btnFormaX;
            int xMax = btnFormaX + shapeBtnWidth;
            int yMin = btnFormaY;
            int yMax = btnFormaY + shapeBtnHeight;

            if (click==1&&mouseX >= xMin && mouseX <= xMax && mouseY >= yMin && mouseY <= yMax)
            {
                if (i == 0)
                {
                    camadas[idCamadaAtiva]->insereImagem(".\\__LAB2\\images\\agua.bmp");
                }
                else if (i == 1)
                {
                    camadas[idCamadaAtiva]->insereImagem(".\\__LAB2\\images\\naruto.bmp");
                }
                else if (i == 2)
                {
                    camadas[idCamadaAtiva]->insereImagem(".\\__LAB2\\images\\terra.bmp");
                }
            }
        }
    }


    void renderTela(int mouseX, int mouseY, int isPressed, int click)
    {
        desenhaMenu();

        acaoInserirImagem(mouseX, mouseY, click);
        acaoPreferenciaCamada(mouseX, mouseY, click);
        acaoScrollCamada(mouseX, mouseY, click);
        if(isPressed == 1)
        {
            int novaCamadaAtiva = qualCamadaFoiClicada(mouseX, mouseY);
            if(novaCamadaAtiva != -1)
            {
                idCamadaAtiva = mudaCamadaAtiva(novaCamadaAtiva, idCamadaAtiva);
            }
        }
        acaoAdicionaRemoveCamada(mouseX, mouseY, click);
        camadas[idCamadaAtiva]->isVisible(isPressed, click, mouseX, mouseY);



        // Ação da matriz de cores
        if (isPressed == 1 && mouseX >= 0 && mouseX < menuWidth)
        {
            int startX = (menuWidth - (colorCols * colorBoxSize + (colorCols - 1) * colorBoxSpacing)) / 2;
            int startY = 40;

            for (int i = 0; i < 14; i++)
            {
                int linha = i / colorCols;
                int coluna = i % colorCols;
                int x = startX + coluna * (colorBoxSize + colorBoxSpacing);
                int y = startY + linha * (colorBoxSize + colorBoxSpacing);

                if (mouseX >= x && mouseX <= x + colorBoxSize &&
                    mouseY >= y - colorBoxSize && mouseY <= y)
                    corSelecionada = i;
            }

            int numLinhasCores = (14 + colorCols - 1) / colorCols;
            int btnY = startY + numLinhasCores * (colorBoxSize + colorBoxSpacing) + controlBtnSpacing;
            int iconX = (menuWidth - 74) / 2; // espaço para 2 ícones + espaçamento

            // Pincel
            if (mouseX >= iconX && mouseX <= iconX + 32 &&
                mouseY >= btnY && mouseY <= btnY + 32)
            {
                modo = 0;
            }

            // Flip H
            if (click == 1 && mouseX >= iconX + 42 && mouseX <= iconX + 74 &&
                mouseY >= btnY && mouseY <= btnY + 32)
            {
                flipHorizontal();
                return;
            }

            btnY += 32 + controlBtnSpacing;

            // Borracha
            if (mouseX >= iconX && mouseX <= iconX + 32 &&
                mouseY >= btnY && mouseY <= btnY + 32)
            {
                modo = 1;
            }

            // Flip V
            if (click == 1 && mouseX >= iconX + 42 && mouseX <= iconX + 74 &&
                mouseY >= btnY && mouseY <= btnY + 32)
            {
                flipVertical();
                return;
            }


            btnY += 32 + controlBtnSpacing;
            sliderX = 20;
            sliderY = btnY;

            //Acao Slider de tamanho
            if (mouseX >= sliderX && mouseX <= sliderX + sliderWidth &&
                mouseY >= sliderY && mouseY <= sliderY + sliderHeight + 10)
                tamanhoPincel = 1 + ((float)(mouseX - sliderX) / sliderWidth) * 29;


            //Acao slider de brilho
            int brilhoSliderX = sliderX + 100;
            int brilhoSliderY = sliderY;

            if (mouseX >= brilhoSliderX && mouseX <= brilhoSliderX + sliderWidth &&
                mouseY >= brilhoSliderY && mouseY <= brilhoSliderY + sliderHeight + 10 &&
                isPressed == 1) // apenas enquanto arrasta o slider
            {
                brilhoSliderValue = brilhoSliderMin + ((float)(mouseX - brilhoSliderX) / sliderWidth) * (brilhoSliderMax - brilhoSliderMin);
                brilhoAlterado = true;
            }

            //Matriz/linha de formatos de pinceis
            int btnFormaY = startY + numLinhasCores * (colorBoxSize + colorBoxSpacing - 8) + controlBtnSpacing;
            int btnFormaX = menuWidth/2 - (((shapeBtnWidth + 4)*8)/2);

            for (int i = 0; i < 7; i++)
            {
                int xMin = btnFormaX;
                int xMax = btnFormaX + shapeBtnWidth;
                int yMin = btnFormaY;
                int yMax = btnFormaY + shapeBtnHeight;

                if (mouseX >= xMin && mouseX <= xMax && mouseY >= yMin && mouseY <= yMax)
                    tipoPincel = i;

                btnFormaX += shapeBtnWidth + shapeBtnSpacing;
            }

        }

        int xRel = mouseX - menuWidth;
        int yRel = mouseY;

        if (mouseX >= menuWidth && mouseX < screenWidth && mouseY >= 0 && mouseY < screenHeight)
        {
            if (isPressed == 1)
            {
                if (modo == 0)
                    desenhaNaMatriz(camadas[idCamadaAtiva]->getMatrizDesenho(), yRel, xRel, tamanhoPincel, corSelecionada, tipoPincel);
                else
                    desenhaNaMatriz(camadas[idCamadaAtiva]->getMatrizDesenho(), yRel, xRel, tamanhoPincel, -1, tipoPincel);
            }

            //ispressed == 2 é o botão direito do mouse, utilizado para movimentar a imagem
            if(isPressed==2&&camadas[idCamadaAtiva]->getHasImage())
            {

                int imgX0=camadas[idCamadaAtiva]->getImgX0();
                int imgX1=imgX0+camadas[idCamadaAtiva]->getImgWidth();
                int imgY0=camadas[idCamadaAtiva]->getImgY0();
                int imgY1=imgY0+camadas[idCamadaAtiva]->getImgHeight();


                if (mouseX >= imgX0 && mouseX < imgX1 && mouseY >= imgY0 && mouseY < imgY1)
                {
                    if (!movendoImagem)
                    {
                        movendoImagem = true;
                        offsetMouseX = mouseX - imgX0;
                        offsetMouseY = mouseY - imgY0;
                    }

                    // Calcula nova posição da imagem com base no movimento do mouse
                    int novoX0 = mouseX - offsetMouseX;
                    int novoY0 = mouseY - offsetMouseY;

                    camadas[idCamadaAtiva]->setImgX0(novoX0);
                    camadas[idCamadaAtiva]->setImgY0(novoY0);
                }

            }
            if (isPressed == 0)
            {
                movendoImagem = false;
            }

        }

        desenhaMenuTodasCamadas();
        desenhaMatrizesNaTela();

        if(camadas[idCamadaAtiva]->getHasImage())
        {

            modificaBrilho();
        }


    }

    void desenhaMenu()
    {
        CV::color(0.7, 0.7, 0.7);
        CV::rectFill(0, 0, menuWidth, screenHeight);

        int startX = (menuWidth - (colorCols * colorBoxSize + (colorCols - 1) * colorBoxSpacing)) / 2;
        int startY = 40;
        // Matriz de cores
        for (int i = 0; i < 14; i++)
        {
            int linha = i / colorCols;
            int coluna = i % colorCols;
            int x = startX + coluna * (colorBoxSize + colorBoxSpacing);
            int y = startY + linha * (colorBoxSize + colorBoxSpacing);

            CV::color(i);
            CV::rectFill(x, y - colorBoxSize, x + colorBoxSize, y);

            if (i == corSelecionada)
            {
                CV::color(13);
                CV::rect(x - 2, y - colorBoxSize - 2, x + colorBoxSize + 2, y + 2);
            }
        }

        int numLinhasCores = (14 + colorCols - 1) / colorCols;
        int btnY = startY + numLinhasCores * (colorBoxSize + colorBoxSpacing) + controlBtnSpacing;
        int iconX = (menuWidth - 74) / 2;

        // Ícone pincel
        iconePincel->renderLocalizado(iconX, btnY);
        if (modo == 0)
        {
            CV::color(1);
            CV::rect(iconX - 2, btnY - 2, iconX + 34, btnY + 34);
        }

        // Ícone flip H
        iconeFlipH->renderLocalizado(iconX + 42, btnY);

        btnY += 32 + controlBtnSpacing;

        // Ícone borracha
        iconeBorracha->renderLocalizado(iconX, btnY);
        if (modo == 1)
        {
            CV::color(1);
            CV::rect(iconX - 2, btnY - 2, iconX + 34, btnY + 34);
        }

        // Ícone flip V
        iconeFlipV->renderLocalizado(iconX + 42, btnY);

        // Slider do tamanho do pincel
        btnY += 32 + controlBtnSpacing;
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

        // Desenho do Slider de Brilho...
        // --- Slider de Brilho ---
        int brilhoSliderX = sliderX + 100;
        int brilhoSliderY = sliderY;

        CV::color(0.5, 0.5, 0.5); // trilho
        CV::rectFill(brilhoSliderX, brilhoSliderY, brilhoSliderX + sliderWidth, brilhoSliderY + sliderHeight);

        // Posição do botão no slider de brilho
        float percentBrilho = (float)(brilhoSliderValue - brilhoSliderMin) / (brilhoSliderMax - brilhoSliderMin);
        int knobXBrilho = brilhoSliderX + percentBrilho * sliderWidth;

        CV::color(0); // botão do slider
        CV::rectFill(knobXBrilho - 4, brilhoSliderY - 4, knobXBrilho + 4, brilhoSliderY + sliderHeight + 4);

        //Label
        char labelBrilho[30];
        sprintf(labelBrilho, "Brilho: %d", brilhoSliderValue);
        CV::text(brilhoSliderX, brilhoSliderY + 18, labelBrilho);




        int btnFormaY = startY + numLinhasCores * (colorBoxSize + colorBoxSpacing - 8) + controlBtnSpacing;
        int btnFormaX = menuWidth/2 - (((shapeBtnWidth + 4)*8)/2);

        for (int i = 0; i < 7; i++)
        {
            CV::color(0.7, 0.7, 0.7);
            CV::rectFill(btnFormaX, btnFormaY, btnFormaX + shapeBtnWidth, btnFormaY + shapeBtnHeight);

            // Borda de destaque pra escolha do pincel
            if (tipoPincel == i)
            {
                CV::color(0);
                CV::rect(btnFormaX - 2, btnFormaY - 2, btnFormaX + shapeBtnWidth + 2, btnFormaY + shapeBtnHeight + 2);
            }

            CV::color(0, 0, 0);

            float centerX = btnFormaX + shapeBtnWidth / 2.0;
            float centerY = btnFormaY + shapeBtnHeight / 2.0;
            float size = shapeBtnHeight / 3.0;

            if (i == 0) // Círculo
            {
                CV::circleFill(centerX, centerY, size, 20);
            }
            else if (i == 1) // Quadrado
            {
                CV::rectFill(centerX - size, centerY - size, centerX + size, centerY + size);
            }
            else if (i == 2) // Triângulo
            {
                desenhoTrianguloMenu(centerX, centerY, size);
            }
            else if (i == 3) // Losango
            {
                desenhoLosanguloMenu(centerX, centerY, size);
            }
            else if (i == 4) // Folha espetada/estrela do capiroto
            {
                desenhoFolhaMenu(centerX, centerY, size);
            }
            else if (i == 5) // Anel
            {
                CV::circle(centerX, centerY, size, 20);
            }
            else if (i == 6) // Coração
            {
                desenhoCoracaoMenu(centerX, centerY, size);
            }

            btnFormaX += shapeBtnWidth + shapeBtnSpacing;
        }

        desenhaInserirImagem();
        desenhaPreferenciaCamada();
        desenhaScrollCamada();
        desenhaMaisMenos();
        camadas[idCamadaAtiva]->desenhaIsVisible();
    }



    void desenhoLosanguloMenu(int centerX, int centerY, float size)
    {

        float vx[4], vy[4];
        vx[0] = centerX;
        vy[0] = centerY - size;
        vx[1] = centerX + size;
        vy[1] = centerY;
        vx[2] = centerX;
        vy[2] = centerY + size;
        vx[3] = centerX - size;
        vy[3] = centerY;
        CV::polygonFill(vx, vy, 4);
    }



    void desenhoTrianguloMenu(int centerX, int centerY, float size)
    {

        float vx[3], vy[3];
        vx[0] = centerX;
        vy[0] = centerY + size;
        vx[1] = centerX - size;
        vy[1] = centerY - size;
        vx[2] = centerX + size;
        vy[2] = centerY - size;
        CV::polygonFill(vx, vy, 3);
    }

    void desenhoFolhaMenu(int centerX, int centerY, float size)
    {
        CV::line(centerX - size, centerY, centerX + size, centerY);
        CV::line(centerX, centerY - size, centerX, centerY + size);
        CV::line(centerX - size, centerY - size, centerX + size, centerY + size);
        CV::line(centerX - size, centerY + size, centerX + size, centerY - size);
    }

    void desenhoCoracaoMenu(int centerX, int centerY, float size)
    {
        float scale = size / 2;
                for (float t = 0; t <= 2 * 3.14; t += 0.1)
                {
                    float x = 16 * pow(sin(t), 3);
                    float y = 13 * cos(t) - 5 * cos(2 * t) - 2 * cos(3 * t) - cos(4 * t);
                    x *= scale * 0.05;
                    y *= scale * 0.05;
                    CV::point(centerX + x * scale, centerY + y * scale);
                }
    }

    void flipHorizontal()
    {
        int **matrizDesenho = camadas[idCamadaAtiva]->getMatrizDesenho();
        for (int i = 0; i < matrizHeight; i++)
        {
            for (int j = 0; j < matrizWidth / 2; j++)
            {
                int tmp = matrizDesenho[i][j];
                matrizDesenho[i][j] = matrizDesenho[i][matrizWidth - 1 - j];
                matrizDesenho[i][matrizWidth - 1 - j] = tmp;
            }
        }
        if(camadas[idCamadaAtiva]->getHasImage())
        {

            camadas[idCamadaAtiva]->getImage()->flipH();
            camadas[idCamadaAtiva]->setImgX0(matrizWidth-camadas[idCamadaAtiva]->getImgX0()+menuWidth);
        }
    }

    void flipVertical()
    {
        int **matrizDesenho = camadas[idCamadaAtiva]->getMatrizDesenho();
        for (int i = 0; i < matrizHeight / 2; i++)
        {
            for (int j = 0; j < matrizWidth; j++)
            {
                int tmp = matrizDesenho[i][j];
                matrizDesenho[i][j] = matrizDesenho[matrizHeight - 1 - i][j];
                matrizDesenho[matrizHeight - 1 - i][j] = tmp;
            }
        }
        if(camadas[idCamadaAtiva]->getHasImage())
        {

            camadas[idCamadaAtiva]->getImage()->flipV();
            camadas[idCamadaAtiva]->setImgY0(matrizHeight-camadas[idCamadaAtiva]->getImgY0()-camadas[idCamadaAtiva]->getImgHeight());
        }
    }

    void desenhaNaMatriz(int **matrizDesenho, int i, int j, int r, int cor, int tipoPincel)
    {
        switch (tipoPincel)
        {
            case 0: // Redondo
            for (int _x = -r; _x <= r; _x++)
                for (int _y = -r; _y <= r; _y++)
                {
                    int yi = i + _y;
                    int xj = j + _x;
                    if (yi >= 0 && yi < matrizHeight && xj >= 0 && xj < matrizWidth && _x * _x + _y * _y <= r * r)
                        matrizDesenho[yi][xj] = cor;
                }
            break;

            case 1: // Quadrado
            for (int yi = i - r; yi <= i + r; yi++)
            for (int xj = j - r; xj <= j + r; xj++)
                if (yi >= 0 && yi < matrizHeight && xj >= 0 && xj < matrizWidth)
                    matrizDesenho[yi][xj] = cor;
        break;

    case 2: // Triângulo (ponta para cima)
        for (int h = 0; h <= r; h++)
        {
            int inicio = j - h;
            int fim = j + h;
            int yi = i - h;
            for (int xj = inicio; xj <= fim; xj++)
                if (yi >= 0 && yi < matrizHeight && xj >= 0 && xj < matrizWidth)
                    matrizDesenho[yi][xj] = cor;
        }
        break;

    case 3: // Losango
        for (int dy = -r; dy <= r; dy++)
        {
            int largura = r - abs(dy);
            for (int dx = -largura; dx <= largura; dx++)
            {
                int yi = i + dy;
                int xj = j + dx;
                if (yi >= 0 && yi < matrizHeight && xj >= 0 && xj < matrizWidth)
                    matrizDesenho[yi][xj] = cor;
            }
        }
        break;

    case 4: // Folha
        for (int dx = -r; dx <= r; dx++)
        {
            int xj = j + dx;
            if (i >= 0 && i < matrizHeight && xj >= 0 && xj < matrizWidth)
                matrizDesenho[i][xj] = cor;

            int yi = i + dx;
            if (yi >= 0 && yi < matrizHeight && j >= 0 && j < matrizWidth)
                matrizDesenho[yi][j] = cor;

            int diag1_y = i + dx;
            int diag1_x = j + dx;
            if (diag1_y >= 0 && diag1_y < matrizHeight && diag1_x >= 0 && diag1_x < matrizWidth)
                matrizDesenho[diag1_y][diag1_x] = cor;

            int diag2_y = i + dx;
            int diag2_x = j - dx;
            if (diag2_y >= 0 && diag2_y < matrizHeight && diag2_x >= 0 && diag2_x < matrizWidth)
                matrizDesenho[diag2_y][diag2_x] = cor;
        }
        break;

    case 5: // Anel
        for (int _x = -r; _x <= r; _x++)
            for (int _y = -r; _y <= r; _y++)
            {
                int dist2 = _x * _x + _y * _y;
                if (dist2 <= r * r && dist2 >= (r - 2) * (r - 2))
                {
                    int yi = i + _y;
                    int xj = j + _x;
                    if (yi >= 0 && yi < matrizHeight && xj >= 0 && xj < matrizWidth)
                        matrizDesenho[yi][xj] = cor;
                }
            }
        break;

    case 6: // Coração
        {
            float scale = r / 17.0; // escala baseada no raioo

        for (float t = 0; t <= 2 * M_PI; t += 0.05)
        {
            float x = 16 * pow(sin(t), 3);
            float y = 13 * cos(t) - 5 * cos(2 * t) - 2 * cos(3 * t) - cos(4 * t);

            int xj = j + (int)(x * scale);
            int yi = i + (int)(y * scale);

            if (yi >= 0 && yi < matrizHeight && xj >= 0 && xj < matrizWidth)
                matrizDesenho[yi][xj] = cor;
        }

        // Preenchee um pouco mais ao redor pra parecer mais solido, mas ficou feio
        /*for (float t = 0; t <= 2 * M_PI; t += 0.05)
        {
            float x = 16 * pow(sin(t), 3);
            float y = 13 * cos(t) - 5 * cos(2 * t) - 2 * cos(3 * t) - cos(4 * t);

            int cx = j + (int)(x * scale);
            int cy = i - (int)(y * scale);

            for (int dx = -1; dx <= 1; dx++)
            {
                for (int dy = -1; dy <= 1; dy++)
                {
                    int xj = cx + dx;
                    int yi = cy + dy;
                    if (yi >= 0 && yi < matrizHeight && xj >= 0 && xj < matrizWidth)
                        matrizDesenho[yi][xj] = cor;
                }
            }
        }*/
        break;

        }


    /*case 7: // Raio/
        for (int d = 0; d < r; d++)
        {
            int yi = i - d;
            int xj = (d % 2 == 0) ? j + d : j - d;
            if (yi >= 0 && yi < matrizHeight && xj >= 0 && xj < matrizWidth)
                matrizDesenho[yi][xj] = cor;
        }
        break;*/
    }
}


    void desenhaMatrizNaTela(int **matrizDesenho)
    {
        for (int i = 0; i < matrizHeight; i++)
            for (int j = 0; j < matrizWidth; j++)
                if (matrizDesenho[i][j] != -1)
                {
                    CV::color(matrizDesenho[i][j]);
                    CV::point(j + menuWidth, i);
                }
    }

    void desenhaMatrizesNaTela()
    {

        for(int i = numeroCamadas-1; i>=0; i--)
        {
            if(camadas[i]->getAtiva()== 1)
                {
                    desenhaMatrizNaTela(grade);
                }
            if(camadas[i]->getVisivel() == 1)
            {
                if(camadas[i]->getHasImage())
                {
                    camadas[i]->getImage()->renderLocalizado(camadas[i]->getImgX0(), camadas[i]->getImgY0());
                }
                desenhaMatrizNaTela(camadas[i]->getMatrizDesenho());
            }
        }
    }
    void modificaBrilho()
    {
        int brilhoAtual = camadas[idCamadaAtiva]->getBrilho();

        if(brilhoAtual!=brilhoSliderValue)
        {
            if(brilhoAtual<brilhoSliderValue)
            {
                camadas[idCamadaAtiva]->getImage()->aumentaBrilho(brilhoSliderValue-brilhoAtual);

            }
            else
            {
                camadas[idCamadaAtiva]->getImage()->diminuiBrilho(brilhoAtual-brilhoSliderValue);

            }
            camadas[idCamadaAtiva]->setBrilho(brilhoSliderValue);
        }
    }
};

#endif

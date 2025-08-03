/* -------------- O que foi feito ----------------------

Foram cumpridas todas as exigências obrigatórias do trabalho e mais alguns extras.
Abaixo há uma breve descrição do cumprimento das exigências:

--------Obrigatórias--------

1.Carregamento de imagens BMP 24bits sem compactação


    Imagens coloridas em formato BMP são carregadas corretamente, inclusive em formato ímpar.


2.Menu lateral funcional implementado


    Interface com botões, checkbox e slider.


3.Camadas separadas para cada imagem


    Cada imagem é exibida em sua própria camada com transparência binária.


4.Desenho com o mouse sobre a camada ativa


    Ao pressionar o botão esquerdo, é possível pintar com um raio ao redor do mouse.


5.Identificação visual da camada ativa


    Apenas uma camada é ativa por vez e há destaque visual indicando qual é a ativa.


6.Possibilidade de trocar a cor do desenho


    Interface permite selecionar diferentes cores para desenhar.


7.Plano de fundo quadriculado visível


    Usado para representar a transparência da camada inferior.


8.Reposicionamento e ocultação de camadas


    Ordem de exibição das camadas pode ser alterada e camadas podem ser ocultadas.


9.Botões para trocar a camada ativa


    Interface permite alternar facilmente qual camada está recebendo modificações.


10.Implementação de efeitos na camada ativa:


    Flip Horizontal
    Flip Vertical
    Controle de Brilho


12.Componentes obrigatórios da interface foram utilizados corretamente:


    Botão funcional
    Checkbox com efeito aplicado
    Slider para controle (como brilho, por exemplo)


13.Uso de estruturas e/ou orientação a objetos


-------------Extras-----------
1.Diferentes tipos de pincel/formas para desenhar com o mouse

    O usuário pode escolher entre 7 diferentes formas ou tamanhos para utilizar ou a borracha.

2.Redimensionamento e movimentação das imagens

    As imagens das camadas podem ser movimentadas



----------------------- Codigo ---------------------------
    O código tem inicio na main.cpp original do professor pozzer com alguns ajustes
para identificação de mais funcionalidades do mouse bem como reconhecer outras classes e chama-las
através do render. Modifiquei algumas funcionalidades do bmp.cpp para poder renderizar as imagens
em locais diferentes de x=0 e y=0, para poder flipar as imagens junto a matriz, para alterar o brilho
e para renderizar imagens de tamanhos impares. Na gl_canvas foi adicionado uma nova cor apenas para a
grade não ser de uma tonalidade muito forte. Além disso, utilizei 2 arquivos extras, Camada.h e Tela.h,
a Camada é uma classe suporte para o Tela.h. O Tela.h começa com as variáveis e logo em seguida
o contrutor da classe, onde ocorre as inicializações. Na função renderTela(), acontece todas as renderizações
primeiramente por meio da função desenhaMenu() onde têm a lógica do menus e de algumas funções, na continuidade
da renderTela() há a continuação das ações realizadas mediante a interação do usuário com o canvas, tanto no menu,
tanto para a parte gráfica.


---------Como usar----------------
🎨 Seção de Camadas
    Botões “Camada: N”
         Exibem a lista das camadas carregadas. A camada ativa é destacada visualmente.
         A camada ativa é aquela que receberá os desenhos e alterações.


    ⬆️⬇️ (primeiros botões à esquerda)
        Funcionam como scroll do menu, permitindo visualizar outras camadas quando houver muitas.


    ⬆️⬇️ (segundos botões à direita)
        Servem para alterar a ordem de exibição das camadas (mudar a prioridade/visibilidade delas).


    [ + ] e [ - ]
    [ + ]: Adiciona uma nova camada vazia.
    [ - ]: Remove a camada atualmente ativa.


    Retângulos pretos acima da barra de brilho
        Ao clicar, carregam novas imagens BMP para a camada ativa.


    Quadrado com um ponto no meio
        Alterna a visibilidade da camada ativa (exibir ou ocultar).



🖌️ Seção de Desenho
    Formas pretas (quadrado, círculo, coração etc.)
     Selecionam o tipo de pincel para desenhar com o mouse.


    Tamanho (Tam)
     Slider que define o raio ou tamanho do pincel.


    Brilho
     Slider que altera o brilho da imagem na camada ativa.


    Ferramentas diagonais (formas com linhas azuis)
     Flip vertical e horizontal da camada ativa;


    🎨 Seletor de Cores
    Paleta colorida na parte inferior permite selecionar a cor atual de desenho.
     Ao clicar em uma cor, ela passa a ser usada ao pintar com o mouse.



📌 Uso Geral
    Para desenhar:
        Selecione a camada desejada, ative a visibilidade, selecione a forma, o tamanho do pincel e uma cor.
        Depois, clique e segure o botão esquerdo do mouse na tela de desenho.

    Para carregar imagens novas:
        Clique em um dos retângulos pretos (acima do brilho) com a camada desejada ativa.

    Para mover:
        Selecione a camada e arraste a imagem pressionando o botão direito do mouse.



*/
#include <GL/glut.h>
#include <GL/freeglut_ext.h> //callback da wheel do mouse.

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "gl_canvas2d.h"
#include "bmp.h"
#include "Tela.h"
#include "Camada.h"

Bmp *img1;
unsigned char *data;
Tela *screen;
// mouse
int mouseX;
int mouseY;
int isPressed;
int click;
int estado_anterior = -2;

void render()
{
    screen->renderTela(mouseX, mouseY, isPressed, click);
    click = 0;
}

// funcao chamada toda vez que uma tecla for pressionada.
void keyboard(int key)
{
}

// funcao chamada toda vez que uma tecla for liberada
void keyboardUp(int key)
{
}

// funcao para tratamento de mouse: cliques, movimentos e arrastos
void mouse(int button, int state, int wheel, int direction, int x, int y)
{
    //
    mouseX = x;
    mouseY = y;

    if (state == 0)
    {
        if(estado_anterior != 0)
        {
            click = 1;
        }
        if (button == 0)
        {
            printf("Botao Esquerdo: %d\n", button);
            isPressed = 1;
            printf("x: %d   y: %d\n", x, y);
        }
        else if (button == 2)
        {
            printf("Botao Direito: %d\n", button);
            isPressed = 2;
            printf("x: %d   y: %d\n", x, y);

        }
        else if(button == 1)
        {
            isPressed = 3;

        }
        else
        {
            printf("Botao Desconhecido: %d\n", button);
        }

        printf("\n\n");
    }
    if (state == 1)
    {
        isPressed = 0;

    }
    estado_anterior = state;
}

int main(void)
{

    /* img1 = new Bmp(".\\__LAB2\\images\\img3.bmp");
    // img1 = new Bmp(".\\__LAB2\\images\\img1.bmp");
    // img1 = new Bmp(".\\__LAB2\\images\\img2.bmp");
    img1->convertBGRtoRGB();
    data = img1->getImage(); */

    // inicializaMatrizDesenho(&matrizDesenho, screenWidth, screenHeight);
    screen = new Tela(1200, 650, 240);
    screen->initTela();

    //Teste



    CV::run();
}

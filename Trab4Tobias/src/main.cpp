/*
=============================================================================
                    3D MODELER - BEZIER SWEEP WITH NORMALS
=============================================================================

QUESITOS IMPLEMENTADOS:
✓ Editor de perfil 2D com curvas de Bézier
✓ Geração de modelo 3D por sweep rotacional
✓ Sweep helicoidal (helical sweep) 
✓ Visualização wireframe e sólida com Z-buffer
✓ Iluminação com modelo Phong (ambiente, difusa, especular)
✓ Cálculo e visualização de normais das faces
✓ Back-face culling
✓ Projeção perspectiva e ortográfica
✓ Controle interativo de câmera (órbita e zoom)
✓ Transformações 3D do modelo (translação e rotação)
✓ Controle interativo da fonte de luz
✓ Interface gráfica com botões e informações em tempo real
✓ Rasterização de triângulos com interpolação de cores
✓ Sistema de coordenadas 3D com eixos de referência

INSTRUÇÕES DE USO:

=== EDITOR DE PERFIL (Área Esquerda) ===
• Mouse Esquerdo + Arrastar: Move pontos de controle existentes
• Mouse Direito: Adiciona novo ponto de controle
• Tecla DEL/Backspace: Remove ponto de controle selecionado
• Mínimo de 2 pontos de controle necessários

=== CONTROLE DE CÂMERA 3D ===
• Setas do Teclado: Orbita a câmera ao redor do modelo
  - ← → : Rotação horizontal (Yaw)
  - ↑ ↓ : Rotação vertical (Pitch)
• Page Up/Page Down: Zoom da câmera (aproximar/afastar)
• Tecla P: Alterna entre projeção Perspectiva e Ortográfica

=== CONTROLE DO MODELO 3D ===
• Tecla M: Alterna entre mover MODELO ou mover LUZ
• WASD + QE: Translação do objeto selecionado (modelo ou luz)
  - W/S: Move para cima/baixo (eixo Y)
  - A/D: Move para esquerda/direita (eixo X)
  - Q/E: Move para frente/trás (eixo Z)
• JLIKUO: Rotação do modelo
  - J/L: Rotação em Y (esquerda/direita)
  - I/K: Rotação em X (cima/baixo)
  - U/O: Rotação em Z (anti-horário/horário)

=== PARÂMETROS DO MODELO ===
• Teclas -/=: Diminui/Aumenta amostras da curva de Bézier
• Teclas [/]: Diminui/Aumenta fatias de rotação do sweep
• Tecla H: Liga/Desliga modo Sweep Helicoidal
  - T/G: Aumenta/Diminui altura da hélice (pitch)
  - R/F: Aumenta/Diminui número de voltas da hélice

=== VISUALIZAÇÃO ===
• Tecla Z: Alterna entre renderização Wireframe e Sólida
• Tecla N: Liga/Desliga visualização das normais das faces
• Tecla B: Liga/Desliga Back-face Culling

=== INTERFACE ===
• Botões na área direita para acesso rápido às funções principais
• Informações em tempo real sobre parâmetros e posições
• Indicador visual da fonte de luz (sol amarelo)
• Eixos de coordenadas 3D para referência

=== RECURSOS TÉCNICOS ===
• Z-buffer para renderização correta de profundidade
• Iluminação Phong com componentes ambiente, difusa e especular
• Rasterização de triângulos com interpolação bilinear
• Transformações matriciais 4x4 completas
• Cálculo automático de normais das faces
• Detecção de colisão para interação com pontos de controle

NOTA: A fonte de luz é representada por um sol amarelo e pode ser movida
      independentemente do modelo. Use M para alternar o controle.

IMPORTANTE: O sweep rotacional gira o perfil ao redor do eixo Y. A 
            rotação depende de qual lado do eixo Y os pontos de controle
            estão posicionados no editor 2D e se a bezier cruza o eixo. Pontos à
            direita do eixo resultam em rotação diferente dos à esquerda.
*/

#include <GL/glut.h>
#include <GL/freeglut_ext.h> //callback da wheel do mouse.

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "gl_canvas2d.h"
#include "Frames.h"
#include "Tela.h"


float fps = 60;
int mouseX, mouseY, _key = -1; // _key initialized to -1
Frames frameCounter;
Tela *tela;
int isPressed = 0; // 0 = nenhum, 1 = esquerdo, 2 = direito

// largura e altura inicial da tela . Alteram com o redimensionamento de tela.
int screenWidth = 1280, screenHeight = 720;



void render()
{
    fps = frameCounter.getFrames(); // Atualiza o FPS
    tela->updateInputsAndState(fps, mouseX, mouseY, isPressed, _key); // Updated method call
    tela->render(); // Updated method call
}

// funcao chamada toda vez que uma tecla for pressionada.
void keyboard(int key)
{
    _key = key;
}

// funcao chamada toda vez que uma tecla for liberada
void keyboardUp(int key)
{
    if (_key == key)
    {
        _key = -1;
    }

}

// funcao para tratamento de mouse: cliques, movimentos e arrastos
void mouse(int button, int state, int wheel, int direction, int x, int y)
{

    mouseX = x; // guarda as coordenadas do mouse para exibir dentro da render()
    mouseY = y;

    //printf("\nmouse %d %d %d %d %d %d", button, state, wheel, direction, x, y);
    if (state == 0)
    {
        if (button == 0)
        {
            //Esquerdo
            isPressed = 1;
        }
        else if (button == 2)
        {
            //Direito
            isPressed = 2;
        }
        else
        {
            //Botao desconhecido
        }
    }
    if (state == 1)
    {
        isPressed = 0;
    }

}

int main(void)
{
    tela = new Tela(screenWidth, screenHeight);

    CV::run();
}

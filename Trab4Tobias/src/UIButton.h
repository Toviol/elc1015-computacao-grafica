/*
=============================================================================
                              UIBUTTON.H
=============================================================================

DESCRIÇÃO GERAL:
Este arquivo define a estrutura UIButton, que implementa um sistema simples
de botões para interface gráfica. Os botões suportam dois
tipos: normais (executam ação ao clicar) e toggle (alternam entre dois estados).

FUNCIONALIDADES:
• Detecção de hover do mouse sobre o botão
• Execução de callbacks quando clicado
• Suporte a botões toggle com estado visual diferenciado
• Renderização com cores diferentes para estados (normal, hover, toggle ativo)
• Texto centralizado no botão

MÉTODOS:
• UIButton(): Construtor que configura todas as propriedades do botão
• isMouseOver(): Verifica se o mouse está sobre o botão (hit testing)
• draw(): Renderiza o botão na tela com cores baseadas no estado atual

EXEMPLO DE USO:
UIButton btn("Meu Botão", 10, 10, 100, 30, [](){...}, true, false);
if (btn.isMouseOver(mouseX, mouseY)) { btn.isHovered = true; }
btn.draw();

=============================================================================
*/

#ifndef ___UIBUTTON__H___
#define ___UIBUTTON__H___

#include "gl_canvas2d.h"
#include <string>
#include <functional>

/**
 * Estrutura que representa um botão de interface gráfica.
 * Suporta botões normais e toggle com diferentes estados visuais.
 */
struct UIButton {
    std::string text;                    // Texto exibido no botão
    float x, y, width, height;           // Posição e dimensões do botão
    std::function<void()> onClick;       // Callback executado ao clicar
    bool isHovered;                      // Estado de hover do mouse
    bool isToggle;                       // Se é um botão toggle
    bool toggledState;                   // Estado atual do toggle

    /**
     * Construtor do botão
     * @param t Texto do botão
     * @param bx Posição X
     * @param by Posição Y  
     * @param bw Largura
     * @param bh Altura
     * @param func Função callback ao clicar
     * @param toggle Se é botão toggle (padrão: false)
     * @param initialToggleState Estado inicial do toggle (padrão: false)
     */
    UIButton(std::string t, float bx, float by, float bw, float bh, std::function<void()> func, bool toggle = false, bool initialToggleState = false)
        : text(t), x(bx), y(by), width(bw), height(bh), onClick(func), isHovered(false), isToggle(toggle), toggledState(initialToggleState) {}

    /**
     * Verifica se o mouse está sobre o botão (hit testing)
     * @param mouseX Coordenada X do mouse
     * @param mouseY Coordenada Y do mouse
     * @return true se mouse está sobre o botão
     */
    bool isMouseOver(int mouseX, int mouseY) const {
        return mouseX >= x && mouseX <= x + width &&
               mouseY >= y && mouseY <= y + height;
    }

    /**
     * Renderiza o botão na tela com cores baseadas no estado atual.
     * Cores: normal (cinza), hover (cinza claro), toggle ativo (verde)
     */
    void draw() const {
        if (isHovered) CV::color(0.7f, 0.7f, 0.7f);
        else CV::color(0.5f, 0.5f, 0.5f);
        if (isToggle && toggledState) CV::color(0.4f, 0.8f, 0.4f);

        CV::rectFill(x, y, x + width, y + height);
        CV::color(0,0,0);
        CV::rect(x, y, x + width, y + height);
        CV::color(1.0f, 1.0f, 1.0f);
        CV::text(x + 5, y + height / 2 + 5, text.c_str());
    }
};

#endif // ___UIBUTTON__H___
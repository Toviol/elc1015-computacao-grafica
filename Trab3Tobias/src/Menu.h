// Este arquivo define a classe Menu, responsável por gerenciar e renderizar
// a tela de menu principal do jogo. Ele inclui a lógica para criar botões,
// detectar interações do mouse (cliques e hover) e determinar a ação
// selecionada pelo usuário, como iniciar o jogo ou editar a pista.
#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

#include "gl_canvas2d.h"
#include <string>
#include <vector>

class Menu {
public:
    // Enumeração para representar as possíveis opções do menu.
    enum class MenuOption {
        NONE,
        START_GAME,
        EDIT_TRACK
    };

    // Estrutura para representar um botão no menu.
    struct Button {
        std::string text;
        float x, y, width, height;
        MenuOption option;
        bool isHovered;

        // Construtor para um botão.
        Button(std::string t, float bx, float by, float bw, float bh, MenuOption opt)
            : text(t), x(bx), y(by), width(bw), height(bh), option(opt), isHovered(false) {}

        // Verifica se as coordenadas do mouse estão dentro dos limites do botão.
        bool isClicked(int mouseX, int mouseY) const {
            return mouseX >= x && mouseX <= x + width &&
                   mouseY >= y && mouseY <= y + height;
        }

        // Desenha o botão na tela.
        void draw() const {
            if (isHovered) {
                CV::color(0.7f, 0.7f, 0.7f); 
            } else {
                CV::color(0.5f, 0.5f, 0.5f); 
            }
            CV::rectFill(x, y, x + width, y + height);
            CV::color(1.0f, 1.0f, 1.0f); 
            
            float textPosX = x + width / 2 - (text.length() * 8) / 2; 
            float textPosY = y + height / 2 - 8; 
            CV::textStroke(textPosX, textPosY, text.c_str(), 0.15f, 2.0f);
        }
    };

private:
    std::vector<Button> buttons;
    int screenWidth, screenHeight;

public:
    // Construtor da classe Menu.
    Menu(int sw, int sh) : screenWidth(sw), screenHeight(sh) {
        
        float buttonWidth = 250;
        float buttonHeight = 60;
        float spacing = 30;
        float startY = (screenHeight / 2.0f) - (2 * buttonHeight + spacing) / 2.0f;
        float buttonX = (screenWidth / 2.0f) - (buttonWidth / 2.0f);

        buttons.emplace_back("Iniciar Jogo", buttonX, startY + buttonHeight + spacing, buttonWidth, buttonHeight, MenuOption::START_GAME);
        buttons.emplace_back("Modificar Pista", buttonX, startY, buttonWidth, buttonHeight, MenuOption::EDIT_TRACK);
    }

    // Renderiza todos os elementos do menu.
    void render() {
        CV::clear(0.2f, 0.2f, 0.3f); 
        CV::color(1.0f, 1.0f, 1.0f);
        std::string title = "Chaos on Island";
        float titleX = screenWidth / 2.0f - (title.length() * 20) / 2.0f; 
        CV::textStroke(titleX, screenHeight * 0.7f, title.c_str(), 0.3f, 2.5f);

        for (const auto& button : buttons) {
            button.draw();
        }
    }

    // Processa um clique do mouse e retorna a opção do menu selecionada, se houver.
    MenuOption handleMouseClick(int mouseX, int mouseY) {
        for (const auto& button : buttons) {
            if (button.isClicked(mouseX, mouseY)) {
                return button.option;
            }
        }
        return MenuOption::NONE;
    }

    // Atualiza o estado de 'hover' dos botões com base na posição do mouse.
    void handleMouseMove(int mouseX, int mouseY) {
        for (auto& button : buttons) {
            button.isHovered = button.isClicked(mouseX, mouseY);
        }
    }
};

#endif // MENU_H_INCLUDED
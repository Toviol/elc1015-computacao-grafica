// Este arquivo define a classe Tela, que atua como o controlador principal
// do aplicativo. Ele gerencia os diferentes estados da aplicação (Menu, Jogo, Editor de Pista),
// lida com as transições entre esses estados, e coordena a renderização e
// atualização da lógica com base no estado atual. Também gerencia a pista global
// e os botões específicos do editor de pista.
#ifndef ___TELA__H___
#define ___TELA__H___

#include "gl_canvas2d.h"
#include "Menu.h"
#include "Game.h"
#include "Background.h" 
#include <ctime>   
#include <cstdlib> 
#include <vector> 
#include <string> 

class Tela {
public:
    // Enumeração para os diferentes estados da aplicação.
    enum class AppState {
        MENU,
        GAME_PLAYING,
        TRACK_EDITOR
    };

    // Estrutura para os botões do editor de pista.
    struct EditorButton { 
        // Enumeração para as ações dos botões do editor.
        enum class Action {
            ADD_INNER,
            ADD_OUTER,
            DELETE_POINT,
            RESET_TRACK 
        };

        std::string text;
        float x, y, width, height;
        Action action;
        bool isHovered;
        float r, g, b; 

        // Construtor para um botão do editor.
        EditorButton(std::string t, float bx, float by, float bw, float bh, Action act, float cr, float cg, float cb)
            : text(t), x(bx), y(by), width(bw), height(bh), action(act), isHovered(false), r(cr), g(cg), b(cb) {}

        // Verifica se as coordenadas do mouse estão dentro dos limites do botão.
        bool isClicked(int mouseX, int mouseY) const {
            return mouseX >= x && mouseX <= x + width &&
                   mouseY >= y && mouseY <= y + height;
        }

        // Desenha o botão do editor.
        void draw() const {
            if (isHovered) {
                CV::color(0.4f, 0.4f, 0.4f); 
            } else {
                CV::color(0.3f, 0.3f, 0.3f); 
            }
            CV::rectFill(x, y, x + width, y + height);
            
            
            CV::color(r, g, b);
            if (action == Action::DELETE_POINT) { 
                CV::textStroke(x + 10, y + height/2.0f - 8, "X", 0.15f, 2.0f);
            } else { 
                CV::textStroke(x + 10, y + height/2.0f - 8, "+", 0.15f, 2.0f);
            }

            CV::color(0.9f, 0.9f, 0.9f); 
            CV::textStroke(x + 35, y + height/2.0f - 7, text.c_str(), 0.1f, 1.5f);
        }
    };

private:
    AppState currentState;
    Menu* menuInstance;
    Game* gameInstance; 
    Track globalTrack; 

    std::vector<EditorButton> editorButtons; 

    std::string trackEditorErrorMessage; 
    bool displayTrackEditorError;      

    Background background; 

    int screenWidth, screenHeight;
    float currentFps;

    
    int mouseX, mouseY;
    int isMousePressed; 
    int prevMousePressed;
    int currentKey;

public:
    // Construtor da classe Tela.
    Tela(int sw, int sh)
        : 
          currentState(AppState::MENU),
          menuInstance(nullptr), 
          gameInstance(nullptr), 
          globalTrack(sw, sh, sw / 2.0f, sh / 2.0f, sh / 5.0f, sh / 2.0f, 10), 
          
          
          displayTrackEditorError(false), 
          background(sw, sh),             
          screenWidth(sw),                
          screenHeight(sh),               
          currentFps(60.0f),              
          mouseX(0),                      
          mouseY(0),                      
          isMousePressed(0),              
          prevMousePressed(0),            
          currentKey(-1)                  
    {
        srand(time(0)); 
        CV::init(&this->screenWidth, &this->screenHeight, "GTA 2D - Refatorado");
        menuInstance = new Menu(screenWidth, screenHeight);

        
        float btnWidth = 250;
        float btnHeight = 40;
        float btnX = screenWidth - btnWidth - 20; 
        float btnY = 50; 
        float btnSpacing = 15;

        editorButtons.emplace_back("Adicionar Ponto (Externo)", btnX, btnY, btnWidth, btnHeight, EditorButton::Action::ADD_OUTER, 0.2f, 0.5f, 1.0f); 
        btnY += btnHeight + btnSpacing;
        editorButtons.emplace_back("Adicionar Ponto (Interno)", btnX, btnY, btnWidth, btnHeight, EditorButton::Action::ADD_INNER, 1.0f, 0.3f, 0.3f); 
        btnY += btnHeight + btnSpacing + 10; 
        editorButtons.emplace_back("Excluir Ponto Selecionado", btnX, btnY, btnWidth, btnHeight, EditorButton::Action::DELETE_POINT, 1.0f, 0.2f, 0.2f); 
        btnY += btnHeight + btnSpacing + 20; 
        editorButtons.emplace_back("Restaurar Pista Original", btnX, btnY, btnWidth, btnHeight, EditorButton::Action::RESET_TRACK, 0.6f, 0.6f, 0.6f); 
    }

    // Destrutor da classe Tela.
    ~Tela() {
        delete menuInstance;
        delete gameInstance;
    }

    // Renderiza o conteúdo da tela com base no estado atual da aplicação.
    void render() {
        CV::clear(0.0f, 0.0f, 0.0f); 

        background.draw(screenWidth, screenHeight, globalTrack); 

        if (currentState == AppState::MENU) {
            if (menuInstance) {
                menuInstance->render();
            }
        } else if (currentState == AppState::GAME_PLAYING) {
            if (gameInstance) {
                gameInstance->render();
            }
        } else if (currentState == AppState::TRACK_EDITOR) {
            globalTrack.renderTrack(); 
            CV::color(1,1,1);
            const char* editorMsg = "Para confirmar mudancas pressione \"Enter\"";
            
            CV::textStroke(30, screenHeight - 30, editorMsg, 0.1f, 1.5f);

            
            for (const auto& btn : editorButtons) {
                btn.draw();
            }

            
            if (displayTrackEditorError) {
                CV::color(1.0f, 0.0f, 0.0f); 
                
                float errorMsgWidthEstimate = trackEditorErrorMessage.length() * 8; 
                CV::textStroke(screenWidth / 2.0f - errorMsgWidthEstimate / 2.0f, screenHeight - 60, trackEditorErrorMessage.c_str(), 0.15f, 2.0f);
            }
        }
    }

    // Atualiza os inputs (mouse, teclado) e o estado da aplicação.
    void updateInputsAndState(float fps, int mx, int my, int mouseStatus, int keyFromMain) {
        this->currentFps = fps;
        this->mouseX = mx;
        this->mouseY = my;
        this->isMousePressed = mouseStatus; 
        this->currentKey = keyFromMain;

        if (currentState == AppState::MENU) {
            if (menuInstance) menuInstance->handleMouseMove(mouseX, mouseY);
            if (isMousePressed == 1 && prevMousePressed == 0) { 
                Menu::MenuOption selected = menuInstance->handleMouseClick(mouseX, mouseY);
                if (selected == Menu::MenuOption::START_GAME) {
                    startGame();
                } else if (selected == Menu::MenuOption::EDIT_TRACK) {
                    startTrackEditor();
                }
            }
        } else if (currentState == AppState::GAME_PLAYING) {
            if (gameInstance) {
                gameInstance->updateInputs(fps, mouseX, mouseY, isMousePressed, currentKey);
                if (currentKey == 27 && gameInstance->getCurrentGameState() == Game::GameState::GAME_OVER) { 
                    returnToMenu();
                }
            }
        } else if (currentState == AppState::TRACK_EDITOR) {
            
            if (currentKey == 13) { 
                
                float tankWidthForValidation = 80.0f; 
                if (globalTrack.validateTrackWidth(tankWidthForValidation)) {
                    globalTrack.setControlPointsVisibility(false); 
                    currentState = AppState::MENU;
                    displayTrackEditorError = false; 
                } else {
                    trackEditorErrorMessage = "A largura da pista nao se adequa as dimensoes do tanque";
                    displayTrackEditorError = true;
                }
                currentKey = -1; 
            }

            
            bool buttonClickedThisFrame = false;
            for (auto& btn : editorButtons) {
                btn.isHovered = btn.isClicked(mouseX, mouseY);
                if (btn.isHovered && isMousePressed == 1 && prevMousePressed == 0) {
                    performEditorButtonAction(btn.action);
                    buttonClickedThisFrame = true;
                    break; 
                }
            }
            
            
            if (!buttonClickedThisFrame) {
                if (isMousePressed == 1 && prevMousePressed == 0) {
                    globalTrack.handleMousePress(mouseX, mouseY);
                } else if (isMousePressed == 1 && prevMousePressed == 1) { 
                    globalTrack.handleMouseDrag(mouseX, mouseY);
                } else if (isMousePressed == 0 && prevMousePressed == 1) { 
                    globalTrack.handleMouseRelease();
                }
            }
        }
        prevMousePressed = isMousePressed;
    }

    // Executa a ação associada a um botão do editor de pista.
    void performEditorButtonAction(EditorButton::Action action) {
        Vector2 defaultPos(screenWidth / 2.0f, screenHeight / 2.0f);
        Vector2 newPointPos = defaultPos; 
        displayTrackEditorError = false; 

        switch (action) {
            case EditorButton::Action::ADD_INNER:
                newPointPos = globalTrack.getLastUniqueControlPointPosition(true, defaultPos);
                newPointPos.x += 30; 
                newPointPos.y += 30;
                globalTrack.addControlPoint(true, newPointPos);
                break;
            case EditorButton::Action::ADD_OUTER:
                newPointPos = globalTrack.getLastUniqueControlPointPosition(false, defaultPos);
                newPointPos.x += 30;
                newPointPos.y -= 30; 
                globalTrack.addControlPoint(false, newPointPos);
                break;
            case EditorButton::Action::DELETE_POINT:
                globalTrack.deleteSelectedControlPoint();
                break;
            case EditorButton::Action::RESET_TRACK: 
                globalTrack = Track(screenWidth, screenHeight, screenWidth / 2.0f, screenHeight / 2.0f, screenHeight / 5.0f, screenHeight / 2.0f, 10);
                globalTrack.setControlPointsVisibility(true); 
                break;
        }
    }

private:
    // Inicia o estado de jogo.
    void startGame() {
        delete gameInstance; 
        gameInstance = new Game(screenWidth, screenHeight, globalTrack); 
        currentState = AppState::GAME_PLAYING;
    }

    // Inicia o estado de editor de pista.
    void startTrackEditor() {
        globalTrack.setControlPointsVisibility(true); 
        currentState = AppState::TRACK_EDITOR;
    }

    // Retorna ao menu principal.
    void returnToMenu() {
        
        
        delete gameInstance;
        gameInstance = nullptr;
        currentState = AppState::MENU;
    }
};

#endif

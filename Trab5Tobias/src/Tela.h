/*
DESCRIÇÃO GERAL:
Este arquivo define a classe Tela, que atua como o controlador principal do 
simulador de nave espacial. A classe gerencia a câmera 3D que navega em meio 
a asteroides, controlando movimento suave, renderização 3D com OpenGL, 
e iluminaçcão.

FUNCIONALIDADES PRINCIPAIS:
- Simulação de voo espacial com câmera livre
- Sistema de movimento baseado em vetores (posição, direção, velocidade)
- Geração procedural de campo de asteroides
- Renderização wireframe e sólida com iluminação
- Controle de FPS para animação suave
- Movimentação contínua (não baseada em refresh de teclado)

MÉTODOS PRINCIPAIS:
- render(): Renderiza toda a cena 3D
- updateInputsAndState(): Processa entrada contínua do usuário e física
- updateCameraMovement(): Atualiza posição e orientação da câmera
- generateAsteroids(): Gera campo de asteroides proceduralmente
- renderAsteroids(): Desenha asteroides com iluminação
- setupLighting(): Configura iluminação OpenGL
- drawSpaceship(): Renderiza indicador visual da nave (opcional)

MÉTODOS AUXILIARES:
- Vector3: Operações matemáticas vetoriais (normalizadas para Vector3.h)
- generateRandomPosition(): Gera posições aleatórias para asteroides
- drawWireframeSphere()/drawSolidSphere(): Renderização de esferas
- updateFPS(): Controla taxa de quadros


*/

#ifndef ___TELA_SPACEFLIGHT__H___
#define ___TELA_SPACEFLIGHT__H___

#include "Vector3.h"
#include "Asteroids.h"
#include "Compass.h"
#include "Sun.h"
#include "SistemaSolar.h"
#include <GL/glut.h>
#include <vector>
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <cstdio>

class Tela {
public:
    Tela(int sw, int sh);
    ~Tela() {}

    void render();

    void updateInputsAndState(float fps, int mx, int my, int mouseStatus, int keyFromMain);
private:
    // Constantes de configuração do simulador
    const float SHIP_SPEED = 50.0f;                    // Velocidade base da nave
    const float SHIP_ACCELERATION = 100.0f;            // Aceleração da nave
    const float SHIP_ROTATION_SPEED = 2.0f;            // Velocidade de rotação
    // Constantes de asteroides agora definidas em Asteroids.h:
    // - NUM_ASTEROIDS: Número de asteroides
    // - ASTEROID_FIELD_SIZE: Tamanho do campo de asteroides 
    // - MIN_ASTEROID_SCALE: Escala mínima dos asteroides
    // - MAX_ASTEROID_SCALE: Escala máxima dos asteroides

    // Variáveis de estado da aplicação
    int screenWidth, screenHeight;
    int mouseX, mouseY;
    int isMousePressed;
    int currentKey;
    int uniqueKey;                                      // Tecla pressionada uma única vez (evita spam)
    int lastKey;                                        // Última tecla para detectar mudanças
    float deltaTime;                                    // Tempo entre frames
    
    // Sistema de câmera/nave espacial baseado em vetores
    Vector3 cameraPosition;                             // Posição atual da câmera
    Vector3 cameraDirection;                            // Direção para onde a câmera aponta
    Vector3 cameraUp;                                   // Vetor "para cima" da câmera
    Vector3 cameraVelocity;                             // Velocidade atual da câmera
    float cameraYaw;                                    // Rotação horizontal (Y)
    float cameraPitch;                                  // Rotação vertical (X)
    
    // Campo de asteroides
    std::vector<Asteroid> asteroids;
    
    // Opções de renderização
    RenderMode renderMode;
    bool enableLighting;
    
    // Modos de visualização
    enum class ViewMode {
        BOTH,    // Ambos sistema solar e asteroides
        ASTEROIDS_ONLY,       // Apenas asteroides
        SOLAR_SYSTEM_ONLY                  // Apenas sistema solar
    };
    ViewMode currentViewMode;
    
    // Sistema de iluminação
    Vector3 lightPosition;
    Vector3 lightColor;
    Vector3 ambientLight;
    
    // Controle de FPS
    float fps;
    float targetFPS;
    
    // Estado do jogo
    bool gameRunning;
    Vector3 lastGenerationPosition;                     // Última posição onde asteroides foram gerados

    // === MÉTODOS DE MOVIMENTO E FÍSICA ===
    void updateCameraMovement();                        // Atualiza movimento da câmera
    Vector3 calculateCameraDirection();                 // Calcula direção baseada em yaw/pitch
    void processInput();                                // Processa input do teclado
    
    // === MÉTODOS DE RENDERIZAÇÃO ===
    void setupLighting();                              // Configura iluminação OpenGL (apenas uma vez)
    void renderScene();                                 // Renderiza cena principal
    void renderHUD();                                   // Renderiza informações na tela
    void renderTextInfo();                              // Renderiza informações de texto do HUD
    void renderText(int x, int y, const char* text, void* font); // Renderiza texto com sombra
    
    // === MÉTODOS DE CONTROLE ===
    void resetSimulation();                             // Reinicia simulação
    void toggleRenderMode();                            // Alterna wireframe/sólido
    void toggleViewMode();                              // Alterna modo de visualização
    void updateFPS(float currentFPS);                   // Atualiza controle de FPS
};

#endif // ___TELA_SPACEFLIGHT__H___
/*
=============================================================================
                                TELA.H
=============================================================================

DESCRIÇÃO GERAL:
Este arquivo define a classe Tela, que atua como o controlador principal do 
aplicativo de modelagem 3D. A classe gerencia toda a interface do usuário,
manipulação de pontos de controle para curvas de Bézier, geração de modelos 3D 
por varredura rotacional/helicoidal, e visualização desses modelos com diferentes 
projeções, transformações e iluminação.

A tela é dividida em duas áreas principais:
- Área Esquerda (40%): Editor 2D de perfil com curvas de Bézier
- Área Direita (60%): Visualização 3D do modelo gerado com controles de câmera

FUNCIONALIDADES PRINCIPAIS:
• Editor interativo de curvas de Bézier 2D
• Geração automática de modelos 3D por sweep rotacional e helicoidal
• Sistema completo de câmera 3D (órbita, zoom, projeções)
• Iluminação Phong com fonte de luz móvel
• Renderização wireframe e sólida com Z-buffer
• Interface com botões e informações em tempo real
• Sistema de transformações 3D para modelo e luz

MÉTODOS PRINCIPAIS:
• render(): Desenha toda a interface dividida em áreas
• updateInputsAndState(): Processa entrada do usuário (mouse, teclado)
• regenerateModel(): Gera modelo 3D a partir do perfil 2D
• draw3DArea(): Renderiza viewport 3D com modelo, câmera e iluminação
• drawControlArea(): Renderiza editor 2D com grid e pontos de controle
• drawSolidModel()/drawWireframeModel(): Renderização sólida/wireframe
• calculateLighting(): Implementa modelo de iluminação Phong
• projectToScreen(): Converte coordenadas 3D para 2D na tela
• setupUI(): Configura botões e interface do usuário

MÉTODOS AUXILIARES:
• screenToPanelLocal()/panelLocalToScreen(): Conversão de coordenadas 2D
• drawGrid(): Desenha grade de referência no editor 2D
• drawBezierProfile2D(): Visualiza curva de Bézier gerada
• drawControlPoints2D(): Desenha pontos de controle editáveis
• drawScreenDivision(): Divide tela em áreas funcionais
• updateHelpText(): Atualiza textos informativos da interface

=============================================================================
*/

#ifndef ___TELA_MODELING__H___
#define ___TELA_MODELING__H___

#include "gl_canvas2d.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Matrix4x4.h"
#include "UIButton.h"
#include "BezierGenerator.h"
#include "ModelGenerator.h"
#include "Camera3D.h"
#include "Rasterizer.h"
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <functional>

/**
 * Classe principal que gerencia toda a aplicação de modelagem 3D.
 * Controla a interface dividida entre editor 2D e visualizador 3D.
 */
class Tela {
public:
    enum class AppState {
        MODELING_EDITOR
    };

    /**
     * Construtor - Inicializa interface e dados padrão
     * @param sw Largura da tela
     * @param sh Altura da tela
     */
    Tela(int sw, int sh);
    ~Tela() {}

    /**
     * Método principal de renderização - desenha toda a interface
     */
    void render() {
        CV::clear(0.1f, 0.1f, 0.15f);
        drawScreenDivision();
        drawControlArea();
        draw3DArea();
        drawUI();
    }

    /**
     * Processa entrada do usuário e atualiza estado da aplicação
     * @param fps Taxa de quadros atual
     * @param mx Posição X do mouse
     * @param my Posição Y do mouse
     * @param mouseStatus Estado dos botões do mouse (0=nenhum, 1=esquerdo, 2=direito)
     * @param keyFromMain Tecla pressionada (-1 se nenhuma)
     */
    void updateInputsAndState(float fps, int mx, int my, int mouseStatus, int keyFromMain);

private:
    // Constantes de configuração
    const int MIN_CONTROL_POINTS = 2;
    const float CONTROL_POINT_RADIUS = 5.0f;
    const float NORMAL_DISPLAY_LENGTH = 10.0f;
    const float KEY_CAMERA_ORBIT_SPEED_RAD_PER_SEC = 1.5f;
    const float KEY_CAMERA_ZOOM_SPEED_UNITS_PER_SEC = 400.0f;
    const float KEY_MODEL_TRANSLATE_SPEED_UNITS_PER_SEC = 200.0f;
    const float KEY_MODEL_ROTATE_SPEED_RAD_PER_SEC = 1.5f;

    // Variáveis de estado da aplicação
    AppState currentState;
    int screenWidth, screenHeight;
    int mouseX, mouseY;
    int isMousePressed;
    int prevMousePressed;
    int currentKey;
    int prevKey = -1;
    
    // Dados do modelo Bézier/3D
    std::vector<Vector2> controlPoints2D;           // Pontos de controle do perfil 2D
    std::vector<Vector2> bezierProfilePoints;       // Pontos da curva de Bézier gerada
    ModelGenerator::Model3D currentModel;           // Modelo 3D atual

    int numBezierSamples = 20;      // Resolução da curva de Bézier
    int numRotationSlices = 16;     // Número de fatias na rotação
    int draggingPointIndex = -1;    // Índice do ponto sendo arrastado
    int selectedPointIndex = -1;    // Índice do ponto selecionado

    // Sistema de câmera 3D
    Camera3D camera;

    // Transformações do modelo
    Vector3 modelTranslation = {0, 0, 0};
    Vector3 modelRotation = {0, 0, 0};

    // Opções de renderização
    bool enableBackFaceCulling = false; 
    bool enableHelicalSweep = false;    
    float helicalPitch = 50.0f;         
    float numHelicalTurns = 1.0f;       
    bool showFaceNormals = false;
    bool enableSolidRendering = false;
    bool enableLighting = true;
    
    // Sistema de iluminação
    Vector3 lightPosition;      // Posição da fonte de luz
    bool movingLight;           // Controle se move luz ou modelo
    
    // Z-Buffer para renderização sólida
    ZBuffer zBuffer;
    
    // Interface do usuário
    std::vector<UIButton> uiButtons;
    std::string helpText;

    // Divisão da tela
    float leftAreaWidth = 0.4f;    // 40% para editor 2D
    float rightAreaWidth = 0.6f;   // 60% para visualização 3D

    // === MÉTODOS DE CONVERSÃO DE COORDENADAS ===
    Vector2 screenToPanelLocal(int mx, int my) const;           // Tela -> Coordenadas locais 2D
    Vector2 panelLocalToScreen(const Vector2& localPos) const;  // Coordenadas locais 2D -> Tela
    Vector2 projectToScreen(Vector3 worldPos, const Matrix4x4& model, const Matrix4x4& view, const Matrix4x4& proj); // 3D -> 2D

    // === MÉTODOS DE INTERFACE ===
    void setupUI();         // Configura botões da interface
    void updateHelpText();  // Atualiza textos informativos
    void drawUI();          // Desenha elementos da interface

    // === MÉTODOS DE MODELAGEM ===
    void regenerateModel(); // Gera modelo 3D a partir do perfil 2D

    // === MÉTODOS DE RENDERIZAÇÃO 3D ===
    void drawWireframeModel(const Matrix4x4& modelMatrix, const Matrix4x4& viewMatrix, const Matrix4x4& projectionMatrix, const Vector3& cameraWorldPos);
    void drawSolidModel(const Matrix4x4& modelMatrix, const Matrix4x4& viewMatrix, const Matrix4x4& projectionMatrix, const Vector3& cameraWorldPos);
    Vector3 calculateLighting(const Vector3& worldPos, const Vector3& normal, const Vector3& cameraPos, const Vector3& lightPos, const Vector3& baseColor);

    // === MÉTODOS DE INTERFACE 2D/3D ===
    void drawScreenDivision(); // Divide tela em áreas funcionais
    void drawControlArea();    // Desenha área do editor 2D
    void draw3DArea();         // Desenha área de visualização 3D

    // === MÉTODOS AUXILIARES DE DESENHO ===
    void drawGrid();              // Grade de referência no editor 2D
    void drawBezierProfile2D();   // Curva de Bézier no editor 2D
    void drawControlPoints2D();   // Pontos de controle editáveis
};

#endif // ___TELA_MODELING__H___


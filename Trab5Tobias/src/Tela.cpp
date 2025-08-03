#include "Tela.h"

// Construtor
Tela::Tela(int sw, int sh)
    : screenWidth(sw), screenHeight(sh),
      mouseX(0), mouseY(0), isMousePressed(0), currentKey(-1), uniqueKey(-1), lastKey(-1),
      deltaTime(0.0f),
      cameraPosition(0.0f, 100.0f, 250.0f),
      cameraDirection(0.0f, 0.0f, -1.0f),
      cameraUp(0.0f, 1.0f, 0.0f),
      cameraVelocity(0.0f, 0.0f, 0.0f),
      cameraYaw(0.0f), cameraPitch(0.0f),
      renderMode(RenderMode::SOLID),
      enableLighting(true),
      currentViewMode(ViewMode::ASTEROIDS_ONLY),  // Iniciar mostrando asteroides
      lightPosition(100.0f, 100.0f, 100.0f),
      lightColor(1.2f, 1.1f, 0.9f),  // Luz solar mais intensa e levemente amarelada
      ambientLight(0.4f, 0.4f, 0.5f), // Luz ambiente aumentada significativamente
      fps(60.0f), targetFPS(60.0f),
      gameRunning(true),
      lastGenerationPosition(0.0f, 0.0f, 10.0f)
{
    // Inicializa gerador de números aleatórios
    srand(static_cast<unsigned int>(time(nullptr)));
    
    // Gera campo de asteroides
    AsteroidManager::generateAsteroids(asteroids);
    
    // Inicializa sistema solar
    SistemaSolar::initialize(lightPosition);
    
    // Configura OpenGL
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    if (enableLighting) {
        setupLighting();
    }
}

// IMPLEMENTAÇÃO DO MÉTODO RENDER PRINCIPAL
void Tela::render() {
    // Limpa buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.1f, 1.0f); // Cor do espaço (azul escuro)
    
    // Configura matrizes
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // Configura perspectiva usando gluPerspective (mais seguro e confiável)
    float aspect = (float)screenWidth / (float)screenHeight;
    
    // Valida aspect ratio para evitar divisão por zero
    if (aspect <= 0.0f) {
        aspect = 1.0f; // Fallback para ratio quadrado
    }
    
    gluPerspective(45.0f, aspect, 0.1f, 1000.0f);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Usa gluLookAt - mais seguro e padrão da indústria
    // Calcula posição de destino baseada na direção da câmera
    Vector3 target = cameraPosition + cameraDirection;
    
    gluLookAt(
        cameraPosition.x, cameraPosition.y, cameraPosition.z,  // eye (posição da câmera)
        target.x, target.y, target.z,                          // center (para onde olhamos)
        cameraUp.x, cameraUp.y, cameraUp.z                     // up (vetor up da câmera)
    );
    
    // Renderiza cena
    renderScene();
    
    // Renderiza HUD
    renderHUD();
}

void Tela::updateInputsAndState(float fps, int mx, int my, int mouseStatus, int keyFromMain) {
    
    // Atualiza variáveis de estado
    this->mouseX = mx;
    this->mouseY = my;
    this->isMousePressed = mouseStatus;
    this->currentKey = keyFromMain;
    this->fps = fps;
    
    // Lógica do uniqueKey: detecta quando uma tecla é pressionada pela primeira vez
    // Se a tecla atual for diferente da última, é uma nova tecla sendo pressionada
    if (keyFromMain != lastKey) {
        uniqueKey = keyFromMain;  // Primeira vez que a tecla é pressionada
        lastKey = keyFromMain;    // Atualiza a última tecla
    } else {
        uniqueKey = -1;          // Tecla já estava sendo pressionada (evita spam)
    }
    
    // Calcula deltaTime
    this->deltaTime = (fps > 0.0f) ? (1.0f / fps) : (1.0f / 60.0f);
    this->deltaTime = std::min(deltaTime, 1.0f / 15.0f); // Max step for 15 FPS
    
    if (!gameRunning) return;
    
    // Processa input
    processInput();
    
    // Atualiza movimento da câmera
    updateCameraMovement();
    
    // Atualiza rotação dos asteroides
    AsteroidManager::updateAsteroidRotations(asteroids, deltaTime);
    
    // Atualiza sistema solar
    SistemaSolar::update(deltaTime, lightPosition); // Passa posição do sol para cálculo das órbitas

    // Atualiza controle de FPS
    updateFPS(fps);

}

// === MÉTODOS DE MOVIMENTO E FÍSICA ===

void Tela::updateCameraMovement() {
    // Aplica velocidade à posição
    cameraPosition = cameraPosition + cameraVelocity * deltaTime;
    
    // Recalcula direção baseada em yaw/pitch
    cameraDirection = calculateCameraDirection();
    
    // Recalcula vetor "right" e "up" usando produto vetorial
    // Right = direction × world_up
    Vector3 worldUp(0, 1, 0);
    Vector3 right = Vector3::cross(cameraDirection, worldUp);
    right.normalize();
    
    // Up = right × direction  
    cameraUp = Vector3::cross(right, cameraDirection);
    cameraUp.normalize();
}

Vector3 Tela::calculateCameraDirection() {
    Vector3 direction;
    // Usa convenção padrão: 
    // - Yaw = 0 aponta para -Z (frente)
    // - Yaw positivo gira para a direita
    // - Pitch positivo aponta para cima
    direction.x = cos(cameraPitch) * sin(cameraYaw);
    direction.y = sin(cameraPitch);  
    direction.z = -(cos(cameraPitch) * cos(cameraYaw)); // negativo para -Z ser frente
    direction.normalize();
    return direction;
}

void Tela::processInput() {
    Vector3 acceleration(0, 0, 0);
    
    // Movimento WASD
    if (currentKey == 'w' || currentKey == 'W') {
        acceleration = cameraDirection * SHIP_ACCELERATION;
    }
    if (currentKey == 's' || currentKey == 'S') {
        acceleration = cameraDirection * (-SHIP_ACCELERATION);
    }
    if (currentKey == 'a' || currentKey == 'A') {
        Vector3 left = Vector3::cross(cameraUp, cameraDirection);
        left.normalize();
        acceleration = left * SHIP_ACCELERATION;
    }
    if (currentKey == 'd' || currentKey == 'D') {
        Vector3 right = Vector3::cross(cameraDirection, cameraUp);
        right.normalize();
        acceleration = right * SHIP_ACCELERATION;
    }
    
    // Movimento vertical
    if (currentKey == 'q' || currentKey == 'Q') {
        acceleration = cameraUp * SHIP_ACCELERATION;
    }
    if (currentKey == 'e' || currentKey == 'E') {
        acceleration = cameraUp * (-SHIP_ACCELERATION);
    }
    
    // Rotação com setas
    if (currentKey == 200) { // Seta esquerda
        cameraYaw -= SHIP_ROTATION_SPEED * deltaTime;
    }
    if (currentKey == 202) { // Seta direita
        cameraYaw += SHIP_ROTATION_SPEED * deltaTime;
    }
    if (currentKey == 201) { // Seta cima
        cameraPitch += SHIP_ROTATION_SPEED * deltaTime;
    }
    if (currentKey == 203) { // Seta baixo
        cameraPitch -= SHIP_ROTATION_SPEED * deltaTime;
    }
    
    // Limitar pitch para evitar gimbal lock
    const float MAX_PITCH = 1.57f; // ~90 graus em radianos
    if (cameraPitch > MAX_PITCH) cameraPitch = MAX_PITCH;
    if (cameraPitch < -MAX_PITCH) cameraPitch = -MAX_PITCH;
    
    // Aplica aceleração à velocidade
    cameraVelocity = cameraVelocity + acceleration * deltaTime;
    
    // Aplica atrito/desaceleração baseado em tempo (independente do FPS)
    const float FRICTION_PER_SECOND = 0.1f; // Fator de atrito por segundo (0-1)
    float frictionThisFrame = pow(FRICTION_PER_SECOND, deltaTime);
    cameraVelocity = cameraVelocity * frictionThisFrame;
    
    // Controles especiais usando uniqueKey (evita spam automático)
    if (uniqueKey == 'r' || uniqueKey == 'R') {
        resetSimulation();
    }
    if (uniqueKey == 't' || uniqueKey == 'T') {
        toggleRenderMode();
    }
    if (uniqueKey == 'v' || uniqueKey == 'V') {
        toggleViewMode();
    }
}

// === MÉTODOS DE RENDERIZAÇÃO ===

void Tela::setupLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    // === LUZ PRINCIPAL (SOL) ===
    
    // Luz ambiente mais intensa para objetos no espaço
    GLfloat ambient[] = {0.4f, 0.4f, 0.5f, 1.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    
    // Luz difusa forte e amarelada
    GLfloat diffuse[] = {1.0f, 0.95f, 0.8f, 1.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    
    // Luz especular forte para reflexos maiss brilhantes
    GLfloat specular[] = {1.0f, 0.95f, 0.8f, 1.0f}; // Cor levemente amarelada
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

    // Atenuação da luz solar
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f); 
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.00005f);  
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.000005f); 
    

    // === LUZ DE PREENCHIMENTO ===
    glEnable(GL_LIGHT1);

    // Luz fill suave e azulada
    GLfloat fillAmbient[] = {0.1f, 0.1f, 0.15f, 1.0f};
    GLfloat fillDiffuse[] = {0.3f, 0.4f, 0.6f, 1.0f};
    GLfloat fillSpecular[] = {0.2f, 0.3f, 0.4f, 1.0f};
    
    glLightfv(GL_LIGHT1, GL_AMBIENT, fillAmbient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, fillDiffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, fillSpecular);
    
    // Atenuação para a luz de preenchimento
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.005f);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.001f);
    
    // === CONFIGURAÇÕES GLOBAIS DE ILUMINAÇÃO ===
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    
    // Modelo de iluminação global
    GLfloat global_ambient[] = {0.1f, 0.1f, 0.15f, 1.0f}; 
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);  
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);      // Iluminação bilateral


    // === POSIÇÃO DAS LUZES === (botei depois pois parece fazer diferença na ordem de renderização)
    GLfloat fillPosition[] = {-lightPosition.x * 0.6f, -lightPosition.y * 0.4f, -lightPosition.z * 0.8f, 1.0f};
    glLightfv(GL_LIGHT1, GL_POSITION, fillPosition);

    GLfloat position[] = {lightPosition.x, lightPosition.y, lightPosition.z, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    
    // Normalização automática para objetos escalados
    glEnable(GL_NORMALIZE);
    
}

void Tela::renderScene() {
    // Renderizar baseado no modo de visualização atual
    switch (currentViewMode) {
        case ViewMode::SOLAR_SYSTEM_ONLY:
            // Apenas sistema solar
            SistemaSolar::render(lightPosition, deltaTime);
            break;
            
        case ViewMode::ASTEROIDS_ONLY:
            // Apenas asteroides + sol
            Sun::render(lightPosition, deltaTime);
            AsteroidManager::renderAsteroids(asteroids, renderMode);
            break;
            
        case ViewMode::BOTH:
            // Sistema solar completo + asteroides
            SistemaSolar::render(lightPosition, deltaTime);
            AsteroidManager::renderAsteroids(asteroids, renderMode);
            break;
    }
}

void Tela::renderHUD() {
    // Salva estado das matrizes
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    
    // Configura projeção ortogonal para HUD
    glOrtho(0, screenWidth, 0, screenHeight, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    
    // Desabilita iluminação para o HUD
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING); 
    
    // Renderiza bússola
    Compass::render(screenWidth, screenHeight, cameraDirection, cameraUp);

    // Renderiza informações de texto
    renderTextInfo();
    
    // Restaura estado das matrizes
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glPopMatrix();
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    
    glMatrixMode(GL_MODELVIEW);
}


// === CONTROLE ===

void Tela::resetSimulation() {
    cameraPosition = Vector3(0.0f, 100.0f, 250.0f);
    cameraDirection = Vector3(0.0f, 0.0f, -1.0f);
    cameraUp = Vector3(0.0f, 1.0f, 0.0f);
    cameraVelocity = Vector3(0.0f, 0.0f, 0.0f);
    cameraYaw = 0.0f;
    cameraPitch = 0.0f;
    uniqueKey = -1;     
    lastKey = -1;
    gameRunning = true;
}

void Tela::toggleRenderMode() {
    renderMode = (renderMode == RenderMode::WIREFRAME) ? 
                 RenderMode::SOLID : RenderMode::WIREFRAME;
}

void Tela::toggleViewMode() {
    switch (currentViewMode) {
        case ViewMode::BOTH:
            currentViewMode = ViewMode::SOLAR_SYSTEM_ONLY;
            break;
        case ViewMode::SOLAR_SYSTEM_ONLY:
            currentViewMode = ViewMode::ASTEROIDS_ONLY;
            break;
        case ViewMode::ASTEROIDS_ONLY:
            currentViewMode = ViewMode::BOTH;
            break;
    }
}

void Tela::updateFPS(float currentFPS) {
    this->fps = currentFPS;
}

// === TEXTO/HUD ===

void Tela::renderTextInfo() {
    // Desenha fundo semi-transparente para melhor legibilidade
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Fundo do HUD (painel inferior)
    glColor4f(0.0f, 0.0f, 0.0f, 0.7f); // Preto semi-transparente
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(screenWidth, 0);
    glVertex2f(screenWidth, 120);
    glVertex2f(0, 120);
    glEnd();
    
    glDisable(GL_BLEND);
    
    // Configura cor do texto (branco brilhante)
    glColor3f(1.0f, 1.0f, 1.0f);
    
    // Layout organizado - otimizado para fontes bitmap
    int leftColumnX = 20;
    int rightColumnX = screenWidth / 2 + 20;
    int lineHeight = 18; // Espaçamento adequado para fontes bitmap
    int startY = 85;
    
    // Coluna esquerda - Informações da posição
    char coordText[256];
    sprintf(coordText, "Posicao: X:%.1f Y:%.1f Z:%.1f", 
            cameraPosition.x, cameraPosition.y, cameraPosition.z);
    renderText(leftColumnX, startY, coordText, GLUT_BITMAP_9_BY_15);
    
    // Velocidade
    float speed = cameraVelocity.length();
    char speedText[256];
    sprintf(speedText, "Velocidade: %.1f unidades/s", speed);
    renderText(leftColumnX, startY - lineHeight, speedText, GLUT_BITMAP_9_BY_15);
    
    // Modo de renderização
    char modeText[256];
    sprintf(modeText, "Render: %s", 
            (renderMode == RenderMode::WIREFRAME) ? "Wireframe" : "Solido");
    renderText(leftColumnX, startY - lineHeight * 2, modeText, GLUT_BITMAP_9_BY_15);
    
    // Modo de visualização
    char viewText[256];
    const char* viewModeStr = "";
    switch (currentViewMode) {
        case ViewMode::SOLAR_SYSTEM_ONLY: viewModeStr = "Sistema Solar"; break;
        case ViewMode::ASTEROIDS_ONLY: viewModeStr = "Asteroides"; break;
        case ViewMode::BOTH: viewModeStr = "Ambos"; break;
    }
    sprintf(viewText, "Visualizacao: %s", viewModeStr);
    renderText(leftColumnX, startY - lineHeight * 3, viewText, GLUT_BITMAP_9_BY_15);
    
    // Coluna direita - Controles
    renderText(rightColumnX, startY, "Controles:", GLUT_BITMAP_9_BY_15);
    renderText(rightColumnX, startY - lineHeight, "WASD: Mover  |  QE: Subir/Descer", GLUT_BITMAP_8_BY_13);
    renderText(rightColumnX, startY - lineHeight * 2, "Setas: Rotacao  |  T: Wireframe  |  R: Reset", GLUT_BITMAP_8_BY_13);
    renderText(rightColumnX, startY - lineHeight * 3, "V: Alternar Visualizacao", GLUT_BITMAP_8_BY_13);
    
    // Linha inferior - Status
    char statusText[256];
    sprintf(statusText, "Sistema Solar: %d Planetas  |  Asteroides: %d  |  FPS: %.0f", 
            (int)SistemaSolar::getPlanets().size(), NUM_ASTEROIDS, fps);
    renderText(leftColumnX, 20, statusText, GLUT_BITMAP_8_BY_13);
}

void Tela::renderText(int x, int y, const char* text, void* font) {

    // Renderiza "sombra"
    glColor3f(0.0f, 0.0f, 0.0f);
    glRasterPos2i(x + 1, y - 1);
    for (int i = 0; text[i] != '\0'; i++) {
        glutBitmapCharacter(font, text[i]);
    }
    
    // Renderiza texto principal (branco)
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2i(x, y);
    for (int i = 0; text[i] != '\0'; i++) {
        glutBitmapCharacter(font, text[i]);
    }
}


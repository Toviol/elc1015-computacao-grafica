#include "Tela.h"
//Maiores  detalhes sobre a classe Tela estão no arquivo Tela.h
//para melhor legibilidade do código.

// IMPLEMENTAÇÃO DO CONSTRUTOR
Tela::Tela(int sw, int sh)
    : currentState(AppState::MODELING_EDITOR),
      screenWidth(sw), screenHeight(sh),
      mouseX(0), mouseY(0), isMousePressed(0), prevMousePressed(0), currentKey(-1), prevKey(-1),  // ADICIONADO prevKey(-1)
      lightPosition(30.0f, 50.0f, 30.0f),
      movingLight(false),
      zBuffer(sw, sh)
{
    CV::init(&this->screenWidth, &this->screenHeight, "3D Modeler - Bezier Sweep");
    
    // Pontos de controle iniciais
    float panel_width = screenWidth * leftAreaWidth;
    float panel_height = static_cast<float>(screenHeight);
    float initial_placement_scale = std::min(panel_width, panel_height) * 0.3f;

    controlPoints2D.push_back(Vector2(-initial_placement_scale * 0.25f, -initial_placement_scale * 0.5f));
    controlPoints2D.push_back(Vector2( initial_placement_scale * 0.25f, -initial_placement_scale * 0.25f));
    controlPoints2D.push_back(Vector2( initial_placement_scale * 0.15f,  initial_placement_scale * 0.25f));
    controlPoints2D.push_back(Vector2(-initial_placement_scale * 0.2f,   initial_placement_scale * 0.5f));
    
    setupUI();
    regenerateModel();
}

void Tela::updateInputsAndState(float fps, int mx, int my, int mouseStatus, int keyFromMain) {
    this->mouseX = mx;
    this->mouseY = my;
    this->isMousePressed = mouseStatus;
    this->currentKey = keyFromMain;

    bool modelNeedsUpdate = false;
    float divisionX = screenWidth * leftAreaWidth;

    // Atualizar posições dos botões quando a tela redimensiona
    if (uiButtons.size() >= 5) {
        uiButtons[0].x = divisionX + 10;
        uiButtons[1].x = divisionX + 10;
        uiButtons[2].x = divisionX + 10;
        uiButtons[3].x = divisionX + 10;
        uiButtons[4].x = divisionX + 10;
    }

    // Calcula o deltaoTime
    float deltaTime = (fps > 0.0f) ? (1.0f / fps) : (1.0f / 60.0f);
    deltaTime = std::min(deltaTime, 1.0f / 15.0f); // Max step for 15 FPS

    // UI Button Interaction
    for (auto& button : uiButtons) {
        button.isHovered = button.isMouseOver(mouseX, mouseY);
        if (button.isHovered && isMousePressed == 1 && prevMousePressed == 0) {
            if (button.onClick) button.onClick();
        }
    }
    
    // Verificar se clicou em algum botão
    bool clickedOnButton = false;
    for(const auto& btn : uiButtons) {
        if(btn.isMouseOver(mouseX, mouseY)) {
            clickedOnButton = true;
            break;
        }
    }

    // Interação com pontos de controle - apenas na área esquerda
    if (mouseX < divisionX) {
        // Clique esquerdo - arrastar pontos existentes
        if (isMousePressed == 1 && prevMousePressed == 0) {
            if (!clickedOnButton) {
                draggingPointIndex = -1;
                selectedPointIndex = -1;
                for (int i = 0; i < (int)controlPoints2D.size(); ++i) {
                    Vector2 cp_screen = panelLocalToScreen(controlPoints2D[i]);
                    if (Vector2(mouseX, mouseY).distanceTo(cp_screen) < CONTROL_POINT_RADIUS * 2.0f) {
                        draggingPointIndex = i;
                        selectedPointIndex = i;
                        break;
                    }
                }
            }
        } else if (isMousePressed == 1 && draggingPointIndex != -1) {
            controlPoints2D[draggingPointIndex] = screenToPanelLocal(mouseX, mouseY);
            modelNeedsUpdate = true;
        } else if (isMousePressed == 0 && prevMousePressed == 1) {
            draggingPointIndex = -1;
        }
        // Clique direito - adicionar novo ponto de controle
        else if (isMousePressed == 2 && prevMousePressed == 0) {
            if (!clickedOnButton) {
                Vector2 newCpLocalPos = screenToPanelLocal(mouseX, mouseY);
                controlPoints2D.push_back(newCpLocalPos);
                selectedPointIndex = controlPoints2D.size() - 1;
                modelNeedsUpdate = true;
            }
        }
    }

    // Keyboard Input
    if (currentKey != -1) {
        float orbitAmount = KEY_CAMERA_ORBIT_SPEED_RAD_PER_SEC * deltaTime;
        float zoomAmount = KEY_CAMERA_ZOOM_SPEED_UNITS_PER_SEC * deltaTime;
        float transAmount = KEY_MODEL_TRANSLATE_SPEED_UNITS_PER_SEC * deltaTime;
        float rotAmount = KEY_MODEL_ROTATE_SPEED_RAD_PER_SEC * deltaTime;

        // FUNÇÃO AUXILIAR - Verificar se a tecla foi pressionada agora (não estava pressionada antes)
        auto isKeyJustPressed = [&](int key) -> bool {
            return (currentKey == key && prevKey != key);
        };

        // Camera Orbit (Arrow Keys)
        if (currentKey == 200) camera.orbitAngleY -= orbitAmount; // Left
        if (currentKey == 202) camera.orbitAngleY += orbitAmount; // Right
        if (currentKey == 201) camera.orbitAngleX -= orbitAmount; // Up
        if (currentKey == 203) camera.orbitAngleX += orbitAmount; // Down

        const float pitchLimit = (PI / 2.0f) - 0.01f; 
        camera.orbitAngleX = std::max(-pitchLimit, std::min(pitchLimit, camera.orbitAngleX));

        // Camera Zoom (PgUp/PgDn)
        if (currentKey == 204) camera.distance = std::max(10.0f, camera.distance - zoomAmount);
        if (currentKey == 205) camera.distance += zoomAmount;

        // TOGGLES
        if (isKeyJustPressed('m') || isKeyJustPressed('M')) {
            movingLight = !movingLight;
            updateHelpText();
        }

        // Model/Light Translation (WASDQE)
        if (!movingLight) {
            // Mover objeto
            if (currentKey == 'w') modelTranslation.y += transAmount;
            if (currentKey == 's') modelTranslation.y -= transAmount;
            if (currentKey == 'a') modelTranslation.x -= transAmount;
            if (currentKey == 'd') modelTranslation.x += transAmount;
            if (currentKey == 'q') modelTranslation.z += transAmount;
            if (currentKey == 'e') modelTranslation.z -= transAmount;
        } else {
            // Mover luz
            if (currentKey == 'w') lightPosition.y += transAmount;
            if (currentKey == 's') lightPosition.y -= transAmount;
            if (currentKey == 'a') lightPosition.x -= transAmount;
            if (currentKey == 'd') lightPosition.x += transAmount;
            if (currentKey == 'q') lightPosition.z += transAmount;
            if (currentKey == 'e') lightPosition.z -= transAmount;
        }

        // Model Rotation (JLIKUO)
        if (currentKey == 'j') modelRotation.y -= rotAmount;
        if (currentKey == 'l') modelRotation.y += rotAmount;
        if (currentKey == 'i') modelRotation.x -= rotAmount;
        if (currentKey == 'k') modelRotation.x += rotAmount;
        if (currentKey == 'u') modelRotation.z -= rotAmount;
        if (currentKey == 'o') modelRotation.z += rotAmount;

        // TOGGLES
        if (isKeyJustPressed('b') || isKeyJustPressed('B')) {
            enableBackFaceCulling = !enableBackFaceCulling;
            for (auto& btn : uiButtons) {
                if (btn.text.find("Backface Cull") != std::string::npos) {
                    btn.toggledState = enableBackFaceCulling;
                    break;
                }
            }
            updateHelpText();
        }

        if (isKeyJustPressed('n') || isKeyJustPressed('N')) {
            showFaceNormals = !showFaceNormals;
            for (auto& btn : uiButtons) {
                if (btn.text.find("Show Normals") != std::string::npos) {
                    btn.toggledState = showFaceNormals;
                    break;
                }
            }
            updateHelpText();
        }

        if (isKeyJustPressed('h') || isKeyJustPressed('H')) {
            enableHelicalSweep = !enableHelicalSweep;
            for (auto& btn : uiButtons) {
                if (btn.text.find("Helical Sweep") != std::string::npos) {
                    btn.toggledState = enableHelicalSweep;
                    break;
                }
            }
            modelNeedsUpdate = true;
            updateHelpText();
        }

        if (isKeyJustPressed('z') || isKeyJustPressed('Z')) {
            enableSolidRendering = !enableSolidRendering;
            for (auto& btn : uiButtons) {
                if (btn.text.find("Solid Render") != std::string::npos) {
                    btn.toggledState = enableSolidRendering;
                    break;
                }
            }
            updateHelpText();
        }

        if (isKeyJustPressed('p') || isKeyJustPressed('P')) {
            camera.usePerspectiveProjection = !camera.usePerspectiveProjection;
            for (auto& btn : uiButtons) {
                if (btn.text.find("Projection") != std::string::npos) {
                    btn.toggledState = camera.usePerspectiveProjection;
                    break;
                }
            }
            updateHelpText();
        }

        // Helical Sweep Parameters
        if (enableHelicalSweep) {
            float pitchStep = 5.0f;
            float turnStep = 0.1f;

            if (currentKey == 't' || currentKey == 'T') { 
                helicalPitch += pitchStep; 
                modelNeedsUpdate = true; 
            }
            if (currentKey == 'g' || currentKey == 'G') { 
                helicalPitch = std::max(0.0f, helicalPitch - pitchStep); 
                modelNeedsUpdate = true; 
            }
            if (currentKey == 'r' || currentKey == 'R') { 
                numHelicalTurns += turnStep; 
                if(numHelicalTurns < 0.05f) numHelicalTurns = 0.05f;
                modelNeedsUpdate = true; 
            }
            if (currentKey == 'f' || currentKey == 'F') { 
                numHelicalTurns -= turnStep;
                if (numHelicalTurns < 0.05f) numHelicalTurns = 0.0f;
                modelNeedsUpdate = true; 
            }
        }

        // Parameter changes
        if (currentKey == '=') { numBezierSamples++; modelNeedsUpdate = true; }
        if (currentKey == '-') { numBezierSamples = std::max(2, numBezierSamples - 1); modelNeedsUpdate = true; }
        if (currentKey == ']') { numRotationSlices++; modelNeedsUpdate = true; }
        if (currentKey == '[') { numRotationSlices = std::max(3, numRotationSlices - 1); modelNeedsUpdate = true; }
        
        // Delete point
        if (isKeyJustPressed(127) || isKeyJustPressed(8)) {
            if (selectedPointIndex != -1 && static_cast<int>(controlPoints2D.size()) > MIN_CONTROL_POINTS) {
                controlPoints2D.erase(controlPoints2D.begin() + selectedPointIndex);
                selectedPointIndex = -1;
                draggingPointIndex = -1;
                modelNeedsUpdate = true;
            }
        }
    }

    if (modelNeedsUpdate) {
        regenerateModel();
    }
    
    // ATUALIZAR ESTADOS ANTERIORES NO FINAL
    prevMousePressed = isMousePressed;
    prevKey = currentKey;  
}

Vector2 Tela::screenToPanelLocal(int mx, int my) const {
    float panel_center_x_screen = (screenWidth * leftAreaWidth) * 0.5f;
    float panel_center_y_screen = screenHeight * 0.5f;
    return Vector2(static_cast<float>(mx) - panel_center_x_screen, panel_center_y_screen - static_cast<float>(my));
}

Vector2 Tela::panelLocalToScreen(const Vector2& localPos) const {
    float panel_center_x_screen = (screenWidth * leftAreaWidth) * 0.5f;
    float panel_center_y_screen = screenHeight * 0.5f;
    return Vector2(panel_center_x_screen + localPos.x, panel_center_y_screen - localPos.y);
}

void Tela::setupUI() {
    float divisionX = screenWidth * leftAreaWidth;
    
    uiButtons.emplace_back("Toggle Projection (P)", divisionX + 10, 10, 220, 30, [this](){
        camera.usePerspectiveProjection = !camera.usePerspectiveProjection; 
        this->uiButtons[0].toggledState = camera.usePerspectiveProjection;
        updateHelpText();
    }, true, camera.usePerspectiveProjection);

    uiButtons.emplace_back("Backface Cull: OFF (B)", divisionX + 10, 50, 220, 30, [this](){
        enableBackFaceCulling = !enableBackFaceCulling;
        this->uiButtons[1].toggledState = enableBackFaceCulling; 
        updateHelpText();
    }, true, enableBackFaceCulling);

    uiButtons.emplace_back("Helical Sweep: OFF (H)", divisionX + 10, 90, 220, 30, [this](){
        enableHelicalSweep = !enableHelicalSweep;
        this->uiButtons[2].toggledState = enableHelicalSweep; 
        regenerateModel(); 
        updateHelpText();
    }, true, enableHelicalSweep);

    uiButtons.emplace_back("Show Normals: OFF (N)", divisionX + 10, 130, 220, 30, [this](){
        showFaceNormals = !showFaceNormals;
        this->uiButtons[3].toggledState = showFaceNormals;
        updateHelpText();
    }, true, showFaceNormals);

    uiButtons.emplace_back("Solid Render: OFF (Z)", divisionX + 10, 170, 220, 30, [this](){
        enableSolidRendering = !enableSolidRendering;
        this->uiButtons[4].toggledState = enableSolidRendering;
        updateHelpText();
    }, true, enableSolidRendering);

    updateHelpText();
}

void Tela::updateHelpText() {
    helpText = "Profile Editor:\n\n"
               "Mouse Left: Drag control point\n"
               "Mouse Right: Add control point\n"
               "DEL: Delete selected point\n\n"
               "3D Controls:\n"
               "Arrow Keys: Orbit Camera\n"
               "PgUp/PgDn: Zoom Camera\n"
               "M: Toggle Move " + std::string(movingLight ? "LIGHT" : "MODEL") + "\n"  // NOVO
               "WASDQE: Translate " + std::string(movingLight ? "Light" : "Model") + "\n"  // MODIFICADO
               "JLIKUO: Rotate Model\n\n"
               "Parameters:\n"
               "-/=: Bezier Samples (" + std::to_string(numBezierSamples) + ")\n"
               "[/]: Rotation Slices (" + std::to_string(numRotationSlices) + ")\n"
               "H: Helical Sweep (" + (enableHelicalSweep ? "ON" : "OFF") + ")";
    
    if (enableHelicalSweep) {
        std::ostringstream ssPitch, ssTurns;
        ssPitch << std::fixed << std::setprecision(1) << helicalPitch;
        ssTurns << std::fixed << std::setprecision(2) << numHelicalTurns;
        helpText += "\n  T/G: Helical Pitch (" + ssPitch.str() + ")";
        helpText += "\n  R/F: Helical Turns (" + ssTurns.str() + ")";
    }
    
    
    // Atualizar textos dos botões
    if (uiButtons.size() > 0) {
        uiButtons[0].text = camera.usePerspectiveProjection ? "Projection: Perspective (P)" : "Projection: Orthographic (P)";
    }
    if (uiButtons.size() > 1) {
        uiButtons[1].text = "Backface Cull: " + std::string(enableBackFaceCulling ? "ON" : "OFF") + " (B)";
    }
    if (uiButtons.size() > 2) {
        uiButtons[2].text = "Helical Sweep: " + std::string(enableHelicalSweep ? "ON" : "OFF") + " (H)";
    }
    if (uiButtons.size() > 3) {
        uiButtons[3].text = "Show Normals: " + std::string(showFaceNormals ? "ON" : "OFF") + " (N)";
    }
    if (uiButtons.size() > 4) {
        uiButtons[4].text = "Solid Render: " + std::string(enableSolidRendering ? "ON" : "OFF") + " (Z)";  // MUDANÇA: S -> Z
    }
}

void Tela::drawUI() {
    for (const auto& button : uiButtons) {
        button.draw();
    }
    
    CV::color(0.9f, 0.9f, 0.9f);
    
    std::stringstream ss(helpText);
    std::string line;
    float currentY = 20; 
    float lineHeight = 15; 

    while (std::getline(ss, line, '\n')) {
        CV::text(10, currentY, line.c_str());
        currentY += lineHeight;
    }
}

void Tela::regenerateModel() {
    if (controlPoints2D.size() < 2) {
        bezierProfilePoints.clear();
        currentModel.vertices.clear();
        currentModel.triangles.clear();
        return;
    }
    
    // Generate Bezier profile
    bezierProfilePoints = BezierGenerator::generateBezierProfile(controlPoints2D, numBezierSamples);
    
    // Generate 3D model
    currentModel = ModelGenerator::generateSweepModel(
        bezierProfilePoints, 
        numRotationSlices, 
        screenWidth, 
        leftAreaWidth,
        screenHeight,
        enableHelicalSweep,
        helicalPitch,
        numHelicalTurns
    );
    
    updateHelpText();
}

Vector2 Tela::projectToScreen(Vector3 worldPos, const Matrix4x4& model, const Matrix4x4& view, const Matrix4x4& proj) {
    Vector3 clipPos = proj.transformPoint(view.transformPoint(model.transformPoint(worldPos)));
    
    float divisionX = screenWidth * leftAreaWidth;
    float rightAreaWidth_calc = screenWidth - divisionX;
    
    float screenX = divisionX + (clipPos.x * 0.5f + 0.5f) * rightAreaWidth_calc;
    float screenY = (1.0f - (clipPos.y * 0.5f + 0.5f)) * screenHeight;
    return Vector2(screenX, screenY);
}

void Tela::drawWireframeModel(const Matrix4x4& modelMatrix, const Matrix4x4& viewMatrix, const Matrix4x4& projectionMatrix, const Vector3& cameraWorldPos) {
    CV::color(0.7f, 0.8f, 1.0f);
    for (const auto& triangleIndices : currentModel.triangles) {
        if (triangleIndices.size() != 3) continue;

        Vector3 v0_model = currentModel.vertices[triangleIndices[0]];
        Vector3 v1_model = currentModel.vertices[triangleIndices[1]];
        Vector3 v2_model = currentModel.vertices[triangleIndices[2]];

        Vector3 v0_world = modelMatrix.transformPoint(v0_model);
        Vector3 v1_world = modelMatrix.transformPoint(v1_model);
        Vector3 v2_world = modelMatrix.transformPoint(v2_model);

        Vector3 edge1 = v1_world - v0_world;
        Vector3 edge2 = v2_world - v0_world;
        Vector3 faceNormal_world = Vector3::cross(edge1, edge2);

        bool isBackFacing = false;
        if (enableBackFaceCulling) {
            Vector3 viewVector = cameraWorldPos - v0_world; 
            if (Vector3::dot(faceNormal_world, viewVector) <= 0.001f) { 
                isBackFacing = true;
            }
        }

        if (isBackFacing) {
            continue;
        }

        Vector2 p0_2D = projectToScreen(v0_model, modelMatrix, viewMatrix, projectionMatrix);
        Vector2 p1_2D = projectToScreen(v1_model, modelMatrix, viewMatrix, projectionMatrix);
        Vector2 p2_2D = projectToScreen(v2_model, modelMatrix, viewMatrix, projectionMatrix);

        float divisionX = screenWidth * leftAreaWidth;
        if (p0_2D.x >= divisionX && p1_2D.x >= divisionX && p2_2D.x >= divisionX) {
            CV::color(0.7f, 0.8f, 1.0f);
            CV::line(p0_2D, p1_2D);
            CV::line(p1_2D, p2_2D);
            CV::line(p2_2D, p0_2D);
        }

        if (showFaceNormals) {
            Vector3 centroid_world = (v0_world + v1_world + v2_world) / 3.0f;
            Vector3 normal_normalized = faceNormal_world;
            normal_normalized.normalize();
            Vector3 normal_end_world = centroid_world + normal_normalized * NORMAL_DISPLAY_LENGTH;

            Vector2 centroid_2D = projectToScreen(centroid_world, Matrix4x4(), viewMatrix, projectionMatrix);
            Vector2 normal_end_2D = projectToScreen(normal_end_world, Matrix4x4(), viewMatrix, projectionMatrix);

            if (centroid_2D.x >= divisionX && normal_end_2D.x >= divisionX) {
                CV::color(1.0f, 1.0f, 0.0f);
                CV::line(centroid_2D, normal_end_2D);
            }
        }
    }
}

void Tela::drawSolidModel(const Matrix4x4& modelMatrix, const Matrix4x4& viewMatrix, const Matrix4x4& projectionMatrix, const Vector3& cameraWorldPos) {
    float divisionX = screenWidth * leftAreaWidth;
    
    for (const auto& triangleIndices : currentModel.triangles) {
        if (triangleIndices.size() != 3) continue;

        Vector3 v0_model = currentModel.vertices[triangleIndices[0]];
        Vector3 v1_model = currentModel.vertices[triangleIndices[1]];
        Vector3 v2_model = currentModel.vertices[triangleIndices[2]];

        Vector3 v0_world = modelMatrix.transformPoint(v0_model);
        Vector3 v1_world = modelMatrix.transformPoint(v1_model);
        Vector3 v2_world = modelMatrix.transformPoint(v2_model);

        // Calcular normal da face
        Vector3 edge1 = v1_world - v0_world;
        Vector3 edge2 = v2_world - v0_world;
        Vector3 faceNormal_world = Vector3::cross(edge1, edge2);
        faceNormal_world.normalize();

        // Back-face culling
        bool isBackFacing = false;
        if (enableBackFaceCulling) {
            Vector3 viewVector = cameraWorldPos - v0_world; 
            if (Vector3::dot(faceNormal_world, viewVector) <= 0.001f) { 
                isBackFacing = true;
            }
        }

        if (isBackFacing) continue;

        // Projetar vértices para tela
        Vector2 p0_2D = projectToScreen(v0_model, modelMatrix, viewMatrix, projectionMatrix);
        Vector2 p1_2D = projectToScreen(v1_model, modelMatrix, viewMatrix, projectionMatrix);
        Vector2 p2_2D = projectToScreen(v2_model, modelMatrix, viewMatrix, projectionMatrix);

        // Verificar se está na área 3D
        if (p0_2D.x < divisionX || p1_2D.x < divisionX || p2_2D.x < divisionX) continue;

        // Calcular cores com iluminação - USANDO NOVA POSIÇÃO DA LUZ
        Vector3 baseColor(0.7f, 0.8f, 1.0f);
        Vector3 color0 = enableLighting ? calculateLighting(v0_world, faceNormal_world, cameraWorldPos, lightPosition, baseColor) : baseColor;
        Vector3 color1 = enableLighting ? calculateLighting(v1_world, faceNormal_world, cameraWorldPos, lightPosition, baseColor) : baseColor;
        Vector3 color2 = enableLighting ? calculateLighting(v2_world, faceNormal_world, cameraWorldPos, lightPosition, baseColor) : baseColor;

        // Preparar triângulo para rasterização
        Triangle2D tri;
        tri.v0 = p0_2D; tri.v1 = p1_2D; tri.v2 = p2_2D;
        
        // Calcular profundidades (Z em espaço de vista)
        Vector3 v0_view = viewMatrix.transformPoint(v0_world);
        Vector3 v1_view = viewMatrix.transformPoint(v1_world);
        Vector3 v2_view = viewMatrix.transformPoint(v2_world);
        tri.z0 = v0_view.z; tri.z1 = v1_view.z; tri.z2 = v2_view.z;
        
        tri.color0 = color0; tri.color1 = color1; tri.color2 = color2;
        tri.normal0 = faceNormal_world; tri.normal1 = faceNormal_world; tri.normal2 = faceNormal_world;
        tri.worldPos0 = v0_world; tri.worldPos1 = v1_world; tri.worldPos2 = v2_world;

        // Rasterizar triângulo
        std::vector<Fragment> fragments = Rasterizer::rasterizeTriangle(tri);
        
        // Renderizar fragmentos
        for (const Fragment& frag : fragments) {
            if (zBuffer.testAndSet(frag.x, frag.y, frag.depth)) {
                CV::color(frag.color.x, frag.color.y, frag.color.z);
                CV::point(frag.x, frag.y);
            }
        }

        // Desenhar normais se habilitado
        if (showFaceNormals) {
            Vector3 centroid_world = (v0_world + v1_world + v2_world) / 3.0f;
            Vector3 normal_end_world = centroid_world + faceNormal_world * NORMAL_DISPLAY_LENGTH;

            Vector2 centroid_2D = projectToScreen(centroid_world, Matrix4x4(), viewMatrix, projectionMatrix);
            Vector2 normal_end_2D = projectToScreen(normal_end_world, Matrix4x4(), viewMatrix, projectionMatrix);

            if (centroid_2D.x >= divisionX && normal_end_2D.x >= divisionX) {
                CV::color(1.0f, 1.0f, 0.0f);
                CV::line(centroid_2D, normal_end_2D);
            }
        }
    }
}

// NOVO - Método de iluminação modificado para receber posição da luz
Vector3 Tela::calculateLighting(const Vector3& worldPos, const Vector3& normal, const Vector3& cameraPos, const Vector3& lightPos, const Vector3& baseColor) {
    // Calcular direção da luz dinamicamente
    Vector3 lightDir = worldPos - lightPos;
    lightDir.normalize();
    
    // Componente ambiente
    Vector3 ambient = baseColor * 0.3f;
    
    // Componente difusa
    float diffuse = std::max(0.0f, -Vector3::dot(normal, lightDir));
    Vector3 diffuseColor = baseColor * diffuse * 0.7f;
    
    // Componente especular
    Vector3 viewDir = cameraPos - worldPos;
    viewDir.normalize();
    Vector3 reflectDir = lightDir - normal * (2.0f * Vector3::dot(lightDir, normal));
    reflectDir.normalize();
    float spec = std::pow(std::max(0.0f, Vector3::dot(viewDir, reflectDir)), 32.0f);
    Vector3 specular = Vector3(1.0f, 1.0f, 1.0f) * spec * 0.3f;
    
    Vector3 finalColor = ambient + diffuseColor + specular;
    
    // Clamping
    finalColor.x = std::min(1.0f, finalColor.x);
    finalColor.y = std::min(1.0f, finalColor.y);
    finalColor.z = std::min(1.0f, finalColor.z);
    
    return finalColor;
}

void Tela::drawScreenDivision() {
    float divisionX = screenWidth * leftAreaWidth;
    
    CV::color(0.3f, 0.3f, 0.4f);
    CV::line(divisionX, 0, divisionX, screenHeight);
    
    CV::color(0.8f, 0.8f, 0.9f);
    CV::text(10, 25, "Profile Editor");
    CV::text(divisionX + 10, 25, "3D Preview");
}

void Tela::drawControlArea() {
    float divisionX = screenWidth * leftAreaWidth;
    
    CV::color(0.15f, 0.15f, 0.2f);
    CV::rectFill(0, 0, divisionX, screenHeight);
    
    drawGrid();
    drawBezierProfile2D();
    drawControlPoints2D();
    
    if (controlPoints2D.size() >= 2) {
        CV::color(0.4f, 0.4f, 0.5f);
        for (int i = 0; i < (int)controlPoints2D.size() - 1; ++i) {
            Vector2 p0_screen = panelLocalToScreen(controlPoints2D[i]);
            Vector2 p1_screen = panelLocalToScreen(controlPoints2D[i+1]);
            CV::line(p0_screen, p1_screen);
        }
    }
    
    CV::color(0.8f, 0.8f, 0.8f);
    std::stringstream info;
    float infoOffsetY = 70.0f;
    float lineHeight = 20.0f;

    info.str("");
    info << "Control Points: " << controlPoints2D.size();
    CV::text(10, screenHeight - infoOffsetY, info.str().c_str());
    
    info.str("");
    info << "Bezier Samples: " << numBezierSamples;
    CV::text(10, screenHeight - infoOffsetY + lineHeight, info.str().c_str());
    
    info.str("");
    info << "Rotation Slices: " << numRotationSlices;
    CV::text(10, screenHeight - infoOffsetY + 2 * lineHeight, info.str().c_str());
}

void Tela::draw3DArea() {
    float divisionX = screenWidth * leftAreaWidth;
    float rightAreaWidth_calc = screenWidth - divisionX;
    
    // Clear Z-buffer
    zBuffer.resize(screenWidth, screenHeight);
    zBuffer.clear();
    
    CV::color(0.05f, 0.05f, 0.1f);
    CV::rectFill(divisionX, 0, screenWidth, screenHeight);
    
    CV::color(0.15f, 0.15f, 0.2f);
    int gridSize3D = std::max(30, (int)(std::min(rightAreaWidth_calc, (float)screenHeight) / 15));
    for (int x_grid = 0; x_grid < rightAreaWidth_calc; x_grid += gridSize3D) {
        CV::line(divisionX + x_grid, 0, divisionX + x_grid, screenHeight);
    }
    for (int y_grid = 0; y_grid < screenHeight; y_grid += gridSize3D) {
        CV::line(divisionX, y_grid, screenWidth, y_grid);
    }
    
    float centerX_2D_viewport = divisionX + rightAreaWidth_calc * 0.5f;
    float centerY_2D_viewport = screenHeight * 0.5f;
    float axisLength_2D_viewport = std::min(rightAreaWidth_calc, (float)screenHeight) * 0.05f;
    
    // EIXOS DE REFERÊNCIA - REATIVADOS
    CV::color(0.5f, 0.2f, 0.2f);
    CV::line(centerX_2D_viewport - axisLength_2D_viewport, centerY_2D_viewport, centerX_2D_viewport + axisLength_2D_viewport, centerY_2D_viewport);
    CV::color(0.2f, 0.5f, 0.2f);
    CV::line(centerX_2D_viewport, centerY_2D_viewport - axisLength_2D_viewport, centerX_2D_viewport, centerY_2D_viewport + axisLength_2D_viewport);
    
    Matrix4x4 viewMatrix = camera.buildViewMatrix();
    Matrix4x4 projectionMatrix = camera.buildProjectionMatrix(screenWidth, leftAreaWidth, screenHeight);
    Matrix4x4 modelMatrix = Matrix4x4::createTranslation(modelTranslation)
                           .multiply(Matrix4x4::createRotationY(modelRotation.y))
                           .multiply(Matrix4x4::createRotationX(modelRotation.x))
                           .multiply(Matrix4x4::createRotationZ(modelRotation.z));
    
    // Desenhar eixos 3D mundo
    Matrix4x4 identityMatrix;
    float worldAxisLength = 75.0f;
    Vector3 worldOrigin(0,0,0);
    Vector3 xAxisEndWorld(worldAxisLength, 0, 0);
    Vector3 yAxisEndWorld(0, worldAxisLength, 0);
    Vector3 zAxisEndWorld(0, 0, worldAxisLength);

    Vector2 screenOrigin = projectToScreen(worldOrigin, identityMatrix, viewMatrix, projectionMatrix);
    Vector2 screenX = projectToScreen(xAxisEndWorld, identityMatrix, viewMatrix, projectionMatrix);
    Vector2 screenY = projectToScreen(yAxisEndWorld, identityMatrix, viewMatrix, projectionMatrix);
    Vector2 screenZ = projectToScreen(zAxisEndWorld, identityMatrix, viewMatrix, projectionMatrix);

    auto is_in_3d_viewport = [&](const Vector2& p) {
        return p.x >= divisionX && p.x <= screenWidth && p.y >= 0 && p.y <= screenHeight;
    };

    if (is_in_3d_viewport(screenOrigin) && is_in_3d_viewport(screenX) && screenOrigin.distanceTo(screenX) > 1.0f) {
        CV::color(1.0f, 0.0f, 0.0f);
        CV::line(screenOrigin, screenX);
    }
    if (is_in_3d_viewport(screenOrigin) && is_in_3d_viewport(screenY) && screenOrigin.distanceTo(screenY) > 1.0f) {
        CV::color(0.0f, 1.0f, 0.0f);
        CV::line(screenOrigin, screenY);
    }
    if (is_in_3d_viewport(screenOrigin) && is_in_3d_viewport(screenZ) && screenOrigin.distanceTo(screenZ) > 1.0f) {
        CV::color(0.0f, 0.0f, 1.0f);
        CV::line(screenOrigin, screenZ);
    }
    
    // CALCULAR POSIÇÃO E PROFUNDIDADE DO SOL
    Vector3 lightViewSpace = viewMatrix.transformPoint(lightPosition);
    float lightDepth = lightViewSpace.z;
    Vector2 lightScreen = projectToScreen(lightPosition, identityMatrix, viewMatrix, projectionMatrix);
    
    // RENDERIZAR MODELO PRIMEIRO
    if (enableSolidRendering) {
        drawSolidModel(modelMatrix, viewMatrix, projectionMatrix, camera.currentWorldPos);
    } else {
        drawWireframeModel(modelMatrix, viewMatrix, projectionMatrix, camera.currentWorldPos);
    }
    
    // RENDERIZAR SOL DEPOIS DO MODELO - VERIFICANDO Z-BUFFER COM LÓGICA CORRETA
    if (is_in_3d_viewport(lightScreen)) {
        int sunX = (int)lightScreen.x;
        int sunY = (int)lightScreen.y;
        
        // Verificar se o sol deve ser visível comparando com o Z-buffer
        bool sunVisible = false;
        
        // Verificar se as coordenadas estão dentro dos limites do Z-buffer
        if (sunX >= 0 && sunX < screenWidth && sunY >= 0 && sunY < screenHeight) {
            // Obter o valor atual do Z-buffer nesta posição
            float currentDepth = zBuffer.getDepth(sunX, sunY);
            
            // CORRIGIDO: Se a profundidade do sol é MAIOR (mais distante) que o que está no Z-buffer, 
            // OU se não há nada no Z-buffer nesta posição (valor inicial), desenhar o sol
            if (lightDepth > currentDepth || currentDepth >= 999999.0f) {
                sunVisible = true;
            }
        } else {
            // Se está fora dos limites do Z-buffer, assumir que é visível
            sunVisible = true;
        }
        
        if (sunVisible) {
            // Desenhar sol
            CV::color(1.0f, 1.0f, 0.0f);
            CV::circleFill(lightScreen.x, lightScreen.y, 8, 8);
            
            // Desenhar raios do sol
            for (int i = 0; i < 8; i++) {
                float angle = (PI * 2.0f * i) / 8.0f;
                float rayLength = 15.0f;
                Vector2 rayEnd(lightScreen.x + cos(angle) * rayLength, 
                              lightScreen.y + sin(angle) * rayLength);
                CV::line(lightScreen.x + cos(angle) * 10, lightScreen.y + sin(angle) * 10,
                        rayEnd.x, rayEnd.y);
            }
        }
    }
    
    // INDICADOR DE MOVIMENTO DO SOL (sempre visível, independente do Z-buffer)
    if (movingLight && is_in_3d_viewport(lightScreen)) {
        CV::color(1.0f, 0.0f, 1.0f);
        CV::circle(lightScreen.x, lightScreen.y, 12, 8);
    }
    
    CV::color(0.7f, 0.7f, 0.7f);
    std::stringstream info;
    float infoOffsetY = 105.0f; // Aumentado para dar espaço aos botões
    float lineHeight = 20.0f;

    info.str("");
    info << "Camera Distance: " << std::fixed << std::setprecision(1) << camera.distance;
    CV::text(divisionX + 10, screenHeight - infoOffsetY, info.str().c_str());
    
    info.str("");
    info << "Projection: " << (camera.usePerspectiveProjection ? "Perspective" : "Orthographic");
    CV::text(divisionX + 10, screenHeight - infoOffsetY + lineHeight, info.str().c_str());
    
    info.str("");
    info << "Model Pos: (" << std::fixed << std::setprecision(0) 
         << modelTranslation.x << ", " << modelTranslation.y << ", " << modelTranslation.z << ")";
    CV::text(divisionX + 10, screenHeight - infoOffsetY + 2 * lineHeight, info.str().c_str());
    
    info.str("");
    info << "Light Pos: (" << std::fixed << std::setprecision(0) 
         << lightPosition.x << ", " << lightPosition.y << ", " << lightPosition.z << ")";
    CV::text(divisionX + 10, screenHeight - infoOffsetY + 3 * lineHeight, info.str().c_str());
    
    info.str("");
    info << "Moving: " << (movingLight ? "LIGHT (M to toggle)" : "MODEL (M to toggle)");
    CV::text(divisionX + 10, screenHeight - infoOffsetY + 4 * lineHeight, info.str().c_str());
}

void Tela::drawGrid() {
    float panel_width_pixels = screenWidth * leftAreaWidth;
    float panel_height_pixels = static_cast<float>(screenHeight);
    float panel_center_x_screen = panel_width_pixels * 0.5f;
    float panel_center_y_screen = panel_height_pixels * 0.5f;
    int gridSize = 20;

    CV::color(0.2f, 0.2f, 0.25f);

    // Vertical lines (local X)
    for (float x_local = 0; panel_center_x_screen + x_local < panel_width_pixels; x_local += gridSize) {
        Vector2 p1_screen = panelLocalToScreen(Vector2(x_local, -panel_height_pixels * 0.5f));
        Vector2 p2_screen = panelLocalToScreen(Vector2(x_local,  panel_height_pixels * 0.5f));
        CV::line(p1_screen, p2_screen);
        if (x_local > 0) {
             Vector2 p3_screen = panelLocalToScreen(Vector2(-x_local, -panel_height_pixels * 0.5f));
             Vector2 p4_screen = panelLocalToScreen(Vector2(-x_local,  panel_height_pixels * 0.5f));
             CV::line(p3_screen, p4_screen);
        }
    }
    // Horizontal lines (local Y)
    for (float y_local = 0; panel_center_y_screen + y_local < panel_height_pixels; y_local += gridSize) {
        Vector2 p1_screen = panelLocalToScreen(Vector2(-panel_width_pixels * 0.5f, y_local));
        Vector2 p2_screen = panelLocalToScreen(Vector2( panel_width_pixels * 0.5f, y_local));
        CV::line(p1_screen, p2_screen);
        if (y_local > 0) {
            Vector2 p3_screen = panelLocalToScreen(Vector2(-panel_width_pixels * 0.5f, -y_local));
            Vector2 p4_screen = panelLocalToScreen(Vector2( panel_width_pixels * 0.5f, -y_local));
            CV::line(p3_screen, p4_screen);
        }
    }
    // Center Axes
    CV::color(0.4f, 0.4f, 0.4f);
    CV::line(panelLocalToScreen(Vector2(-panel_width_pixels*0.5f, 0)), panelLocalToScreen(Vector2(panel_width_pixels*0.5f, 0)));
    CV::line(panelLocalToScreen(Vector2(0, -panel_height_pixels*0.5f)), panelLocalToScreen(Vector2(0, panel_height_pixels*0.5f)));
}

void Tela::drawBezierProfile2D() {
    if (bezierProfilePoints.size() < 2) return;
    CV::color(1.0f, 0.6f, 0.2f);
    for (size_t i = 0; i < bezierProfilePoints.size() - 1; ++i) {
        Vector2 p0_screen = panelLocalToScreen(bezierProfilePoints[i]);
        Vector2 p1_screen = panelLocalToScreen(bezierProfilePoints[i+1]);
        CV::line(p0_screen, p1_screen);
    }
}

void Tela::drawControlPoints2D() {
    for (int i = 0; i < (int)controlPoints2D.size(); ++i) {
        Vector2 cp_screen = panelLocalToScreen(controlPoints2D[i]);
        if (i == selectedPointIndex) CV::color(1,1,0);
        else CV::color(1,0,0);
        CV::circleFill(cp_screen.x, cp_screen.y, CONTROL_POINT_RADIUS, 10);
    }
}
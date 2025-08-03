// Este arquivo implementa os métodos da classe Enemy.
// Contém a lógica detalhada para o comportamento de cada tipo de inimigo,
// incluindo movimento, colisões, ataques (bombas, estilhaços) e renderização.
// As principais funcionalidades implementadas aqui são:
// - updateActivatedStar: Lógica de movimento e colisão para a estrela Nível 2.
// - updateHealthBarDisplay e drawHealthBar: Gerenciamento e desenho da barra de vida.
// - draw: Renderização visual dos diferentes tipos de inimigos.
// - generateShrapnel: Criação de projéteis de estilhaços para o inimigo Nível 3.
// - initializeFlightPath, updatePlaneSpecifics, dropBomb, drawPlaneVisuals:
//   Lógica completa para o inimigo avião Nível 4 (movimento, ataque, renderização).
#include "Enemies.h"
#include "Tank.h"       
#include "Track.h"      
#include "CollisionUtils.h" 
#include <cmath>        
#include <cstdlib>      

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Atualiza o estado da estrela ativada (Nível 2), incluindo rotação, movimento e colisões.
void Enemy::updateActivatedStar(float fps, const Track& track, std::vector<Enemy>& allEnemies, Tank* playerTank) {
    if (level != 2 || starMode != StarActivationState::ACTIVATED) {
        return;
    }

    
    currentRotationAngle += rotationSpeed * (1.0f / fps);
    while (currentRotationAngle >= 2.0f * M_PI) currentRotationAngle -= 2.0f * M_PI;
    while (currentRotationAngle < 0.0f) currentRotationAngle += 2.0f * M_PI;

    
    Vector2 deltaMove = movementDirection * movementSpeed * (1.0f / fps);
    
    
    updateDynamicHitboxVertices();

    
    if (playerTank && !playerTank->shield.isEffectActive() && CollisionUtils::checkPolygonPolygonCollision(this->vertices, playerTank->vertices)) {
        playerTank->takeDamage(5.0f);
        starMode = StarActivationState::DESTROYED_AFTER_ACTIVATION;
        playerTank->addExplosion(this->position, this->size / 10.0f); 
        return; 
    }

    
    Vector2 proposedPosition = position + deltaMove;
    std::vector<Vector2> projectedVertices; 
    float cosA = cos(currentRotationAngle);
    float sinA = sin(currentRotationAngle);
    if (!localStarShapeVertices.empty()){ 
        for(const auto& localP : localStarShapeVertices) {
            float rX = localP.x * cosA - localP.y * sinA;
            float rY = localP.x * sinA + localP.y * cosA;
            projectedVertices.push_back(Vector2(proposedPosition.x + rX, proposedPosition.y + rY));
        }
    }
    
    bool reflectedX = false;
    

    
    Vector2 posAfterXOnly = position + Vector2(deltaMove.x, 0);
    std::vector<Vector2> verticesAfterXOnly;
    if (!localStarShapeVertices.empty()){
        for(const auto& localP : localStarShapeVertices) {
            float rX = localP.x * cosA - localP.y * sinA;
            float rY = localP.x * sinA + localP.y * cosA;
            verticesAfterXOnly.push_back(Vector2(posAfterXOnly.x + rX, position.y + rY));
        }
    }
    for(const auto& v : verticesAfterXOnly) {
        if(!track.isPointInsideTrack(v)) {
            movementDirection.x *= -1.0f;
            reflectedX = true;
            break;
        }
    }
    
    
    
    float currentDeltaX = reflectedX ? movementDirection.x * movementSpeed * (1.0f/fps) : deltaMove.x;
    
    std::vector<Vector2> verticesAfterYOnly;
    if(!localStarShapeVertices.empty()){
        for(const auto& localP : localStarShapeVertices) {
            float rX = localP.x * cosA - localP.y * sinA;
            float rY = localP.x * sinA + localP.y * cosA;
            verticesAfterYOnly.push_back(Vector2(position.x + rX + currentDeltaX, 
                                                position.y + rY + deltaMove.y));
        }
    }
    for(const auto& v : verticesAfterYOnly) {
        if(!track.isPointInsideTrack(v)) {
            movementDirection.y *= -1.0f;
            
            break;
        }
    }

    
    deltaMove = movementDirection * movementSpeed * (1.0f / fps);
    
    
    proposedPosition = position + deltaMove;
    projectedVertices.clear();
    if(!localStarShapeVertices.empty()){
        for(const auto& localP : localStarShapeVertices) {
            float rX = localP.x * cosA - localP.y * sinA;
            float rY = localP.x * sinA + localP.y * cosA;
            projectedVertices.push_back(Vector2(proposedPosition.x + rX, proposedPosition.y + rY));
        }
    }

    for (auto& otherEnemy : allEnemies) {
        if (&otherEnemy == this || otherEnemy.isDestroyed()) continue;
        
        if (!projectedVertices.empty() && !otherEnemy.vertices.empty() && CollisionUtils::checkPolygonPolygonCollision(projectedVertices, otherEnemy.vertices)) {
            Vector2 normal = (otherEnemy.position - this->position).normalizedSafe();
            if (normal.lengthSquared() > 0.0001f) { 
                this->movementDirection = this->movementDirection.reflect(normal);

                if (otherEnemy.level == 2 && otherEnemy.starMode == StarActivationState::ACTIVATED) {
                    otherEnemy.movementDirection = otherEnemy.movementDirection.reflect(normal * -1.0f);
                }
            }
            deltaMove = movementDirection * movementSpeed * (1.0f / fps); 
            break; 
        }
    }
    
    
    position = position + deltaMove; 
    updateDynamicHitboxVertices(); 
}

// Atualiza o temporizador de visibilidade da barra de vida do inimigo.
void Enemy::updateHealthBarDisplay(float fps) {
    if (healthBarVisibleTimer > 0.0f) {
        healthBarVisibleTimer -= (1.0f / fps);
        if (healthBarVisibleTimer < 0.0f) {
            healthBarVisibleTimer = 0.0f;
        }
    }
}

// Desenha a barra de vida do inimigo, se visível.
void Enemy::drawHealthBar() const {
    if (healthBarVisibleTimer > 0.0f && initialHealth > 1 && health > 0 && !isDestroyed()) {
        float barWidth = size * 1.2f; 
        float barHeight = 4.0f;      
        
        
        float yOffset = size * 0.8f + 3.0f; 
        if (level == 4) yOffset = size * 0.5f + 8.0f; 

        Vector2 barPosition(position.x - barWidth / 2.0f, planeCurrentDisplayPosition.y + yOffset);
        if(level != 4) { 
             barPosition.set(position.x - barWidth / 2.0f, position.y + yOffset);
        }


        
        CV::color(0.25f, 0.25f, 0.25f); 
        CV::rectFill(barPosition.x, barPosition.y, barPosition.x + barWidth, barPosition.y + barHeight);

        
        float healthPercentage = static_cast<float>(health) / static_cast<float>(initialHealth);
        healthPercentage = std::max(0.0f, healthPercentage); 

        if (healthPercentage > 0.66f) CV::color(0.0f, 0.8f, 0.0f); 
        else if (healthPercentage > 0.33f) CV::color(1.0f, 1.0f, 0.0f); 
        else CV::color(1.0f, 0.0f, 0.0f); 
        
        CV::rectFill(barPosition.x, barPosition.y, barPosition.x + barWidth * healthPercentage, barPosition.y + barHeight);

        
        CV::color(0.1f, 0.1f, 0.1f); 
        CV::rect(barPosition.x, barPosition.y, barPosition.x + barWidth, barPosition.y + barHeight);
    }
}

// Desenha o inimigo na tela com base em seu nível e estado.
void Enemy::draw()
{
    
    if (level == 1) 
    {
        CV::color(0.9, 0.1, 0.1); 
        Vector2 p1(position.x, position.y - size);
        Vector2 p2(position.x - size * cos(M_PI / 6.0f), position.y + size * sin(M_PI / 6.0f));
        Vector2 p3(position.x + size * cos(M_PI / 6.0f), position.y + size * sin(M_PI / 6.0f));
        CV::triangleFill(p1, p2, p3);

        CV::color(0.6, 0.05, 0.05);
        float innerScale = 0.5f;
        Vector2 ip1(position.x, position.y - size * innerScale);
        Vector2 ip2(position.x - size * innerScale * cos(M_PI / 6.0f), position.y + size * innerScale * sin(M_PI / 6.0f));
        Vector2 ip3(position.x + size * innerScale * cos(M_PI / 6.0f), position.y + size * innerScale * sin(M_PI / 6.0f));
        CV::triangleFill(ip1, ip2, ip3);
    }
    else if (level == 2) 
    {
        CV::color(0.9, 0.9, 0.1); 

        
        
        
        
        

        if (!localStarShapeVertices.empty()) {
            
            
            std::vector<Vector2> worldStarPoints;
            float cosA = cos(currentRotationAngle); 
            float sinA = sin(currentRotationAngle);

            for (const auto& localP : localStarShapeVertices) {
                float rotatedX = localP.x * cosA - localP.y * sinA;
                float rotatedY = localP.x * sinA + localP.y * cosA;
                worldStarPoints.push_back(Vector2(position.x + rotatedX, position.y + rotatedY));
            }

            
            
            if (!worldStarPoints.empty()) { 
                for (size_t i = 0; i < worldStarPoints.size(); ++i)
                {
                    Vector2 p1_star = worldStarPoints[i];
                    Vector2 p2_star = worldStarPoints[(i + 1) % worldStarPoints.size()];
                    CV::triangleFill(this->position, p1_star, p2_star);
                }
            }
        } else {
            
            
            if (!this->vertices.empty()) {
                 std::vector<float> vxs, vys;
                 for(const auto& v_world : this->vertices) { 
                     vxs.push_back(v_world.x);
                     vys.push_back(v_world.y);
                 }
                 CV::polygonFill(vxs.data(), vys.data(), vxs.size());
            }
        }

        CV::color(0.6, 0.6, 0.05); 
        CV::circleFill(position.x, position.y, size * 0.15f, 12); 
    }
    else if (level == 3) 
    {
        CV::color(0.1, 0.8, 0.1); 
        int numSides = 8; 
        float bodyRadius = size * 0.8f;
        std::vector<Vector2> bodyPointsVec; 
        for (int i = 0; i < numSides; ++i)
        {
            float angle = (2.0f * M_PI / numSides) * i + (M_PI / numSides); 
            bodyPointsVec.push_back(Vector2(position.x + bodyRadius * cos(angle),
                                         position.y + bodyRadius * sin(angle)));
        }
        std::vector<float> bodyVx, bodyVy;
        for(const auto& p : bodyPointsVec) {
            bodyVx.push_back(p.x);
            bodyVy.push_back(p.y);
        }
        CV::polygonFill(bodyVx.data(), bodyVy.data(), bodyPointsVec.size()); 

        CV::color(0.05, 0.6, 0.05); 
        int numTeeth = 8;
        float toothOuterRadius = size;
        float toothInnerRadius = bodyRadius * 0.95f; 
        float toothWidthAngle = (2.0f * M_PI / numTeeth) * 0.3f; 

        for (int i = 0; i < numTeeth; ++i)
        {
            float baseAngle = (2.0f * M_PI / numTeeth) * i + (M_PI / numTeeth); 
            
            Vector2 p1_tooth(position.x + toothInnerRadius * cos(baseAngle - toothWidthAngle / 2.0f),
                       position.y + toothInnerRadius * sin(baseAngle - toothWidthAngle / 2.0f));
            Vector2 p2_tooth(position.x + toothOuterRadius * cos(baseAngle - toothWidthAngle / 3.0f), 
                       position.y + toothOuterRadius * sin(baseAngle - toothWidthAngle / 3.0f));
            Vector2 p3_tooth(position.x + toothOuterRadius * cos(baseAngle + toothWidthAngle / 3.0f),
                       position.y + toothOuterRadius * sin(baseAngle + toothWidthAngle / 3.0f));
            Vector2 p4_tooth(position.x + toothInnerRadius * cos(baseAngle + toothWidthAngle / 2.0f),
                       position.y + toothInnerRadius * sin(baseAngle + toothWidthAngle / 2.0f));
            
            std::vector<Vector2> toothShapeVec = {p1_tooth, p2_tooth, p3_tooth, p4_tooth}; 
            std::vector<float> toothVx, toothVy;
            for(const auto& p : toothShapeVec) {
                toothVx.push_back(p.x);
                toothVy.push_back(p.y);
            }
            CV::polygonFill(toothVx.data(), toothVy.data(), toothShapeVec.size()); 
        }

        CV::color(0.5, 0.5, 0.5); 
        CV::circleFill(position.x, position.y, size * 0.3f, 16);
        CV::color(0.3, 0.3, 0.3); 
        CV::circle(position.x, position.y, size * 0.3f, 16);
    }
    else if (level == 4) { 
        drawPlaneVisuals();
        for (const auto& bomb : activeBombs) {
            bomb.draw();
        }
    }

    
    drawHealthBar();
}

// Gera projéteis de estilhaços quando um inimigo do Nível 3 é destruído.
std::vector<Projectile> Enemy::generateShrapnel(float projectileSpeed, float projectileDamage, float shrapnelWidth, float shrapnelHeight) const
{
    std::vector<Projectile> shrapnels;
    if (this->level != 3) { 
        return shrapnels;
    }

    int numTeeth = 8;
    float toothOuterRadius = this->size; 

    for (int i = 0; i < numTeeth; ++i)
    {
        float shrapnelAngle = (2.0f * M_PI / numTeeth) * i + (M_PI / numTeeth);
        Vector2 shrapnelDirection(cos(shrapnelAngle), sin(shrapnelAngle));
        Vector2 shrapnelStartPosition = this->position + shrapnelDirection * (toothOuterRadius * 0.8f);

        shrapnels.emplace_back(
            shrapnelStartPosition,
            shrapnelDirection,
            projectileSpeed,
            shrapnelWidth,  
            shrapnelHeight, 
            Projectile::Owner::ENEMY_SHRAPNEL,
            projectileDamage
        );
    }
    return shrapnels;
}

// Inicializa a trajetória de voo para o inimigo avião (Nível 4).
void Enemy::initializeFlightPath(const Track& track) {
    if (level != 4 || planePathInitialized) return;

    flightPathNodes.clear();
    const auto& outerPoints = track.getOuterCurvePoints();
    const auto& innerPoints = track.getInnerCurvePoints();

    if (outerPoints.empty() || innerPoints.empty()) {
        position.set(track.getScreenWidth() / 2.0f, track.getScreenHeight() / 2.0f);
        planeCurrentDisplayPosition = position;
        flightPathNodes.push_back(position); 
        currentFlightPathNodeIndex = 0;
        planePathInitialized = true;
        return;
    }

    size_t numPathPoints = std::min(outerPoints.size(), innerPoints.size());
    if (numPathPoints == 0) { 
        position.set(track.getScreenWidth() / 2.0f, track.getScreenHeight() / 2.0f);
        planeCurrentDisplayPosition = position;
        flightPathNodes.push_back(position);
        currentFlightPathNodeIndex = 0;
        planePathInitialized = true;
        return;
    }
    
    float base_margin = 35.0f; 
    float actual_margin = base_margin + planePathOffsetSeed; 

    for (size_t i = 0; i < numPathPoints; ++i) {
        Vector2 midPoint = (outerPoints[i % outerPoints.size()] + innerPoints[i % innerPoints.size()]) * 0.5f;
        
        Vector2 p_prev = (outerPoints[(i + numPathPoints - 1) % numPathPoints] + innerPoints[(i + numPathPoints - 1) % numPathPoints]) * 0.5f;
        Vector2 p_next = (outerPoints[(i + 1) % numPathPoints] + innerPoints[(i + 1) % numPathPoints]) * 0.5f;
        Vector2 tangent = (p_next - p_prev).normalizedSafe();
        Vector2 normal_to_mid_path = tangent.perpendicular(); 

        Vector2 trackCenterApprox(0,0);
        if (!outerPoints.empty()) { 
            for(const auto& op : outerPoints) trackCenterApprox = trackCenterApprox + op;
            trackCenterApprox = trackCenterApprox * (1.0f / (float)outerPoints.size());
        }

        if ((midPoint + normal_to_mid_path * actual_margin - trackCenterApprox).lengthSquared() >
            (midPoint - normal_to_mid_path * actual_margin - trackCenterApprox).lengthSquared()) {
            normal_to_mid_path = normal_to_mid_path * -1.0f; 
        }
        
        flightPathNodes.push_back(midPoint + normal_to_mid_path * actual_margin);
    }
    
    if (!flightPathNodes.empty()) {
        currentFlightPathNodeIndex = rand() % flightPathNodes.size(); 
        position = flightPathNodes[currentFlightPathNodeIndex]; 
        planeCurrentDisplayPosition = position;
        planePathInitialized = true;
    } else {
        position.set(track.getScreenWidth() / 2.0f, track.getScreenHeight() / 2.0f);
        planeCurrentDisplayPosition = position;
        flightPathNodes.push_back(position);
        currentFlightPathNodeIndex = 0;
        planePathInitialized = true; 
    }
}

// Atualiza a lógica específica do inimigo avião (Nível 4), incluindo movimento, ataque e bombas.
void Enemy::updatePlaneSpecifics(float fps, const Track& track, Tank* playerTank) {
    if (level != 4) return;

    if (!planePathInitialized) {
        initializeFlightPath(track);
        if (!planePathInitialized || flightPathNodes.empty()) return; 
    }

    Vector2 targetNode = flightPathNodes[currentFlightPathNodeIndex];
    Vector2 directionToNode = (targetNode - position).normalizedSafe();

    movementDirection = directionToNode; 
    position = position + movementDirection * movementSpeed * (1.0f / fps);
    planeVisualDirection = movementDirection; 

    if ((targetNode - position).lengthSquared() < planeTargetReachedThresholdSq) {
        currentFlightPathNodeIndex = (currentFlightPathNodeIndex + 1) % flightPathNodes.size();
    }

    planeSineCycle += 2.5f * (1.0f / fps); 
    if (planeSineCycle > 2.0f * M_PI) planeSineCycle -= 2.0f * M_PI;
    
    float baseSineAmplitude = 25.0f; 
    float actualSineAmplitude = baseSineAmplitude * planeSineAmplitudeModifier; 
    Vector2 perpendicularDirection = movementDirection.perpendicular();
    Vector2 sineOffset = perpendicularDirection * sin(planeSineCycle) * actualSineAmplitude; 
    planeCurrentDisplayPosition = position + sineOffset;

    planeBombDropTimer -= (1.0f / fps);
    if (planeBombDropTimer <= 0.0f) {
        dropBomb();
        planeBombDropTimer = planeBombDropCooldownMax; 
    }

    for (auto& bomb : activeBombs) {
        bomb.update(fps);
    }

    for (auto it = activeBombs.begin(); it != activeBombs.end(); ) {
        if (!it->active) { 
            if (playerTank) { 
                playerTank->addExplosion(it->position, 2.5f); 
            }
            if (playerTank && !playerTank->isDestroyed() && !playerTank->shield.isEffectActive()) {
                if ((it->position - playerTank->pivot).lengthSquared() < (it->explosionRadius * it->explosionRadius)) {
                    playerTank->takeDamage(it->damage);
                }
            }
            it = activeBombs.erase(it);
        } else {
            ++it;
        }
    }
    updateDynamicHitboxVertices(); 
}

// Faz o inimigo avião (Nível 4) soltar uma bomba.
void Enemy::dropBomb() {
    if (level != 4) return;

    Vector2 bombStartPosition = planeCurrentDisplayPosition - planeVisualDirection * (size * 0.2f); 
    bombStartPosition.y += 5.0f; 

    Vector2 bombInitialVelocity = planeVisualDirection * -1.0f * 20.0f; 
    bombInitialVelocity.y += 30.0f; 
    
    float bombFallAngle = atan2(bombInitialVelocity.y, bombInitialVelocity.x) + M_PI/2.0f;

    activeBombs.emplace_back(bombStartPosition, bombInitialVelocity, bombFallAngle);
}

// Desenha os elementos visuais do inimigo avião (Nível 4).
void Enemy::drawPlaneVisuals() const {
    if (level != 4) return;

    glPushMatrix();
    glTranslatef(planeCurrentDisplayPosition.x, planeCurrentDisplayPosition.y, 0);
    float angleDegrees = atan2(planeVisualDirection.y, planeVisualDirection.x) * 180.0f / M_PI + 90.0f;
    glRotatef(angleDegrees, 0, 0, 1);

    float s = size * 0.8f; 

    float bodyColor[] = {0.5f, 0.55f, 0.5f}; 
    float wingColor[] = {0.4f, 0.45f, 0.4f}; 
    float cockpitColor[] = {0.2f, 0.3f, 0.4f};
    float propellerColor[] = {0.2f, 0.2f, 0.2f};
    float tailColor[] = {0.45f, 0.5f, 0.45f};

    CV::color(tailColor[0], tailColor[1], tailColor[2]);
    float tailFinPointsX[] = {0, -s*0.15f, s*0.15f};
    float tailFinPointsY[] = {s*0.9f, s*0.5f, s*0.5f}; 
    CV::polygonFill(tailFinPointsX, tailFinPointsY, 3);
    CV::rectFill(-s*0.4f, s*0.45f, s*0.4f, s*0.6f);

    CV::color(wingColor[0], wingColor[1], wingColor[2]);
    float wingLeftPointsX[] = {-s*0.2f, -s*1.0f, -s*0.9f, -s*0.15f};
    float wingLeftPointsY[] = {s*0.2f, -s*0.1f, -s*0.3f, -s*0.05f};
    CV::polygonFill(wingLeftPointsX, wingLeftPointsY, 4);
    float wingRightPointsX[] = {s*0.2f, s*1.0f, s*0.9f, s*0.15f};
    float wingRightPointsY[] = {s*0.2f, -s*0.1f, -s*0.3f, -s*0.05f};
    CV::polygonFill(wingRightPointsX, wingRightPointsY, 4);

    CV::color(bodyColor[0], bodyColor[1], bodyColor[2]);
    float fuselagePointsX[] = {-s*0.25f, s*0.25f, s*0.2f, 0, -s*0.2f}; 
    float fuselagePointsY[] = {-s*0.6f, -s*0.6f, s*0.65f, s*0.8f, s*0.65f};
    CV::polygonFill(fuselagePointsX, fuselagePointsY, 5);

    CV::color(cockpitColor[0], cockpitColor[1], cockpitColor[2]);
    CV::circleFill(0, -s*0.15f, s*0.18f, 16); 
    CV::rectFill(-s*0.15f, -s*0.4f, s*0.15f, -s*0.05f);

    CV::color(propellerColor[0], propellerColor[1], propellerColor[2]);
    CV::circleFill(0, -s*0.7f, s*0.1f, 8); 
    CV::rectFill(-s*0.05f, -s*0.95f, s*0.05f, -s*0.45f); 
    glPushMatrix();
    glRotatef(90, 0,0,1); 
    CV::rectFill(-s*0.05f, -s*0.95f, s*0.05f, -s*0.45f); 
    glPopMatrix();

    glPopMatrix(); 
}
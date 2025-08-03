// Este arquivo define a classe NitroBoost, que representa a habilidade de nitro do tanque.
// A classe gerencia o estado do nitro, incluindo seu nível de carga,
// tempo de ativação, duração máxima, cooldowns e o multiplicador de velocidade aplicado.
// Também lida com a lógica para ativar o nitro e a animação do efeito de chama.
// Inclui funcionalidades para:
// - Atualizar o estado do nitro (carga, duração, cooldowns, animação).
// - Ativar o nitro.
// - Fornecer o multiplicador de velocidade.
// - Desenhar a interface do usuário (UI) para a habilidade de nitro.
// - Desenhar o efeito visual da chama do nitro.
// A função drawNitroFlameSprite é uma auxiliar para renderizar a animação da chama.
#ifndef NITROBOOST_H_INCLUDED
#define NITROBOOST_H_INCLUDED

#include "gl_canvas2d.h"
#include "Vector2.h"
#include <cmath>   
#include <vector>  
#include <cstdlib> 


void inline drawNitroFlameSprite(Vector2 basePosition, Vector2 direction, int frame, float scale);

class NitroBoost {
public:
    float chargeLevel;          
    const float maxChargeTime;  
    float activeTimer;          
    const float maxActiveDuration; 
    bool isActive;
    bool isCharging;            
    float postUseCooldown;      
    float currentPostUseCooldown;

    const float speedMultiplier;

    
    int currentEffectFrame;
    float effectFrameDuration;
    float timeSinceLastEffectFrame;
    static const int totalEffectFrames = 5; 

    float uiPulseAccumulator; 

public:
    // Construtor da classe NitroBoost. Inicializa os parâmetros do nitro.
    NitroBoost() :
        chargeLevel(0.0f), maxChargeTime(5.0f),
        activeTimer(0.0f), maxActiveDuration(3.0f),
        isActive(false), isCharging(true), 
        postUseCooldown(1.0f), currentPostUseCooldown(0.0f),
        speedMultiplier(2.0f),
        currentEffectFrame(0), effectFrameDuration(0.05f), timeSinceLastEffectFrame(0.0f),
        uiPulseAccumulator(0.0f) 
    {
    }

    // Atualiza o estado do nitro, incluindo carga, tempo ativo, cooldowns e animações.
    void update(float fps) {
        if (fps <= 0) return;

        uiPulseAccumulator += 5.0f * (1.0f / fps); 
        if (uiPulseAccumulator > 2.0f * (float)M_PI) { 
            uiPulseAccumulator -= 2.0f * (float)M_PI;
        }

        if (isActive) {
            activeTimer += 1.0f / fps;
            timeSinceLastEffectFrame += 1.0f / fps;
            if (timeSinceLastEffectFrame >= effectFrameDuration) {
                currentEffectFrame = (currentEffectFrame + 1) % totalEffectFrames;
                timeSinceLastEffectFrame = 0.0f;
            }
            if (activeTimer >= maxActiveDuration) {
                isActive = false;
                activeTimer = 0.0f;
                chargeLevel = 0.0f; 
                isCharging = false; 
                currentPostUseCooldown = postUseCooldown; 
            }
        } else {
            if (currentPostUseCooldown > 0.0f) {
                currentPostUseCooldown -= 1.0f / fps;
                if (currentPostUseCooldown <= 0.0f) {
                    isCharging = true; 
                }
            } else if (isCharging && chargeLevel < maxChargeTime) {
                chargeLevel += 1.0f / fps;
                if (chargeLevel > maxChargeTime) {
                    chargeLevel = maxChargeTime;
                }
            }
        }
    }

    // Tenta ativar o nitro. Retorna true se ativado com sucesso, false caso contrário.
    // O nitro só pode ser ativado se não estiver ativo e estiver totalmente carregado.
    bool activate() {
        if (!isActive && chargeLevel >= maxChargeTime) {
            isActive = true;
            activeTimer = 0.0f;
            isCharging = false; 
            currentEffectFrame = 0; 
            timeSinceLastEffectFrame = 0.0f;
            return true;
        }
        return false;
    }

    // Verifica se o nitro está atualmente ativo.
    bool isNitroActive() const {
        return isActive;
    }

    // Retorna o multiplicador de velocidade. É maior que 1.0 se o nitro estiver ativo.
    float getSpeedMultiplier() const {
        return isActive ? speedMultiplier : 1.0f;
    }

    // Desenha a interface do usuário (UI) para a habilidade de nitro.
    // Mostra o nível de carga e um ícone representativo de chama.
    void drawUI(float x, float y, float width, float height) const {
        
        CV::color(0.1f, 0.1f, 0.15f); 
        CV::rectFill(x, y, x + width, y + height);

        
        CV::color(0.3f, 0.3f, 0.4f);
        CV::rect(x, y, x + width, y + height);

        
        float fillHeight = (chargeLevel / maxChargeTime) * height;
        if (chargeLevel >= maxChargeTime) {
            CV::color(0.2f, 0.8f, 1.0f); 
        } else {
            CV::color(0.1f, 0.4f, 0.6f); 
        }
        CV::rectFill(x, y + (height - fillHeight), x + width, y + height);

        
        float iconCenterX = x + width / 2.0f;
        float iconBaseY = y + height * 0.2f; 
        float iconTipY = y + height * 0.8f;  
        float iconWidth = width * 0.5f;      

        float flameR = 1.0f, flameG = 0.5f, flameB = 0.0f; 
        float innerFlameR = 1.0f, innerFlameG = 1.0f, innerFlameB = 0.5f; 

        if (chargeLevel >= maxChargeTime) { 
            float pulseFactor = (sin(uiPulseAccumulator) + 1.0f) / 2.0f; 
            flameR = 0.8f + 0.2f * pulseFactor;
            flameG = 0.4f + 0.2f * pulseFactor;
            innerFlameG = 0.7f + 0.3f * pulseFactor;
            innerFlameB = 0.3f + 0.3f * pulseFactor;
            CV::color(flameR, flameG, flameB);
        } else {
            CV::color(0.8f, 0.4f, 0.0f);
        }

        Vector2 baseLeft(iconCenterX - iconWidth / 2, iconBaseY);
        Vector2 baseRight(iconCenterX + iconWidth / 2, iconBaseY);
        Vector2 tip(iconCenterX, iconTipY);
        Vector2 midLeft(iconCenterX - iconWidth / 3, iconBaseY + (iconTipY - iconBaseY) * 0.5f);
        Vector2 midRight(iconCenterX + iconWidth / 3, iconBaseY + (iconTipY - iconBaseY) * 0.5f);
        
        
        std::vector<Vector2> flameVertices;
        flameVertices.push_back(baseLeft);
        flameVertices.push_back(midLeft);
        flameVertices.push_back(tip);
        flameVertices.push_back(midRight);
        flameVertices.push_back(baseRight);

        int numFlameVertices = flameVertices.size();
        float flameVx[numFlameVertices];
        float flameVy[numFlameVertices];
        for(int i=0; i < numFlameVertices; ++i) {
            flameVx[i] = flameVertices[i].x;
            flameVy[i] = flameVertices[i].y;
        }
        CV::polygonFill(flameVx, flameVy, numFlameVertices);


        if (chargeLevel >= maxChargeTime) {
             CV::color(innerFlameR, innerFlameG, innerFlameB);
        } else {
            CV::color(0.9f, 0.8f, 0.3f); 
        }
        float innerFlameWidth = iconWidth * 0.5f;
        Vector2 innerBaseLeft(iconCenterX - innerFlameWidth / 2, iconBaseY);
        Vector2 innerBaseRight(iconCenterX + innerFlameWidth / 2, iconBaseY);
        Vector2 innerTip(iconCenterX, iconBaseY + (iconTipY - iconBaseY) * 0.65f); 

        std::vector<Vector2> innerFlameVertices;
        innerFlameVertices.push_back(innerBaseLeft);
        innerFlameVertices.push_back(innerTip);
        innerFlameVertices.push_back(innerBaseRight);
        
        int numInnerFlameVertices = innerFlameVertices.size();
        float innerFlameVx[numInnerFlameVertices];
        float innerFlameVy[numInnerFlameVertices];
        for(int i=0; i < numInnerFlameVertices; ++i) {
            innerFlameVx[i] = innerFlameVertices[i].x;
            innerFlameVy[i] = innerFlameVertices[i].y;
        }
        CV::polygonFill(innerFlameVx, innerFlameVy, numInnerFlameVertices);
    }

    // Desenha o efeito visual da chama do nitro na posição e direção especificadas.
    void drawEffect(Vector2 basePosition, Vector2 flameDirection, float scale) const { 
        if (!isActive) return;
        
        drawNitroFlameSprite(basePosition, flameDirection, currentEffectFrame, scale);
    }
};

// Desenha um quadro específico da animação da chama do nitro.
// Utiliza diferentes formas e cores para simular o efeito de chama.
void drawNitroFlameSprite(Vector2 basePosition, Vector2 direction, int frame, float scale) {
    
    direction.normalize();

    
    const float brightBlue[] = {0.3f, 0.7f, 1.0f};
    const float midBlue[]    = {0.1f, 0.4f, 0.8f};
    const float whiteHot[]   = {0.9f, 0.9f, 1.0f};
    const float darkBlue[]   = {0.0f, 0.2f, 0.5f};

    
    float baseLength = 25.0f * scale; 
    float baseWidth = 12.0f * scale;  

    
    
    Vector2 perpendicular = direction.perpendicular();

    
    float currentLength = baseLength * (1.0f + (rand() % 10 - 5) / 15.0f); 
    currentLength *= (1.0f + (float)frame / NitroBoost::totalEffectFrames * 0.6f); 
    float currentWidth = baseWidth * (1.0f + (rand() % 10 - 5) / 20.0f); 
    
    currentWidth *= (1.0f - (float)frame / NitroBoost::totalEffectFrames * 0.2f); 

    Vector2 p_rear_left  = basePosition - perpendicular * (currentWidth / 2.0f);
    Vector2 p_rear_right = basePosition + perpendicular * (currentWidth / 2.0f);
    Vector2 p_tip        = basePosition + direction * currentLength;
    Vector2 p_mid_core   = basePosition + direction * (currentLength * 0.3f); 


    
    switch (frame % NitroBoost::totalEffectFrames) { 
        case 0: 
        case 4: 
            CV::color(whiteHot[0], whiteHot[1], whiteHot[2]);
            CV::triangleFill(p_rear_left, p_rear_right, p_mid_core + direction * 7 * scale);
            CV::color(brightBlue[0], brightBlue[1], brightBlue[2]);
            CV::triangleFill(p_rear_left, p_rear_right, p_tip);
            break;
        case 1:
        case 3: 
            CV::color(whiteHot[0], whiteHot[1], whiteHot[2]);
            CV::triangleFill(p_rear_left  + perpendicular * 1.5f * scale, 
                             p_rear_right - perpendicular * 1.5f * scale,
                             p_mid_core + direction * 10 * scale);
            CV::color(brightBlue[0], brightBlue[1], brightBlue[2]);
            CV::triangleFill(p_rear_left, p_rear_right, p_tip + direction * 7 * scale);
            CV::color(midBlue[0], midBlue[1], midBlue[2]);
            CV::triangleFill(p_rear_left - perpendicular * 2.5f * scale,
                             p_rear_right + perpendicular * 2.5f * scale,
                             p_tip);
            break;
        case 2: 
            CV::color(whiteHot[0], whiteHot[1], whiteHot[2]);
            CV::triangleFill(p_rear_left  + perpendicular * 2.5f * scale,
                             p_rear_right - perpendicular * 2.5f * scale,
                             p_mid_core + direction * 12 * scale);
            CV::color(brightBlue[0], brightBlue[1], brightBlue[2]);
            CV::triangleFill(p_rear_left, p_rear_right, p_tip + direction * 10 * scale);
            CV::color(midBlue[0], midBlue[1], midBlue[2]);
            CV::triangleFill(p_rear_left - perpendicular * 3.5f * scale,
                             p_rear_right + perpendicular * 3.5f * scale,
                             p_tip + direction * 4 * scale);
            CV::color(darkBlue[0], darkBlue[1], darkBlue[2]);
             CV::triangleFill(p_rear_left - perpendicular * 5.0f * scale, 
                             p_rear_right + perpendicular * 5.0f * scale,
                             p_tip - direction * 1.5f * scale);
            break;
    }

    
    int numParticles = 4 + rand() % 4; 
    for (int i = 0; i < numParticles; ++i) {
        float particleDist = ((rand() % 100) / 100.0f) * currentLength * 0.8f; 
        float particleOffsetFactor = ((rand() % 100) / 100.0f) - 0.5f; 
        float particleOffset = particleOffsetFactor * currentWidth * 0.9f; 
        float particleSize = (1.5f + (rand() % 15) / 10.0f) * scale; 
        Vector2 particlePos = basePosition + direction * particleDist + perpendicular * particleOffset;
        
        int colorChoice = rand() % 3;
        if (colorChoice == 0) CV::color(whiteHot[0], whiteHot[1], whiteHot[2]);
        else if (colorChoice == 1) CV::color(brightBlue[0], brightBlue[1], brightBlue[2]);
        else CV::color(midBlue[0], midBlue[1], midBlue[2]);
        
        CV::circleFill(particlePos.x, particlePos.y, particleSize, 5);
    }
}


#endif // NITROBOOST_H_INCLUDED
// Este arquivo define a classe RapidFire, que representa uma habilidade de disparo rápido do tanque.
// A classe gerencia o estado da habilidade, incluindo seu nível de carga,
// tempo de ativação, duração máxima, cooldowns e a mecânica de disparar uma curta rajada de projéteis.
// Inclui funcionalidades para:
// - Atualizar o estado da habilidade (carga, duração, cooldowns, disparos).
// - Ativar e desativar a habilidade.
// - Controlar a lógica de disparo em rajada.
// - Desenhar a interface do usuário (UI) para a habilidade.
// A função drawMiniProjectileIcon é uma auxiliar para renderizar o ícone de projétil na UI.
#ifndef RAPIDFIRE_H_INCLUDED
#define RAPIDFIRE_H_INCLUDED

#include "gl_canvas2d.h"
#include "Vector2.h"
#include <cmath>
#include <vector>
#include <cstdlib>

// Função auxiliar para desenhar um ícone de projétil pequeno para a UI.
void inline drawMiniProjectileIcon(Vector2 center, float scale, float rotationAngleRad = 0.0f) {
    float projWidth = 10.0f * scale;
    float projHeight = 20.0f * scale;

    float halfW = projWidth / 2.0f;
    float bodyH = projHeight * 0.7f;
    float tipH = projHeight * 0.3f;

    
    
    Vector2 p1(-halfW, -bodyH / 2.0f); 
    Vector2 p2(halfW, -bodyH / 2.0f);  
    Vector2 p3(halfW, bodyH / 2.0f);   
    Vector2 p4(-halfW, bodyH / 2.0f);  
    
    Vector2 tip(0, bodyH / 2.0f + tipH); 

    
    float cosA = cos(rotationAngleRad);
    float sinA = sin(rotationAngleRad);

    auto rotateAndTranslate = [&](Vector2 v) {
        float rx = v.x * cosA - v.y * sinA;
        float ry = v.x * sinA + v.y * cosA;
        return Vector2(center.x + rx, center.y + ry);
    };

    p1 = rotateAndTranslate(p1);
    p2 = rotateAndTranslate(p2);
    p3 = rotateAndTranslate(p3);
    p4 = rotateAndTranslate(p4);
    tip = rotateAndTranslate(tip);

    
    CV::color(0.6f, 0.6f, 0.6f);
    float bodyVx[] = {p1.x, p2.x, p3.x, p4.x};
    float bodyVy[] = {p1.y, p2.y, p3.y, p4.y};
    CV::polygonFill(bodyVx, bodyVy, 4);

    
    CV::color(0.8f, 0.1f, 0.1f);
    float tipVx[] = {p4.x, p3.x, tip.x}; 
    float tipVy[] = {p4.y, p3.y, tip.y};
    CV::polygonFill(tipVx, tipVy, 3);
}


class RapidFire {
public:
    float chargeLevel;
    const float maxChargeTime;
    float activeTimer;
    const float maxActiveDuration; 
    bool isActive;
    bool isCharging;
    float postUseCooldown;
    float currentPostUseCooldown;

    int shotsFired;
    const int shotsToFireTotal;
    float timeBetweenShots; 
    float currentShotCooldown; 

    float uiPulseAccumulator;

public:
    // Construtor da classe RapidFire. Inicializa os parâmetros da habilidade.
    RapidFire() :
        chargeLevel(0.0f), maxChargeTime(7.0f), 
        activeTimer(0.0f), maxActiveDuration(2.0f), 
        isActive(false), isCharging(true),
        postUseCooldown(1.5f), currentPostUseCooldown(0.0f),
        shotsFired(0), shotsToFireTotal(3),
        timeBetweenShots(0.1f), currentShotCooldown(0.0f),
        uiPulseAccumulator(0.0f)
    {}

    // Atualiza o estado da habilidade RapidFire, incluindo carga, tempo ativo, cooldowns e lógica de disparo.
    void update(float fps) {
        if (fps <= 0) return;

        uiPulseAccumulator += 5.0f * (1.0f / fps);
        if (uiPulseAccumulator > 2.0f * (float)M_PI) {
            uiPulseAccumulator -= 2.0f * (float)M_PI;
        }

        if (isActive) {
            activeTimer += 1.0f / fps;
            if (currentShotCooldown > 0.0f) {
                currentShotCooldown -= 1.0f / fps;
            }

            if (shotsFired >= shotsToFireTotal || activeTimer >= maxActiveDuration) {
                deactivate();
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

    // Tenta ativar a habilidade RapidFire. Retorna true se ativada com sucesso, false caso contrário.
    // A habilidade só pode ser ativada se não estiver ativa e estiver totalmente carregada.
    bool activate() {
        if (!isActive && chargeLevel >= maxChargeTime) {
            isActive = true;
            activeTimer = 0.0f;
            shotsFired = 0;
            currentShotCooldown = 0.0f; 
            isCharging = false;
            return true;
        }
        return false;
    }

    // Desativa a habilidade RapidFire, reinicia seus contadores e inicia o cooldown pós-uso.
    void deactivate() {
        isActive = false;
        activeTimer = 0.0f;
        chargeLevel = 0.0f;
        isCharging = false;
        currentPostUseCooldown = postUseCooldown;
        shotsFired = 0;
    }

    // Verifica se o efeito da habilidade RapidFire está atualmente ativo.
    bool isEffectActive() const {
        return isActive;
    }

    // Verifica se a habilidade RapidFire pode disparar um projétil da rajada.
    bool canShootBurst() {
        if (isActive && shotsFired < shotsToFireTotal && currentShotCooldown <= 0.0f) {
            return true;
        }
        return false;
    }

    // Registra que um projétil da rajada foi disparado e define o cooldown para o próximo.
    void recordBurstShot(float fps) {
        if (isActive) {
            shotsFired++;
            currentShotCooldown = timeBetweenShots; 
                                                     
                                                     
        }
    }

    // Desenha a interface do usuário (UI) para a habilidade RapidFire.
    // Mostra o nível de carga e um ícone representativo de três projéteis.
    void drawUI(float x, float y, float width, float height) const {
        
        CV::color(0.1f, 0.1f, 0.15f);
        CV::rectFill(x, y, x + width, y + height);
        
        
        float fillHeight = (chargeLevel / maxChargeTime) * height;
        if (chargeLevel >= maxChargeTime) {
            CV::color(1.0f, 0.8f, 0.2f); 
        } else {
            CV::color(0.6f, 0.4f, 0.1f); 
        }
        CV::rectFill(x, y + (height - fillHeight), x + width, y + height);

        
        
        if (chargeLevel >= maxChargeTime) {
            float pulseAlpha = (sin(uiPulseAccumulator) * 0.2f + 0.3f); 
            CV::color(1.0f, 0.9f, 0.5f, pulseAlpha); 
            CV::rectFill(x + 2, y + 2, x + width - 2, y + height - 2); 
        }

        
        float iconScale = 0.8f; 
        float projSpacingY = height * 0.15f * iconScale;
        float projSpacingX = width * 0.2f * iconScale;
        
        Vector2 centerIconPos(x + width / 2.0f, y + height / 2.0f);

        
        float projBodyR = 0.6f, projBodyG = 0.6f, projBodyB = 0.6f;
        float projTipR = 0.8f, projTipG = 0.1f, projTipB = 0.1f;

        if (chargeLevel >= maxChargeTime) {
            float colorPulse = (sin(uiPulseAccumulator * 1.5f) + 1.0f) / 2.0f * 0.2f + 0.8f; 
            projBodyR *= colorPulse; projBodyG *= colorPulse; projBodyB *= colorPulse;
            projTipR *= colorPulse; projTipG *= colorPulse; projTipB *= colorPulse;
        }
        
        
        

        drawMiniProjectileIcon(Vector2(centerIconPos.x, centerIconPos.y + projSpacingY * 0.5f), iconScale, -M_PI / 2.0f);
        drawMiniProjectileIcon(Vector2(centerIconPos.x - projSpacingX, centerIconPos.y - projSpacingY * 0.5f), iconScale, -M_PI / 2.0f);
        drawMiniProjectileIcon(Vector2(centerIconPos.x + projSpacingX, centerIconPos.y - projSpacingY * 0.5f), iconScale, -M_PI / 2.0f);

        
        CV::color(0.3f, 0.3f, 0.4f);
        CV::rect(x, y, x + width, y + height);
    }
};

#endif // RAPIDFIRE_H_INCLUDED
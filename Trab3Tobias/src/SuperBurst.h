// Este arquivo define a classe SuperBurst, que representa uma habilidade de disparo especial do tanque.
// A classe gerencia o estado da habilidade, incluindo seu nível de carga,
// tempo de ativação, duração máxima, cooldowns e a mecânica de disparar uma rajada de projéteis.
// Inclui funcionalidades para:
// - Atualizar o estado da habilidade (carga, duração, cooldowns, disparos).
// - Ativar e desativar a habilidade.
// - Controlar a lógica de disparo em rajada.
// - Desenhar a interface do usuário (UI) para a habilidade.
#ifndef SUPERBURST_H_INCLUDED
#define SUPERBURST_H_INCLUDED

#include "gl_canvas2d.h"
#include "Vector2.h"
#include <cmath>
#include <vector>
#include <cstdlib>


class SuperBurst {
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
    // Construtor da classe SuperBurst. Inicializa os parâmetros da habilidade.
    SuperBurst() :
        chargeLevel(0.0f), maxChargeTime(10.0f), 
        activeTimer(0.0f), maxActiveDuration(3.0f), 
        isActive(false), isCharging(true),
        postUseCooldown(2.0f), currentPostUseCooldown(0.0f),
        shotsFired(0), shotsToFireTotal(5), 
        timeBetweenShots(0.08f), currentShotCooldown(0.0f), 
        uiPulseAccumulator(0.0f)
    {}

    // Atualiza o estado da habilidade SuperBurst, incluindo carga, tempo ativo, cooldowns e lógica de disparo.
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

    // Tenta ativar a habilidade SuperBurst. Retorna true se ativada com sucesso, false caso contrário.
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

    // Desativa a habilidade SuperBurst, reinicia seus contadores e inicia o cooldown pós-uso.
    void deactivate() {
        isActive = false;
        activeTimer = 0.0f;
        chargeLevel = 0.0f;
        isCharging = false;
        currentPostUseCooldown = postUseCooldown;
        shotsFired = 0;
    }

    // Verifica se o efeito da habilidade SuperBurst está atualmente ativo.
    bool isEffectActive() const {
        return isActive;
    }

    // Verifica se a habilidade SuperBurst pode disparar um projétil da rajada.
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

    // Desenha a interface do usuário (UI) para a habilidade SuperBurst.
    // Mostra o nível de carga e um ícone representativo da rajada de projéteis.
    void drawUI(float x, float y, float width, float height) const {
        
        CV::color(0.1f, 0.1f, 0.15f);
        CV::rectFill(x, y, x + width, y + height);

        
        float fillHeight = (chargeLevel / maxChargeTime) * height;
        if (chargeLevel >= maxChargeTime) {
            CV::color(1.0f, 0.5f, 0.8f); 
        } else {
            CV::color(0.5f, 0.2f, 0.4f); 
        }
        CV::rectFill(x, y + (height - fillHeight), x + width, y + height);
        
        
        if (chargeLevel >= maxChargeTime) {
            float pulseAlpha = (sin(uiPulseAccumulator) * 0.2f + 0.3f);
            CV::color(1.0f, 0.7f, 0.9f, pulseAlpha); 
            CV::rectFill(x + 2, y + 2, x + width - 2, y + height - 2);
        }

        
        float iconScale = 0.7f; 
        float projRadiusX = width * 0.25f * iconScale;
        float projRadiusY = height * 0.20f * iconScale;
        Vector2 centerIconPos(x + width / 2.0f, y + height / 2.0f);

        
        
        drawMiniProjectileIcon(Vector2(centerIconPos.x - projRadiusX * 1.2f, centerIconPos.y - projRadiusY), iconScale, -M_PI / 2.0f);
        drawMiniProjectileIcon(Vector2(centerIconPos.x, centerIconPos.y - projRadiusY), iconScale, -M_PI / 2.0f);
        drawMiniProjectileIcon(Vector2(centerIconPos.x + projRadiusX * 1.2f, centerIconPos.y - projRadiusY), iconScale, -M_PI / 2.0f);
        
        drawMiniProjectileIcon(Vector2(centerIconPos.x - projRadiusX * 0.6f, centerIconPos.y + projRadiusY * 0.8f), iconScale, -M_PI / 2.0f);
        drawMiniProjectileIcon(Vector2(centerIconPos.x + projRadiusX * 0.6f, centerIconPos.y + projRadiusY * 0.8f), iconScale, -M_PI / 2.0f);

        
        CV::color(0.3f, 0.3f, 0.4f);
        CV::rect(x, y, x + width, y + height);
    }
};

#endif // SUPERBURST_H_INCLUDED
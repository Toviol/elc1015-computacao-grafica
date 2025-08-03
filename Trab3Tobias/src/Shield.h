// Este arquivo define a classe Shield, que representa a habilidade de escudo do tanque.
// A classe gerencia o estado do escudo, incluindo seu nível de carga,
// tempo de ativação, duração máxima, e cooldowns.
// Também lida com a lógica para ativar e desativar o escudo.
// Inclui funcionalidades para:
// - Atualizar o estado do escudo (carga, duração, cooldowns).
// - Ativar e desativar o escudo.
// - Fornecer um fator de pulsação para efeitos visuais quando o escudo está ativo.
// - Desenhar a interface do usuário (UI) para a habilidade de escudo.
#ifndef SHIELD_H_INCLUDED
#define SHIELD_H_INCLUDED

#include "gl_canvas2d.h"
#include "Vector2.h"
#include <cmath>
#include <vector>
#include <cstdlib>

class Shield {
public:
    float chargeLevel;
    const float maxChargeTime;
    float activeTimer;
    const float maxActiveDuration;
    bool isActive;
    bool isCharging;
    float postUseCooldown;
    float currentPostUseCooldown;

    float uiPulseAccumulator;       
    float effectPulseAccumulator;   

public:
    // Construtor da classe Shield. Inicializa os parâmetros do escudo.
    Shield() :
        chargeLevel(0.0f), maxChargeTime(15.0f), 
        activeTimer(0.0f), maxActiveDuration(7.0f),  
        isActive(false), isCharging(true),
        postUseCooldown(3.0f), currentPostUseCooldown(0.0f),
        uiPulseAccumulator(0.0f), effectPulseAccumulator(0.0f)
    {}

    // Atualiza o estado do escudo, incluindo carga, tempo ativo e cooldowns.
    // Também atualiza os acumuladores para efeitos de pulsação visual.
    void update(float fps) {
        if (fps <= 0) return;

        uiPulseAccumulator += 5.0f * (1.0f / fps);
        if (uiPulseAccumulator > 2.0f * (float)M_PI) {
            uiPulseAccumulator -= 2.0f * (float)M_PI;
        }

        if (isActive) {
            effectPulseAccumulator += 8.0f * (1.0f / fps); 
            if (effectPulseAccumulator > 2.0f * (float)M_PI) {
                effectPulseAccumulator -= 2.0f * (float)M_PI;
            }
            activeTimer += 1.0f / fps;
            if (activeTimer >= maxActiveDuration) {
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

    // Tenta ativar o escudo. Retorna true se ativado com sucesso, false caso contrário.
    // O escudo só pode ser ativado se não estiver ativo e estiver totalmente carregado.
    bool activate() {
        if (!isActive && chargeLevel >= maxChargeTime) {
            isActive = true;
            activeTimer = 0.0f;
            effectPulseAccumulator = 0.0f; 
            isCharging = false;
            return true;
        }
        return false;
    }

    // Desativa o escudo, reinicia seus contadores e inicia o cooldown pós-uso.
    void deactivate() {
        isActive = false;
        activeTimer = 0.0f;
        chargeLevel = 0.0f;
        isCharging = false;
        currentPostUseCooldown = postUseCooldown;
    }

    // Verifica se o efeito do escudo está atualmente ativo.
    bool isEffectActive() const {
        return isActive;
    }

    // Retorna um fator de pulsação (0.0 a 1.0) para o efeito visual do escudo no tanque.
    // Usado para criar uma animação de pulsação quando o escudo está ativo.
    float getEffectPulseFactor() const {
        if (!isActive) return 0.0f;
        return (sin(effectPulseAccumulator) + 1.0f) / 2.0f; 
    }

    // Desenha a interface do usuário (UI) para a habilidade de escudo.
    // Mostra o nível de carga e um ícone representativo do escudo.
    void drawUI(float x, float y, float width, float height) const {
        
        CV::color(0.1f, 0.1f, 0.15f);
        CV::rectFill(x, y, x + width, y + height);

        
        float fillHeight = (chargeLevel / maxChargeTime) * height;
        float chargeColorR = 0.2f, chargeColorG = 0.5f, chargeColorB = 0.8f; 
        if (chargeLevel >= maxChargeTime) {
            chargeColorR = 0.3f; chargeColorG = 0.7f; chargeColorB = 1.0f; 
        }
        CV::color(chargeColorR, chargeColorG, chargeColorB);
        CV::rectFill(x, y + (height - fillHeight), x + width, y + height);

        
        float iconCenterX = x + width / 2.0f;
        float iconCenterY = y + height / 2.0f;
        float shieldW = width * 0.7f; 
        float shieldH = height * 0.75f; 

        std::vector<Vector2> outlineVertices;
        outlineVertices.push_back(Vector2(iconCenterX, iconCenterY + shieldH * 0.5f)); 
        outlineVertices.push_back(Vector2(iconCenterX - shieldW * 0.45f, iconCenterY + shieldH * 0.3f)); 
        outlineVertices.push_back(Vector2(iconCenterX - shieldW * 0.5f, iconCenterY - shieldH * 0.2f));  
        outlineVertices.push_back(Vector2(iconCenterX - shieldW * 0.15f, iconCenterY - shieldH * 0.5f)); 
        outlineVertices.push_back(Vector2(iconCenterX + shieldW * 0.15f, iconCenterY - shieldH * 0.5f)); 
        outlineVertices.push_back(Vector2(iconCenterX + shieldW * 0.5f, iconCenterY - shieldH * 0.2f));  
        outlineVertices.push_back(Vector2(iconCenterX + shieldW * 0.45f, iconCenterY + shieldH * 0.3f)); 
        
        int numOutlineVertices = outlineVertices.size();
        float outlineVx[numOutlineVertices];
        float outlineVy[numOutlineVertices];
        for(int i=0; i < numOutlineVertices; ++i) {
            outlineVx[i] = outlineVertices[i].x;
            outlineVy[i] = outlineVertices[i].y;
        }

        float baseR = 0.3f, baseG = 0.6f, baseB = 0.85f; 
        float pulseFactor = 0.0f;
        if (chargeLevel >= maxChargeTime) {
            pulseFactor = (sin(uiPulseAccumulator) + 1.0f) / 2.0f; 
        }

        
        float fillR = baseR * (0.7f + 0.3f * pulseFactor);
        float fillG = baseG * (0.7f + 0.3f * pulseFactor);
        float fillB = baseB * (0.7f + 0.3f * pulseFactor);
        CV::color(fillR, fillG, fillB);
        CV::polygonFill(outlineVx, outlineVy, numOutlineVertices);

        
        float detailPulse = 0.5f + 0.5f * pulseFactor; 
        float detailR = 0.8f * detailPulse, detailG = 0.9f * detailPulse, detailB = 1.0f * detailPulse;
        CV::color(detailR, detailG, detailB);

        
        CV::line(iconCenterX, iconCenterY + shieldH * 0.45f, iconCenterX, iconCenterY - shieldH * 0.4f);

        
        float topDetailY = iconCenterY + shieldH * 0.22f;
        float topDetailWidth = shieldW * 0.3f;
        CV::line(iconCenterX - topDetailWidth, topDetailY, iconCenterX + topDetailWidth, topDetailY); 
        
        CV::line(iconCenterX - topDetailWidth * 0.5f, topDetailY, iconCenterX, iconCenterY + shieldH * 0.1f);
        CV::line(iconCenterX + topDetailWidth * 0.5f, topDetailY, iconCenterX, iconCenterY + shieldH * 0.1f);
        
        
        CV::line(outlineVertices[1].x + shieldW * 0.05f, outlineVertices[1].y, 
                 iconCenterX - shieldW * 0.1f, iconCenterY - shieldH * 0.1f); 
        CV::line(outlineVertices[6].x - shieldW * 0.05f, outlineVertices[6].y, 
                 iconCenterX + shieldW * 0.1f, iconCenterY - shieldH * 0.1f);

        
        float borderR_col = baseR * 0.5f * (1.0f + pulseFactor); 
        float borderG_col = baseG * 0.5f * (1.0f + pulseFactor); 
        float borderB_col = baseB * 0.6f * (1.0f + pulseFactor); 
        CV::color(borderR_col, borderG_col, borderB_col);
        CV::polygon(outlineVx, outlineVy, numOutlineVertices);

        
        CV::color(0.3f, 0.3f, 0.4f);
        CV::rect(x, y, x + width, y + height);
    }
};

#endif // SHIELD_H_INCLUDED
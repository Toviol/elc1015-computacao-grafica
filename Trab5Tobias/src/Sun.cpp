#include "Sun.h"
#include "Textura.h"

// Inicializar variável estática
float Sun::sunRotation = 0.0f;

void Sun::render(const Vector3& lightPosition, float deltaTime) {
    glPushMatrix();
    
    // Posicionar o sol na posição da luz
    glTranslatef(lightPosition.x, lightPosition.y, lightPosition.z);
    
    // Adicionar rotação lenta para simular rotação solar
    sunRotation += deltaTime * 8.0f; // Rotação um pouco mais rápida para ser visível
    glRotatef(sunRotation, 0, 1, 0);
    
    // === RENDERIZAÇÃO PROTEGIDA DO SOL ===
    // Salvar estado da iluminação antes de desabilitá-la
    GLboolean lightingWasEnabled = glIsEnabled(GL_LIGHTING);
    
    // Tornar o sol emissor de luz (não afetado pela iluminação)
    glDisable(GL_LIGHTING);
    
    // Tamanho do sol
    float sunRadius = 7.0f;
    
    // === CAMADA PRINCIPAL - FOTOSFERA COM TEXTURA ===
    // Cor base clara para permitir que a textura apareça
    glColor3f(1.0f, 1.0f, 1.0f);
    
    // Usar textura realista do sol
    TexturaManager::renderSunSphere(sunRadius, 32, 24);
    
    // Restaura estado da iluminação (proteção contra falhas)
    if (lightingWasEnabled) {
        glEnable(GL_LIGHTING);
    }
    
    glPopMatrix();
}

/*
                                PLANET.H

DESCRIÇÃO:
Este arquivo define a estrutura e classe para representar planetas no sistema solar.
Cada planeta possui propriedades físicas (posição, tamanho, órbita) e propriedades
de material (difusa, especular, brilho) para renderização realista.

FUNCIONALIDADES:
- Estrutura Planet com propriedades completas
- Materiais específicos para cada planeta
- Texturas individuais para cada planeta
- Órbitas elípticas realistas
- Rotação própria de cada planeta
- Renderização com iluminação

PLANETAS IMPLEMENTADOS:
- Mercúrio, Vênus, Terra (com Lua), Marte
- Júpiter, Saturno (com anéis), Urano, Netuno

*/

#ifndef ___PLANET__H___
#define ___PLANET__H___

#include "Vector3.h"
#include "Textura.h"
#include <GL/glut.h>
#include <cmath>
#include <string>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// === ESTRUTURA PARA PLANETAS ===
struct Planet {
    // Identificação
    std::string name;
    
    // Propriedades físicas
    Vector3 position;
    float radius;
    float distanceFromSun;
    float orbitalSpeed;
    float rotationSpeed;
    float currentAngle;
    float rotationAngle;
    
    // Propriedades de material (para iluminação)
    float diffuse;
    float specular;
    float shininess;
    
    // Textura
    std::string texturePath;
    GLuint textureID;
    
    // Propriedades especiais
    bool hasRings;        // Para Saturno
    bool hasMoon;         // Para Terra
    
    // Construtor
    Planet(const std::string& planetName, float planetRadius, float sunDistance, 
           float orbSpeed, float rotSpeed, float diff, float spec, float shine,
           const std::string& texPath, bool rings = false, bool moon = false)
        : name(planetName), radius(planetRadius), distanceFromSun(sunDistance),
          orbitalSpeed(orbSpeed), rotationSpeed(rotSpeed), currentAngle(0.0f),
          rotationAngle(0.0f), diffuse(diff), specular(spec), shininess(shine),
          texturePath(texPath), textureID(0), hasRings(rings), hasMoon(moon) {
        // Inicializa posição temporariamente na origem - será ajustada no primeiro updateOrbit
        position = Vector3(sunDistance, 0.0f, 0.0f);
    }

    // Define a posição inicial com base na posição do sol
    void initializePosition(const Vector3& sunPosition) {
        position = Vector3(sunPosition.x + distanceFromSun, sunPosition.y, sunPosition.z);
    }
    
    // Atualiza posição orbital relativa à posição do sol
    void updateOrbit(float deltaTime, const Vector3& sunPosition = Vector3(0, 0, 0)) {
        float choiceSpeedFactor = 0.5f; // Fator de velocidade das orbita
        currentAngle += orbitalSpeed * deltaTime * choiceSpeedFactor;
        if (currentAngle > 360.0f) currentAngle -= 360.0f;
        
        // Calcula nova posição orbital relativa ao sol
        float radianAngle = currentAngle * M_PI / 180.0f;
        position.x = sunPosition.x + distanceFromSun * cos(radianAngle);
        position.y = sunPosition.y;  // Manter no mesmo plano Y do sol
        position.z = sunPosition.z + distanceFromSun * sin(radianAngle);
        
        // Atualiza rotação própria
        rotationAngle += rotationSpeed * deltaTime;
        if (rotationAngle > 360.0f) rotationAngle -= 360.0f;
    }
    
    // Aplica material do planeta
    void applyMaterial() {
        // Configura material difuso
        float diffuseColor[] = {diffuse, diffuse, diffuse, 1.0f};
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuseColor);
        
        // Configura material especular
        float specularColor[] = {specular, specular, specular, 1.0f};
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularColor);
        
        // Configura brilho
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
        
        // Configura material ambiente
        float ambientColor[] = {diffuse * 0.2f, diffuse * 0.2f, diffuse * 0.2f, 1.0f};
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientColor);
    }
    
    // Carrega textura
    void loadTexture() {
        if (textureID == 0) {
            textureID = TexturaManager::loadTexture(texturePath.c_str());
        }
    }
    
    // Renderiza planeta
    void render() {
        glPushMatrix();
        
        // Aplica transformações
        glTranslatef(position.x, position.y, position.z);
        glRotatef(rotationAngle, 0.0f, 1.0f, 0.0f);
        
        // Aplica material
        applyMaterial();
        
        // Renderizar a bola com textura
        if (textureID != 0) {
            TexturaManager::renderTexturedSphere(radius, 32, 32, textureID);
        } else {
            // Se não houver textura :(, renderiza esfera sólida 
            glutSolidSphere(radius, 32, 32);
        }
        
        // Anel de Saturno
        if (hasRings) {
            renderRings();
        }
        
        // Lua da Terra
        if (hasMoon) {
            renderMoon();
        }
        
        glPopMatrix();
    }
    
    // Renderizar anéis de Saturno
    void renderRings() {
        glPushMatrix();
        
        // Configura material para os anéis, ilumina ambos os lados
        float ringColor[] = {0.8f, 0.7f, 0.5f, 0.7f};
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, ringColor);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, ringColor);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 20.0f);
        
        // Ativar blending para transparência
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        // Renderizar múltiplos anéis
        for (int i = 0; i < 3; i++) {
            float innerRadius = radius * 1.5f + i * 0.5f;
            float outerRadius = innerRadius + 0.3f;
            
            renderRing(innerRadius, outerRadius);
        }
        
        glDisable(GL_BLEND);
        glPopMatrix();
    }
    
    // Renderizar um anel individual
    void renderRing(float innerRadius, float outerRadius) {
        const int segments = 64;
        const float PI = 3.14159265359f;
        
        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i <= segments; i++) {
            float angle = 2.0f * PI * i / segments;
            float x = cos(angle);
            float z = sin(angle);
            
            // Normal para cima
            glNormal3f(0.0f, 1.0f, 0.0f);
            
            // Vértice interno
            glVertex3f(x * innerRadius, 0.0f, z * innerRadius);
            
            // Vértice externo
            glVertex3f(x * outerRadius, 0.0f, z * outerRadius);
        }
        glEnd();
    }
    
    // Renderizar lua da Terra
    void renderMoon() {
        glPushMatrix();
        
        // Posição da lua (orbita em torno da Terra)
        float moonDistance = radius * 3.0f;
        float moonAngle = rotationAngle * 4.0f; // Lua orbita mais rápido
        float moonRad = moonAngle * M_PI / 180.0f;
        
        glTranslatef(moonDistance * cos(moonRad), 0.0f, moonDistance * sin(moonRad));
        
        // Material da lua (corrigido para ambos os lados)
        float moonColor[] = {0.5f, 0.5f, 0.5f, 1.0f};
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, moonColor);
        float moonSpecular[] = {0.1f, 0.1f, 0.1f, 1.0f};
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, moonSpecular);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 5.0f);
        
        // Renderizar lua
        float moonRadius = radius * 0.27f; // Lua é ~27% do tamanho da Terra
        TexturaManager::renderMoonSphere(moonRadius, 16, 16);
        
        glPopMatrix();
    }
};

#endif // ___PLANET__H___

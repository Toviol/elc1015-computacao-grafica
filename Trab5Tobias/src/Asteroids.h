/*
DESCRIÇÃO:
Este arquivo contém toda a lógica relacionada à geração, renderização e 
gerenciamento de asteroides.

FUNCIONALIDADES:
- Estrutura Asteroid com propriedades completas
- NUM_ASTEROID_TYPES tipos diferentes de asteroides (configurável)
- Métodos de renderização wireframe e sólida para cada tipo
- Geração procedural de campos de asteroides
- Sistema de rotação individual para cada asteroide

TIPOS DE ASTEROIDES:
0 - Esfera (cinza) - Asteroide clássico esférico
1 - Cubo (marrom) - Asteroide cristalino cúbico  
2 - Torus (azul) - Asteroide em forma de anel
3 - Cone (verde) - Asteroide pontiagudo cônico
4 - Multi-Cilindros (roxo) - Asteroide composto por múltiplos cilindros
5 - Tetraedro (laranja) - Asteroide usando múltiplos tetraedros
6 - Icosaedro (vermelho) - Asteroide com esferas em padrão icosaédrico
7 - Irregular (cinza escuro) - Asteroide de formato orgânico irregular
8 - Anéis Rotativos (azul claro) - Asteroide com anéis concêntricos rotativos

*/

#ifndef ___ASTEROIDS__H___
#define ___ASTEROIDS__H___

#include "Vector3.h"
#include "Textura.h"
#include <GL/glut.h>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>

// === CONSTANTES PARA ASTEROIDES ===
const int NUM_ASTEROIDS = 150;                         // Número total de asteroides
const float ASTEROID_FIELD_SIZE = 200.0f;              // Tamanho do campo de asteroides
const float MIN_ASTEROID_SCALE = 1.0f;                 // Escala mínima dos asteroides
const float MAX_ASTEROID_SCALE = 4.0f;                 // Escala máxima dos asteroides
const int NUM_ASTEROID_TYPES = 12;                     // Número de tipos diferentes (0-11)

// === ESTRUTURA PARA ASTEROIDES ===
struct Asteroid {
    Vector3 position;
    Vector3 rotation;
    Vector3 rotationSpeed;
    float scale;
    int renderType; // 0 a (NUM_ASTEROID_TYPES-1): esfera, cubo, torus, cone, cilindro, octaedro, dodecaedro, irregular, rings, crystal-cluster, orbital-debris, fusion-core
    
    Asteroid() : position(0,0,0), rotation(0,0,0), rotationSpeed(0,0,0), scale(1.0f), renderType(0) {}
    Asteroid(Vector3 pos, float s, int type) : position(pos), rotation(0,0,0), scale(s), renderType(type) {
        // Rotação aleatória para cada asteroide
        rotationSpeed.x = ((rand() % 200) - 100) / 100.0f; // -1.0 a 1.0
        rotationSpeed.y = ((rand() % 200) - 100) / 100.0f;
        rotationSpeed.z = ((rand() % 200) - 100) / 100.0f;
    }
};

// === ENUMS PARA RENDERIZAÇÃO ===
enum class RenderMode {
    WIREFRAME,
    SOLID
};

// === CLASSE PARA GERENCIAMENTO DE ASTEROIDES ===
class AsteroidManager {
public:
    // === MÉTODOS PRINCIPAIS ===
    static void generateAsteroids(std::vector<Asteroid>& asteroids);
    static void updateAsteroidRotations(std::vector<Asteroid>& asteroids, float deltaTime);
    static void renderAsteroids(const std::vector<Asteroid>& asteroids, RenderMode renderMode);
    static void renderAsteroid(const Asteroid& asteroid, RenderMode renderMode);
    static Vector3 generateRandomPosition();
    static Vector3 generateRandomPositionAroundCamera(const Vector3& cameraPos, float distance);
    
    // === MÉTODOS DE RENDERIZAÇÃO DE FORMAS COMPOSTAS ===
    static void renderSphericalAsteroid(float size, RenderMode renderMode);
    static void renderCubicAsteroid(float size, RenderMode renderMode);
    static void renderTorusAsteroid(float size, RenderMode renderMode);
    static void renderConicAsteroid(float size, RenderMode renderMode);
    static void renderMultiCylinderAsteroid(float size, RenderMode renderMode);
    static void renderTetrahedralAsteroid(float size, RenderMode renderMode);
    static void renderIcosahedralAsteroid(float size, RenderMode renderMode);
    static void renderIrregularComposite(float size, RenderMode renderMode);
    static void renderRotatingRings(float size, RenderMode renderMode, const Vector3& rotation);
    static void renderCrystalCluster(float size, RenderMode renderMode, const Vector3& rotation);
    static void renderOrbitalDebris(float size, RenderMode renderMode, const Vector3& rotation);
    static void renderFusionCore(float size, RenderMode renderMode, const Vector3& rotation);
    
private:
    // Movi as funções matemáticas para Vector3.h para evitar duplicação
};

// === IMPLEMENTAÇÕES INLINE ===

inline void AsteroidManager::generateAsteroids(std::vector<Asteroid>& asteroids) {
    asteroids.clear();
    asteroids.resize(NUM_ASTEROIDS);
    
    for (int i = 0; i < NUM_ASTEROIDS; i++) {
        asteroids[i].position = generateRandomPosition();
        asteroids[i].rotation = Vector3(0, 0, 0);
        asteroids[i].scale = MIN_ASTEROID_SCALE + 
                            (static_cast<float>(rand()) / RAND_MAX) * 
                            (MAX_ASTEROID_SCALE - MIN_ASTEROID_SCALE);
        asteroids[i].renderType = rand() % NUM_ASTEROID_TYPES; // 0 a (NUM_ASTEROID_TYPES-1): diferentes tipos de asteroides
        
        // Rotação aleatória para cada asteroide
        asteroids[i].rotationSpeed.x = ((rand() % 200) - 100) / 100.0f; // -1.0 a 1.0
        asteroids[i].rotationSpeed.y = ((rand() % 200) - 100) / 100.0f;
        asteroids[i].rotationSpeed.z = ((rand() % 200) - 100) / 100.0f;
    }
}

inline Vector3 AsteroidManager::generateRandomPosition() {
    float x = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * ASTEROID_FIELD_SIZE * 2.0f;
    float y = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * ASTEROID_FIELD_SIZE * 2.0f;
    float z = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * ASTEROID_FIELD_SIZE * 2.0f;
    
    // Evitar gerar muito próximo da posição inicial da câmera
    Vector3 pos(x, y, z);
    float distanceFromStart = (pos - Vector3(0, 0, 10)).length();
    if (distanceFromStart < 20.0f) {
        // Se muito próximo, empurrar para longe
        Vector3 direction = pos - Vector3(0, 0, 10);
        direction.normalize();
        pos = Vector3(0, 0, 10) + direction * 25.0f;
    }
    
    return pos;
}

inline Vector3 AsteroidManager::generateRandomPositionAroundCamera(const Vector3& cameraPos, float distance) {
    float theta = (static_cast<float>(rand()) / RAND_MAX) * 2.0f * 3.14159f;
    float phi = (static_cast<float>(rand()) / RAND_MAX) * 3.14159f;
    
    float x = cameraPos.x + distance * sin(phi) * cos(theta);
    float y = cameraPos.y + distance * cos(phi);
    float z = cameraPos.z + distance * sin(phi) * sin(theta);
    
    return Vector3(x, y, z);
}

inline void AsteroidManager::updateAsteroidRotations(std::vector<Asteroid>& asteroids, float deltaTime) {
    for (auto& asteroid : asteroids) {
        asteroid.rotation.x += asteroid.rotationSpeed.x * deltaTime;
        asteroid.rotation.y += asteroid.rotationSpeed.y * deltaTime;
        asteroid.rotation.z += asteroid.rotationSpeed.z * deltaTime;
    }
}

inline void AsteroidManager::renderAsteroids(const std::vector<Asteroid>& asteroids, RenderMode renderMode) {
    for (const auto& asteroid : asteroids) {
        renderAsteroid(asteroid, renderMode);
    }
}

inline void AsteroidManager::renderAsteroid(const Asteroid& asteroid, RenderMode renderMode) {
    glPushMatrix();
    
    // Aplica transformações
    glTranslatef(asteroid.position.x, asteroid.position.y, asteroid.position.z);
    glRotatef(asteroid.rotation.x * 180.0f / 3.14159f, 1, 0, 0);
    glRotatef(asteroid.rotation.y * 180.0f / 3.14159f, 0, 1, 0);
    glRotatef(asteroid.rotation.z * 180.0f / 3.14159f, 0, 0, 1);
    glScalef(asteroid.scale, asteroid.scale, asteroid.scale);
    
    // === CONFIGURAÇÃO DE MATERIAIS ESPECÍFICOS POR TIPO ===
    GLfloat mat_ambient[4], mat_diffuse[4], mat_specular[4], mat_shininess[1];
    
    // Define cor e propriedades de material baseadas no tipo
    switch (asteroid.renderType) {
        case 0: // Esfera - cinza rochoso com reflexo
            glColor3f(0.6f, 0.6f, 0.6f);
            mat_ambient[0] = 0.2f; mat_ambient[1] = 0.2f; mat_ambient[2] = 0.2f; mat_ambient[3] = 1.0f;
            mat_diffuse[0] = 0.8f; mat_diffuse[1] = 0.8f; mat_diffuse[2] = 0.8f; mat_diffuse[3] = 1.0f;
            mat_specular[0] = 0.4f; mat_specular[1] = 0.4f; mat_specular[2] = 0.4f; mat_specular[3] = 1.0f;
            mat_shininess[0] = 20.0f;
            break;
        case 1: // Cubo - marrom metálico
            glColor3f(0.5f, 0.3f, 0.2f);
            mat_ambient[0] = 0.25f; mat_ambient[1] = 0.15f; mat_ambient[2] = 0.1f; mat_ambient[3] = 1.0f;
            mat_diffuse[0] = 0.8f; mat_diffuse[1] = 0.5f; mat_diffuse[2] = 0.3f; mat_diffuse[3] = 1.0f;
            mat_specular[0] = 0.7f; mat_specular[1] = 0.5f; mat_specular[2] = 0.3f; mat_specular[3] = 1.0f;
            mat_shininess[0] = 64.0f;
            break;
        case 2: // Torus - azul cristalino
            glColor3f(0.3f, 0.3f, 0.8f);
            mat_ambient[0] = 0.15f; mat_ambient[1] = 0.15f; mat_ambient[2] = 0.4f; mat_ambient[3] = 1.0f;
            mat_diffuse[0] = 0.5f; mat_diffuse[1] = 0.5f; mat_diffuse[2] = 1.0f; mat_diffuse[3] = 1.0f;
            mat_specular[0] = 0.8f; mat_specular[1] = 0.8f; mat_specular[2] = 1.0f; mat_specular[3] = 1.0f;
            mat_shininess[0] = 96.0f;
            break;
        case 3: // Cone - verde jade
            glColor3f(0.2f, 0.6f, 0.2f);
            mat_ambient[0] = 0.1f; mat_ambient[1] = 0.3f; mat_ambient[2] = 0.1f; mat_ambient[3] = 1.0f;
            mat_diffuse[0] = 0.3f; mat_diffuse[1] = 0.9f; mat_diffuse[2] = 0.3f; mat_diffuse[3] = 1.0f;
            mat_specular[0] = 0.6f; mat_specular[1] = 0.8f; mat_specular[2] = 0.6f; mat_specular[3] = 1.0f;
            mat_shininess[0] = 40.0f;
            break;
        case 4: // Multi-Cilindros - roxo metálico
            glColor3f(0.6f, 0.2f, 0.6f);
            mat_ambient[0] = 0.3f; mat_ambient[1] = 0.1f; mat_ambient[2] = 0.3f; mat_ambient[3] = 1.0f;
            mat_diffuse[0] = 0.9f; mat_diffuse[1] = 0.3f; mat_diffuse[2] = 0.9f; mat_diffuse[3] = 1.0f;
            mat_specular[0] = 0.9f; mat_specular[1] = 0.6f; mat_specular[2] = 0.9f; mat_specular[3] = 1.0f;
            mat_shininess[0] = 80.0f;
            break;
        case 5: // Tetraedro - laranja fosco
            glColor3f(0.8f, 0.4f, 0.1f);
            mat_ambient[0] = 0.4f; mat_ambient[1] = 0.2f; mat_ambient[2] = 0.05f; mat_ambient[3] = 1.0f;
            mat_diffuse[0] = 1.0f; mat_diffuse[1] = 0.6f; mat_diffuse[2] = 0.2f; mat_diffuse[3] = 1.0f;
            mat_specular[0] = 0.3f; mat_specular[1] = 0.2f; mat_specular[2] = 0.1f; mat_specular[3] = 1.0f;
            mat_shininess[0] = 10.0f;
            break;
        case 6: // Icosaedro - vermelho brilhante
            glColor3f(0.7f, 0.1f, 0.1f);
            mat_ambient[0] = 0.35f; mat_ambient[1] = 0.05f; mat_ambient[2] = 0.05f; mat_ambient[3] = 1.0f;
            mat_diffuse[0] = 1.0f; mat_diffuse[1] = 0.2f; mat_diffuse[2] = 0.2f; mat_diffuse[3] = 1.0f;
            mat_specular[0] = 1.0f; mat_specular[1] = 0.7f; mat_specular[2] = 0.7f; mat_specular[3] = 1.0f;
            mat_shininess[0] = 50.0f;
            break;
        case 7: // Irregular - cinza escuro rugoso
            glColor3f(0.4f, 0.4f, 0.4f);
            mat_ambient[0] = 0.2f; mat_ambient[1] = 0.2f; mat_ambient[2] = 0.2f; mat_ambient[3] = 1.0f;
            mat_diffuse[0] = 0.6f; mat_diffuse[1] = 0.6f; mat_diffuse[2] = 0.6f; mat_diffuse[3] = 1.0f;
            mat_specular[0] = 0.1f; mat_specular[1] = 0.1f; mat_specular[2] = 0.1f; mat_specular[3] = 1.0f;
            mat_shininess[0] = 5.0f;
            break;
        case 8: // Rotating Rings - azul claro metálico
            glColor3f(0.2f, 0.5f, 0.8f);
            mat_ambient[0] = 0.1f; mat_ambient[1] = 0.25f; mat_ambient[2] = 0.4f; mat_ambient[3] = 1.0f;
            mat_diffuse[0] = 0.4f; mat_diffuse[1] = 0.7f; mat_diffuse[2] = 1.0f; mat_diffuse[3] = 1.0f;
            mat_specular[0] = 0.7f; mat_specular[1] = 0.8f; mat_specular[2] = 1.0f; mat_specular[3] = 1.0f;
            mat_shininess[0] = 72.0f;
            break;
        case 9: // Crystal Cluster - cristal multicolorido emanando energia
            glColor3f(0.8f, 0.4f, 0.9f);
            mat_ambient[0] = 0.4f; mat_ambient[1] = 0.2f; mat_ambient[2] = 0.45f; mat_ambient[3] = 1.0f;
            mat_diffuse[0] = 1.0f; mat_diffuse[1] = 0.6f; mat_diffuse[2] = 1.0f; mat_diffuse[3] = 1.0f;
            mat_specular[0] = 1.0f; mat_specular[1] = 0.9f; mat_specular[2] = 1.0f; mat_specular[3] = 1.0f;
            mat_shininess[0] = 128.0f; // Extremamente brilhoso
            break;
        case 10: // Orbital Debris - detritos espaciais conectados
            glColor3f(0.3f, 0.3f, 0.3f);
            mat_ambient[0] = 0.15f; mat_ambient[1] = 0.15f; mat_ambient[2] = 0.15f; mat_ambient[3] = 1.0f;
            mat_diffuse[0] = 0.5f; mat_diffuse[1] = 0.5f; mat_diffuse[2] = 0.5f; mat_diffuse[3] = 1.0f;
            mat_specular[0] = 0.6f; mat_specular[1] = 0.6f; mat_specular[2] = 0.6f; mat_specular[3] = 1.0f;
            mat_shininess[0] = 30.0f; // Metálico mas desgastado
            break;
        case 11: // Fusion Core - núcleo energético com halos
            glColor3f(1.0f, 0.8f, 0.2f);
            mat_ambient[0] = 0.5f; mat_ambient[1] = 0.4f; mat_ambient[2] = 0.1f; mat_ambient[3] = 1.0f;
            mat_diffuse[0] = 1.0f; mat_diffuse[1] = 0.9f; mat_diffuse[2] = 0.4f; mat_diffuse[3] = 1.0f;
            mat_specular[0] = 1.0f; mat_specular[1] = 1.0f; mat_specular[2] = 0.8f; mat_specular[3] = 1.0f;
            mat_shininess[0] = 100.0f; // Muito brilhoso, energético
            break;
    }
    
    // Aplica propriedades do material para iluminação realista
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
    
    // Renderizar baseado no tipo
    switch (asteroid.renderType) {
        case 0:
            // Asteroide esférico
            renderSphericalAsteroid(2.0, renderMode);
            break;
        case 1:
            // Cubo - asteroide cúbico
            renderCubicAsteroid(2.0, renderMode);
            break;
        case 2:
            // Torus - asteroide em anel
            renderTorusAsteroid(2.0, renderMode);
            break;
        case 3:
            // Cone - asteroide pontiagudo
            renderConicAsteroid(2.0, renderMode);
            break;
        case 4:
            // Multi-Cilindros - asteroide composto por cilindros
            renderMultiCylinderAsteroid(2.0, renderMode);
            break;
        case 5:
            // Tetraedro Composto - usando múltiplos tetraedros
            renderTetrahedralAsteroid(2.0, renderMode);
            break;
        case 6:
            // Icosaedro - usando múltiplas esferas pequenas
            renderIcosahedralAsteroid(2.0, renderMode);
            break;
        case 7:
            // Asteroide Irregular - composição de formas primitivas
            renderIrregularComposite(2.0, renderMode);
            break;
        case 8:
            // Rotating Rings - anéis concêntricos rotativos
            renderRotatingRings(4.0, renderMode, asteroid.rotation);
            break;
        case 9:
            // Crystal Cluster - cluster de cristais multicoloridos
            renderCrystalCluster(3.0, renderMode, asteroid.rotation);
            break;
        case 10:
            // Orbital Debris - detritos espaciais conectados
            renderOrbitalDebris(2.5, renderMode, asteroid.rotation);
            break;
        case 11:
            // Fusion Core - núcleo energético com halos
            renderFusionCore(2.0, renderMode, asteroid.rotation);
            break;
    }
    
    glPopMatrix();
}

// === IMPLEMENTAÇÕES DE RENDERIZAÇÃO POR TIPO ===

inline void AsteroidManager::renderSphericalAsteroid(float size, RenderMode renderMode) {
    // Planeta esférico com textura da lua
    if (renderMode == RenderMode::WIREFRAME) {
        glutWireSphere(size, 16, 16);
    } else {
        // Usar textura da lua
        TexturaManager::renderMoonSphere(size, 32, 24);
    }
}

inline void AsteroidManager::renderCubicAsteroid(float size, RenderMode renderMode) {
    // Asteroide cúbico
    if (renderMode == RenderMode::WIREFRAME) {
        glutWireCube(size * 2.0);
    } else {
        glutSolidCube(size * 2.0);
    }
}

inline void AsteroidManager::renderTorusAsteroid(float size, RenderMode renderMode) {
    // Asteroide em forma de anel
    float innerRadius = size * 0.3f;
    float outerRadius = size * 0.8f;
    if (renderMode == RenderMode::WIREFRAME) {
        glutWireTorus(innerRadius, outerRadius, 8, 16);
    } else {
        glutSolidTorus(innerRadius, outerRadius, 8, 16);
    }
}

inline void AsteroidManager::renderConicAsteroid(float size, RenderMode renderMode) {
    // Asteroide cônico pontiagudo
    if (renderMode == RenderMode::WIREFRAME) {
        glutWireCone(size, size * 2.0, 12, 8);
    } else {
        glutSolidCone(size, size * 2.0, 12, 8);
    }
}

inline void AsteroidManager::renderMultiCylinderAsteroid(float size, RenderMode renderMode) {
    // Asteroide composto por múltiplos cilindros em ângulos diferentes
    for (int i = 0; i < 3; i++) {
        glPushMatrix();
        
        // Rotacionar cada cilindro em ângulo diferente
        glRotatef(i * 60.0f, 1, 0, 0);
        glRotatef(i * 45.0f, 0, 1, 0);
        glScalef(0.3f + i * 0.2f, 1.0f, 0.3f + i * 0.2f);
        
        // Renderizar cilindro usando cones (base + topo)
        if (renderMode == RenderMode::WIREFRAME) {
            glutWireCone(size * 0.5f, size * 1.5f, 8, 6);
        } else {
            glutSolidCone(size * 0.5f, size * 1.5f, 8, 6);
        }
        
        glPopMatrix();
    }
}

inline void AsteroidManager::renderTetrahedralAsteroid(float size, RenderMode renderMode) {
    // Asteroide usando múltiplos tetraedros (cones com base triangular)
    for (int i = 0; i < 4; i++) {
        glPushMatrix();
        
        // Posicionar tetraedros nos vértices de um tetraedro maior
        float angle = i * 120.0f;
        glRotatef(angle, 0, 1, 0);
        glTranslatef(size * 0.6f, 0, 0);
        glRotatef(i * 90.0f, 1, 1, 1);
        glScalef(0.4f, 0.8f, 0.4f);
        
        if (renderMode == RenderMode::WIREFRAME) {
            glutWireCone(size * 0.4f, size * 0.8f, 3, 4); // Base triangular
        } else {
            glutSolidCone(size * 0.4f, size * 0.8f, 3, 4);
        }
        
        glPopMatrix();
    }
}

inline void AsteroidManager::renderIcosahedralAsteroid(float size, RenderMode renderMode) {
    // Asteroide usando múltiplas esferas pequenas em padrão icosaédrico
    const int numSpheres = 12;
    float phi = (1.0f + sqrt(5.0f)) / 2.0f; // Razão áurea
    
    // Posições baseadas nos vértices de um icosaedro
    Vector3 positions[12] = {
        Vector3(-1, phi, 0), Vector3(1, phi, 0), Vector3(-1, -phi, 0), Vector3(1, -phi, 0),
        Vector3(0, -1, phi), Vector3(0, 1, phi), Vector3(0, -1, -phi), Vector3(0, 1, -phi),
        Vector3(phi, 0, -1), Vector3(phi, 0, 1), Vector3(-phi, 0, -1), Vector3(-phi, 0, 1)
    };
    
    for (int i = 0; i < numSpheres; i++) {
        glPushMatrix();
        
        Vector3 pos = positions[i];
        float scale = size / 3.0f;
        glTranslatef(pos.x * scale, pos.y * scale, pos.z * scale);
        
        float sphereSize = size * (0.15f + 0.1f * sin(i * 1.3f));
        if (renderMode == RenderMode::WIREFRAME) {
            glutWireSphere(sphereSize, 8, 8);
        } else {
            glutSolidSphere(sphereSize, 8, 8);
        }
        
        glPopMatrix();
    }
}

inline void AsteroidManager::renderIrregularComposite(float size, RenderMode renderMode) {
    // Asteroide irregular usando composição de formas primitivas
    
    // Núcleo central (esfera deformada)
    glPushMatrix();
    glScalef(0.8f, 1.2f, 0.9f);
    if (renderMode == RenderMode::WIREFRAME) {
        glutWireSphere(size * 0.6f, 10, 10);
    } else {
        glutSolidSphere(size * 0.6f, 10, 10);
    }
    glPopMatrix();
    
    // Protuberâncias (pequenos cones e esferas)
    for (int i = 0; i < 8; i++) {
        glPushMatrix();
        
        float angle = (2.0f * 3.14159f * i) / 8.0f;
        float radius = size * (0.7f + 0.3f * sin(i * 1.7f));
        float height = size * (0.3f + 0.2f * cos(i * 2.3f));
        
        glRotatef(angle * 180.0f / 3.14159f, 0, 1, 0);
        glTranslatef(radius, height * sin(i), 0);
        glRotatef(i * 45.0f, 1, 1, 0);
        
        if (i % 2 == 0) {
            // Cones pequenos
            if (renderMode == RenderMode::WIREFRAME) {
                glutWireCone(size * 0.2f, size * 0.4f, 6, 4);
            } else {
                glutSolidCone(size * 0.2f, size * 0.4f, 6, 4);
            }
        } else {
            // Esferas pequenas
            if (renderMode == RenderMode::WIREFRAME) {
                glutWireSphere(size * 0.15f, 6, 6);
            } else {
                glutSolidSphere(size * 0.15f, 6, 6);
            }
        }
        
        glPopMatrix();
    }
}

// === RENDERIZAÇÃO ESPECIAL PARA ANÉIS ROTATIVOS ===

inline void AsteroidManager::renderRotatingRings(float size, RenderMode renderMode, const Vector3& rotation) {
    const int numRings = 3;
    const int segments = 30;
    
    // Configura diferentes raios e espessuras para cada anel
    float ringRadii[numRings] = {0.6f * size, 0.8f * size, 1.0f * size};
    float ringThickness[numRings] = {0.08f * size, 0.06f * size, 0.04f * size};
    float ringRotationSpeed[numRings] = {1.0f, -1.5f, 0.8f}; // Velocidades diferentes
    
    for (int ring = 0; ring < numRings; ring++) {
        glPushMatrix();
        
        // Aplica materiais específicos para cada anel (preservando o material principal)
        GLfloat ring_ambient[4], ring_diffuse[4], ring_specular[4], ring_shininess[1];
        
        switch (ring % 3) {
            case 0: // Anel marrom metálico
                glColor3f(0.6f, 0.3f, 0.1f);
                ring_ambient[0] = 0.3f; ring_ambient[1] = 0.15f; ring_ambient[2] = 0.05f; ring_ambient[3] = 1.0f;
                ring_diffuse[0] = 0.9f; ring_diffuse[1] = 0.5f; ring_diffuse[2] = 0.2f; ring_diffuse[3] = 1.0f;
                ring_specular[0] = 0.8f; ring_specular[1] = 0.6f; ring_specular[2] = 0.4f; ring_specular[3] = 1.0f;
                ring_shininess[0] = 60.0f;
                break;
            case 1: // Anel verde cristalino
                glColor3f(0.3f, 0.6f, 0.2f);
                ring_ambient[0] = 0.15f; ring_ambient[1] = 0.3f; ring_ambient[2] = 0.1f; ring_ambient[3] = 1.0f;
                ring_diffuse[0] = 0.5f; ring_diffuse[1] = 0.9f; ring_diffuse[2] = 0.4f; ring_diffuse[3] = 1.0f;
                ring_specular[0] = 0.7f; ring_specular[1] = 1.0f; ring_specular[2] = 0.6f; ring_specular[3] = 1.0f;
                ring_shininess[0] = 80.0f;
                break;
            case 2: // Anel azul metálico
                glColor3f(0.2f, 0.3f, 0.7f);
                ring_ambient[0] = 0.1f; ring_ambient[1] = 0.15f; ring_ambient[2] = 0.35f; ring_ambient[3] = 1.0f;
                ring_diffuse[0] = 0.4f; ring_diffuse[1] = 0.5f; ring_diffuse[2] = 1.0f; ring_diffuse[3] = 1.0f;
                ring_specular[0] = 0.6f; ring_specular[1] = 0.7f; ring_specular[2] = 1.0f; ring_specular[3] = 1.0f;
                ring_shininess[0] = 70.0f;
                break;
        }
        
        // Aplica material específico do anel
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ring_ambient);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, ring_diffuse);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, ring_specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, ring_shininess);
        
        // Aplica rotação independente para cada anel
        // Cada anel roda em velocidades diferentes e eixos ligeiramente diferentes
        float ringRotX = rotation.x * ringRotationSpeed[ring] + ring * 30.0f;
        float ringRotY = rotation.y * ringRotationSpeed[ring] * 0.7f + ring * 45.0f;
        float ringRotZ = rotation.z * ringRotationSpeed[ring] * 1.3f + ring * 60.0f;
        
        glRotatef(ringRotX * 180.0f / 3.14159f, 1, 0, 0);
        glRotatef(ringRotY * 180.0f / 3.14159f, 0, 1, 0);
        glRotatef(ringRotZ * 180.0f / 3.14159f, 0, 0, 1);
        
        // Inclina cada anel em um ângulo diferente para criar complexidade visual
        glRotatef(ring * 20.0f, 1, 0, 1);
        
        if (renderMode == RenderMode::WIREFRAME) {
            // Renderizar anel wireframe usando glutWireTorus (consistente com modo sólido)
            float innerRadius = ringThickness[ring] / 2.0f;
            float outerRadius = ringRadii[ring] - ringThickness[ring] / 2.0f;
            
            glutWireTorus(innerRadius, outerRadius, 8, segments);
        } else {
            // Renderizar anel sólido usando glutSolidTorus
            float innerRadius = ringThickness[ring] / 2.0f;
            float outerRadius = ringRadii[ring] - ringThickness[ring] / 2.0f;
            
            glutSolidTorus(innerRadius, outerRadius, 8, segments);
        }
        
        glPopMatrix();
    }
    
    // Adicionar núcleo central pequeno para dar contexto
    glPushMatrix();
    glScalef(0.3f, 0.3f, 0.3f);
    if (renderMode == RenderMode::WIREFRAME) {
        glutWireSphere(size * 0.3f, 12, 12);
    } else {
        glutSolidSphere(size * 0.3f, 12, 12);
    }
    glPopMatrix();
}

// === NOVOS TIPOS DE ASTEROIDES ===

// Tipo 9: Crystal Cluster - cluster de cristais multicoloridos emanando energia
inline void AsteroidManager::renderCrystalCluster(float size, RenderMode renderMode, const Vector3& rotation) {
    // Núcleo central principal
    glPushMatrix();
    
    // Rotação lenta do cluster inteiro
    glRotatef(rotation.y * 20.0f, 0, 1, 0);
    
    // Cristal central maior
    if (renderMode == RenderMode::WIREFRAME) {
        glutWireOctahedron();
    } else {
        glutSolidOctahedron();
    }
    
    // Cristais menores em formação orbital ao redor do centro
    for (int i = 0; i < 8; i++) {
        glPushMatrix();
        
        float angle = (i * 45.0f) + rotation.x * 30.0f; // Rotação individual
        float radius = size * 0.8f;
        float height = sin(i * 0.7f + rotation.z * 10.0f) * size * 0.3f;
        
        // Posicionar cristal satélite
        glTranslatef(
            radius * cos(angle * 3.14159f / 180.0f),
            height,
            radius * sin(angle * 3.14159f / 180.0f)
        );
        
        // Rotação individual de cada cristal
        glRotatef(angle * 2.0f, 1, 1, 0);
        glScalef(0.3f, 0.6f, 0.3f); // Cristais alongados
        
        if (renderMode == RenderMode::WIREFRAME) {
            glutWireOctahedron();
        } else {
            glutSolidOctahedron();
        }
        
        glPopMatrix();
    }
    
    glPopMatrix();
}

// Tipo 10: Orbital Debris - detritos espaciais conectados por estruturas
inline void AsteroidManager::renderOrbitalDebris(float size, RenderMode renderMode, const Vector3& rotation) {
    // Estrutura principal central 
    glPushMatrix();
    
    // Rotação complexa do conjunto
    glRotatef(rotation.x * 15.0f, 1, 0, 0);
    glRotatef(rotation.y * 25.0f, 0, 1, 0);
    glRotatef(rotation.z * 20.0f, 0, 0, 1);
    
    // Núcleo central - cubo deformado
    glPushMatrix();
    glScalef(1.2f, 0.8f, 1.0f);
    if (renderMode == RenderMode::WIREFRAME) {
        glutWireCube(size * 0.6f);
    } else {
        glutSolidCube(size * 0.6f);
    }
    glPopMatrix();
    
    // Detritos conectados - esferas pequenas em posições irregulares
    Vector3 debrisPositions[] = {
        {1.5f, 0.8f, 0.3f}, {-1.2f, 1.1f, -0.7f}, {0.2f, -1.4f, 1.2f},
        {-0.9f, -0.6f, -1.1f}, {1.1f, -0.3f, -1.3f}, {-1.5f, 0.2f, 0.9f}
    };
    
    for (int i = 0; i < 6; i++) {
        glPushMatrix();
        
        Vector3 pos = debrisPositions[i];
        glTranslatef(pos.x * size, pos.y * size, pos.z * size);
        
        // Rotação individual de cada detrito
        glRotatef(rotation.x * (20.0f + i * 10.0f), 1, 0, 0);
        
        float debrisSize = size * (0.15f + (i % 3) * 0.1f);
        if (renderMode == RenderMode::WIREFRAME) {
            glutWireSphere(debrisSize, 8, 8);
        } else {
            glutSolidSphere(debrisSize, 8, 8);
        }
        
        glPopMatrix();
        
        // Conectores - cilindros finos ligando ao centro
        glPushMatrix();
        
        // Calcula orientação do conector
        float length = sqrt(pos.x*pos.x + pos.y*pos.y + pos.z*pos.z) * size;
        glTranslatef(pos.x * size * 0.5f, pos.y * size * 0.5f, pos.z * size * 0.5f);
        
        // Orienta cilindro na direção do detrito
        float angleY = atan2(pos.x, pos.z) * 180.0f / 3.14159f;
        float angleX = atan2(pos.y, sqrt(pos.x*pos.x + pos.z*pos.z)) * 180.0f / 3.14159f;
        glRotatef(angleY, 0, 1, 0);
        glRotatef(-angleX, 1, 0, 0);
        
        glScalef(0.05f, length, 0.05f); // Cilindro fino e longo
        
        if (renderMode == RenderMode::WIREFRAME) {
            glutWireCube(1.0f);
        } else {
            glutSolidCube(1.0f);
        }
        
        glPopMatrix();
    }
    
    glPopMatrix();
}

// Tipo 11: Fusion Core - núcleo energético com halos concêntricos
inline void AsteroidManager::renderFusionCore(float size, RenderMode renderMode, const Vector3& rotation) {
    glPushMatrix();
    
    // Rotação rápida do núcleo principal
    glRotatef(rotation.x * 50.0f, 1, 0, 0);
    glRotatef(rotation.y * 60.0f, 0, 1, 0);
    glRotatef(rotation.z * 45.0f, 0, 0, 1);
    
    // Núcleo central - esfera energética
    glPushMatrix();
    glScalef(1.1f, 1.1f, 1.1f); // Ligeiramente maior para efeito
    if (renderMode == RenderMode::WIREFRAME) {
        glutWireSphere(size * 0.4f, 16, 16);
    } else {
        glutSolidSphere(size * 0.4f, 16, 16);
    }
    glPopMatrix();
    
    // Halos energéticos concêntricos
    for (int halo = 0; halo < 3; halo++) {
        glPushMatrix();
        
        // Cada halo tem rotação independente
        float haloSpeed = (halo + 1) * 20.0f;
        glRotatef(rotation.y * haloSpeed + halo * 120.0f, 0, 1, 0);
        glRotatef(rotation.x * (haloSpeed * 0.7f) + halo * 60.0f, 1, 0, 0);
        
        float haloRadius = size * (0.8f + halo * 0.4f);
        
        // Cria halo usando torus achatado
        glPushMatrix();
        glScalef(1.0f, 0.1f, 1.0f); // Achatar o torus para parecer um disco
        if (renderMode == RenderMode::WIREFRAME) {
            glutWireTorus(haloRadius * 0.1f, haloRadius, 8, 16);
        } else {
            glutSolidTorus(haloRadius * 0.1f, haloRadius, 8, 16);
        }
        glPopMatrix();
        
        // Partículas energéticas no halo
        for (int particle = 0; particle < 6; particle++) {
            glPushMatrix();
            
            float particleAngle = (particle * 60.0f) + rotation.z * (30.0f + halo * 10.0f);
            glRotatef(particleAngle, 0, 1, 0);
            glTranslatef(haloRadius, 0, 0);
            
            float particleSize = size * 0.08f;
            if (renderMode == RenderMode::WIREFRAME) {
                glutWireOctahedron();
            } else {
                glScalef(particleSize, particleSize, particleSize);
                glutSolidOctahedron();
            }
            
            glPopMatrix();
        }
        
        glPopMatrix();
    }
    
    glPopMatrix();
}

#endif // ___ASTEROIDS__H___

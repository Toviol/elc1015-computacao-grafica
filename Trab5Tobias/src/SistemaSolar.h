/*
DESCRIÇÃO:
Este arquivo gerencia todo o sistema solar, incluindo o sol central e todos
os planetas com suas respectivas órbitas, propriedades de material e texturas.

FUNCIONALIDADES:
- Gerenciamento completo do sistema solar
- Planetas com propriedades físicas e de material específicas
- Órbitas realistas com velocidades diferentes
- Texturas individuais para cada planeta
- Lua orbitando a Terra
- Anéis de Saturno
- Iluminação realista

PLANETAS IMPLEMENTADOS:
- Mercúrio, Vênus, Terra (com Lua), Marte
- Júpiter, Saturno (com anéis), Urano, Netuno

*/

#ifndef ___SISTEMA_SOLAR__H___
#define ___SISTEMA_SOLAR__H___

#include "Planet.h"
#include "Sun.h"
#include "Vector3.h"
#include <vector>
#include <GL/glut.h>

class SistemaSolar {
public:
    static void initialize(const Vector3& sunPosition = Vector3(0, 0, 0));
    
    static void update(float deltaTime, const Vector3& sunPosition = Vector3(0, 0, 0));
    
    static void render(const Vector3& lightPosition, float deltaTime);
    
    static void renderPlanets();
    
    static const std::vector<Planet>& getPlanets() { return planets; }
    
private:
    static std::vector<Planet> planets;
    static bool initialized;
    
    static void createPlanets();
    
    static void loadPlanetTextures();
};

// === IMPLEMENTAÇÕES INLINE ===

inline void SistemaSolar::initialize(const Vector3& sunPosition) {
    if (!initialized) {
        createPlanets();
        loadPlanetTextures();
        // Inicializa posições dos planetas com base na posição do sol
        for (auto& planet : planets) {
            planet.initializePosition(sunPosition);
        }
        initialized = true;
    }
}

inline void SistemaSolar::update(float deltaTime, const Vector3& sunPosition) {
    for (auto& planet : planets) {
        planet.updateOrbit(deltaTime, sunPosition);
    }
}

inline void SistemaSolar::render(const Vector3& lightPosition, float deltaTime) {
    // Renderiza o sol
    Sun::render(lightPosition, deltaTime);
    
    // Renderiza os planetas
    renderPlanets();
}

inline void SistemaSolar::renderPlanets() {
    for (auto& planet : planets) {
        planet.render();
    }
}

inline void SistemaSolar::createPlanets() {
    planets.clear();
    
    // Raios reais baseados no sol (raio = 0.7 = 695.700 km)
    const float SUN_RADIUS = 0.7f;  // 695.700 km
    const float DISTANCE_FACTOR = 0.1f;  // Fator para distâncias orbitais
    const float RADIO_FACTOR= 100.0f;  // Fator para raios dos planetas
    
    // MERCÚRIO (raio real: 2.439 km)
    planets.emplace_back(
        "Mercúrio",
        SUN_RADIUS * (2439.0f / 695700.0f) * RADIO_FACTOR,  // 0.0025 (proporção real)
        58.0f * DISTANCE_FACTOR + SUN_RADIUS,             // distância orbital
        4.74f,                               // velocidade orbital
        58.65f,                              // velocidade rotação
        0.5f,                                // difusa
        0.2f,                                // especular  
        10.0f,                               // brilho
        ".\\trab5Tobias\\imagens\\planets\\8k_mercury.bmp"
    );
    
    // VÊNUS (raio real: 6.051 km)
    planets.emplace_back(
        "Vênus",
        SUN_RADIUS * (6051.0f / 695700.0f) * RADIO_FACTOR,  // 0.0061 (proporção real)
        108.0f * DISTANCE_FACTOR + SUN_RADIUS,          // distância orbital
        3.50f,                               // velocidade orbital
        -243.0f,                             // velocidade rotação (retrógrada)
        0.7f,                                // difusa
        0.3f,                                // especular
        20.0f,                               // brilho
        ".\\trab5Tobias\\imagens\\planets\\4k_venus_atmosphere.bmp"
    );
    
    // TERRA (raio real: 6.371 km)
    planets.emplace_back(
        "Terra",
        SUN_RADIUS * (6371.0f / 695700.0f) * RADIO_FACTOR,  // 0.0064 (proporção real)
        150.0f * DISTANCE_FACTOR + SUN_RADIUS,         // distância orbital
        2.98f,                               // velocidade orbital
        24.0f,                               // velocidade rotação
        0.8f,                                // difusa
        0.5f,                                // especular
        40.0f,                               // brilho
        ".\\trab5Tobias\\imagens\\planets\\8k_earth_daymap.bmp",
        false,                               // sem anéis
        true                                 // com lua
    );
    
    // MARTE (raio real: 3.389 km)
    planets.emplace_back(
        "Marte",
        SUN_RADIUS * (3389.0f / 695700.0f) * RADIO_FACTOR,  // 0.0034 (proporção real)
        228.0f * DISTANCE_FACTOR + SUN_RADIUS,         // distância orbital
        2.41f,                               // velocidade orbital
        24.6f,                               // velocidade rotação
        0.6f,                                // difusa
        0.1f,                                // especular
        5.0f,                                // brilho
        ".\\trab5Tobias\\imagens\\planets\\8k_mars.bmp"
    );
    
    // JÚPITER (raio real: 69.911 km)
    planets.emplace_back(
        "Júpiter",
        SUN_RADIUS * (69911.0f / 695700.0f) * RADIO_FACTOR, // 0.0704 (proporção real)
        //300.0f * DISTANCE_FACTOR + SUN_RADIUS,         // distância orbital
        778.5f * DISTANCE_FACTOR + SUN_RADIUS, // distância orbital
        1.31f,                               // velocidade orbital
        9.9f,                                // velocidade rotação
        0.9f,                                // difusa
        0.4f,                                // especular
        30.0f,                               // brilho
        ".\\trab5Tobias\\imagens\\planets\\8k_jupiter.bmp"
    );
    
    // SATURNO (raio real: 58.232 km)
    planets.emplace_back(
        "Saturno",
        SUN_RADIUS * (58232.0f / 695700.0f) * RADIO_FACTOR, // 0.0586 (proporção real)
        //500.0f * DISTANCE_FACTOR + SUN_RADIUS,            // distância orbital
        1433.5f * DISTANCE_FACTOR + SUN_RADIUS,         // distância orbital
        0.97f,                               // velocidade orbital
        10.7f,                               // velocidade rotação
        0.9f,                                // difusa
        0.4f,                                // especular
        30.0f,                               // brilho
        ".\\trab5Tobias\\imagens\\planets\\8k_saturn.bmp",
        true                                 // com anéis
    );
    
    // URANO (raio real: 25.362 km)
    planets.emplace_back(
        "Urano",
        SUN_RADIUS * (25362.0f / 695700.0f) * RADIO_FACTOR, // 0.0255 (proporção real)
        //700.0f * DISTANCE_FACTOR + SUN_RADIUS,            // distância orbital
        2872.5f * DISTANCE_FACTOR + SUN_RADIUS,         // distância orbital
        0.68f,                               // velocidade orbital
        17.2f,                               // velocidade rotação
        0.8f,                                // difusa
        0.3f,                                // especular
        25.0f,                               // brilho
        ".\\trab5Tobias\\imagens\\planets\\2k_uranus.bmp"
    );
    
    // NETUNO (raio real: 24.622 km)
    planets.emplace_back(
        "Netuno",
        SUN_RADIUS * (24622.0f / 695700.0f) * RADIO_FACTOR, // 0.0248 (proporção real)
        //900.0f * DISTANCE_FACTOR + SUN_RADIUS,            // distância orbital
        4495.1f * DISTANCE_FACTOR + SUN_RADIUS,         // distância orbital
        0.54f,                               // velocidade orbital
        16.1f,                               // velocidade rotação
        0.8f,                                // difusa
        0.3f,                                // especular
        25.0f,                               // brilho
        ".\\trab5Tobias\\imagens\\planets\\2k_neptune.bmp"
    );
}

inline void SistemaSolar::loadPlanetTextures() {
    for (auto& planet : planets) {
        planet.loadTexture();
    }
}

#endif // ___SISTEMA_SOLAR__H___
// Este arquivo define a classe Enemy e a estrutura Bomb.
// A classe Enemy gerencia os diferentes tipos de inimigos no jogo,
// incluindo sua lógica de movimento, comportamento, dano e destruição.
// Suporta diferentes níveis de inimigos com características distintas:
// - Nível 1: Inimigo triangular estático.
// - Nível 2: Inimigo estrela que se torna ativo e móvel após receber dano.
// - Nível 3: Inimigo octogonal estático que libera estilhaços ao ser destruído.
// - Nível 4: Inimigo avião que segue uma trajetória e solta bombas.
// A estrutura Bomb define as bombas lançadas pelo inimigo avião.
// Funcionalidades incluem:
// - Inicialização de inimigos com base no nível.
// - Geração de vértices para hitbox e renderização.
// - Lógica de dano e destruição.
// - Comportamento específico para cada nível (movimento da estrela, voo do avião, etc.).
// - Geração de estilhaços e bombas.
// - Renderização dos inimigos e suas bombas.
// - Gerenciamento de barras de vida para inimigos.
#ifndef ENEMIES_H_INCLUDED
#define ENEMIES_H_INCLUDED

#include "Vector2.h"
#include "gl_canvas2d.h"
#include <cstdlib> 
#include <vector>  
#include <cmath>   
#include "Track.h"      
#include "Projectile.h" 
#include "CollisionUtils.h" 

#ifndef M_PI 
#define M_PI 3.14159265358979323846
#endif

class Tank; 

// Estrutura que representa uma bomba lançada pelo inimigo avião (Nível 4).
struct Bomb {
    Vector2 position;
    Vector2 velocity;
    float timer; 
    bool active;
    float explosionRadius; 
    float damage;
    float currentAngle; 
    float rotationSpeed;

    // Construtor da bomba.
    Bomb(Vector2 pos, Vector2 vel, float initialFallAngle, float explosionTime = 2.0f, float dmg = 25.0f)
        : position(pos), velocity(vel), timer(explosionTime), active(true),
          explosionRadius(40.0f), damage(dmg), currentAngle(initialFallAngle), rotationSpeed(1.5f) {} 

    // Atualiza a posição, rotação e temporizador da bomba.
    void update(float fps) {
        if (!active) return;
        position = position + velocity * (1.0f / fps);
        
        velocity.y += 10.0f * (1.0f/fps); 
        currentAngle += rotationSpeed * (1.0f/fps);

        timer -= (1.0f / fps);
        if (timer <= 0.0f) {
            active = false; 
        }
    }

    // Desenha a bomba na tela.
    void draw() const {
        if (!active) return;
        
        glPushMatrix();
        glTranslatef(position.x, position.y, 0);
        glRotatef(currentAngle * 180.0f / M_PI, 0, 0, 1); 

        
        CV::color(0.15f, 0.15f, 0.15f);
        CV::circleFill(0, 0, 7, 16); 

        
        CV::color(0.4f, 0.4f, 0.4f);
        
        float finPointsX1[] = {0, -2, 2};
        float finPointsY1[] = {7, 12, 12};
        CV::polygonFill(finPointsX1, finPointsY1, 3);

        
        CV::color(0.25f, 0.25f, 0.25f);
        CV::circleFill(0, 0, 3, 8); 

        glPopMatrix();
    }
};


class Enemy
{
public:
    // Enumeração para o estado de ativação do inimigo estrela (Nível 2).
    enum class StarActivationState { NORMAL, ACTIVATED, DESTROYED_AFTER_ACTIVATION };

    Vector2 position; 
    int level;        
    int health;       
    float size;       
    std::vector<Vector2> vertices; 

    
    int initialHealth;
    float healthBarVisibleTimer;

    
    StarActivationState starMode;
    Vector2 movementDirection; 
    float movementSpeed;
    float currentRotationAngle;
    float rotationSpeed;
    std::vector<Vector2> localStarShapeVertices; 

    
    std::vector<Bomb> activeBombs;
    std::vector<Vector2> flightPathNodes;
    int currentFlightPathNodeIndex;
    float planeTargetReachedThresholdSq;
    float planeSineCycle; 
    float planeBombDropCooldownMax;
    float planeBombDropTimer;
    Vector2 planeVisualDirection;      
    Vector2 planeCurrentDisplayPosition; 
    bool planePathInitialized;
    float planePathOffsetSeed; 
    float planeSineAmplitudeModifier; 


    // Retorna a contribuição de vida do inimigo para a barra de progresso do nível.
    static float getHealthContribution(int enemyLevel) {
        if (enemyLevel == 1) return 1.0f;
        if (enemyLevel == 2) return 3.0f; 
        if (enemyLevel == 3) return 4.0f;
        if (enemyLevel == 4) return 10.0f; 
        return 0.0f; 
    }

    // Retorna o valor da pontuação ao destruir este inimigo.
    int getScoreValue() const {
        if (level == 1) return 1;
        if (level == 2) return 3; 
        if (level == 3) return 5;
        if (level == 4) return 15; 
        return 0; 
    }

    // Construtor da classe Enemy.
    Enemy(Vector2 pos, int lvl)
    {
        position = pos;
        level = lvl;
        starMode = StarActivationState::NORMAL;
        currentRotationAngle = 0.0f;
        healthBarVisibleTimer = 0.0f; 
        
        planePathInitialized = false; 

        if (level == 1) {
            health = 1;
            size = 10.0f;
            movementSpeed = 0; 
            rotationSpeed = 0; 
        } else if (level == 2) {
            health = 2; 
            size = 15.0f;
            movementSpeed = 150.0f; 
            rotationSpeed = 2.5f;   
            
            int numStarVisualPoints = 4; 
            float outerRadius = size;
            float innerRadiusFactor = 0.30f;
            float innerRadius = size * innerRadiusFactor;
            float staticBaseRotation = M_PI / 4.0f; 

            for (int i = 0; i < numStarVisualPoints; ++i) {
                float angle_outer = staticBaseRotation + (float)i * (2.0f * M_PI / numStarVisualPoints);
                localStarShapeVertices.push_back(Vector2(outerRadius * cos(angle_outer), outerRadius * sin(angle_outer)));
                float angle_inner = staticBaseRotation + ((float)i + 0.5f) * (2.0f * M_PI / numStarVisualPoints);
                localStarShapeVertices.push_back(Vector2(innerRadius * cos(angle_inner), innerRadius * sin(angle_inner)));
            }
        } else if (level == 3) {
            health = 4;
            size = 20.0f;
            movementSpeed = 0; 
            rotationSpeed = 0; 
        } else if (level == 4) { 
            health = 10; 
            size = 35.0f;   
            movementSpeed = 100.0f; 
            planeBombDropCooldownMax = 3.5f; 
            planeBombDropTimer = 1.0f; 
            planeTargetReachedThresholdSq = 60.0f * 60.0f; 
            currentFlightPathNodeIndex = 0;
            planeSineCycle = (rand() / (float)RAND_MAX) * 2.0f * M_PI; 
            planeVisualDirection.set(0, -1); 
            planeCurrentDisplayPosition = pos;
            planePathInitialized = false; 
            planePathOffsetSeed = (rand() / (float)RAND_MAX) * 20.0f - 10.0f; 
            planeSineAmplitudeModifier = 0.75f + (rand() / (float)RAND_MAX) * 0.5f; 
            
            
            rotationSpeed = 0; 
        }
        initialHealth = health; 
        generateInitialVertices(); 
    }

    // Gera os vértices iniciais para a hitbox do inimigo.
    void generateInitialVertices() { 
        vertices.clear();
        if (level == 1) { 
            float s = size;
            vertices.push_back(Vector2(position.x, position.y - s));
            vertices.push_back(Vector2(position.x - s * cos(M_PI / 6.0f), position.y + s * sin(M_PI / 6.0f)));
            vertices.push_back(Vector2(position.x + s * cos(M_PI / 6.0f), position.y + s * sin(M_PI / 6.0f)));
        } else if (level == 2) {
            
            if (!localStarShapeVertices.empty()) {
                for(const auto& localP : localStarShapeVertices) {
                    vertices.push_back(position + localP);
                }
            } else { 
                int numSides = 8; float hitboxRadius = size;
                for (int i = 0; i < numSides; ++i) {
                    float angle = (2.0f * M_PI / numSides) * i;
                    vertices.push_back(Vector2(position.x + hitboxRadius * cos(angle), position.y + hitboxRadius * sin(angle)));
                }
            }
        } else if (level == 3) { 
            int numSides = 8; float hitboxRadius = size;
            for (int i = 0; i < numSides; ++i) {
                float angle = (2.0f * M_PI / numSides) * i;
                vertices.push_back(Vector2(position.x + hitboxRadius * cos(angle), position.y + hitboxRadius * sin(angle)));
            }
        } else if (level == 4) {
            
            
            
            float s = size * 0.7f; 
            
            
            Vector2 localPoints[] = {
                Vector2(-s, -s * 0.5f), Vector2(s, -s * 0.5f),
                Vector2(s, s * 0.5f), Vector2(-s, s * 0.5f)
            };
            for(const auto& lp : localPoints) {
                 
                vertices.push_back(position + lp);
            }
        }
    }
    
    // Atualiza os vértices da hitbox para inimigos dinâmicos (Estrela Nível 2, Avião Nível 4).
    void updateDynamicHitboxVertices() { 
        if (level == 2 && starMode == StarActivationState::ACTIVATED && !localStarShapeVertices.empty()) {
            vertices.clear();
            float cosA = cos(currentRotationAngle);
            float sinA = sin(currentRotationAngle);
            for (const auto& localP : localStarShapeVertices) {
                float rotatedX = localP.x * cosA - localP.y * sinA;
                float rotatedY = localP.x * sinA + localP.y * cosA;
                vertices.push_back(Vector2(position.x + rotatedX, position.y + rotatedY));
            }
        } else if (level == 4) { 
            
            vertices.clear();
            float s = size * 0.7f; 
            float angle = atan2(planeVisualDirection.y, planeVisualDirection.x) + (M_PI/2.0f) ; 
            float cosA = cos(angle);
            float sinA = sin(angle);
            Vector2 localPoints[] = {
                Vector2(-s*0.8f, -s * 0.6f), Vector2(s*0.8f, -s * 0.6f), 
                Vector2(s*0.8f, s * 0.6f), Vector2(-s*0.8f, s * 0.6f)
            };
            for(const auto& lp : localPoints) {
                float rotatedX = lp.x * cosA - lp.y * sinA;
                float rotatedY = lp.x * sinA + lp.y * cosA;
                vertices.push_back(planeCurrentDisplayPosition + Vector2(rotatedX, rotatedY));
            }
        }
        
        
        
    }

    // Aplica dano ao inimigo e lida com a ativação da estrela (Nível 2).
    void takeDamage(int damageAmount)
    {
        if (isDestroyed()) return; 

        if (level == 2) {
            if (starMode == StarActivationState::NORMAL) {
                health -= damageAmount;
                if (health <= 0) { 
                    starMode = StarActivationState::ACTIVATED;
                    health = 1; 
                    
                    float angle = (static_cast<float>(rand()) / RAND_MAX) * 2.0f * M_PI;
                    movementDirection.set(cos(angle), sin(angle));
                    
                }
            } else if (starMode == StarActivationState::ACTIVATED) {
                health -= damageAmount; 
                if (health <= 0) {
                    starMode = StarActivationState::DESTROYED_AFTER_ACTIVATION;
                }
            }
        } else {
            health -= damageAmount;
            if (health < 0) health = 0;
        }

        if (initialHealth > 1) { 
            healthBarVisibleTimer = 1.0f; 
        }
    }

    // Verifica se o inimigo foi destruído.
    bool isDestroyed() const
    {
        if (level == 2) {
            return starMode == StarActivationState::DESTROYED_AFTER_ACTIVATION;
        }
        return health <= 0;
    }

    // Atualiza o estado da estrela ativada (Nível 2).
    void updateActivatedStar(float fps, const Track& track, std::vector<Enemy>& allEnemies, Tank* playerTank);

    
    // Inicializa a trajetória de voo para o inimigo avião (Nível 4).
    void initializeFlightPath(const Track& track);
    // Atualiza a lógica específica do inimigo avião (Nível 4).
    void updatePlaneSpecifics(float fps, const Track& track, Tank* playerTank);
    // Faz o inimigo avião (Nível 4) soltar uma bomba.
    void dropBomb();
    // Desenha os elementos visuais do inimigo avião (Nível 4).
    void drawPlaneVisuals() const;


    
    // Atualiza o temporizador de visibilidade da barra de vida.
    void updateHealthBarDisplay(float fps);
    // Desenha a barra de vida do inimigo.
    void drawHealthBar() const;

    // Desenha o inimigo na tela.
    void draw(); 
    // Gera estilhaços quando um inimigo do Nível 3 é destruído.
    std::vector<Projectile> generateShrapnel(float projectileSpeed, float projectileDamage, float shrapnelWidth, float shrapnelHeight) const; 
};

#endif // ENEMIES_H_INCLUDED
// Este arquivo define a classe Tank, que representa o tanque do jogador no jogo.
// A classe gerencia o estado do tanque, incluindo sua posição, rotação, velocidade,
// vida, e suas habilidades (Nitro, Tiro Rápido, Super Rajada, Escudo).
#ifndef TANK_H_INCLUDED
#define TANK_H_INCLUDED

#include "gl_canvas2d.h" 
#include "Vector2.h"
#include <vector>
#include "Projectile.h"
#include <algorithm>
#include "Enemies.h" 
#include "Track.h"
#include "CollisionUtils.h" 
#include "Explosion.h"
#include "NitroBoost.h"
#include "RapidFire.h" 
#include "SuperBurst.h" 
#include "Shield.h" 
#include <cmath>

#include "TankRenderer.h" 

class Tank
{
public:
    std::vector<Vector2> vertices;
    Vector2 pivot;
    Vector2 direction;
    float baseSpeed; 
    float currentSpeed; 
    float bodyW;
    float bodyH;
    float health;

    std::vector<Projectile> projectiles;
    float cooldown;
    float cooldownTimer;
    float pushBackTimer;
    float rotationCollisionCooldownTimer;

    std::vector<Explosion> activeExplosions;
    NitroBoost nitro; 
    RapidFire rapidFire; 
    SuperBurst superBurst; 
    Shield shield; 
    
    TankRenderer renderer; 

    // Construtor da classe Tank.
    // Inicializa os atributos do tanque, como dimensões, posição, vida e habilidades.
    Tank(int initialTankHeight, int initialTankWidth, float pivotX, float pivotY)
        : renderer(*this) 
    {
        pivot.set(pivotX, pivotY);
        direction.set(0, -1); 
        baseSpeed = 20.0f; 
        currentSpeed = baseSpeed;

        this->bodyW = static_cast<float>(initialTankWidth);
        this->bodyH = static_cast<float>(initialTankHeight);

        float halfWidth = this->bodyW / 2.0f;
        float halfHeight = this->bodyH / 2.0f;

        vertices.clear(); 
        vertices.push_back(Vector2(pivotX - halfWidth, pivotY - halfHeight)); 
        vertices.push_back(Vector2(pivotX + halfWidth, pivotY - halfHeight)); 
        vertices.push_back(Vector2(pivotX + halfWidth, pivotY + halfHeight)); 
        vertices.push_back(Vector2(pivotX - halfWidth, pivotY + halfHeight)); 

        cooldown = 1.0f; 
        cooldownTimer = 0.0f;
        health = 100.0f; 
        pushBackTimer = 0.0f; 
        rotationCollisionCooldownTimer = 0.0f;
    }

    // Aplica dano ao tanque, reduzindo sua vida.
    // O dano é ignorado se a habilidade Escudo estiver ativa.
    void takeDamage(float amount)
    {
        if (!shield.isEffectActive())
        {
            health -= amount;
            if (health < 0.0f)
            {
                health = 0.0f;
            }
        }
    }

    // Verifica se o tanque foi destruído (vida menor ou igual a zero).
    bool isDestroyed() const { 
        return health <= 0.0f;
    }

    // Atualiza o estado da habilidade Nitro.
    // Permite ativar o nitro se o jogador desejar.
    void updateNitro(float fps, bool wantsToActivate) {
        nitro.update(fps);
        if (wantsToActivate) {
            nitro.activate();
        }
    }

    // Atualiza o estado da habilidade Tiro Rápido.
    // Permite ativar o tiro rápido se o jogador desejar.
    void updateRapidFire(float fps, bool wantsToActivate) { 
        rapidFire.update(fps);
        if (wantsToActivate) {
            rapidFire.activate();
        }
    }

    // Atualiza o estado da habilidade Super Rajada.
    // Permite ativar a super rajada se o jogador desejar.
    void updateSuperBurst(float fps, bool wantsToActivate) { 
        superBurst.update(fps);
        if (wantsToActivate) {
            superBurst.activate();
        }
    }

    // Atualiza o estado da habilidade Escudo.
    // Permite ativar o escudo se o jogador desejar.
    void updateShield(float fps, bool wantsToActivate) { 
        shield.update(fps);
        if (wantsToActivate) {
            shield.activate();
        }
    }

    // Move o tanque com base na direção atual e velocidade.
    // Lida com colisões com a pista e inimigos, aplicando recuo e dano se necessário.
    void move(float fps, const Track& track, const std::vector<Enemy> &enemies)
    {
        currentSpeed = (baseSpeed * nitro.getSpeedMultiplier()) * (1.0f / fps) ; 

        float proposedDeltaX = 0.0f;
        float proposedDeltaY = 0.0f;
        float actualDeltaX = 0.0f;
        float actualDeltaY = 0.0f;

        if (pushBackTimer > 0.0f)
        {
            float pushBackSpeedFactor = 0.5f; 
            float pushBackDeltaX = -direction.x * baseSpeed * (1.0f/fps) * pushBackSpeedFactor;
            float pushBackDeltaY = -direction.y * baseSpeed * (1.0f/fps) * pushBackSpeedFactor;

            pushBackTimer -= 1.0f / fps;
            if (pushBackTimer < 0.0f) pushBackTimer = 0.0f;

            bool recuoCollidesWithTrack = false;
            bool recuoCollidesWithEnemy = false;

            if (pushBackDeltaX != 0.0f || pushBackDeltaY != 0.0f)
            {
                std::vector<Vector2> proposedRecuoVertices;
                for (const auto& currentVertex : vertices)
                {
                    proposedRecuoVertices.push_back(Vector2(currentVertex.x + pushBackDeltaX, currentVertex.y + pushBackDeltaY));
                }
                for (const auto& proposedVertex : proposedRecuoVertices)
                {
                    if (!track.isPointInsideTrack(proposedVertex))
                    {
                        recuoCollidesWithTrack = true;
                        break;
                    }
                }
                if (!recuoCollidesWithTrack)
                {
                    for (const auto& enemy : enemies)
                    {
                        if (enemy.level == 4) continue; 
                        if (CollisionUtils::checkPolygonPolygonCollision(proposedRecuoVertices, enemy.vertices))
                        {
                            recuoCollidesWithEnemy = true;
                            break;
                        }
                    }
                }
            }
            if (!recuoCollidesWithTrack && !recuoCollidesWithEnemy)
            {
                actualDeltaX = pushBackDeltaX;
                actualDeltaY = pushBackDeltaY;
            }
        }
        else 
        {
            proposedDeltaX = direction.x * currentSpeed;
            proposedDeltaY = direction.y * currentSpeed;
            actualDeltaX = proposedDeltaX; 
            actualDeltaY = proposedDeltaY;

            bool collisionWithTrack = false;
            if (actualDeltaX != 0.0f || actualDeltaY != 0.0f)
            {
                std::vector<Vector2> nextTankVertices;
                for (const auto& v : vertices) {
                    nextTankVertices.push_back(Vector2(v.x + actualDeltaX, v.y + actualDeltaY));
                }
                for (const auto& nextVertex : nextTankVertices) {
                    if (!track.isPointInsideTrack(nextVertex)) {
                        collisionWithTrack = true;
                        break;
                    }
                }
            }

            if (collisionWithTrack)
            {
                actualDeltaX = 0; 
                actualDeltaY = 0;
                takeDamage(10.0f); 
                pushBackTimer = 2.0f; 
            }
            else 
            {
                static float enemyDamagePushbackCooldownTimer = 0.0f; 
                if (enemyDamagePushbackCooldownTimer > 0.0f)
                {
                    enemyDamagePushbackCooldownTimer -= 1.0f / fps;
                    if(enemyDamagePushbackCooldownTimer < 0.0f) enemyDamagePushbackCooldownTimer = 0.0f;
                }

                if (actualDeltaX != 0.0f || actualDeltaY != 0.0f) 
                {
                    std::vector<Vector2> nextTankVertices; 
                    for(const auto& v : vertices) {
                        nextTankVertices.push_back(Vector2(v.x + actualDeltaX, v.y + actualDeltaY));
                    }
                    for (const auto &enemy : enemies)
                    {
                        if (enemy.level == 4) continue; 
                        if (CollisionUtils::checkPolygonPolygonCollision(nextTankVertices, enemy.vertices))
                        {
                            actualDeltaX = 0; 
                            actualDeltaY = 0;
                            if (enemyDamagePushbackCooldownTimer <= 0.0f) 
                            {
                                takeDamage(5.0f); 
                                enemyDamagePushbackCooldownTimer = 1.0f; 
                                pushBackTimer = 2.0f; 
                            }
                            break; 
                        }
                    }
                }
            }
        }

        if (actualDeltaX != 0.0f || actualDeltaY != 0.0f)
        {
            pivot.x += actualDeltaX;
            pivot.y += actualDeltaY;
            for (auto &vertex : vertices)
            {
                vertex.x += actualDeltaX;
                vertex.y += actualDeltaY;
            }
        }
    }

    // Rotaciona o tanque com base em um ângulo delta.
    // Lida com colisões com a pista e inimigos durante a rotação.
    void rotateDirection(float angleDelta, float fps, const Track& track, const std::vector<Enemy>& enemies)
    {
        if (rotationCollisionCooldownTimer > 0.0f)
        {
            rotationCollisionCooldownTimer -= 1.0f / fps;
            if (rotationCollisionCooldownTimer < 0.0f) rotationCollisionCooldownTimer = 0.0f;
        }
        if (rotationCollisionCooldownTimer > 0.0f)
        {
            return;
        }

        float rotationAmount = angleDelta / fps * 12.0f;

        std::vector<Vector2> proposedVertices;
        float cosA = cos(rotationAmount);
        float sinA = sin(rotationAmount);

        for (const auto &currentVertex : vertices)
        {
            float translatedX = currentVertex.x - pivot.x;
            float translatedY = currentVertex.y - pivot.y;
            float rotatedX = translatedX * cosA - translatedY * sinA;
            float rotatedY = translatedX * sinA + translatedY * cosA;
            proposedVertices.push_back(Vector2(rotatedX + pivot.x, rotatedY + pivot.y));
        }


        bool collisionWithTrack = false;
        for (const auto& proposedVertex : proposedVertices)
        {
            if (!track.isPointInsideTrack(proposedVertex))
            {
                collisionWithTrack = true;
                break;
            }
        }

        bool collisionWithEnemies = false;
        if (!collisionWithTrack)
        {
            for (const auto& enemy : enemies)
            {
                if (enemy.level == 4) continue; 
                if (CollisionUtils::checkPolygonPolygonCollision(proposedVertices, enemy.vertices))
                {
                    collisionWithEnemies = true;
                    break;
                }
            }
        }

        if (!collisionWithTrack && !collisionWithEnemies)
        {
            float newDirX = direction.x * cosA - direction.y * sinA;
            float newDirY = direction.x * sinA + direction.y * cosA;
            direction.set(newDirX, newDirY);
            vertices = proposedVertices;
        }
        else 
        {
            if (collisionWithTrack)
            {
                takeDamage(10.0f); 
            }
            else if (collisionWithEnemies)
            {
                takeDamage(5.0f);  
            }
            rotationCollisionCooldownTimer = 0.5f; 
        }
    }

    // Dispara um projétil na direção do mouse.
    // Considera o estado das habilidades Tiro Rápido e Super Rajada para modificar o disparo.
    void shoot(float fps, float mouseX, float mouseY)
    {
        bool canShootRegular = cooldownTimer <= 0.0f;
        bool tryRapidFire = rapidFire.isEffectActive() && rapidFire.canShootBurst();
        bool trySuperBurst = superBurst.isEffectActive() && superBurst.canShootBurst();
        
        float playerProjectileDamage = 1.0f; 
        float projectileW = bodyW * 0.1f;    
        float projectileH = bodyH * 0.25f;   

        if (trySuperBurst)
        {
            float deltaX = mouseX - pivot.x;
            float deltaY = mouseY - pivot.y;
            float angle = atan2(deltaY, deltaX);
            Vector2 turretDirection(cos(angle), sin(angle));
            float barrelLength = bodyH * 0.6f; 
            Vector2 startPos(pivot.x + turretDirection.x * barrelLength, pivot.y + turretDirection.y * barrelLength);
            float projectileSpeed = 600.0f;
            projectiles.emplace_back(startPos, turretDirection, projectileSpeed, projectileW, projectileH, Projectile::Owner::PLAYER, playerProjectileDamage * 1.5f); 
            
            superBurst.recordBurstShot(fps);
        }
        else if (tryRapidFire)
        {
            float deltaX = mouseX - pivot.x;
            float deltaY = mouseY - pivot.y;
            float angle = atan2(deltaY, deltaX);
            Vector2 turretDirection(cos(angle), sin(angle));
            float barrelLength = bodyH * 0.6f; 
            Vector2 startPos(pivot.x + turretDirection.x * barrelLength, pivot.y + turretDirection.y * barrelLength);
            float projectileSpeed = 600.0f;
            projectiles.emplace_back(startPos, turretDirection, projectileSpeed, projectileW, projectileH, Projectile::Owner::PLAYER, playerProjectileDamage);
            
            rapidFire.recordBurstShot(fps); 
        }
        else if (!rapidFire.isEffectActive() && !superBurst.isEffectActive() && canShootRegular) 
        {
            float deltaX = mouseX - pivot.x;
            float deltaY = mouseY - pivot.y;
            float angle = atan2(deltaY, deltaX);
            Vector2 turretDirection(cos(angle), sin(angle));
            float barrelLength = bodyH * 0.6f; 
            Vector2 startPos(pivot.x + turretDirection.x * barrelLength, pivot.y + turretDirection.y * barrelLength);
            float projectileSpeed = 600.0f;
            projectiles.emplace_back(startPos, turretDirection, projectileSpeed, projectileW, projectileH, Projectile::Owner::PLAYER, playerProjectileDamage);

            cooldownTimer = cooldown * fps; 
        }
    }

    // Atualiza a posição dos projéteis e verifica colisões.
    // Projéteis do jogador colidem com inimigos, e estilhaços de inimigos colidem com o jogador.
    // Projéteis são removidos se colidirem ou saírem da pista.
    void updateProjectiles(float fps, const Track& track, std::vector<Enemy> &enemies)
    {
        for (auto &proj : projectiles)
        {
            proj.move(fps); 
        }

        for (auto it_proj = projectiles.begin(); it_proj != projectiles.end(); )
        {
            bool projectile_removed = false;

            if (it_proj->ownerType == Projectile::Owner::PLAYER) 
            {
                for (auto &enemy : enemies)
                {
                    if (enemy.isDestroyed()) continue;

                    float dx = it_proj->position.x - enemy.position.x;
                    float dy = it_proj->position.y - enemy.position.y;
                    float distanceSq = dx * dx + dy * dy;
                    float enemyCollisionRadius = enemy.size; 

                    if (distanceSq < (enemyCollisionRadius * enemyCollisionRadius))
                    {
                        enemy.takeDamage(static_cast<int>(it_proj->damage));
                        addExplosion(it_proj->position, 1.0f); 
                        it_proj = projectiles.erase(it_proj); 
                        projectile_removed = true;
                        break; 
                    }
                }
            }
            else if (it_proj->ownerType == Projectile::Owner::ENEMY_SHRAPNEL) 
            {
                float shrapnelRadius = (it_proj->width + it_proj->height) / 4.0f; 
                if (shrapnelRadius < 1.0f) shrapnelRadius = 1.0f; 

                if (CollisionUtils::checkPolygonCircleCollision(this->vertices, it_proj->position, shrapnelRadius))
                {
                    this->takeDamage(it_proj->damage); 
                    addExplosion(it_proj->position, 0.7f); 
                    it_proj = projectiles.erase(it_proj);
                    projectile_removed = true;
                }
            }

            if (!projectile_removed) {
                if (!track.isPointInsideTrack(it_proj->position)) {
                    if(it_proj->ownerType == Projectile::Owner::PLAYER) { 
                        this->addExplosion(it_proj->position, 0.6f); 
                    }
                    it_proj = projectiles.erase(it_proj);
                } else {
                    ++it_proj; 
                }
            }
        }


        if (cooldownTimer > 0.0f)
        {
            cooldownTimer -= 1.0f; 
        }
    }

    // Adiciona uma nova explosão à lista de explosões ativas.
    void addExplosion(Vector2 position, float scale = 1.0f)
    {
        activeExplosions.emplace_back(position, scale);
    }

    // Atualiza o estado de todas as explosões ativas.
    // Remove explosões que não estão mais ativas.
    void updateExplosions(float fps)
    {
        for (auto &exp : activeExplosions)
        {
            exp.update(fps);
        }
        activeExplosions.erase(std::remove_if(activeExplosions.begin(), activeExplosions.end(),
                                              [](const Explosion& exp){ return !exp.active; }),
                               activeExplosions.end());
    }

    // Verifica colisão entre o tanque e um inimigo específico.
    // Ignora inimigos do tipo avião para esta verificação.
    bool checkCollisionWithEnemy(const Enemy &enemy)
    {
        if (enemy.level == 4) return false; 
        float dx = pivot.x - enemy.position.x;
        float dy = pivot.y - enemy.position.y;
        float distance = sqrt(dx * dx + dy * dy);
        float collisionDistance = (bodyW / 2.0f) + enemy.size; 
        return distance < collisionDistance;
    }

};

#endif // TANK_H_INCLUDED

// Este arquivo define a classe Game, que encapsula toda a lógica do jogo principal.
// Isso inclui o gerenciamento do tanque do jogador, inimigos, a pista de corrida,
// o placar, os níveis do jogo e os diferentes estados do jogo (jogando, transição
// de nível, fim de jogo). A classe Game é responsável por atualizar o estado
// dos elementos do jogo, renderizá-los na tela e lidar com as interações
// e regras do jogo.
#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "gl_canvas2d.h"
#include "Tank.h"
#include "Enemies.h"
#include "Track.h"
#include "Scoreboard.h"
#include "Levels.h"
#include <vector>
#include <cstdlib>
#include <sstream>
#include <cmath> 

class Game {
public:
    // Enumeração para os diferentes estados do jogo.
    enum class GameState { PLAYING, LEVEL_TRANSITION, GAME_OVER };

private:
    Tank *tanque;
    std::vector<Enemy> enemies;
    Track track;
    Scoreboard scoreboard;

    int currentLevel;
    GameState gameState;
    float levelTransitionTimer;
    const float timeBetweenLevels = 3.0f;

    float initialTotalEnemyHealthForLevel;
    float currentTotalEnemyHealthForLevel;

    int screenWidth, screenHeight;
    float currentFps;
    int currentMouseX, currentMouseY, currentIsPressed, currentKey;

    // Verifica se o tanque foi destruído e transiciona para o estado GAME_OVER.
    void checkAndTransitionToGameOver() {
        if (tanque && tanque->isDestroyed() && gameState != GameState::GAME_OVER) {
            gameState = GameState::GAME_OVER;
            
            
            
        }
    }

public:
    // Construtor da classe Game (com pista padrão).
    Game(int sw, int sh)
        : track(sw, sh, sw / 2.0f, sh / 2.0f, sh / 5.0f, sh / 2.0f, 10),
          scoreboard(),
          currentLevel(1),
          gameState(GameState::PLAYING),
          levelTransitionTimer(0.0f),
          initialTotalEnemyHealthForLevel(0.0f), 
          currentTotalEnemyHealthForLevel(0.0f), 
          screenWidth(sw),
          screenHeight(sh),
          currentFps(60.0f),
          currentMouseX(0), currentMouseY(0), currentIsPressed(0), currentKey(-1)
    {
        tanque = new Tank(100, 80, screenWidth / 2.0f, screenHeight / 2.0f - screenHeight / 4.0f);
        generateEnemies();
    }

    // Construtor da classe Game (com pista existente).
    Game(int sw, int sh, const Track& existingTrack)
        : track(existingTrack), 
          scoreboard(),
          currentLevel(1),
          gameState(GameState::PLAYING),
          levelTransitionTimer(0.0f),
          initialTotalEnemyHealthForLevel(0.0f),
          currentTotalEnemyHealthForLevel(0.0f),
          screenWidth(sw),
          screenHeight(sh),
          currentFps(60.0f),
          currentMouseX(0), currentMouseY(0), currentIsPressed(0), currentKey(-1)
    {
        tanque = new Tank(100, 80, screenWidth / 2.0f, screenHeight / 2.0f - screenHeight / 4.0f);
        generateEnemies();
    }

    // Destrutor da classe Game.
    ~Game() {
        delete tanque;
    }

    // Atualiza os inputs (mouse, teclado, fps) recebidos da classe Tela.
    void updateInputs(float fps, int mouseX, int mouseY, int isPressed, int key) {
        this->currentFps = fps;
        this->currentMouseX = mouseX;
        this->currentMouseY = mouseY;
        this->currentIsPressed = isPressed;
        this->currentKey = key;
    }

    // Renderiza todos os elementos do jogo.
    void render() {
        track.renderTrack();
        updateLevelLogic(); 

        if (!track.arePointsVisible()) { 
            if (gameState == GameState::PLAYING) {
                if (tanque) {
                    tanqueManager(); 
                }
                checkAndTransitionToGameOver(); 
                
                updateAndDrawEnemies(); 
                checkAndTransitionToGameOver(); 
            } else if (gameState == GameState::LEVEL_TRANSITION) {
                
                updateAndDrawEnemies(); 

                
                if (tanque) {
                    tanque->renderer.desenhaDetalhado();
                    tanque->renderer.desenhaTorre(currentMouseX, currentMouseY);
                    tanque->renderer.drawHealthBar();
                    tanque->renderer.drawProjectiles(); 
                    
                    
                    tanque->renderer.drawExplosions();
                }
            } else if (gameState == GameState::GAME_OVER) {
                
                
                updateAndDrawEnemies(); 

                
                
                if (tanque) {
                    tanque->updateExplosions(currentFps); 

                    
                    tanque->renderer.desenhaDetalhado(); 
                    tanque->renderer.desenhaTorre(currentMouseX, currentMouseY); 
                    tanque->renderer.drawHealthBar();    
                    tanque->renderer.drawProjectiles();  
                    tanque->renderer.drawExplosions();   
                }
            }
        } else { 
            for (auto& enemy : enemies) {
                enemy.draw();
            }
            if(tanque) tanque->renderer.desenhaDetalhado(); 
        }

        scoreboard.draw(screenWidth, screenHeight);
        drawGameUI(); 
    }

    // Gerencia a lógica de atualização e desenho do tanque.
    void tanqueManager() {
        
        if (!tanque || tanque->isDestroyed() || gameState != GameState::PLAYING) {
            return; 
        }

        
        bool wantsNitro = (this->currentKey == 'w' || this->currentKey == 'W');
        bool wantsRapidFire = (this->currentKey == 'q' || this->currentKey == 'Q');
        bool wantsSuperBurst = (this->currentKey == 'e' || this->currentKey == 'E');
        bool wantsShield = (this->currentKey == 's' || this->currentKey == 'S');

        tanque->updateNitro(currentFps, wantsNitro);
        tanque->updateRapidFire(currentFps, wantsRapidFire);
        tanque->updateSuperBurst(currentFps, wantsSuperBurst);
        tanque->updateShield(currentFps, wantsShield);

        tanque->move(currentFps, track, enemies); 
        tanque->updateProjectiles(currentFps, track, enemies); 
        tanque->updateExplosions(currentFps);

        
        tanque->renderer.desenhaDetalhado();
        tanque->renderer.drawNitroEffects();
        tanque->renderer.desenhaTorre(currentMouseX, currentMouseY);
        tanque->renderer.drawHealthBar();

        if (tanque->pushBackTimer <= 0.0f) {
            if (this->currentKey == 'a' || this->currentKey == 'A') {
                tanque->rotateDirection(-0.1f, currentFps, track, enemies);
            } else if (this->currentKey == 'd' || this->currentKey == 'D') {
                tanque->rotateDirection(0.1f, currentFps, track, enemies);
            }
        }

        if (currentIsPressed == 1) { 
            tanque->shoot(currentFps, currentMouseX, currentMouseY);
        }
        tanque->renderer.drawProjectiles();
        tanque->renderer.drawExplosions();
        
    }

    // Atualiza a lógica dos inimigos, processa destruições e os desenha.
    void updateAndDrawEnemies() {
        
        if (gameState == GameState::LEVEL_TRANSITION) {
            for (auto& enemy : enemies) {
                enemy.updateHealthBarDisplay(currentFps); 
                enemy.draw(); 
            }
            return;
        }

        
        bool allEnemiesEffectivelyClearedForLevel = (gameState == GameState::PLAYING); 

        for (auto it = enemies.begin(); it != enemies.end(); ) {
            
            it->updateHealthBarDisplay(currentFps);

            
            
            
            
            if (it->level == 2) { 
                it->updateActivatedStar(currentFps, track, enemies, tanque);
            } else if (it->level == 4) { 
                it->updatePlaneSpecifics(currentFps, track, tanque);
            }
            

            
            
            if (gameState == GameState::PLAYING && it->isDestroyed()) {
                scoreboard.addScore(it->getScoreValue());
                currentTotalEnemyHealthForLevel -= Enemy::getHealthContribution(it->level);
                if (currentTotalEnemyHealthForLevel < 0.0f) currentTotalEnemyHealthForLevel = 0.0f;

                if (it->level == 3) { 
                    float shrapnelSpeed = 180.0f; float shrapnelDamage = 3.0f;
                    float shrapnelW = 5.0f; float shrapnelH = 10.0f;
                    std::vector<Projectile> newShrapnels = it->generateShrapnel(shrapnelSpeed, shrapnelDamage, shrapnelW, shrapnelH);
                    if (tanque) { 
                        tanque->projectiles.insert(tanque->projectiles.end(), newShrapnels.begin(), newShrapnels.end());
                    }
                }
                it = enemies.erase(it); 
            } else {
                
                
                if (gameState == GameState::PLAYING) { 
                     allEnemiesEffectivelyClearedForLevel = false;
                }
                it->draw(); 
                ++it;
            }
        }

        
        
        if (gameState == GameState::PLAYING && allEnemiesEffectivelyClearedForLevel && enemies.empty()) {
            startNextLevelTransition();
        }
    }
    
    // Inicia a transição para o próximo nível.
    void startNextLevelTransition() {
        gameState = GameState::LEVEL_TRANSITION;
        levelTransitionTimer = timeBetweenLevels;
    }

    // Atualiza a lógica de transição de nível e avança para o próximo nível.
    void updateLevelLogic() {
        if (gameState == GameState::LEVEL_TRANSITION) {
            if (levelTransitionTimer > 0.0f) {
                levelTransitionTimer -= (1.0f / currentFps);
                if (levelTransitionTimer < 0.0f) {
                    levelTransitionTimer = 0.0f;
                }
            } else {
                currentLevel++;
                
                gameState = GameState::PLAYING;
                generateEnemies();
            }
        }
    }

    // Gera os inimigos para o nível atual.
    void generateEnemies() {
        
        enemies.clear();
        initialTotalEnemyHealthForLevel = 0.0f; 
        currentTotalEnemyHealthForLevel = 0.0f; 

        LevelConfig currentLevelConfig = getLevelConfiguration(currentLevel);
        int maxAttemptsPerEnemy = 200;

        for (const auto &spawnInfo : currentLevelConfig.enemySpawns) {
            for (int i = 0; i < spawnInfo.count; ++i) {
                int attempts = 0;
                bool placed = false;
                while (!placed && attempts < maxAttemptsPerEnemy) {
                    float x = static_cast<float>(rand() % screenWidth);
                    float y = static_cast<float>(rand() % screenHeight);
                    Vector2 randomPos(x, y);
                    float enemySizeForCheck = (spawnInfo.enemyLevel == 1) ? 10.0f : (spawnInfo.enemyLevel == 2) ? 15.0f : 20.0f;

                    bool centerInTrack = track.isPointInsideTrack(randomPos);
                    bool topInTrack = track.isPointInsideTrack(Vector2(randomPos.x, randomPos.y - enemySizeForCheck));
                    bool bottomInTrack = track.isPointInsideTrack(Vector2(randomPos.x, randomPos.y + enemySizeForCheck));
                    bool leftInTrack = track.isPointInsideTrack(Vector2(randomPos.x - enemySizeForCheck, randomPos.y));
                    bool rightInTrack = track.isPointInsideTrack(Vector2(randomPos.x + enemySizeForCheck, randomPos.y));

                    if (centerInTrack && topInTrack && bottomInTrack && leftInTrack && rightInTrack) {
                        bool tooClose = false;
                        float minDistanceToTank = 100.0f;
                        if (tanque && (randomPos - tanque->pivot).length() < (minDistanceToTank + enemySizeForCheck)) {
                            tooClose = true;
                        }
                        if (!tooClose) {
                            for (const auto &existingEnemy : enemies) {
                                if ((randomPos - existingEnemy.position).length() < (existingEnemy.size + enemySizeForCheck + 10.0f)) {
                                    tooClose = true;
                                    break;
                                }
                            }
                        }
                        if (!tooClose) {
                            enemies.push_back(Enemy(randomPos, spawnInfo.enemyLevel));
                            
                            initialTotalEnemyHealthForLevel += Enemy::getHealthContribution(spawnInfo.enemyLevel);
                            placed = true;
                        }
                    }
                    attempts++;
                }
            }
        }
        currentTotalEnemyHealthForLevel = initialTotalEnemyHealthForLevel; 
    }

    // Desenha a interface do usuário do jogo (habilidades, nível, barra de progresso, mensagens).
    void drawGameUI() {
        
        if (tanque) {
            float uiX = 20; float uiY = 20; float uiW = 50; float uiH = 50; float uiSpacing = 10;
            tanque->renderer.drawNitroUIElements(uiX, uiY, uiW, uiH); uiX += uiW + uiSpacing;
            tanque->renderer.drawRapidFireUIElements(uiX, uiY, uiW, uiH); uiX += uiW + uiSpacing;
            tanque->renderer.drawSuperBurstUIElements(uiX, uiY, uiW, uiH); uiX += uiW + uiSpacing;
            tanque->renderer.drawShieldUIElements(uiX, uiY, uiW, uiH);
        }

        
        float uiMargin = 20.0f;
        float levelTextY = screenHeight - uiMargin - 15; 
        float progressBarHeight = 20.0f;
        float progressBarY = levelTextY - progressBarHeight - 5; 
        float progressBarWidth = 200.0f;

        std::ostringstream levelTextStr;
        levelTextStr << "Level " << currentLevel;
        CV::color(1.0f, 1.0f, 1.0f);
        CV::textStroke(uiMargin, levelTextY, levelTextStr.str().c_str(), 0.15f, 1.8f);

        
        if (initialTotalEnemyHealthForLevel > 0) {
            float progress = currentTotalEnemyHealthForLevel / initialTotalEnemyHealthForLevel;
            progress = std::max(0.0f, std::min(progress, 1.0f)); 

            
            CV::color(0.3f, 0.3f, 0.3f); 
            CV::rectFill(uiMargin, progressBarY, uiMargin + progressBarWidth, progressBarY + progressBarHeight);

            
            CV::color(0.1f, 0.8f, 0.1f); 
            CV::rectFill(uiMargin, progressBarY, uiMargin + (progressBarWidth * progress), progressBarY + progressBarHeight);
            
            
            CV::color(0.5f, 0.5f, 0.5f);
            CV::rect(uiMargin, progressBarY, uiMargin + progressBarWidth, progressBarY + progressBarHeight);
        } else {
            
            
            CV::color(0.3f, 0.3f, 0.3f);
            CV::rectFill(uiMargin, progressBarY, uiMargin + progressBarWidth, progressBarY + progressBarHeight);
            CV::color(0.5f, 0.5f, 0.5f);
            CV::rect(uiMargin, progressBarY, uiMargin + progressBarWidth, progressBarY + progressBarHeight);
        }

        
        if (gameState == GameState::LEVEL_TRANSITION) {
            std::ostringstream countdownText;
            countdownText << "Level " << (currentLevel + 1) << " em " << static_cast<int>(ceil(levelTransitionTimer)) << "...";
            CV::color(1.0f, 1.0f, 0.0f);
            float countdownX = screenWidth / 2.0f - 150;
            float countdownY = screenHeight / 2.0f;
            CV::textStroke(countdownX, countdownY, countdownText.str().c_str(), 0.25f, 2.5f);
        } else if (gameState == GameState::GAME_OVER) {
            CV::color(1.0f, 0.0f, 0.0f); 
            std::string gameOverMsg = "GAME OVER";
            
            
            float gameOverCharWidthFactor = 18.0f; 
            float msgX = screenWidth / 2.0f - (gameOverMsg.length() * gameOverCharWidthFactor);
            float msgY = screenHeight / 2.0f - 50;
            CV::textStroke(msgX, msgY, gameOverMsg.c_str(), 0.4f, 3.0f);

            std::string scoreMsg = "Final Score: " + std::to_string(scoreboard.getScore());
            
            
            float scoreCharWidthFactor = 7.5f; 
            float scoreMsgX = screenWidth / 2.0f - (scoreMsg.length() * scoreCharWidthFactor);
            float scoreMsgY = screenHeight / 2.0f + 10;
            CV::textStroke(scoreMsgX, scoreMsgY, scoreMsg.c_str(), 0.2f, 2.0f);
            
            std::string returnMsg = "Pressione ESC para voltar ao Menu";
            
            
            float returnCharWidthFactor = 5.6f; 
            float returnMsgX = screenWidth / 2.0f - (returnMsg.length() * returnCharWidthFactor);
            float returnMsgY = screenHeight / 2.0f + 50;
            CV::textStroke(returnMsgX, returnMsgY, returnMsg.c_str(), 0.15f, 2.0f);
        }
    }

    

    

    
    // Lida com o clique do mouse no editor de pista (se os pontos estiverem visíveis).
    void handleTrackEditorMousePress(int mouseX, int mouseY) {
        if (track.arePointsVisible()) track.handleMousePress(mouseX, mouseY);
    }
    // Lida com o arrastar do mouse no editor de pista (se os pontos estiverem visíveis).
    void handleTrackEditorMouseDrag(int mouseX, int mouseY) {
        if (track.arePointsVisible()) track.handleMouseDrag(mouseX, mouseY);
    }
    // Lida com o soltar do mouse no editor de pista (se os pontos estiverem visíveis).
    void handleTrackEditorMouseRelease() {
        if (track.arePointsVisible()) track.handleMouseRelease();
    }
    
    // Retorna o estado atual do jogo.
    GameState getCurrentGameState() const { return gameState; }
};

#endif // GAME_H_INCLUDED
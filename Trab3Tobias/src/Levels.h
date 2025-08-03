// Este arquivo define as configurações para os diferentes níveis do jogo.
// Ele utiliza estruturas para organizar a quantidade e os tipos de inimigos
// que aparecem em cada nível.
// Estruturas principais:
// - EnemySpawnConfig: Define o tipo (nível) e a quantidade de um inimigo específico a ser gerado.
// - LevelConfig: Agrupa várias EnemySpawnConfig para definir a composição completa de um nível.
// Função principal:
// - getLevelConfiguration: Retorna a LevelConfig para um determinado número de nível do jogo.
//   Para níveis além dos predefinidos, a dificuldade aumenta progressivamente.
#ifndef LEVELS_H_INCLUDED
#define LEVELS_H_INCLUDED

#include <vector>
#include <algorithm> 

// Estrutura para definir a quantidade de cada tipo de inimigo.
struct EnemySpawnConfig {
    int enemyLevel; 
    int count;      
};

// Estrutura para definir a configuração completa de um nível do jogo.
struct LevelConfig {
    std::vector<EnemySpawnConfig> enemySpawns;
};

// Função para obter a configuração de inimigos para um determinado nível do jogo.
inline LevelConfig getLevelConfiguration(int gameLevel) {
    LevelConfig config;

    if (gameLevel == 1) {
        config.enemySpawns = {
            {1, 1}, 
            {2, 2}, 
            {3, 1},  
            {4, 0}  
        };
    } else if (gameLevel == 2) {
        config.enemySpawns = {
            {1, 3},
            {2, 3},
            {3, 2},
            {4, 1}  
        };
    } else if (gameLevel == 3) {
        config.enemySpawns = {
            {1, 1},
            {2, 5},
            {3, 4},
            {4, 2}  
        };
    } else if (gameLevel == 4) { 
        config.enemySpawns = {
            {1, 3}, 
            {2, 3},
            {4, 8},
        };
    } else if (gameLevel == 5) { 
         config.enemySpawns = {
            {1, 3}, 
            {2, 4},
            {3, 7},
            {4, 5},
        };
    }
    else { 
        int progressionFactor = gameLevel - 5; 

        int baseL1Enemies = 1;
        int baseL2Enemies = 3 + progressionFactor;
        int baseL3Enemies = 2 + progressionFactor * 2;
        int baseL4Enemies = 1 + progressionFactor / 2; 

        config.enemySpawns = {
            {1, std::max(0, baseL1Enemies)},
            {2, baseL2Enemies},
            {3, baseL3Enemies},
            {4, std::max(1, baseL4Enemies)} 
        };
    }
    return config;
}

#endif // LEVELS_H_INCLUDED

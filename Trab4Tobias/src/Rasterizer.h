/*
=============================================================================
                              RASTERIZER.H
=============================================================================

DESCRIÇÃO GERAL:
Este arquivo implementa um sistema completo de rasterização de triângulos para
renderização 3D. Inclui rasterização por coordenadas baricêntricas, Z-buffer
para teste de profundidade, e interpolação de atributos por fragmento (cor,
normal, posição mundial).

FUNCIONALIDADES:
• Rasterização de triângulos usando coordenadas baricêntricas
• Z-buffer para renderização correta de profundidade
• Interpolação suave de cores, normais e posições
• Detecção de pixels dentro de triângulos
• Suporte a fragmentos com múltiplos atributos

ESTRUTURAS:
• Fragment: Representa um pixel com cor, profundidade, normal e posição
• Triangle2D: Triângulo projetado com atributos interpoláveis
• ZBuffer: Buffer de profundidade para teste de visibilidade

ALGORITMO DE RASTERIZAÇÃO:
1. Define bounding box do triângulo
2. Para cada pixel no bounding box:
   - Calcula coordenadas baricêntricas
   - Testa se pixel está dentro do triângulo
   - Interpola atributos usando coordenadas baricêntricas
   - Cria fragmento com dados interpolados

COORDENADAS BARICÊNTRICAS:
Sistema que permite interpolação suave de atributos em triângulos.
Cada ponto interno é expresso como combinação linear dos vértices.

Z-BUFFER:
• Armazena profundidade de cada pixel
• Permite renderização correta de objetos sobrepostos
• Testa se novo fragmento está mais próximo que o atual

APLICAÇÕES:
• Renderização sólida de modelos 3D
• Suporte a iluminação por fragmento
• Base para shading avançado (Phong, etc.)

=============================================================================
*/

#ifndef ___RASTERIZER__H___
#define ___RASTERIZER__H___

#include "Vector2.h"
#include "Vector3.h"
#include "gl_canvas2d.h"
#include <vector>
#include <algorithm>
#include <cmath>

/**
 * Estrutura que representa um fragmento (pixel) renderizado.
 * Contém todos os atributos interpolados necessários para shading.
 */
struct Fragment {
    int x, y;           // Coordenadas do pixel na tela
    float depth;        // Profundidade Z para teste de visibilidade
    Vector3 color;      // Cor interpolada do fragmento
    Vector3 normal;     // Normal interpolada para cálculos de iluminação
    Vector3 worldPos;   // Posição no espaço mundial para iluminação
};

/**
 * Estrutura que representa um triângulo 2D projetado com atributos.
 * Contém vértices em coordenadas de tela e atributos para interpolação.
 */
struct Triangle2D {
    Vector2 v0, v1, v2;                         // Vértices 2D (coordenadas de tela)
    float z0, z1, z2;                           // Profundidades Z dos vértices
    Vector3 color0, color1, color2;             // Cores dos vértices
    Vector3 normal0, normal1, normal2;          // Normais dos vértices
    Vector3 worldPos0, worldPos1, worldPos2;    // Posições mundiais dos vértices
};

/**
 * Classe responsável pela rasterização de triângulos.
 * Converte triângulos 2D em fragmentos com atributos interpolados.
 */
class Rasterizer {
public:
    /**
     * Rasteriza um triângulo gerando todos os fragmentos internos
     * @param tri Triângulo 2D com vértices e atributos
     * @return Lista de fragmentos gerados
     */
    static std::vector<Fragment> rasterizeTriangle(const Triangle2D& tri) {
        std::vector<Fragment> fragments;
        
        // Calcula bounding box do triângulo
        int minX = (int)std::floor(std::min({tri.v0.x, tri.v1.x, tri.v2.x}));
        int maxX = (int)std::ceil(std::max({tri.v0.x, tri.v1.x, tri.v2.x}));
        int minY = (int)std::floor(std::min({tri.v0.y, tri.v1.y, tri.v2.y}));
        int maxY = (int)std::ceil(std::max({tri.v0.y, tri.v1.y, tri.v2.y}));
        
        // Testa cada pixel no bounding box
        for (int y = minY; y <= maxY; y++) {
            for (int x = minX; x <= maxX; x++) {
                Vector2 p(x + 0.5f, y + 0.5f);  // Centro do pixel
                
                // Calcula coordenadas baricêntricas
                Vector3 baryCoords = calculateBarycentricCoords(p, tri.v0, tri.v1, tri.v2);
                
                // Verifica se pixel está dentro do triângulo
                if (baryCoords.x >= 0 && baryCoords.y >= 0 && baryCoords.z >= 0) {
                    Fragment frag;
                    frag.x = x;
                    frag.y = y;
                    
                    // Interpola atributos usando coordenadas baricêntricas
                    frag.depth = baryCoords.x * tri.z0 + baryCoords.y * tri.z1 + baryCoords.z * tri.z2;
                    frag.color = tri.color0 * baryCoords.x + tri.color1 * baryCoords.y + tri.color2 * baryCoords.z;
                    frag.normal = tri.normal0 * baryCoords.x + tri.normal1 * baryCoords.y + tri.normal2 * baryCoords.z;
                    frag.normal.normalize();
                    frag.worldPos = tri.worldPos0 * baryCoords.x + tri.worldPos1 * baryCoords.y + tri.worldPos2 * baryCoords.z;
                    
                    fragments.push_back(frag);
                }
            }
        }
        
        return fragments;
    }
    
    /**
     * Calcula coordenadas baricêntricas de um ponto em relação a um triângulo
     * @param p Ponto a ser testado
     * @param a, b, c Vértices do triângulo
     * @return Coordenadas baricêntricas (w, u, v) onde w+u+v=1
     */
    static Vector3 calculateBarycentricCoords(const Vector2& p, const Vector2& a, const Vector2& b, const Vector2& c) {
        Vector2 v0 = c - a;
        Vector2 v1 = b - a;
        Vector2 v2 = p - a;
        
        float dot00 = Vector2::dot(v0, v0);
        float dot01 = Vector2::dot(v0, v1);
        float dot02 = Vector2::dot(v0, v2);
        float dot11 = Vector2::dot(v1, v1);
        float dot12 = Vector2::dot(v1, v2);
        
        float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
        float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
        float v = (dot00 * dot12 - dot01 * dot02) * invDenom;
        
        return Vector3(1.0f - u - v, v, u);  // Mapeamento: w=v0, u=v1, v=v2
    }
};

/**
 * Classe que implementa Z-buffer para teste de profundidade.
 * Garante renderização correta de objetos sobrepostos.
 */
class ZBuffer {
private:
    std::vector<float> buffer;  // Buffer de profundidade
    int width, height;          // Dimensões do buffer
    
public:
    /**
     * Construtor - inicializa buffer com dimensões especificadas
     * @param w Largura do buffer
     * @param h Altura do buffer
     */
    ZBuffer(int w, int h) : width(w), height(h) {
        buffer.resize(width * height, -1000000.0f);  // Valor "infinitamente" distante
    }
    
    /**
     * Limpa o buffer resetando todas as profundidades
     */
    void clear() {
        std::fill(buffer.begin(), buffer.end(), -1000000.0f);
    }
    
    /**
     * Testa profundidade e atualiza buffer se fragmento estiver mais próximo
     * @param x Coordenada X do pixel
     * @param y Coordenada Y do pixel  
     * @param depth Profundidade do novo fragmento
     * @return true se fragmento passou no teste e foi aceito
     */
    bool testAndSet(int x, int y, float depth) {
        if (x < 0 || x >= width || y < 0 || y >= height)
            return false;
        
        int index = y * width + x;
        if (depth > buffer[index]) {  // Valores maiores = mais próximos
            buffer[index] = depth;
            return true;
        }
        return false;
    }
    
    /**
     * Consulta profundidade em uma posição sem modificar o buffer
     * @param x Coordenada X
     * @param y Coordenada Y
     * @return Profundidade armazenada na posição
     */
    float getDepth(int x, int y) const {
        if (x < 0 || x >= width || y < 0 || y >= height)
            return -1000000.0f;
        
        return buffer[y * width + x];
    }
    
    /**
     * Redimensiona o buffer para novas dimensões
     * @param w Nova largura
     * @param h Nova altura
     */
    void resize(int w, int h) {
        if (w != width || h != height) {
            width = w;
            height = h;
            buffer.resize(width * height);
            clear();
        }
    }
    
    // Permite acesso direto pela classe Tela
    friend class Tela;
};

#endif // ___RASTERIZER__H___
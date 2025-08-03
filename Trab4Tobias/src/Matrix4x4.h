/*
=============================================================================
                              MATRIX4X4.H
=============================================================================

DESCRIÇÃO GERAL:
Este arquivo define a estrutura Matrix4x4, que implementa uma matriz 4x4 para
transformações geométricas em computação gráfica 3D. A matriz usa coordenadas
homogêneas e suporta todas as transformações fundamentais: translação, rotação,
escala e suas combinações através de multiplicação matricial.

FUNCIONALIDADES:
• Inicialização como matriz identidade
• Criação de matrizes de transformação específicas
• Multiplicação de matrizes para combinar transformações
• Transformação de pontos 3D usando coordenadas homogêneas
• Suporte completo ao pipeline de transformação 3D

MÉTODOS DE CRIAÇÃO:
• createTranslation(): Matriz de translação (mudança de posição)
• createScale(): Matriz de escala (mudança de tamanho)
• createRotationX/Y/Z(): Matrizes de rotação em torno dos eixos principais

OPERAÇÕES:
• multiply(): Multiplicação matricial para combinar transformações
• transformPoint(): Aplica transformação a um ponto 3D

APLICAÇÕES EM CG:
• Model matrix: Posicionamento de objetos no mundo 3D
• View matrix: Transformação da câmera
• Projection matrix: Projeção perspectiva ou ortográfica
• Combinação de transformações complexas

COORDENADAS HOMOGÊNEAS:
A matriz 4x4 permite trabalhar com coordenadas homogêneas (x,y,z,w),
essenciais para translações e projeções perspectivas.

=============================================================================
*/

#ifndef ___MATRIX4X4__H___
#define ___MATRIX4X4__H___

#include "Vector3.h"
#include <cmath>

/**
 * Estrutura que representa uma matriz 4x4 para transformações 3D.
 * Suporta todas as transformações fundamentais da computação gráfica.
 */
struct Matrix4x4 {
    float m[4][4];  // Elementos da matriz organizados em linhas e colunas
    
    /**
     * Construtor padrão - inicializa como matriz identidade
     * A matriz identidade não modifica pontos transformados
     */
    Matrix4x4() {
        for(int i=0; i<4; ++i) 
            for(int j=0; j<4; ++j) 
                m[i][j] = (i==j) ? 1.0f : 0.0f;
    }
    
    /**
     * Cria matriz de translação - move pontos no espaço 3D
     * @param t Vetor de translação (deslocamento)
     * @return Nova matriz de translação
     */
    static Matrix4x4 createTranslation(const Vector3& t) {
        Matrix4x4 mat;
        mat.m[0][3] = t.x; mat.m[1][3] = t.y; mat.m[2][3] = t.z;
        return mat;
    }
    
    /**
     * Cria matriz de escala - redimensiona objetos
     * @param s Fatores de escala para cada eixo (x, y, z)
     * @return Nova matriz de escala
     */
    static Matrix4x4 createScale(const Vector3& s) {
        Matrix4x4 mat;
        mat.m[0][0] = s.x; mat.m[1][1] = s.y; mat.m[2][2] = s.z;
        return mat;
    }
    
    /**
     * Cria matriz de rotação em torno do eixo X
     * @param angleRad Ângulo de rotação em radianos
     * @return Nova matriz de rotação X
     */
    static Matrix4x4 createRotationX(float angleRad) {
        Matrix4x4 mat;
        float c = cos(angleRad), s = sin(angleRad);
        mat.m[1][1] = c; mat.m[1][2] = -s;
        mat.m[2][1] = s; mat.m[2][2] = c;
        return mat;
    }
    
    /**
     * Cria matriz de rotação em torno do eixo Y
     * @param angleRad Ângulo de rotação em radianos
     * @return Nova matriz de rotação Y
     */
    static Matrix4x4 createRotationY(float angleRad) {
        Matrix4x4 mat;
        float c = cos(angleRad), s = sin(angleRad);
        mat.m[0][0] = c; mat.m[0][2] = s;
        mat.m[2][0] = -s; mat.m[2][2] = c;
        return mat;
    }
    
    /**
     * Cria matriz de rotação em torno do eixo Z
     * @param angleRad Ângulo de rotação em radianos
     * @return Nova matriz de rotação Z
     */
    static Matrix4x4 createRotationZ(float angleRad) {
        Matrix4x4 mat;
        float c = cos(angleRad), s = sin(angleRad);
        mat.m[0][0] = c; mat.m[0][1] = -s;
        mat.m[1][0] = s; mat.m[1][1] = c;
        return mat;
    }
    
    /**
     * Multiplica esta matriz por outra - combina transformações
     * @param other Matriz a ser multiplicada
     * @return Nova matriz resultado da multiplicação
     */
    Matrix4x4 multiply(const Matrix4x4& other) const {
        Matrix4x4 result;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result.m[i][j] = 0;
                for (int k = 0; k < 4; ++k) {
                    result.m[i][j] += m[i][k] * other.m[k][j];
                }
            }
        }
        return result;
    }
    
    /**
     * Transforma um ponto 3D usando coordenadas homogêneas
     * @param p Ponto 3D a ser transformado
     * @return Novo ponto transformado
     */
    Vector3 transformPoint(const Vector3& p) const {
        float x = m[0][0]*p.x + m[0][1]*p.y + m[0][2]*p.z + m[0][3];
        float y = m[1][0]*p.x + m[1][1]*p.y + m[1][2]*p.z + m[1][3];
        float z = m[2][0]*p.x + m[2][1]*p.y + m[2][2]*p.z + m[2][3];
        float w = m[3][0]*p.x + m[3][1]*p.y + m[3][2]*p.z + m[3][3];
        if (w != 0 && w != 1) { x/=w; y/=w; z/=w;}
        return Vector3(x,y,z);
    }
};

#endif // ___MATRIX4X4__H___
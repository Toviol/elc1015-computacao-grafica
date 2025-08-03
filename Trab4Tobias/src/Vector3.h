/*
=============================================================================
                              VECTOR3.H
=============================================================================

DESCRIÇÃO GERAL:
Este arquivo define a estrutura Vector3, que implementa um vetor tridimensional
com operações matemáticas fundamentais para computação gráfica 3D. A classe
fornece operações vetoriais essenciais como soma, subtração, produto escalar,
produto vetorial, normalização e cálculo de magnitude.

FUNCIONALIDADES:
• Operações aritméticas básicas (+, -, *, /)
• Produto escalar (dot product) para calcular ângulos e projeções
• Produto vetorial (cross product) para calcular normais e orientações
• Normalização de vetores para direções unitárias
• Cálculo de magnitude/comprimento do vetor
• Proteção contra divisão por zero

OPERADORES SOBRECARREGADOS:
• operator+(): Soma de vetores (v1 + v2)
• operator-(): Subtração de vetores (v1 - v2)  
• operator*(): Multiplicação por escalar (v * scalar)
• operator/(): Divisão por escalar (v / scalar)

MÉTODOS ESTÁTICOS:
• dot(): Produto escalar entre dois vetores
• cross(): Produto vetorial entre dois vetores

MÉTODOS DE INSTÂNCIA:
• normalize(): Normaliza o vetor (torna unitário)
• length(): Retorna o comprimento/magnitude do vetor

APLICAÇÕES EM CG:
• Representação de posições 3D (x, y, z)
• Vetores de direção e normais de superfície
• Cálculos de iluminação (direção da luz, normal da face)
• Transformações geométricas e projeções

=============================================================================
*/

#ifndef ___VECTOR3__H___
#define ___VECTOR3__H___

#include <cmath>

/**
 * Estrutura que representa um vetor tridimensional com operações matemáticas.
 * Usado para posições 3D, direções, normais e cálculos de iluminação.
 */
struct Vector3 {
    float x, y, z;  // Componentes do vetor

    /**
     * Construtor padrão - inicializa vetor com coordenadas especificadas
     * @param _x Componente X (padrão: 0)
     * @param _y Componente Y (padrão: 0) 
     * @param _z Componente Z (padrão: 0)
     */
    Vector3(float _x = 0, float _y = 0, float _z = 0) : x(_x), y(_y), z(_z) {}

    /**
     * Soma de vetores - retorna novo vetor resultante
     * @param other Vetor a ser somado
     * @return Novo vetor com a soma dos componentes
     */
    Vector3 operator+(const Vector3& other) const { return Vector3(x + other.x, y + other.y, z + other.z); }
    
    /**
     * Subtração de vetores - retorna novo vetor resultante
     * @param other Vetor a ser subtraído
     * @return Novo vetor com a diferença dos componentes
     */
    Vector3 operator-(const Vector3& other) const { return Vector3(x - other.x, y - other.y, z - other.z); }
    
    /**
     * Multiplicação por escalar - escala o vetor
     * @param scalar Valor escalar multiplicador
     * @return Novo vetor escalado
     */
    Vector3 operator*(float scalar) const { return Vector3(x * scalar, y * scalar, z * scalar); }
    
    /**
     * Divisão por escalar - reduz escala do vetor (com proteção contra divisão por zero)
     * @param scalar Valor escalar divisor
     * @return Novo vetor dividido ou vetor zero se divisor for zero
     */
    Vector3 operator/(float scalar) const {
        if (scalar == 0) return Vector3(0,0,0);
        return Vector3(x / scalar, y / scalar, z / scalar); 
    }
    
    /**
     * Produto escalar entre dois vetores - usado para calcular ângulos
     * @param a Primeiro vetor
     * @param b Segundo vetor
     * @return Valor escalar do produto interno
     */
    static float dot(const Vector3& a, const Vector3& b) { 
        return a.x * b.x + a.y * b.y + a.z * b.z; 
    }
    
    /**
     * Produto vetorial entre dois vetores - gera vetor perpendicular
     * @param a Primeiro vetor
     * @param b Segundo vetor
     * @return Novo vetor perpendicular aos dois vetores de entrada
     */
    static Vector3 cross(const Vector3& a, const Vector3& b) {
        return Vector3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
    }
    
    /**
     * Normaliza o vetor (torna unitário) - modifica o vetor atual
     * Usado para obter direções unitárias para cálculos de iluminação
     */
    void normalize() {
        float mag = std::sqrt(x*x + y*y + z*z);
        if (mag > 0.0001f) { x /= mag; y /= mag; z /= mag; }
    }
    
    /**
     * Calcula o comprimento/magnitude do vetor
     * @return Magnitude do vetor (sempre positiva)
     */
    float length() const { 
        return std::sqrt(x*x + y*y + z*z); 
    }
};

#endif // ___VECTOR3__H___
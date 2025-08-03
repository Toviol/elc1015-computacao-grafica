#ifndef __VECTOR_2_H__
#define __VECTOR_2_H__

#include <cmath>   // Para sqrt
#include <cstdio>  // Para printf


class Vector2
{
public:
   float x, y;

   Vector2()
   {
      x = y = 0;
   }

   Vector2(float _x, float _y)
   {
       x = _x;
       y = _y;
   }

   void set(float _x, float _y)
   {
       x = _x;
       y = _y;
   }

   void normalize()
   {
       float norm = (float)sqrt(x*x + y*y);

       if(norm==0.0)
       {
          printf("\n\nNormalize::Divisao por zero");
          x = 1;
          y = 1;
          return;
       }
       x /= norm;
       y /= norm;
   }

   Vector2 operator + (const Vector2& v) const // Adicionado const
   {
       return Vector2(x + v.x, y + v.y);
   }

   Vector2 operator - (const Vector2& v) const // Adicionado const
   {
       return Vector2(x - v.x, y - v.y);
   }

   Vector2 operator * (float scalar) const // Adicionado const
   {
       return Vector2(x * scalar, y * scalar);
   }

   // Operador unário de negação
   Vector2 operator - () const
   {
       return Vector2(-x, -y);
   }

   // Produto escalar
   float dot(const Vector2& other) const {
        return x * other.x + y * other.y;
    }

    Vector2 reflect(const Vector2& normal) const {
        // Assumes 'normal' is a unit vector
        return *this - normal * (2.0f * this->dot(normal));
    }
    
    Vector2 normalizedSafe() const {
        float l = length();
        if (l > 0.0001f) { // Avoid division by zero or very small numbers
            return Vector2(x / l, y / l);
        }
        return Vector2(0, 0); // Or some default like (1,0)
    }

   // Retorna um vetor perpendicular (girado 90 graus)
   Vector2 perpendicular() const
   {
       return Vector2(-y, x); // Ou (y, -x)
   }

   // Normaliza o vetor (torna seu comprimento 1)
   // A função normalize() existente modifica o vetor, vamos criar uma que retorna um novo vetor normalizado
   Vector2 normalized() const
   {
       float l = length();
       if (l > 0) // Evita divisão por zero
       {
           return Vector2(x / l, y / l);
       }
       return Vector2(0, 0); // Retorna vetor zero se o comprimento for zero
   }

   // Método para calcular o comprimento (magnitude) do vetor
   float length() const // Adicionado const pois não modifica o objeto
   {
       return (float)sqrt(x*x + y*y);
   }

   // Método para calcular o comprimento ao quadrado (mais eficiente se só precisar comparar distâncias)
   float lengthSquared() const // Adicionado const
   {
       return x*x + y*y;
   }

   // Método para interpolar entre este vetor e outro
   // t = 0 retorna este vetor, t = 1 retorna 'other'
   Vector2 interpolate(const Vector2& other, float t) const
   {
       return Vector2(x * (1.0f - t) + other.x * t,
                      y * (1.0f - t) + other.y * t);
   }
};

#endif

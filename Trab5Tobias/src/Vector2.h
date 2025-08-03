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

   Vector2 operator + (const Vector2& v) const
   {
       return Vector2(x + v.x, y + v.y);
   }

   Vector2 operator - (const Vector2& v) const
   {
       return Vector2(x - v.x, y - v.y);
   }

   Vector2 operator * (float scalar) const
   {
       return Vector2(x * scalar, y * scalar);
   }

   Vector2 operator - () const
   {
       return Vector2(-x, -y);
   }

   // Produto escalar (método de instância)
   float dot(const Vector2& other) const {
        return x * other.x + y * other.y;
   }

   // Produto escalar (método estático) - ADICIONADO
   static float dot(const Vector2& a, const Vector2& b) {
        return a.x * b.x + a.y * b.y;
   }

   Vector2 reflect(const Vector2& normal) const {
        return *this - normal * (2.0f * this->dot(normal));
   }
    
   Vector2 normalizedSafe() const {
        float l = length();
        if (l > 0.0001f) {
            return Vector2(x / l, y / l);
        }
        return Vector2(0, 0);
   }

   Vector2 perpendicular() const
   {
       return Vector2(-y, x);
   }

   Vector2 normalized() const
   {
       float l = length();
       if (l > 0)
       {
           return Vector2(x / l, y / l);
       }
       return Vector2(0, 0);
   }

   float length() const
   {
       return (float)sqrt(x*x + y*y);
   }

   float lengthSquared() const
   {
       return x*x + y*y;
   }

   Vector2 interpolate(const Vector2& other, float t) const
   {
       return Vector2(x * (1.0f - t) + other.x * t,
                      y * (1.0f - t) + other.y * t);
   }

   float distanceTo(const Vector2& other) const
   {
       float dx = other.x - x;
       float dy = other.y - y;
       return (float)sqrt(dx*dx + dy*dy);
   }
};

#endif

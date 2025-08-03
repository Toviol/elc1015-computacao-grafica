// Este arquivo define a classe Projectile.
// A classe Projectile representa um projétil no jogo, gerenciando sua posição,
// direção, velocidade, dimensões, tipo de proprietário (jogador ou estilhaço de inimigo) e dano.
// Inclui funcionalidades para:
// - Inicialização de projéteis com atributos específicos.
// - Atualização dos vértices do projétil para detecção de colisão e renderização.
// - Movimentação do projétil com base na velocidade e direção.
// - Renderização visual do projétil, com aparências diferentes para projéteis do jogador e estilhaços.
#ifndef PROJECTILE_H_INCLUDED
#define PROJECTILE_H_INCLUDED

#include "Vector2.h"
#include "gl_canvas2d.h"
#include <vector> 
#include <cmath>  

class Projectile
{
public:
    enum class Owner { PLAYER, ENEMY_SHRAPNEL };

    Vector2 position;   
    Vector2 direction;  
    float speed;        
    float width, height; 
    Owner ownerType;
    float damage;
    std::vector<Vector2> vertices; 

    // Construtor do projétil.
    Projectile(Vector2 startPos, Vector2 dir, float spd, float w, float h, Owner owner, float dmg)
    {
        position = startPos;
        direction = dir.normalized(); 
        speed = spd;
        width = w;
        height = h;
        ownerType = owner;
        damage = dmg;
        updateVertices(); 
    }

    // Atualiza os vértices do projétil com base na posição e direção atuais.
    void updateVertices() {
        vertices.clear();
        Vector2 dir_norm = direction; 
        Vector2 perp_norm = dir_norm.perpendicular();

        float half_h = height / 2.0f; 
        float half_w = width / 2.0f;  

        Vector2 front_center = position + dir_norm * half_h;
        Vector2 rear_center  = position - dir_norm * half_h;

        vertices.push_back(front_center + perp_norm * half_w); 
        vertices.push_back(front_center - perp_norm * half_w); 
        vertices.push_back(rear_center  - perp_norm * half_w); 
        vertices.push_back(rear_center  + perp_norm * half_w); 
    }

    // Move o projétil com base na sua velocidade e direção.
    void move(float fps)
    {
        float actualSpeed = speed * (1.0f / fps); 
        position.x += direction.x * actualSpeed;
        position.y += direction.y * actualSpeed;
        updateVertices(); 
    }

    // Desenha o projétil na tela.
    void draw() const
    {
        float visualWidth = width;       
        float visualHeight = height;     
        float halfVisualWidth = visualWidth / 2.0f;
        float tipHeightRatio = 0.3f;  

        float tipVisualH = visualHeight * tipHeightRatio;

        Vector2 body_rear_left(-visualHeight / 2.0f, -halfVisualWidth);
        Vector2 body_rear_right(-visualHeight / 2.0f, halfVisualWidth);
        Vector2 body_front_left(visualHeight / 2.0f - tipVisualH, -halfVisualWidth);
        Vector2 body_front_right(visualHeight / 2.0f - tipVisualH, halfVisualWidth);
        Vector2 tip_point(visualHeight / 2.0f, 0);

        float cosA = direction.x;
        float sinA = direction.y;

        auto transform = [&](Vector2 v_local) {
            return Vector2(
                position.x + v_local.x * cosA - v_local.y * sinA,
                position.y + v_local.x * sinA + v_local.y * cosA
            );
        };

        Vector2 p_brl = transform(body_rear_left);
        Vector2 p_brr = transform(body_rear_right);
        Vector2 p_bfl = transform(body_front_left);
        Vector2 p_bfr = transform(body_front_right);
        Vector2 p_tip = transform(tip_point);

        float body_r, body_g, body_b;
        float tip_r, tip_g, tip_b;

        if (ownerType == Owner::PLAYER) {
            body_r = 0.6f; body_g = 0.6f; body_b = 0.6f; 
            tip_r = 0.8f; tip_g = 0.1f; tip_b = 0.1f;   
        } else { 
            body_r = 0.7f; body_g = 0.3f; body_b = 0.0f; 
            tip_r = 0.9f; tip_g = 0.6f; tip_b = 0.0f;   
        }

        CV::color(body_r, body_g, body_b);
        int numStepsBody = 10; 
        for (int i = 0; i <= numStepsBody; i++)
        {
            float t = (float)i / numStepsBody;
            Vector2 left = p_brl.interpolate(p_bfl, t);    
            Vector2 right = p_brr.interpolate(p_bfr, t);   
            CV::line(left, right);
        }

        CV::color(tip_r, tip_g, tip_b);
        int numStepsTip = 10; 
        for (int i = 0; i <= numStepsTip; i++)
        {
            float t = (float)i / numStepsTip;
            Vector2 left_tip_segment_pt = p_bfl.interpolate(p_tip, t);  
            Vector2 right_tip_segment_pt = p_bfr.interpolate(p_tip, t); 
            CV::line(left_tip_segment_pt, right_tip_segment_pt); 
        }
        CV::color(body_r*0.5f, body_g*0.5f, body_b*0.5f);
        CV::line(p_brl, p_bfl); CV::line(p_bfl, p_bfr);
        CV::line(p_bfr, p_brr); CV::line(p_brr, p_brl);
        CV::line(p_bfl, p_tip); CV::line(p_bfr, p_tip);
    }
};

#endif // PROJECTILE_H_INCLUDED
// Este arquivo define a classe Explosion e uma função auxiliar drawExplosionSprite.
// A classe Explosion gerencia a animação de uma explosão, controlando sua posição,
// quadro atual, duração de cada quadro e escala.
// A função drawExplosionSprite é responsável por desenhar cada quadro individual
// da animação da explosão, utilizando diferentes formas e cores para simular o efeito.
#ifndef EXPLOSION_H_INCLUDED
#define EXPLOSION_H_INCLUDED

#include "gl_canvas2d.h"
#include "Vector2.h"
#include <cmath> 
#include <cstdlib> 

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Desenha um quadro específico da animação de explosão.
inline void drawExplosionSprite(Vector2 center, int frame, float baseScale)
{
    const float yellow[] = {1.0f, 1.0f, 0.3f};
    const float orange[] = {1.0f, 0.6f, 0.1f};
    const float red[]    = {0.9f, 0.2f, 0.1f};
    const float darkRed[] = {0.5f, 0.1f, 0.05f};
    const float smoke[]  = {0.3f, 0.3f, 0.3f}; 

    float radius;
    int numSpikes;

    switch (frame)
    {
    case 0: 
        CV::color(yellow[0], yellow[1], yellow[2]);
        CV::circleFill(center.x, center.y, 3 * baseScale, 8);
        break;

    case 1: 
        CV::color(orange[0], orange[1], orange[2]);
        CV::circleFill(center.x, center.y, 5 * baseScale, 10);
        CV::color(yellow[0], yellow[1], yellow[2]);
        CV::circleFill(center.x, center.y, 2 * baseScale, 8);
        
        numSpikes = 6;
        for (int i = 0; i < numSpikes; ++i) {
            float angle = (2.0f * (float)M_PI / numSpikes) * i;
            Vector2 p1 = center;
            Vector2 p2 = center + Vector2(cos(angle), sin(angle)) * (7 * baseScale);
            CV::line(p1, p2);
        }
        break;

    case 2: 
        CV::color(red[0], red[1], red[2]);
        CV::circleFill(center.x, center.y, 12 * baseScale, 16);
        CV::color(orange[0], orange[1], orange[2]);
        CV::circleFill(center.x, center.y, 8 * baseScale, 12);
        CV::color(yellow[0], yellow[1], yellow[2]);
        CV::circleFill(center.x, center.y, 4 * baseScale, 10);
        
        numSpikes = 8;
        for (int i = 0; i < numSpikes; ++i) {
            float angle = (2.0f * (float)M_PI / numSpikes) * i + (float)M_PI/8.0f; 
            Vector2 p_base = center + Vector2(cos(angle), sin(angle)) * (6 * baseScale);
            Vector2 p_tip  = center + Vector2(cos(angle), sin(angle)) * (15 * baseScale);
            CV::color(orange[0], orange[1], orange[2]);
            CV::triangleFill(center, p_base + Vector2(cos(angle-0.1f),sin(angle-0.1f))*2*baseScale, p_tip);
            CV::triangleFill(center, p_base + Vector2(cos(angle+0.1f),sin(angle+0.1f))*2*baseScale, p_tip);
        }
        break;

    case 3: 
        CV::color(darkRed[0], darkRed[1], darkRed[2]);
        CV::circleFill(center.x, center.y, 20 * baseScale, 20);
        CV::color(red[0], red[1], red[2]);
        CV::circleFill(center.x, center.y, 16 * baseScale, 18);
        CV::color(orange[0], orange[1], orange[2]);
        CV::circleFill(center.x, center.y, 11 * baseScale, 16);
        CV::color(yellow[0], yellow[1], yellow[2]);
        CV::circleFill(center.x, center.y, 6 * baseScale, 12);
        
        numSpikes = 8;
        CV::color(orange[0], orange[1], orange[2]);
        for (int i = 0; i < numSpikes; ++i) {
            float angle = (2.0f * (float)M_PI / numSpikes) * i;
            Vector2 p_start = center + Vector2(cos(angle), sin(angle)) * (8 * baseScale);
            Vector2 p_mid   = center + Vector2(cos(angle), sin(angle)) * (15 * baseScale);
            Vector2 p_end   = center + Vector2(cos(angle), sin(angle)) * (22 * baseScale);
            CV::triangleFill(p_start, p_mid + Vector2(cos(angle-0.15f),sin(angle-0.15f))*3*baseScale, p_end);
            CV::triangleFill(p_start, p_mid + Vector2(cos(angle+0.15f),sin(angle+0.15f))*3*baseScale, p_end);
        }
        break;

    case 4: 
        radius = 18 * baseScale;
        CV::color(red[0], red[1], red[2]);
        for(int j=0; j<5; ++j) { 
            float r_offset = (rand()%5 - 2)*baseScale; 
            float x_offset = (rand()%5 - 2)*baseScale;
            float y_offset = (rand()%5 - 2)*baseScale;
            CV::circleFill(center.x + x_offset, center.y + y_offset, radius*0.5f + r_offset, 16);
        }
        CV::color(orange[0], orange[1], orange[2]);
        CV::circleFill(center.x, center.y, 10 * baseScale, 12);
        CV::color(yellow[0], yellow[1], yellow[2]);
        CV::circleFill(center.x, center.y, 5 * baseScale, 10);
        
        numSpikes = 10;
        CV::color(darkRed[0], darkRed[1], darkRed[2]);
        for (int i = 0; i < numSpikes; ++i) {
            float angle = (2.0f * (float)M_PI / numSpikes) * i + (float)(rand()%100)/100.0f * ( (float)M_PI/numSpikes);
            float dist = (15 + rand()%8) * baseScale;
            float fragSize = (2 + rand()%3) * baseScale;
            CV::circleFill(center.x + cos(angle)*dist, center.y + sin(angle)*dist, fragSize, 6);
        }
        break;

    case 5: 
        CV::color(darkRed[0], darkRed[1], darkRed[2]);
        for(int j=0; j<8; ++j) {
            float angle_offset = (2.0f * (float)M_PI / 8) * j;
            float dist_offset = 12 * baseScale;
            CV::circleFill(center.x + cos(angle_offset)*dist_offset, center.y + sin(angle_offset)*dist_offset, 8 * baseScale, 12);
        }
        CV::color(smoke[0], smoke[1], smoke[2]);
         for(int j=0; j<6; ++j) {
            float angle_offset = (2.0f * (float)M_PI / 6) * j + (float)M_PI/6.0f;
            float dist_offset = 15 * baseScale;
            CV::circleFill(center.x + cos(angle_offset)*dist_offset, center.y + sin(angle_offset)*dist_offset, 6 * baseScale, 10);
        }
        break;

    case 6: 
        CV::color(smoke[0], smoke[1], smoke[2]);
        for(int j=0; j<5; ++j) {
            float angle_offset = (2.0f * (float)M_PI / 5) * j + (float)(rand()%100)/50.0f;
            float dist_offset = (18 + rand()%5) * baseScale;
            CV::circleFill(center.x + cos(angle_offset)*dist_offset, center.y + sin(angle_offset)*dist_offset, (5 - j*0.5f) * baseScale, 8);
        }
        break;
    default:
        break;
    }
}


class Explosion
{
public:
    Vector2 position;
    int currentFrame;
    float frameDuration;      
    float timeSinceLastFrame; 
    float scale;              
    bool active;
    static const int totalFrames = 7; 

    // Construtor da explosão.
    Explosion(Vector2 pos, float s = 1.0f) : 
        position(pos), currentFrame(0), frameDuration(0.07f), 
        timeSinceLastFrame(0.0f), scale(s), active(true) {}

    // Atualiza o estado da animação da explosão.
    void update(float fps)
    {
        if (!active || fps <= 0) return; 
        timeSinceLastFrame += 1.0f / fps;
        if (timeSinceLastFrame >= frameDuration)
        {
            currentFrame++;
            timeSinceLastFrame = 0.0f; 
            if (currentFrame >= totalFrames)
            {
                active = false; 
            }
        }
    }

    // Desenha o quadro atual da explosão.
    void draw() const 
    {
        if (!active) return;
        drawExplosionSprite(position, currentFrame, scale);
    }
};

#endif // EXPLOSION_H_INCLUDED
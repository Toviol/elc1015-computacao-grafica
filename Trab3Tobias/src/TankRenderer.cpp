// Este arquivo implementa os métodos da classe TankRenderer.
// Contém a lógica específica para desenhar cada parte do tanque e seus efeitos,
// utilizando a biblioteca gl_canvas2d para as operações de renderização.
// As funções aqui transformam os dados do estado do tanque (posição, vida, habilidades ativas)
// em representações visuais na tela.
#include "TankRenderer.h"
#include "Tank.h" 
#include "NitroBoost.h"
#include "RapidFire.h"
#include "SuperBurst.h"
#include "Shield.h"
#include "Projectile.h"
#include "Explosion.h"
#include <cmath> 
#include <cstdio> 

// Construtor da classe TankRenderer.
TankRenderer::TankRenderer(const Tank& tank) : tank_ref(tank)
{
}

// Desenha todos os projéteis ativos associados ao tanque.
void TankRenderer::drawProjectiles() const
{
    for (const auto &proj : tank_ref.projectiles)
    {
        proj.draw();
    }
}

// Desenha todas as explosões ativas associadas ao tanque.
void TankRenderer::drawExplosions() const
{
    for (const auto &exp : tank_ref.activeExplosions)
    {
        exp.draw();
    }
}

// Desenha o corpo principal do tanque com detalhes de preenchimento e bordas.
// A cor do tanque pode mudar se o escudo estiver ativo.
void TankRenderer::desenhaDetalhado() const
{
    float r_base = 0.2f, g_base = 0.3f, b_base = 0.2f;
    float r_fill = r_base, g_fill = g_base, b_fill = b_base;

    if (tank_ref.shield.isEffectActive()) {
        float pulse = tank_ref.shield.getEffectPulseFactor();
        float shieldR_col = 0.1f, shieldG_col = 0.4f, shieldB_col = 0.9f;
        r_fill = r_base * (1.0f - pulse) + shieldR_col * pulse;
        g_fill = g_base * (1.0f - pulse) + shieldG_col * pulse;
        b_fill = b_base * (1.0f - pulse) + shieldB_col * pulse;
    }
    CV::color(r_fill, g_fill, b_fill);

    Vector2 topLeft = tank_ref.vertices[0];
    Vector2 topRight = tank_ref.vertices[1];
    Vector2 bottomRight = tank_ref.vertices[2];
    Vector2 bottomLeft = tank_ref.vertices[3];

    int numSteps = static_cast<int>(tank_ref.bodyH); 


    for (int i = 0; i <= numSteps; i++)
    {
        float t = static_cast<float>(i) / numSteps;
        Vector2 pLeft(
            topLeft.x + t * (bottomLeft.x - topLeft.x),
            topLeft.y + t * (bottomLeft.y - topLeft.y)
        );
        Vector2 pRight(
            topRight.x + t * (bottomRight.x - topRight.x),
            topRight.y + t * (bottomRight.y - topRight.y)
        );
        CV::line(pLeft, pRight);
    }

    float borderR_base_tank = 0.1f, borderG_base_tank = 0.2f, borderB_base_tank = 0.1f;
    float borderR_tank = borderR_base_tank, borderG_tank = borderG_base_tank, borderB_tank = borderB_base_tank;
    if (tank_ref.shield.isEffectActive()) {
        float pulse = tank_ref.shield.getEffectPulseFactor();
        float shieldBorderR_tank = 0.2f, shieldBorderG_tank = 0.6f, shieldBorderB_tank = 1.0f;
        borderR_tank = borderR_base_tank * (1.0f - pulse) + shieldBorderR_tank * pulse;
        borderG_tank = borderG_base_tank * (1.0f - pulse) + shieldBorderG_tank * pulse;
        borderB_tank = borderB_base_tank * (1.0f - pulse) + shieldBorderB_tank * pulse;
    }
    CV::color(borderR_tank, borderG_tank, borderB_tank);
    for (size_t i = 0; i < tank_ref.vertices.size(); ++i) {
        CV::line(tank_ref.vertices[i], tank_ref.vertices[(i + 1) % tank_ref.vertices.size()]);
    }
}

// Desenha a torre e o canhão do tanque, orientados pela posição do mouse.
// A cor da torre pode mudar se o escudo estiver ativo.
void TankRenderer::desenhaTorre(float mouseX, float mouseY) const
{
    float angle = atan2(mouseY - tank_ref.pivot.y, mouseX - tank_ref.pivot.x);
    float towerW = tank_ref.bodyW * 0.5f;
    float towerH = tank_ref.bodyH * 0.6f;
    float barrelL = tank_ref.bodyW * 1.0f;
    float barrelW = tank_ref.bodyH * 0.1f;
    float halfTW = towerW / 2.0f, halfTH = towerH / 2.0f;
    float halfBWid = barrelW / 2.0f;

    float r_base_turret = 0.1f, g_base_turret = 0.2f, b_base_turret = 0.1f;
    float r_turret_fill = r_base_turret, g_turret_fill = g_base_turret, b_turret_fill = b_base_turret;

    if (tank_ref.shield.isEffectActive()) {
        float pulse = tank_ref.shield.getEffectPulseFactor();
        float shieldTurretR_col = 0.05f, shieldTurretG_col = 0.2f, shieldTurretB_col = 0.6f;
        r_turret_fill = r_base_turret * (1.0f - pulse) + shieldTurretR_col * pulse;
        g_turret_fill = g_base_turret * (1.0f - pulse) + shieldTurretG_col * pulse;
        b_turret_fill = b_base_turret * (1.0f - pulse) + shieldTurretB_col * pulse;
    }
    CV::color(r_turret_fill, g_turret_fill, b_turret_fill);

    for (float y_local = -halfTH; y_local <= halfTH; y_local += 1.0f)
    {
        float x1_local = -halfTW;
        float x2_local = halfTW;
        float x1_world = tank_ref.pivot.x + cos(angle) * x1_local - sin(angle) * y_local;
        float y1_world = tank_ref.pivot.y + sin(angle) * x1_local + cos(angle) * y_local;
        float x2_world = tank_ref.pivot.x + cos(angle) * x2_local - sin(angle) * y_local;
        float y2_world = tank_ref.pivot.y + sin(angle) * x2_local + cos(angle) * y_local;
        CV::line(x1_world, y1_world, x2_world, y2_world);
    }

    float barrelBaseX_local = halfTW;

    for (float y_local_barrel = -halfBWid; y_local_barrel <= halfBWid; y_local_barrel += 1.0f)
    {
        float x1_local_barrel = barrelBaseX_local;
        float y1_local_barrel = y_local_barrel;
        float x2_local_barrel = barrelBaseX_local + barrelL;
        float y2_local_barrel = y_local_barrel;
        float x1_world_barrel = tank_ref.pivot.x + cos(angle) * x1_local_barrel - sin(angle) * y1_local_barrel;
        float y1_world_barrel = tank_ref.pivot.y + sin(angle) * x1_local_barrel + cos(angle) * y1_local_barrel;
        float x2_world_barrel = tank_ref.pivot.x + cos(angle) * x2_local_barrel - sin(angle) * y2_local_barrel;
        float y2_world_barrel = tank_ref.pivot.y + sin(angle) * x2_local_barrel + cos(angle) * y2_local_barrel;
        CV::line(x1_world_barrel, y1_world_barrel, x2_world_barrel, y2_world_barrel);
    }

    float borderR_turret_base_col = r_base_turret * 0.7f, borderG_turret_base_col = g_base_turret * 0.7f, borderB_turret_base_col = b_base_turret * 0.7f;
    float borderR_turret_col = borderR_turret_base_col, borderG_turret_col = borderG_turret_base_col, borderB_turret_col = borderB_turret_base_col;
     if (tank_ref.shield.isEffectActive()) {
        float pulse = tank_ref.shield.getEffectPulseFactor();
        float shieldTurretBorderR_col = 0.1f, shieldTurretBorderB_col = 0.7f; 
        float shieldTurretBorderG_val = 0.3f; 

        borderR_turret_col = borderR_turret_base_col * (1.0f - pulse) + shieldTurretBorderR_col * pulse;
        borderG_turret_col = borderG_turret_base_col * (1.0f - pulse) + shieldTurretBorderG_val * pulse; 
        borderB_turret_col = borderB_turret_base_col * (1.0f - pulse) + shieldTurretBorderB_col * pulse;
    }
    CV::color(borderR_turret_col, borderG_turret_col, borderB_turret_col);

    Vector2 tower_v_local[4];
    tower_v_local[0] = Vector2(-halfTW, -halfTH);
    tower_v_local[1] = Vector2( halfTW, -halfTH);
    tower_v_local[2] = Vector2( halfTW,  halfTH);
    tower_v_local[3] = Vector2(-halfTW,  halfTH);
    Vector2 tower_v_world[4];
    for(int i=0; i<4; ++i) {
        tower_v_world[i] = Vector2(
            tank_ref.pivot.x + cos(angle) * tower_v_local[i].x - sin(angle) * tower_v_local[i].y,
            tank_ref.pivot.y + sin(angle) * tower_v_local[i].x + cos(angle) * tower_v_local[i].y
        );
    }
    for(int i=0; i<4; ++i) {
        CV::line(tower_v_world[i], tower_v_world[(i+1)%4]);
    }

    Vector2 barrel_v_local[4];
    barrel_v_local[0] = Vector2(halfTW, -halfBWid);
    barrel_v_local[1] = Vector2(halfTW + barrelL, -halfBWid);
    barrel_v_local[2] = Vector2(halfTW + barrelL, halfBWid);
    barrel_v_local[3] = Vector2(halfTW, halfBWid);
    Vector2 barrel_v_world[4];
     for(int i=0; i<4; ++i) {
        barrel_v_world[i] = Vector2(
            tank_ref.pivot.x + cos(angle) * barrel_v_local[i].x - sin(angle) * barrel_v_local[i].y,
            tank_ref.pivot.y + sin(angle) * barrel_v_local[i].x + cos(angle) * barrel_v_local[i].y
        );
    }
    for(int i=0; i<4; ++i) {
        CV::line(barrel_v_world[i], barrel_v_world[(i+1)%4]);
    }
}

// Desenha a barra de vida acima do tanque, mostrando a porcentagem de vida restante.
void TankRenderer::drawHealthBar() const
{
    float barWidth = tank_ref.bodyW * 1.5f;
    float barHeight = 10.0f;
    float barX = tank_ref.pivot.x - barWidth / 2.0f;
    float barY = tank_ref.pivot.y - (tank_ref.bodyH / 2.0f + 20.0f);

    float filledWidth = (tank_ref.health / 100.0f) * barWidth;

    CV::color(0.8, 0.1, 0.1);
    CV::rect(barX, barY, barX + barWidth, barY + barHeight);

    CV::color(0.1, 0.8, 0.1);
    CV::rectFill(barX, barY, barX + filledWidth, barY + barHeight);

    char healthText[10];
    sprintf(healthText, "%.0f%%", tank_ref.health);
    CV::color(1.0, 1.0, 1.0);
    
    float textScale = 0.08f; 
    float approxTextWidth = strlen(healthText) * 10.0f * textScale; 
    float textX = barX + (barWidth - approxTextWidth) / 2.0f;
    
    float renderedTextHeight = 119.05f * textScale; 
    float textY = barY + (barHeight - renderedTextHeight) / 2.0f;
    textY += 1.5f; 

    CV::textStroke(textX, textY, healthText, textScale, 1.5f);
}

// Desenha os efeitos visuais de chamas do nitro quando a habilidade está ativa.
void TankRenderer::drawNitroEffects() const
{
    if (!tank_ref.nitro.isNitroActive()) return;

    Vector2 rearMidPoint = (tank_ref.vertices[2] + tank_ref.vertices[3]) * 0.5f;
    Vector2 perpendicularDirection = tank_ref.direction.perpendicular();
    float flameOffsetDistance = tank_ref.bodyW * 0.25f;

    Vector2 flameBaseLeft = rearMidPoint - perpendicularDirection * flameOffsetDistance;
    Vector2 flameBaseRight = rearMidPoint + perpendicularDirection * flameOffsetDistance;
    Vector2 oppositeDirection(-tank_ref.direction.x, -tank_ref.direction.y);
    float flameScale = 1.2f;

    tank_ref.nitro.drawEffect(flameBaseLeft, oppositeDirection, flameScale);
    tank_ref.nitro.drawEffect(flameBaseRight, oppositeDirection, flameScale);
}

// Desenha os elementos da interface do usuário (UI) para a habilidade Nitro.
void TankRenderer::drawNitroUIElements(float x, float y, float w, float h) const
{
    tank_ref.nitro.drawUI(x, y, w, h);
}

// Desenha os elementos da interface do usuário (UI) para a habilidade Tiro Rápido.
void TankRenderer::drawRapidFireUIElements(float x, float y, float w, float h) const
{
    tank_ref.rapidFire.drawUI(x, y, w, h);
}

// Desenha os elementos da interface do usuário (UI) para a habilidade Super Rajada.
void TankRenderer::drawSuperBurstUIElements(float x, float y, float w, float h) const
{
    tank_ref.superBurst.drawUI(x, y, w, h);
}

// Desenha os elementos da interface do usuário (UI) para a habilidade Escudo.
void TankRenderer::drawShieldUIElements(float x, float y, float w, float h) const
{
    tank_ref.shield.drawUI(x, y, w, h);
}
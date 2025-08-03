// Este arquivo define a classe TankRenderer, responsável por toda a lógica de
// renderização do tanque e seus componentes visuais associados.
// Isso inclui o corpo do tanque, a torre, a barra de vida, os projéteis,
// as explosões e os efeitos visuais das habilidades (Nitro, Escudo, etc.),
// bem como os elementos de interface do usuário (UI) para essas habilidades.
// A classe opera com uma referência constante ao objeto Tank para acessar os dados necessários.
#ifndef TANKRENDERER_H_INCLUDED
#define TANKRENDERER_H_INCLUDED

#include "gl_canvas2d.h"
#include "Vector2.h"

class Tank; 

class TankRenderer
{
public:
    const Tank& tank_ref; 

    // Construtor que armazena uma referência ao tanque a ser renderizado.
    TankRenderer(const Tank& tank);

    // Desenha os projéteis disparados pelo tanque.
    void drawProjectiles() const;
    // Desenha as explosões ativas.
    void drawExplosions() const;
    // Desenha o corpo detalhado do tanque, incluindo preenchimento e bordas.
    void desenhaDetalhado() const;
    // Desenha a torre e o canhão do tanque, orientados em direção ao mouse.
    void desenhaTorre(float mouseX, float mouseY) const;
    // Desenha a barra de vida do tanque.
    void drawHealthBar() const;
    // Desenha os efeitos visuais do nitro quando ativo.
    void drawNitroEffects() const;
    // Desenha os elementos da UI para a habilidade Nitro.
    void drawNitroUIElements(float x, float y, float w, float h) const;
    // Desenha os elementos da UI para a habilidade Tiro Rápido.
    void drawRapidFireUIElements(float x, float y, float w, float h) const;
    // Desenha os elementos da UI para a habilidade Super Rajada.
    void drawSuperBurstUIElements(float x, float y, float w, float h) const;
    // Desenha os elementos da UI para a habilidade Escudo.
    void drawShieldUIElements(float x, float y, float w, float h) const;
};

#endif // TANKRENDERER_H_INCLUDED
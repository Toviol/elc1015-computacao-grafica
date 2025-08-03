// Este arquivo define a classe Scoreboard, responsável por gerenciar e exibir a pontuação do jogador.
// A pontuação é exibida usando um estilo de display de sete segmentos.
// Funcionalidades principais:
// - addScore: Adiciona pontos à pontuação total.
// - resetScore: Reinicia a pontuação para zero.
// - getScore: Retorna a pontuação atual.
// - drawSegment: Desenha um único segmento de um dígito.
// - drawDigit: Desenha um dígito completo (0-9) usando seus segmentos.
// - draw: Desenha o placar completo na tela, incluindo a label "SCORE" e os dígitos da pontuação.
#ifndef SCOREBOARD_H_INCLUDED
#define SCOREBOARD_H_INCLUDED

#include "gl_canvas2d.h"
#include <string>
#include <vector>
#include <iomanip> 
#include <sstream> 

class Scoreboard {
public:
    int score;

    // Segmentoss do visor 0-9. Order: A, B, C, D, E, F, G
    //   --D--
    //  E|   |C
    //   --G--
    //  F|   |B
    //   --A--
    
    const std::vector<std::vector<bool>> digitSegments = {
        {true,  true,  true,  true,  true,  true,  false}, // 0
        {false, true,  true,  false, false, false, false}, // 1
        {true,  false, true,  true,  false, true,  true }, // 2
        {true,  true,  true,  true,  false, false, true }, // 3
        {false, true,  true,  false, true,  false, true }, // 4
        {true,  true,  false, true,  true,  false, true }, // 5
        {true,  true,  false,  true, true,  true,  true }, // 6
        {false,  true,  true,  true, false, false, false}, // 7
        {true,  true,  true,  true,  true,  true,  true }, // 8
        {false,  true,  true,  true,  true, false,  true }  // 9
    };

    
    const float segmentOnColor[3] = {0.2f, 0.9f, 0.2f}; 
    const float segmentOffColor[3] = {0.1f, 0.25f, 0.1f}; 
    const float panelBgColor[3] = {0.15f, 0.15f, 0.15f}; 
    const float panelFrameColor[3] = {0.25f, 0.25f, 0.25f}; 

public:
    // Construtor do placar.
    Scoreboard() : score(0) {}

    // Adiciona pontos à pontuação.
    void addScore(int points) {
        score += points;
        if (score > 999) score = 999; 
        if (score < 0) score = 0;     
    }

    // Reinicia a pontuação para zero.
    void resetScore() {
        score = 0;
    }

    // Retorna a pontuação atual.
    int getScore() const { 
        return score;
    }

    // Desenha um único segmento de um dígito.
    void drawSegment(float x, float y, float width, float height, bool on) {
        if (on) {
            CV::color(segmentOnColor[0], segmentOnColor[1], segmentOnColor[2]);
        } else {
            CV::color(segmentOffColor[0], segmentOffColor[1], segmentOffColor[2]);
        }
        CV::rectFill(x, y, x + width, y + height);
    }

    // Desenha um dígito (0-9) na posição especificada.
    void drawDigit(int digitVal, float startX, float startY, float digitWidth, float digitHeight, float segThick) {
        if (digitVal < 0 || digitVal > 9) return;

        const auto& segmentsOn = digitSegments[digitVal];

        float horizSegW = digitWidth - 2 * segThick;
        float vertSegH = (digitHeight - 3 * segThick) / 2;

        
        drawSegment(startX + segThick, startY, horizSegW, segThick, segmentsOn[0]);
        
        drawSegment(startX + digitWidth - segThick, startY + segThick, segThick, vertSegH, segmentsOn[1]);
        
        drawSegment(startX + digitWidth - segThick, startY + 2 * segThick + vertSegH, segThick, vertSegH, segmentsOn[2]);
        
        drawSegment(startX + segThick, startY + digitHeight - segThick, horizSegW, segThick, segmentsOn[3]);
        
        drawSegment(startX, startY + 2 * segThick + vertSegH, segThick, vertSegH, segmentsOn[4]);
        
        drawSegment(startX, startY + segThick, segThick, vertSegH, segmentsOn[5]);
        
        drawSegment(startX + segThick, startY + segThick + vertSegH, horizSegW, segThick, segmentsOn[6]);
    }

    // Desenha o placar completo na tela.
    void draw(int screenWidth, int screenHeight) {
        float digitW = 30;    
        float digitH = 50;    
        float segThickness = 5; 
        float digitSpace = 8;  
        int numDisplayDigits = 3; 

        float totalDigitsWidth = numDisplayDigits * digitW + (numDisplayDigits - 1) * digitSpace;
        float panelPadding = 10;
        float panelW = totalDigitsWidth + 2 * panelPadding;
        float panelH = digitH + 2 * panelPadding;

        float uiMargin = 20.0f; 

        
        const char* scoreLabel = "SCORE";
        float scoreLabelTextScale = 0.15f;
        float labelWidthEstimate = strlen(scoreLabel) * 85.0f * scoreLabelTextScale;
        float scoreLabelY = screenHeight - uiMargin - 15.0f; 
        float scoreLabelX = screenWidth - labelWidthEstimate - uiMargin;

        
        float panelActualX = screenWidth - panelW - uiMargin; 
        float panelActualY = scoreLabelY - panelH - 5.0f; 

        
        CV::color(panelFrameColor[0], panelFrameColor[1], panelFrameColor[2]);
        CV::rectFill(panelActualX, panelActualY, panelActualX + panelW, panelActualY + panelH);

        
        float innerPadding = 3; 
        CV::color(panelBgColor[0], panelBgColor[1], panelBgColor[2]);
        CV::rectFill(panelActualX + innerPadding, panelActualY + innerPadding,
                     panelActualX + panelW - innerPadding, panelActualY + panelH - innerPadding);

        
        std::ostringstream oss;
        oss << std::setw(numDisplayDigits) << std::setfill('0') << score;
        std::string scoreStr = oss.str();
        if ((int)scoreStr.length() > numDisplayDigits) { 
            scoreStr = "";
            for(int i=0; i<numDisplayDigits; ++i) scoreStr += "9";
        }


        float currentX = panelActualX + panelPadding;
        float currentY = panelActualY + panelPadding;

        for (int i = 0; i < numDisplayDigits; ++i) {
            if (i < (int)scoreStr.length()) { 
                 int digit = scoreStr[i] - '0';
                 drawDigit(digit, currentX, currentY, digitW, digitH, segThickness);
            }
            currentX += digitW + digitSpace;
        }

        
        CV::color(1.0f, 1.0f, 1.0f); 
        CV::textStroke(scoreLabelX, scoreLabelY, scoreLabel, scoreLabelTextScale, 1.8f);
    }
};

#endif // SCOREBOARD_H_INCLUDED

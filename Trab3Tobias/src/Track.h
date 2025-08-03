// Este arquivo define a classe Track, responsável por gerenciar a pista de corrida do jogo.
// A classe lida com a criação, manipulação e renderização da pista,
// utilizando curvas B-Spline para definir os limites interno e externo.
// Funcionalidades incluem:
// - Geração de pontos de controle iniciais para formar uma pista circular.
// - Geração de pontos das curvas B-Spline a partir dos pontos de controle.
// - Renderização da pista, incluindo asfalto, linhas de contorno e listras centrais.
// - Manipulação interativa dos pontos de controle (adicionar, deletar, mover) no modo editor.
// - Verificação se um ponto está dentro da área da pista.
// - Validação da largura da pista para garantir que objetos (como o tanque) caibam.
// - Fornecimento de acesso aos pontos das curvas para outras partes do jogo (ex: Background).
#ifndef ___TRACK__H___
#define ___TRACK__H___

#include <vector>
#include "gl_canvas2d.h"
#include <cmath> 
#include "Vector2.h" 

#define M_PI 3.14159265358979323846
#define CONTROL_POINT_RADIUS 5 

class Track
{
    std::vector<Vector2> innerControlPoints;
    std::vector<Vector2> outerControlPoints;
    int screenWidth, screenHeight;

    bool showPoints; 
    int draggingPointIndex; 
    bool draggingInnerPoint; 
    int selectedPointIndex; 
    bool selectedPointIsInner; 

    std::vector<Vector2> innerCurvePoints;
    std::vector<Vector2> outerCurvePoints;
    std::vector<Vector2> centerCurvePoints; 
    float curveGenerationStep = 0.02f; 

    // Gera os pontos de uma curva B-Spline e os armazena em outCurvePoints.
    // Utiliza os pontos de controle fornecidos para calcular os pontos da curva.
    void generateBSplineCurvePoints(const std::vector<Vector2>& controlPoints, std::vector<Vector2>& outCurvePoints)
    {
        outCurvePoints.clear();
        if (controlPoints.size() < 4) return;

        bool firstPointOfCurve = true;
        for (size_t i = 0; i < controlPoints.size() - 3; ++i)
        {
            for (float t = 0.0f; t <= 1.0f; t += curveGenerationStep)
            {
                
                if (!firstPointOfCurve && t == 0.0f) {
                    continue;
                }

                float t2 = t * t;
                float t3 = t2 * t;

                float b0 = (-t3 + 3*t2 - 3*t + 1) / 6.0f;
                float b1 = (3*t3 - 6*t2 + 0*t + 4) / 6.0f;
                float b2 = (-3*t3 + 3*t2 + 3*t + 1) / 6.0f;
                float b3 = (t3) / 6.0f;

                float x = b0 * controlPoints[i].x + b1 * controlPoints[i+1].x + b2 * controlPoints[i+2].x + b3 * controlPoints[i+3].x;
                float y = b0 * controlPoints[i].y + b1 * controlPoints[i+1].y + b2 * controlPoints[i+2].y + b3 * controlPoints[i+3].y;
                
                outCurvePoints.push_back(Vector2(x, y));
                firstPointOfCurve = false;
            }
        }
    }

    // Gera os pontos da curva central da pista, calculados como a média
    // dos pontos correspondentes das curvas interna e externa.
    void generateCenterCurvePoints()
    {
        centerCurvePoints.clear();
        if (innerCurvePoints.size() != outerCurvePoints.size() || innerCurvePoints.empty()) {
            
            return;
        }
        for (size_t i = 0; i < innerCurvePoints.size(); ++i) {
            
            centerCurvePoints.push_back((innerCurvePoints[i] + outerCurvePoints[i]) * 0.5f);
        }
    }

    // Recalcula os pontos das curvas interna, externa e central da pista.
    // Chamado quando os pontos de controle são modificados.
    void regenerateCurvePoints()
    {
        generateBSplineCurvePoints(innerControlPoints, innerCurvePoints);
        generateBSplineCurvePoints(outerControlPoints, outerCurvePoints);
        generateCenterCurvePoints(); 
    }

    // Implementa o algoritmo de Ray Casting (PNPOLY) para verificar se um ponto
    // está dentro de um polígono definido por uma lista de vértices.
    // Retorna true se o ponto estiver dentro, false caso contrário.
    bool isPointInPolygon(const Vector2& testPoint, const std::vector<Vector2>& polygon) const
    {
        int nvert = polygon.size();
        if (nvert < 3) return false;
        bool c = false;
        float testx = testPoint.x;
        float testy = testPoint.y;
        for (int i = 0, j = nvert - 1; i < nvert; j = i++) {
            float vertix_i = polygon[i].x;
            float vertiy_i = polygon[i].y;
            float vertix_j = polygon[j].x;
            float vertiy_j = polygon[j].y;
            
            if (((vertiy_i > testy) != (vertiy_j > testy)) &&
                (testx < (vertix_j - vertix_i) * (testy - vertiy_i) / std::max(0.00001f, std::fabs(vertiy_j - vertiy_i)) + vertix_i))
                c = !c;
        }
        return c;
    }

    // Desenha uma curva na tela a partir de uma lista de pontos.
    // Conecta os pontos sequencialmente com linhas.
    void drawCurveFromPoints(const std::vector<Vector2>& curvePoints)
    {
        if (curvePoints.size() < 2) return;
        CV::color(0,0,0);
        for (size_t i = 0; i < curvePoints.size() - 1; ++i)
        {
            CV::line(curvePoints[i], curvePoints[i+1]);
        }
        
        
        
        
        
    }

    // Desenha os pontos de controle da pista na tela.
    // Pontos internos são desenhados em vermelho e externos em azul.
    // O ponto selecionado é destacado com uma cor diferente e tamanho maior.
    void drawControlPoints()
    {
        
        for (size_t i = 0; i < innerControlPoints.size() -3; ++i) { 
            if (showPoints && selectedPointIsInner && selectedPointIndex == (int)i) {
                CV::color(1, 1, 0); 
                CV::circleFill(innerControlPoints[i].x, innerControlPoints[i].y, CONTROL_POINT_RADIUS + 2, 12); 
            }
            CV::color(1, 0, 0);
            CV::circleFill(innerControlPoints[i].x, innerControlPoints[i].y, CONTROL_POINT_RADIUS, 10);
        }

        
        for (size_t i = 0; i < outerControlPoints.size() -3; ++i) { 
             if (showPoints && !selectedPointIsInner && selectedPointIndex == (int)i) {
                CV::color(0, 1, 1); 
                CV::circleFill(outerControlPoints[i].x, outerControlPoints[i].y, CONTROL_POINT_RADIUS + 2, 12); 
            }
            CV::color(0, 0, 1);
            CV::circleFill(outerControlPoints[i].x, outerControlPoints[i].y, CONTROL_POINT_RADIUS, 10);
        }
    }

public:
    // Construtor da classe Track.
    // Inicializa a pista com pontos de controle formando um círculo.
    // Parâmetros definem dimensões da tela, centro e raios da pista inicial, e número de pontos.
    Track(int screenWidth, int screenHeight, float centerX, float centerY, float innerRadius, float outerRadius, int numPoints = 6)
    {
        this->screenWidth = screenWidth;
        this->screenHeight = screenHeight;
        this->showPoints = false; 
        this->draggingPointIndex = -1;
        this->selectedPointIndex = -1; 
        this->selectedPointIsInner = false; 

        for (int i = 0; i < numPoints; ++i)
        {
            float angle = 2.0f * M_PI * i / numPoints;
            innerControlPoints.push_back(Vector2(
                (centerX + innerRadius * cos(angle)),
                (centerY + innerRadius * sin(angle))
            ));
            outerControlPoints.push_back(Vector2(
                (centerX + outerRadius * cos(angle)),
                (centerY + outerRadius * sin(angle))
            ));
        }
        if (numPoints > 3) { 
            for(int i=0; i<3; ++i) {
                innerControlPoints.push_back(innerControlPoints[i]);
                outerControlPoints.push_back(outerControlPoints[i]);
            }
        }
        regenerateCurvePoints(); 
    }

    // Renderiza a pista completa na tela.
    // Inclui o asfalto, as listras centrais amarelas, as linhas de contorno
    // e os pontos de controle (se a visibilidade estiver ativa).
    void renderTrack()
    {
        
        CV::color(0.2f, 0.2f, 0.25f); 
        if (innerCurvePoints.size() > 1 && innerCurvePoints.size() == outerCurvePoints.size())
        {
            size_t numPoints = innerCurvePoints.size();
            for (size_t i = 0; i < numPoints; ++i)
            {
                Vector2 o1 = outerCurvePoints[i];
                Vector2 i1 = innerCurvePoints[i];
                
                Vector2 o2 = outerCurvePoints[(i + 1) % numPoints];
                Vector2 i2 = innerCurvePoints[(i + 1) % numPoints];

                
                CV::triangleFill(o1, i1, i2); 
                CV::triangleFill(o1, i2, o2); 
            }
        }

        
        CV::color(1.0f, 0.85f, 0.0f); 
        if (centerCurvePoints.size() > 1)
        {
            float stripeWidth = 4.0f;   
            int stripePatternUnitLength = 15; 
                                             
            bool drawCurrentSegmentAsStripe = true;
            int patternCounter = 0;

            for (size_t i = 0; i < centerCurvePoints.size() - 1; ++i)
            {
                patternCounter++;
                if (patternCounter >= stripePatternUnitLength) {
                    drawCurrentSegmentAsStripe = !drawCurrentSegmentAsStripe; 
                    patternCounter = 0;
                }

                if (!drawCurrentSegmentAsStripe) {
                    continue; 
                }

                Vector2 p_start_center = centerCurvePoints[i];
                Vector2 p_end_center = centerCurvePoints[i+1];

                Vector2 tangent = (p_end_center - p_start_center);
                
                if (tangent.lengthSquared() < 0.0001f) continue; 
                tangent.normalize();
                Vector2 normal = tangent.perpendicular(); 

                
                std::vector<Vector2> stripeRectVertices;
                stripeRectVertices.push_back(p_start_center - normal * (stripeWidth / 2.0f));
                stripeRectVertices.push_back(p_start_center + normal * (stripeWidth / 2.0f));
                stripeRectVertices.push_back(p_end_center   + normal * (stripeWidth / 2.0f));
                stripeRectVertices.push_back(p_end_center   - normal * (stripeWidth / 2.0f));
                
                
                float vx[4], vy[4];
                for(int k=0; k<4; ++k) {
                    vx[k] = stripeRectVertices[k].x;
                    vy[k] = stripeRectVertices[k].y;
                }
                CV::polygonFill(vx, vy, 4); 
            }
        }

        
        
        drawCurveFromPoints(innerCurvePoints);
        drawCurveFromPoints(outerCurvePoints);

        
        if (showPoints)
        {
            drawControlPoints();
        }
    }

    // Alterna a visibilidade dos pontos de controle na tela.
    void toggleControlPointsVisibility()
    {
        showPoints = !showPoints;
    }

    // Define explicitamente a visibilidade dos pontos de controle.
    void setControlPointsVisibility(bool visible) { 
        showPoints = visible;
    }

    // Lida com o clique do mouse quando os pontos de controle estão visíveis.
    // Verifica se um ponto de controle foi clicado e o seleciona para arrastar.
    // Retorna true se um ponto foi selecionado, false caso contrário.
    bool handleMousePress(int mouseX, int mouseY)
    {
        if (!showPoints) return false;
        draggingPointIndex = -1;
        

        for (size_t i = 0; i < innerControlPoints.size() - 3; ++i) {
            float distSq = (mouseX - innerControlPoints[i].x) * (mouseX - innerControlPoints[i].x) +
                           (mouseY - innerControlPoints[i].y) * (mouseY - innerControlPoints[i].y);
            if (distSq < CONTROL_POINT_RADIUS * CONTROL_POINT_RADIUS * 4) { 
                draggingPointIndex = i;
                draggingInnerPoint = true;
                selectedPointIndex = i; 
                selectedPointIsInner = true; 
                return true;
            }
        }
        for (size_t i = 0; i < outerControlPoints.size() - 3; ++i) {
            float distSq = (mouseX - outerControlPoints[i].x) * (mouseX - outerControlPoints[i].x) +
                           (mouseY - outerControlPoints[i].y) * (mouseY - outerControlPoints[i].y);
            if (distSq < CONTROL_POINT_RADIUS * CONTROL_POINT_RADIUS * 4) { 
                draggingPointIndex = i;
                draggingInnerPoint = false;
                selectedPointIndex = i; 
                selectedPointIsInner = false; 
                return true;
            }
        }
        
        
        return false;
    }

    // Lida com o arrastar do mouse quando um ponto de controle está selecionado.
    // Atualiza a posição do ponto de controle arrastado e recalcula as curvas da pista.
    void handleMouseDrag(int mouseX, int mouseY)
    {
        if (draggingPointIndex != -1 && showPoints)
        {
            Vector2* pointToDrag = nullptr;
            if (draggingInnerPoint) {
                pointToDrag = &innerControlPoints[draggingPointIndex];
            } else {
                pointToDrag = &outerControlPoints[draggingPointIndex];
            }
            pointToDrag->set(mouseX, mouseY);
            if (draggingPointIndex < 3) {
                if (draggingInnerPoint) {
                    innerControlPoints[innerControlPoints.size() - 3 + draggingPointIndex].set(mouseX, mouseY);
                } else {
                    outerControlPoints[outerControlPoints.size() - 3 + draggingPointIndex].set(mouseX, mouseY);
                }
            }
            regenerateCurvePoints(); 
        }
    }

    // Lida com o soltar do botão do mouse.
    // Finaliza a operação de arrastar um ponto de controle.
    void handleMouseRelease()
    {
        draggingPointIndex = -1;
        
    }

    // Retorna true se os pontos de controle estão visíveis, false caso contrário.
    bool arePointsVisible() const {
        return showPoints;
    }

    // Verifica se um ponto (Vector2) está dentro da área da pista.
    // Um ponto está na pista se estiver dentro da curva externa e fora da curva interna.
    bool isPointInsideTrack(const Vector2& point) const
    {
        if (outerCurvePoints.empty() || innerCurvePoints.empty()) return false;
        
        return isPointInPolygon(point, outerCurvePoints) && !isPointInPolygon(point, innerCurvePoints);
    }

    // Adiciona um novo ponto de controle à curva interna ou externa.
    // O ponto é inserido de forma a manter a natureza fechada da curva B-Spline.
    void addControlPoint(bool isInner, const Vector2& newPoint) {
        std::vector<Vector2>* controlPointsVec = isInner ? &innerControlPoints : &outerControlPoints;

        if (controlPointsVec->size() < 3) { 
            
            
            controlPointsVec->push_back(newPoint);
            while(controlPointsVec->size() < 4) {
                controlPointsVec->push_back(newPoint); 
            }
        } else {
            
            
            size_t insertPos = controlPointsVec->size() - 3;
            controlPointsVec->insert(controlPointsVec->begin() + insertPos, newPoint);
        }
        regenerateCurvePoints();
    }

    // Deleta o ponto de controle atualmente selecionado.
    // Não permite a exclusão se resultar em menos de 4 pontos de controle únicos.
    // Atualiza os pontos duplicados se um dos três primeiros pontos for excluído.
    void deleteSelectedControlPoint() {
        if (selectedPointIndex == -1) return; 

        std::vector<Vector2>* controlPointsVec = selectedPointIsInner ? &innerControlPoints : &outerControlPoints;

        
        
        if ((controlPointsVec->size() - 3) <= 4) {
            
            return; 
        }

        if (selectedPointIndex < (int)(controlPointsVec->size() - 3)) { 
            controlPointsVec->erase(controlPointsVec->begin() + selectedPointIndex);

            
            if (selectedPointIndex < 3) {
                 
                controlPointsVec->pop_back();
                controlPointsVec->pop_back();
                controlPointsVec->pop_back();
                
                for(int i=0; i<3; ++i) {
                    controlPointsVec->push_back((*controlPointsVec)[i]);
                }
            }
            selectedPointIndex = -1; 
            regenerateCurvePoints();
        }
    }

    // Retorna a posição do último ponto de controle único da curva especificada.
    // Útil para determinar uma posição inicial ao adicionar um novo ponto.
    Vector2 getLastUniqueControlPointPosition(bool isInner, const Vector2& defaultPosition) const {
        const std::vector<Vector2>* controlPointsVec = isInner ? &innerControlPoints : &outerControlPoints;

        if (controlPointsVec->size() > 3) {
            
            return (*controlPointsVec)[controlPointsVec->size() - 4];
        }
        if (!controlPointsVec->empty()){
             return controlPointsVec->back(); 
        }
        
        return defaultPosition;
    }

    // Valida se a largura da pista é suficiente para acomodar um objeto de uma determinada largura.
    // Verifica a distância entre os pontos correspondentes das curvas interna e externa.
    // Retorna true se a pista for larga o suficiente em todos os pontos, false caso contrário.
    bool validateTrackWidth(float objectWidth) const {
        if (innerCurvePoints.empty() || outerCurvePoints.empty() || innerCurvePoints.size() != outerCurvePoints.size()) {
            return false; 
        }

        for (size_t i = 0; i < innerCurvePoints.size(); ++i) {
            
            float distance = (innerCurvePoints[i] - outerCurvePoints[i]).length();
            if (distance < objectWidth) {
                return false; 
            }
        }
        return true; 
    }

    // Retorna uma referência constante ao vetor de pontos da curva externa.
    // Usado, por exemplo, pela classe Background para desenhar a ilha.
    const std::vector<Vector2>& getOuterCurvePoints() const {
        return outerCurvePoints;
    }

    // Retorna uma referência constante ao vetor de pontos da curva interna.
    const std::vector<Vector2>& getInnerCurvePoints() const {
        return innerCurvePoints;
    }

    // Retorna a largura da tela.
    int getScreenWidth() const {
        return screenWidth;
    }

    // Retorna a altura da tela.
    int getScreenHeight() const {
        return screenHeight;
    }
};

#endif
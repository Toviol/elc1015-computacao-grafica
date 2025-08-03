#include "Compass.h"

void Compass::render(int screenWidth, int screenHeight, 
                    const Vector3& cameraDirection, const Vector3& cameraUp) {
    // Posiçsao da bússola (canto superior direito)
    float compassX = screenWidth - 100.0f;  // 100px da borda direita
    float compassY = screenHeight - 100.0f; // 100px da borda superior
    float compassSize = 35.0f; // Tamanho da bússola
    
    // Calcula vetores da câmera normalizados
    Vector3 forward = cameraDirection; forward.normalize();
    Vector3 worldUp(0, 1, 0);
    Vector3 right = Vector3::cross(cameraDirection, worldUp); right.normalize();
    Vector3 up = Vector3::cross(right, cameraDirection); up.normalize();
    
    // Projeta os vetores 3D para 2D
    float forwardX = forward.x * compassSize;
    float forwardY = forward.z * compassSize; // Usa Z para Y na tela
    
    float rightX = right.x * compassSize;
    float rightY = right.z * compassSize;
    
    float upX = up.x * compassSize * 0.7f; // Faz o vetor Y menor para clareza
    float upY = up.y * compassSize * 0.7f;
    
    // Desenha fundo da bússola com borda
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Fundo escuro semi-transparente
    glColor4f(0.0f, 0.0f, 0.0f, 0.8f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(compassX, compassY);
    for (int i = 0; i <= 32; i++) {
        float angle = i * 2.0f * 3.14159f / 32.0f;
        float x = compassX + cos(angle) * (compassSize + 8);
        float y = compassY + sin(angle) * (compassSize + 8);
        glVertex2f(x, y);
    }
    glEnd();
    
    // Borda branca
    glColor4f(1.0f, 1.0f, 1.0f, 0.9f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i <= 32; i++) {
        float angle = i * 2.0f * 3.14159f / 32.0f;
        float x = compassX + cos(angle) * (compassSize + 8);
        float y = compassY + sin(angle) * (compassSize + 8);
        glVertex2f(x, y);
    }
    glEnd();
    glLineWidth(1.0f);
    
    glDisable(GL_BLEND);
    
    // Desenha eixo X (direita) - Vermelho
    glColor3f(1.0f, 0.0f, 0.0f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex2f(compassX, compassY);
    glVertex2f(compassX + rightX, compassY + rightY);
    glEnd();
    
    // Seta do eixo X
    glBegin(GL_TRIANGLES);
    float arrowX = compassX + rightX;
    float arrowY = compassY + rightY;
    float arrowSize = 6.0f;
    float arrowDirX = rightX / sqrt(rightX*rightX + rightY*rightY + 0.001f);
    float arrowDirY = rightY / sqrt(rightX*rightX + rightY*rightY + 0.001f);
    glVertex2f(arrowX, arrowY);
    glVertex2f(arrowX - arrowDirX*arrowSize - arrowDirY*arrowSize/2, arrowY - arrowDirY*arrowSize + arrowDirX*arrowSize/2);
    glVertex2f(arrowX - arrowDirX*arrowSize + arrowDirY*arrowSize/2, arrowY - arrowDirY*arrowSize - arrowDirX*arrowSize/2);
    glEnd();
    
    // Desenha eixo Z (frente) - Azul
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_LINES);
    glVertex2f(compassX, compassY);
    glVertex2f(compassX + forwardX, compassY + forwardY);
    glEnd();
    
    // Seta do eixo Z
    glBegin(GL_TRIANGLES);
    arrowX = compassX + forwardX;
    arrowY = compassY + forwardY;
    arrowDirX = forwardX / sqrt(forwardX*forwardX + forwardY*forwardY + 0.001f);
    arrowDirY = forwardY / sqrt(forwardX*forwardX + forwardY*forwardY + 0.001f);
    glVertex2f(arrowX, arrowY);
    glVertex2f(arrowX - arrowDirX*arrowSize - arrowDirY*arrowSize/2, arrowY - arrowDirY*arrowSize + arrowDirX*arrowSize/2);
    glVertex2f(arrowX - arrowDirX*arrowSize + arrowDirY*arrowSize/2, arrowY - arrowDirY*arrowSize - arrowDirX*arrowSize/2);
    glEnd();
    
    // Desenha eixo Y (cima) - Verde
    glColor3f(0.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex2f(compassX, compassY);
    glVertex2f(compassX + upX, compassY + upY);
    glEnd();
    
    // Seta do eixo Y
    glBegin(GL_TRIANGLES);
    arrowX = compassX + upX;
    arrowY = compassY + upY;
    arrowDirX = upX / sqrt(upX*upX + upY*upY + 0.001f);
    arrowDirY = upY / sqrt(upX*upX + upY*upY + 0.001f);
    glVertex2f(arrowX, arrowY);
    glVertex2f(arrowX - arrowDirX*arrowSize - arrowDirY*arrowSize/2, arrowY - arrowDirY*arrowSize + arrowDirX*arrowSize/2);
    glVertex2f(arrowX - arrowDirX*arrowSize + arrowDirY*arrowSize/2, arrowY - arrowDirY*arrowSize - arrowDirX*arrowSize/2);
    glEnd();
    
    // Mudei a fonte dos labels, mas parece que não deu boa pros betinha
    
    // Label X (vermelho)
    glColor3f(1.0f, 0.3f, 0.3f);
    glRasterPos2f(compassX + rightX + 15, compassY + rightY - 5);
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'X');
    
    // Label Z (azul)
    glColor3f(0.3f, 0.3f, 1.0f);
    glRasterPos2f(compassX + forwardX + 15, compassY + forwardY - 5);
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'Z');
    
    // Label Y (verde)
    glColor3f(0.3f, 1.0f, 0.3f);
    glRasterPos2f(compassX + upX + 15, compassY + upY - 5);
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, 'Y');
    
    // Título da bússola
    glColor3f(1.0f, 1.0f, 1.0f);
    renderText(compassX - 35, compassY - compassSize - 25, "Orientacao", GLUT_BITMAP_8_BY_13);
    
    // Restaura largura da linha
    glLineWidth(1.0f);
}

void Compass::renderText(int x, int y, const char* text, void* font) {
    // Renderiza sombra para melhor contraste
    glColor3f(0.0f, 0.0f, 0.0f);
    glRasterPos2i(x + 1, y - 1);
    for (int i = 0; text[i] != '\0'; i++) {
        glutBitmapCharacter(font, text[i]);
    }
    
    // Texto
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2i(x, y);
    for (int i = 0; text[i] != '\0'; i++) {
        glutBitmapCharacter(font, text[i]);
    }
}

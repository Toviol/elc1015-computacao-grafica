/*
DESCRIÇÃO:
Este arquivo centraliza toda a lógica de carregamento e gerenciamento de 
texturas para o simulador espacial. Fornece funções para carregar texturas
de arquivos BMP e renderizar esferas texturizadas.

FUNCIONALIDADES:
- Carregamento genérico de texturas BMP
- Cache de texturas para evitar recarregamento
- Renderização de esferas com mapeamento equiretangular
- Funções específicas para texturas da lua e sol
- Controle de estado OpenGL para texturas

*/

#ifndef ___TEXTURA__H___
#define ___TEXTURA__H___

#include "Bmp.h"
#include <GL/glut.h>
#include <cmath>
#include <cstring>

class TexturaManager {
public:
    // === MÉTODOS DE CARREGAMENTO DE TEXTURAS ===
    
    static GLuint loadTexture(const char* texturePath);
    
    static GLuint getMoonTextureID();
    
    static GLuint getSunTextureID();
    
    static GLuint getPlanetTextureID(const char* planetName);
    
    // === MÉTODOS DE RENDERIZAÇÃO ===
    
    static void renderTexturedSphere(float radius, int slices, int stacks, GLuint textureID);
    
    static void renderMoonSphere(float radius, int slices, int stacks);
    
    static void renderSunSphere(float radius, int slices, int stacks);
    
    // === MÉTODOS DE CONTROLE DE ESTADO ===
    
    static void enableTexturing();
    
    static void disableTexturing();

private:
    static void renderBasicTexturedSphere(float radius, int slices, int stacks);
};

// === IMPLEMENTAÇÕES INLINE ===

inline GLuint TexturaManager::loadTexture(const char* texturePath) {
    GLuint textureID = 0;
    
    Bmp* texture = new Bmp(texturePath);
    
    if (texture) {
        texture->convertBGRtoRGB();
        unsigned char* textureData = texture->getImage();
        
        if (textureData != NULL) {
            // Gera ID da textura
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);
            
            // Configura parâmetros da textura
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            
            // Construir mipmaps
            gluBuild2DMipmaps(GL_TEXTURE_2D,
                             GL_RGB,
                             texture->getWidth(),
                             texture->getHeight(),
                             GL_RGB,
                             GL_UNSIGNED_BYTE,
                             textureData);
        }
        delete texture;
    }
    
    return textureID;
}

inline GLuint TexturaManager::getMoonTextureID() {
    static GLuint moonTextureID = 0;
    static bool loaded = false;
    
    if (!loaded) {
        moonTextureID = loadTexture(".\\trab5Tobias\\imagens\\8k_moon.bmp");
        loaded = true;
    }
    
    return moonTextureID;
}

inline GLuint TexturaManager::getSunTextureID() {
    static GLuint sunTextureID = 0;
    static bool loaded = false;
    
    if (!loaded) {
        sunTextureID = loadTexture(".\\trab5Tobias\\imagens\\8k_sun.bmp");
        loaded = true;
    }
    
    return sunTextureID;
}

inline GLuint TexturaManager::getPlanetTextureID(const char* planetName) {
    static GLuint mercuryTextureID = 0;
    static GLuint venusTextureID = 0;
    static GLuint earthTextureID = 0;
    static GLuint marsTextureID = 0;
    static GLuint jupiterTextureID = 0;
    static GLuint saturnTextureID = 0;
    static GLuint uranusTextureID = 0;
    static GLuint neptuneTextureID = 0;
    
    if (strcmp(planetName, "mercury") == 0) {
        if (mercuryTextureID == 0) {
            mercuryTextureID = loadTexture(".\\trab5Tobias\\imagens\\planets\\8k_mercury.bmp");
        }
        return mercuryTextureID;
    }
    else if (strcmp(planetName, "venus") == 0) {
        if (venusTextureID == 0) {
            venusTextureID = loadTexture(".\\trab5Tobias\\imagens\\planets\\4k_venus_atmosphere.bmp");
        }
        return venusTextureID;
    }
    else if (strcmp(planetName, "earth") == 0) {
        if (earthTextureID == 0) {
            earthTextureID = loadTexture(".\\trab5Tobias\\imagens\\planets\\8k_earth_daymap.bmp");
        }
        return earthTextureID;
    }
    else if (strcmp(planetName, "mars") == 0) {
        if (marsTextureID == 0) {
            marsTextureID = loadTexture(".\\trab5Tobias\\imagens\\planets\\8k_mars.bmp");
        }
        return marsTextureID;
    }
    else if (strcmp(planetName, "jupiter") == 0) {
        if (jupiterTextureID == 0) {
            jupiterTextureID = loadTexture(".\\trab5Tobias\\imagens\\planets\\8k_jupiter.bmp");
        }
        return jupiterTextureID;
    }
    else if (strcmp(planetName, "saturn") == 0) {
        if (saturnTextureID == 0) {
            saturnTextureID = loadTexture(".\\trab5Tobias\\imagens\\planets\\8k_saturn.bmp");
        }
        return saturnTextureID;
    }
    else if (strcmp(planetName, "uranus") == 0) {
        if (uranusTextureID == 0) {
            uranusTextureID = loadTexture(".\\trab5Tobias\\imagens\\planets\\2k_uranus.bmp");
        }
        return uranusTextureID;
    }
    else if (strcmp(planetName, "neptune") == 0) {
        if (neptuneTextureID == 0) {
            neptuneTextureID = loadTexture(".\\trab5Tobias\\imagens\\planets\\2k_neptune.bmp");
        }
        return neptuneTextureID;
    }
    
    return 0; // Textura não encontrada
}

inline void TexturaManager::enableTexturing() {
    glEnable(GL_TEXTURE_2D);
}

inline void TexturaManager::disableTexturing() {
    glDisable(GL_TEXTURE_2D);
}

inline void TexturaManager::renderBasicTexturedSphere(float radius, int slices, int stacks) {
    // Renderiza esfera com coordenadas de textura adequadas para mapas equiretangulares
    const float PI = 3.14159265359f;
    
    for (int i = 0; i < stacks; i++) {
        float lat0 = PI * (-0.5f + (float)i / stacks);
        float lat1 = PI * (-0.5f + (float)(i + 1) / stacks);
        
        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= slices; j++) {
            float lng = 2 * PI * (float)j / slices;
            
            // Coordenadas de textura para mapeamento equiretangular
            float u = (float)j / slices;
            float v0 = (float)i / stacks;
            float v1 = (float)(i + 1) / stacks;
            
            // Primeira linha do quad (stack i)
            float x0 = cos(lng) * cos(lat0);
            float y0 = sin(lat0);
            float z0 = sin(lng) * cos(lat0);
            
            // A normal é simplesmente o vetor posição normalizado para uma esfera centrada na origem
            glNormal3f(x0, y0, z0);
            glTexCoord2f(u, v0);
            glVertex3f(x0 * radius, y0 * radius, z0 * radius);
            
            // Segunda linha do quad (stack i+1)
            float x1 = cos(lng) * cos(lat1);
            float y1 = sin(lat1);
            float z1 = sin(lng) * cos(lat1);
            
            // A normal é simplesmente o vetor posição normalizado para uma esfera centrada na origem
            glNormal3f(x1, y1, z1);
            glTexCoord2f(u, v1);
            glVertex3f(x1 * radius, y1 * radius, z1 * radius);
        }
        glEnd();
    }
}

inline void TexturaManager::renderTexturedSphere(float radius, int slices, int stacks, GLuint textureID) {
    if (textureID != 0) {
        enableTexturing();
        glBindTexture(GL_TEXTURE_2D, textureID);
        
        renderBasicTexturedSphere(radius, slices, stacks);
        
        disableTexturing();
    } else {
        // Fallback para esfera sem textur
        glutSolidSphere(radius, slices, stacks);
    }
}

inline void TexturaManager::renderMoonSphere(float radius, int slices, int stacks) {
    GLuint moonTextureID = getMoonTextureID();
    renderTexturedSphere(radius, slices, stacks, moonTextureID);
}

inline void TexturaManager::renderSunSphere(float radius, int slices, int stacks) {
    GLuint sunTextureID = getSunTextureID();
    renderTexturedSphere(radius, slices, stacks, sunTextureID);
}

#endif // ___TEXTURA__H___

// Este arquivo define a classe Background, responsável por desenhar o cenário do jogo.
// Isso inclui o mar, a ilha (com praia e vegetação) e objetos decorativos
// como casas, palmeiras, pedras e arbustos.
// Funcionalidades principais:
// - Geração e desenho de "features" da água para dar textura ao mar.
// - Desenho da ilha, com camadas de praia e vegetação baseadas nos contornos da pista.
// - Geração procedural de objetos decorativos, posicionados ao redor da pista.
// - Desenho dos objetos decorativos, cada um com sua lógica de renderização específica.
// - Utiliza a forma da pista (Track) para delimitar a ilha e posicionar objetos.
// - Os objetos decorativos e as features da água são gerados com sementes de aleatoriedade
//   para garantir consistência visual entre execuções ou quando a pista muda.
#ifndef BACKGROUND_H_INCLUDED
#define BACKGROUND_H_INCLUDED

#include "gl_canvas2d.h"
#include "Track.h"
#include <vector>
#include <cmath>
#include <cstdlib> 
#include <algorithm> 


#ifndef PI
#define PI 3.14159265359
#endif
#ifndef PI_2 
#define PI_2 6.28318530718
#endif


struct LeafInfo {
    float angle;    
    float length;   
    
    
    
};


struct BushClumpInfo {
    Vector2 offset; 
    float radius;   
};

enum class ObjectType { HOUSE, PALM_TREE, ROCK, BUSH };

struct DecorativeObject {
    ObjectType type;
    Vector2 position;
    float rotation; 
    float size_param1; 
    float size_param2; 
    float r1, g1, b1; 
    float r2, g2, b2; 

    
    std::vector<LeafInfo> palmLeafDetails;

    
    std::vector<Vector2> rockVertices;

    
    std::vector<BushClumpInfo> bushClumpDetails;
};

class Background {
public:
    // Construtor da classe Background.
    // Inicializa a semente para a geração de features da água e as gera.
    Background(int screenWidth, int screenHeight) {
        srand(12345); 

        
        generateWaterFeatures(screenWidth, screenHeight, 80, 50.0f, 150.0f, 8.0f, 25.0f, 0.05f, 0.3f, 0.6f, 0.7f);
        generateWaterFeatures(screenWidth, screenHeight, 100, 30.0f, 120.0f, 6.0f, 20.0f, 0.1f, 0.4f, 0.7f, 0.6f);
        generateWaterFeatures(screenWidth, screenHeight, 120, 15.0f, 80.0f, 4.0f, 15.0f, 0.3f, 0.6f, 0.85f, 0.8f);
        generateWaterFeatures(screenWidth, screenHeight, 150, 5.0f, 40.0f, 3.0f, 8.0f, 0.7f, 0.85f, 0.95f, 0.75f);
    }

    // Desenha todos os elementos do background.
    // Verifica se a pista mudou para regenerar os objetos decorativos.
    // Desenha o mar, a ilha/praia e os objetos decorativos.
    void draw(int screenWidth, int screenHeight, const Track& track) {
        const auto& currentOuterTrackPoints = track.getOuterCurvePoints();
        bool trackChanged = false;

        if (currentOuterTrackPoints.size() != lastOuterTrackPointsForDecor.size()) {
            trackChanged = true;
        } else {
            for (size_t i = 0; i < currentOuterTrackPoints.size(); ++i) {
                
                
                if (std::fabs(currentOuterTrackPoints[i].x - lastOuterTrackPointsForDecor[i].x) > 0.001f ||
                    std::fabs(currentOuterTrackPoints[i].y - lastOuterTrackPointsForDecor[i].y) > 0.001f) {
                    trackChanged = true;
                    break;
                }
            }
        }

        if (trackChanged || lastOuterTrackPointsForDecor.empty()) {
            srand(54321); 
            generateDecorativeObjects(screenWidth, screenHeight, track);
            lastOuterTrackPointsForDecor = currentOuterTrackPoints; 
        }

        drawSea(screenWidth, screenHeight);
        drawIslandAndBeach(screenWidth, screenHeight, track);
        drawDecorativeObjects(screenWidth, screenHeight);
    }

private:
    struct WaterFeature {
        std::vector<Vector2> vertices;
        float r, g, b, a;
    };
    std::vector<WaterFeature> waterFeatures;
    std::vector<DecorativeObject> decorativeObjects;
    std::vector<Vector2> lastOuterTrackPointsForDecor; 

    // Gera "features" de água (formas poligonais sinuosas) para adicionar detalhe visual ao mar.
    // Os parâmetros controlam o número, tamanho, forma e cor dessas features.
    void generateWaterFeatures(int sWidth, int sHeight, int numFeatures,
                               float minLength, float maxLength,
                               float minWidth, float maxWidth,
                               float cr, float cg, float cb, float ca) {
        for (int i = 0; i < numFeatures; ++i) {
            WaterFeature feature;
            feature.r = cr;
            feature.g = cg;
            feature.b = cb;
            feature.a = ca;

            float x_start = (rand() % (sWidth + 200)) - 100; 
            float y_start = (rand() % (sHeight + 200)) - 100;
            float length = minLength + (rand() / (float)RAND_MAX) * (maxLength - minLength);
            float width = minWidth + (rand() / (float)RAND_MAX) * (maxWidth - minWidth);
            float angle = (rand() / (float)RAND_MAX) * 2.0f * PI;

            int numSegments = 5 + rand()%5; 
            float segmentLength = length / numSegments;

            Vector2 currentPos(x_start, y_start);
            Vector2 direction(cos(angle), sin(angle));

            
            
            feature.vertices.push_back(currentPos + direction.perpendicular() * (width / 2.0f));
            feature.vertices.push_back(currentPos - direction.perpendicular() * (width / 2.0f));

            for(int j=0; j < numSegments; ++j) {
                
                currentPos = currentPos + direction * segmentLength;
                
                float sideOffset = sin( (float)j * 0.5f + (i*0.3f) ) * width * 0.3f; 
                Vector2 centerPoint = currentPos + direction.perpendicular() * sideOffset;

                
                feature.vertices.push_back(centerPoint + direction.perpendicular() * (width / 2.0f));
                feature.vertices.push_back(centerPoint - direction.perpendicular() * (width / 2.0f));
            }
            
            
            
            std::vector<Vector2> finalPolygonVertices;
            std::vector<Vector2> leftEdge, rightEdge;

            
            for(size_t k=0; k < feature.vertices.size(); ++k) {
                if(k % 2 == 0) leftEdge.push_back(feature.vertices[k]);
                else rightEdge.push_back(feature.vertices[k]);
            }

            finalPolygonVertices = leftEdge;
            
            for(int k = rightEdge.size() - 1; k >= 0; --k) {
                finalPolygonVertices.push_back(rightEdge[k]);
            }
            feature.vertices = finalPolygonVertices;

            if(feature.vertices.size() >=3) {
                 waterFeatures.push_back(feature);
            }
        }
    }

    // Desenha o mar, incluindo uma cor base e as features de água geradas.
    void drawSea(int screenWidth, int screenHeight) {
        
        CV::color(0.02f, 0.1f, 0.2f); 
        CV::rectFill(0, 0, screenWidth, screenHeight);

        
        for (const auto& feature : waterFeatures) {
            if (feature.vertices.size() < 3) continue;

            std::vector<float> vx, vy;
            vx.reserve(feature.vertices.size());
            vy.reserve(feature.vertices.size());
            for (const auto& p : feature.vertices) {
                vx.push_back(p.x);
                vy.push_back(p.y);
            }
            CV::color(feature.r, feature.g, feature.b, feature.a);
            CV::polygonFill(vx.data(), vy.data(), feature.vertices.size());
        }
    }

    // Calcula o centroide (ponto médio geométrico) de um conjunto de pontos.
    Vector2 calculateCentroid(const std::vector<Vector2>& points) {
        if (points.empty()) {
            return Vector2(0.0f, 0.0f); 
        }
        Vector2 sum(0.0f, 0.0f);
        for (const auto& p : points) {
            sum.x += p.x;
            sum.y += p.y;
        }
        return Vector2(sum.x / points.size(), sum.y / points.size());
    }

    // Desloca um conjunto de pontos para fora a partir de um centroide por um valor de offset.
    // Cada ponto é movido ao longo da linha que o conecta ao centroide.
    std::vector<Vector2> offsetPoints(const std::vector<Vector2>& originalPoints, float offsetValue, const Vector2& centroid) {
        std::vector<Vector2> newPoints;
        if (originalPoints.empty()) {
            return newPoints;
        }

        for (const auto& p : originalPoints) {
            Vector2 direction = p - centroid;
            if (direction.lengthSquared() > 0.0001f) { 
                direction.normalize();
            } else {
                
                
                
                
                direction = Vector2(1.0f, 0.0f); 
            }
            newPoints.push_back(p + direction * offsetValue);
        }
        return newPoints;
    }
    // Desenha a ilha e a praia.
    // Utiliza os pontos da curva externa da pista como base e cria camadas
    // progressivamente maiores (offset) para simular a praia e a massa de terra da ilha.
    void drawIslandAndBeach(int screenWidth, int screenHeight, const Track& track) {
        const auto& outerTrackPoints = track.getOuterCurvePoints();

        if (outerTrackPoints.size() < 3) {
            
            return;
        }

        Vector2 centroid = calculateCentroid(outerTrackPoints);

        
        
        struct IslandLayer {
            float offset;      
            float r, g, b;     
        };

        std::vector<IslandLayer> layers = {
            
            {80.0f, 0.1f, 0.35f, 0.1f},  
            
            {60.0f, 0.78f, 0.7f, 0.55f}, 
            
            {40.0f, 0.88f, 0.8f, 0.65f}, 
            
            {20.0f, 0.96f, 0.87f, 0.70f}  
        };

        for (const auto& layer : layers) {
            std::vector<Vector2> layerBoundaryPoints = offsetPoints(outerTrackPoints, layer.offset, centroid);

            if (layerBoundaryPoints.size() >= 3) {
                CV::color(layer.r, layer.g, layer.b);

                
                std::vector<float> vx, vy;
                vx.reserve(layerBoundaryPoints.size());
                vy.reserve(layerBoundaryPoints.size());
                for (const auto& p : layerBoundaryPoints) {
                    vx.push_back(p.x);
                    vy.push_back(p.y);
                }
                CV::polygonFill(vx.data(), vy.data(), layerBoundaryPoints.size());
            }
        }
    }

    // Gera os objetos decorativos (casas, palmeiras, pedras, arbustos).
    // Os objetos são posicionados aleatoriamente em uma área ao redor da pista,
    // evitando sobreposição com a própria pista e com outros objetos.
    // A quantidade de cada tipo de objeto é baseada no comprimento estimado da pista.
    void generateDecorativeObjects(int sWidth, int sHeight, const Track& track) {
        decorativeObjects.clear();
        if (track.getOuterCurvePoints().empty()) return;

        const auto& outerTrack = track.getOuterCurvePoints();
        Vector2 islandCentroid = calculateCentroid(outerTrack);

        
        
        float minOffsetFromTrack = 20.0f; 
        float maxOffsetFromTrack = 75.0f; 

        
        float trackLength = 0.0f;
        if (outerTrack.size() > 1) {
            for (size_t i = 0; i < outerTrack.size(); ++i) {
                trackLength += (outerTrack[i] - outerTrack[(i + 1) % outerTrack.size()]).length();
            }
        }
        if (trackLength < 1.0f) trackLength = 1000; 

        int numHouses = std::max(0, (int)(trackLength / 400.0f));
        int numPalmTrees = std::max(0, (int)(trackLength / 150.0f));
        int numRocks = std::max(0, (int)(trackLength / 100.0f));
        int numBushes = std::max(0, (int)(trackLength / 120.0f));

        auto placeObject = [&](ObjectType type, int count) {
            for (int i = 0; i < count; ++i) {
                int attempts = 0;
                bool placed = false;
                while (!placed && attempts < 50) {
                    attempts++;
                    if (outerTrack.empty()) break; 
                    int randomTrackPointIndex = rand() % outerTrack.size();
                    Vector2 pointOnTrackEdge = outerTrack[randomTrackPointIndex];

                    
                    
                    
                                                               

                    
                    
                    
                    Vector2 p1 = outerTrack[randomTrackPointIndex];
                    Vector2 p2 = outerTrack[(randomTrackPointIndex + 1) % outerTrack.size()];
                    Vector2 tangent = (p2 - p1).normalized();
                    Vector2 normal = tangent.perpendicular(); 

                    
                    
                    Vector2 trackCentroid = calculateCentroid(track.getInnerCurvePoints().empty() ? outerTrack : track.getInnerCurvePoints());
                    if ((pointOnTrackEdge + normal * 10.0f - trackCentroid).lengthSquared() < (pointOnTrackEdge - normal * 10.0f - trackCentroid).lengthSquared()) {
                        normal = normal * -1.0f; 
                    }

                    float randomOffset = minOffsetFromTrack + (rand() / (float)RAND_MAX) * (maxOffsetFromTrack - minOffsetFromTrack);
                    Vector2 candidatePos = pointOnTrackEdge + normal * randomOffset;

                    
                    if (track.isPointInsideTrack(candidatePos)) continue; 

                    
                    bool insideIslandArea = false;
                    std::vector<Vector2> outermostIslandBoundary = offsetPoints(outerTrack, maxOffsetFromTrack + 20.0f, islandCentroid);
                    if (!outermostIslandBoundary.empty() && isPointInPolygon(candidatePos, outermostIslandBoundary)) {
                         insideIslandArea = true;
                    }
                    if(!insideIslandArea) continue;

                    
                    bool tooCloseToOtherObject = false;
                    for (const auto& obj : decorativeObjects) {
                        if ((candidatePos - obj.position).lengthSquared() < (20.0f * 20.0f)) { 
                            tooCloseToOtherObject = true;
                            break;
                        }
                    }
                    if (tooCloseToOtherObject) continue;

                    DecorativeObject newObj;
                    newObj.type = type;
                    newObj.position = candidatePos;
                    newObj.rotation = (rand() / (float)RAND_MAX) * PI_2;

                    if (type == ObjectType::HOUSE) {
                        newObj.size_param1 = 20 + rand() % 15; 
                        newObj.size_param2 = 15 + rand() % 10; 
                        newObj.r1 = 0.6f + (rand() % 40) / 100.0f; newObj.g1 = 0.4f + (rand() % 40) / 100.0f; newObj.b1 = 0.2f + (rand() % 30) / 100.0f; 
                        newObj.r2 = 0.4f + (rand() % 30) / 100.0f; newObj.g2 = 0.2f + (rand() % 20) / 100.0f; newObj.b2 = 0.1f + (rand() % 20) / 100.0f; 
                    } else if (type == ObjectType::PALM_TREE) {
                        newObj.size_param1 = 25 + rand() % 20; 
                        newObj.size_param2 = 15 + rand() % 10; 
                        newObj.r1 = 0.4f; newObj.g1 = 0.25f; newObj.b1 = 0.1f; 
                        newObj.r2 = 0.1f; newObj.g2 = 0.5f + (rand()%30)/100.0f; newObj.b2 = 0.1f; 

                        int num_leaves_generated = 7 + rand() % 4;
                        newObj.palmLeafDetails.reserve(num_leaves_generated);
                        for (int k = 0; k < num_leaves_generated; ++k) {
                            LeafInfo leaf;
                            
                            
                            
                            leaf.angle = ((float)k / num_leaves_generated) * PI_2 + (rand() / (float)RAND_MAX - 0.5f) * (PI_2 / num_leaves_generated * 0.5f); 
                            leaf.length = newObj.size_param2 * (0.7f + (rand() / (float)RAND_MAX) * 0.5f); 
                            newObj.palmLeafDetails.push_back(leaf);
                        }
                    } else if (type == ObjectType::ROCK) {
                        newObj.size_param1 = 5 + rand() % 10; 
                        newObj.size_param2 = 0; 
                        newObj.r1 = 0.4f + (rand()%20)/100.0f; newObj.g1 = newObj.r1; newObj.b1 = newObj.r1; 

                        int numRockVertices = 4 + rand() % 4; 
                        newObj.rockVertices.reserve(numRockVertices);
                        for(int v=0; v<numRockVertices; ++v) {
                            float angle_v = (float)v / numRockVertices * PI_2 + (rand()/(float)RAND_MAX - 0.5f) * 0.5f;
                            float radius_v = newObj.size_param1 * (0.7f + (rand()/(float)RAND_MAX) * 0.6f);
                            newObj.rockVertices.push_back(Vector2(cos(angle_v)*radius_v, sin(angle_v)*radius_v));
                        }
                    } else if (type == ObjectType::BUSH) {
                        newObj.size_param1 = 8 + rand() % 8; 
                        newObj.size_param2 = 0;
                        newObj.r1 = 0.1f + (rand()%10)/100.0f; newObj.g1 = 0.3f + (rand()%20)/100.0f; newObj.b1 = 0.05f + (rand()%10)/100.0f; 

                        int num_clumps_generated = 3 + rand()%3;
                        newObj.bushClumpDetails.reserve(num_clumps_generated);
                        for (int c = 0; c < num_clumps_generated; ++c) {
                            BushClumpInfo clump;
                            clump.offset.x = (rand()/(float)RAND_MAX - 0.5f) * newObj.size_param1 * 0.5f;
                            clump.offset.y = (rand()/(float)RAND_MAX - 0.5f) * newObj.size_param1 * 0.5f;
                            clump.radius = newObj.size_param1 * (0.4f + (rand()/(float)RAND_MAX)*0.3f);
                            newObj.bushClumpDetails.push_back(clump);
                        }
                    }
                    decorativeObjects.push_back(newObj);
                    placed = true;
                }
            }
        };

        placeObject(ObjectType::HOUSE, numHouses);
        placeObject(ObjectType::PALM_TREE, numPalmTrees);
        placeObject(ObjectType::ROCK, numRocks);
        placeObject(ObjectType::BUSH, numBushes);
    }

    // Desenha todos os objetos decorativos que foram gerados.
    // Cada tipo de objeto tem sua própria lógica de desenho.
    // Utiliza transformações OpenGL (translate, rotate) para posicionar e orientar os objetos.
    void drawDecorativeObjects(int sWidth, int sHeight) {
        for (const auto& obj : decorativeObjects) {
            glPushMatrix();
            glTranslatef(obj.position.x, obj.position.y, 0);
            glRotatef(obj.rotation * 180.0f / PI, 0, 0, 1);

            if (obj.type == ObjectType::HOUSE) {
                float w = obj.size_param1;
                float h_base = obj.size_param2;
                float h_roof = h_base * 0.6f;
                
                CV::color(obj.r1, obj.g1, obj.b1);
                CV::rectFill(-w/2, -h_base/2, w/2, h_base/2);
                
                CV::color(obj.r2, obj.g2, obj.b2);
                float roof_points_x[] = {-w/2 - 2, w/2 + 2, 0};
                float roof_points_y[] = {h_base/2, h_base/2, h_base/2 + h_roof};
                CV::polygonFill(roof_points_x, roof_points_y, 3);
            } else if (obj.type == ObjectType::PALM_TREE) {
                float trunk_h = obj.size_param1;
                float trunk_w = std::max(2.0f, trunk_h / 10.0f);
                float canopy_r_ref = obj.size_param2; 

                CV::color(obj.r1, obj.g1, obj.b1);
                CV::rectFill(-trunk_w/2, -trunk_h/2, trunk_w/2, trunk_h/2); 

                CV::color(obj.r2, obj.g2, obj.b2);
                Vector2 leaf_base_point_local(0, trunk_h/2); 

                for (const auto& leaf_detail : obj.palmLeafDetails) {
                    float current_leaf_angle = leaf_detail.angle;
                    float current_leaf_length = leaf_detail.length;

                    
                    Vector2 p1 = leaf_base_point_local;
                    Vector2 p2 = leaf_base_point_local + Vector2(cos(current_leaf_angle) * current_leaf_length, sin(current_leaf_angle) * current_leaf_length);

                    
                    float fixed_angular_offset_for_sides = 0.3f;
                    float fixed_length_scale_for_sides = 0.8f;
                    float base_width_factor_for_sides = canopy_r_ref * 0.15f; 

                    Vector2 p3_main_dir(cos(current_leaf_angle + fixed_angular_offset_for_sides), sin(current_leaf_angle + fixed_angular_offset_for_sides));
                    Vector2 p3_perp_dir(-sin(current_leaf_angle + fixed_angular_offset_for_sides), cos(current_leaf_angle + fixed_angular_offset_for_sides)); 
                    Vector2 p3 = leaf_base_point_local + p3_main_dir * (current_leaf_length * fixed_length_scale_for_sides) - p3_perp_dir * base_width_factor_for_sides;

                    Vector2 p4_main_dir(cos(current_leaf_angle - fixed_angular_offset_for_sides), sin(current_leaf_angle - fixed_angular_offset_for_sides));
                    Vector2 p4_perp_dir(-sin(current_leaf_angle - fixed_angular_offset_for_sides), cos(current_leaf_angle - fixed_angular_offset_for_sides)); 
                    Vector2 p4 = leaf_base_point_local + p4_main_dir * (current_leaf_length * fixed_length_scale_for_sides) + p4_perp_dir * base_width_factor_for_sides;

                    float leaf_vx[] = {p1.x, p2.x, (p3.x + p4.x) * 0.5f}; 
                    float leaf_vy[] = {p1.y, p2.y, (p3.y + p4.y) * 0.5f};
                    CV::polygonFill(leaf_vx, leaf_vy, 3);
                }
            } else if (obj.type == ObjectType::ROCK) {
                CV::color(obj.r1, obj.g1, obj.b1);
                if (obj.rockVertices.size() >= 3) {
                    std::vector<float> vx, vy;
                    vx.reserve(obj.rockVertices.size());
                    vy.reserve(obj.rockVertices.size());
                    for(const auto& p_local : obj.rockVertices) {
                        vx.push_back(p_local.x);
                        vy.push_back(p_local.y);
                    }
                    CV::polygonFill(vx.data(), vy.data(), vx.size());
                } else { 
                    CV::circleFill(0,0, obj.size_param1, 10);
                }
            } else if (obj.type == ObjectType::BUSH) {
                CV::color(obj.r1, obj.g1, obj.b1);
                for(const auto& clump_detail : obj.bushClumpDetails) {
                    CV::circleFill(clump_detail.offset.x, clump_detail.offset.y, clump_detail.radius, 12);
                }
            }
            glPopMatrix();
        }
    }

    // Implementação do algoritmo PNPoly (Ray Casting) para verificar se um ponto está dentro de um polígono.
    // Retorna true se o ponto estiver dentro, false caso contrário.
    bool isPointInPolygon(const Vector2& testPoint, const std::vector<Vector2>& polygon) const {
        int n = polygon.size();
        if (n < 3) return false; 
        bool inside = false;
        for (int i = 0, j = n - 1; i < n; j = i++) {
            const Vector2& pi = polygon[i];
            const Vector2& pj = polygon[j];
            if (((pi.y > testPoint.y) != (pj.y > testPoint.y)) &&
                (testPoint.x < (pj.x - pi.x) * (testPoint.y - pi.y) / (std::max(0.00001f, std::fabs(pj.y - pi.y))) + pi.x)) { 
                inside = !inside;
            }
        }
        return inside;
    }
};

#endif // BACKGROUND_H_INCLUDED

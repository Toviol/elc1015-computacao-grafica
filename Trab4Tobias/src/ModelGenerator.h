/*
=============================================================================
                            MODELGENERATOR.H
=============================================================================

DESCRIÇÃO GERAL:
Este arquivo define a classe ModelGenerator, responsável por gerar modelos 3D
através da técnica de sweep rotacional e helicoidal. A partir de um perfil 2D
definido por uma curva de Bézier, o gerador cria superfícies 3D rotacionando
o perfil ao redor de um eixo vertical.

FUNCIONALIDADES:
• Sweep rotacional: Rotaciona perfil 2D para criar superfície de revolução
• Sweep helicoidal: Adiciona movimento vertical durante a rotação
• Geração automática de vértices 3D com coordenadas calculadas
• Criação de faces triangulares para renderização
• Conversão entre coordenadas de tela e unidades do modelo 3D
• Suporte a diferentes resoluções de malha

TIPOS DE SWEEP:
• Rotacional: Perfil gira 360° ao redor do eixo Y
• Helicoidal: Perfil gira e sobe simultaneamente (como uma espiral)

ESTRUTURA MODEL3D:
• vertices: Lista de pontos 3D do modelo
• triangles: Lista de triângulos (índices dos vértices)

PARÂMETROS CONFIGURÁVEIS:
• Número de fatias de rotação (resolução angular)
• Altura da hélice (pitch) para sweep helicoidal
• Número de voltas da hélice
• Escala de conversão entre pixels e unidades 3D

ALGORITMO:
1. Para cada ponto do perfil 2D, gera vértices rotacionados
2. Conecta vértices adjacentes formando quadriláteros
3. Divide quadriláteros em dois triângulos para renderização
4. Ajusta altura para movimento helicoidal (se ativado)

=============================================================================
*/

#ifndef ___MODEL_GENERATOR__H___
#define ___MODEL_GENERATOR__H___

#include "Vector2.h"
#include "Vector3.h"
#include "gl_canvas2d.h"
#include <vector>
#include <cmath>
#include <algorithm>

/**
 * Classe responsável por gerar modelos 3D através de sweep rotacional/helicoidal.
 * Converte perfis 2D em superfícies 3D complexas.
 */
class ModelGenerator {
public:
    /**
     * Estrutura que representa um modelo 3D completo
     * Contém todos os dados necessários para renderização
     */
    struct Model3D {
        std::vector<Vector3> vertices;              // Lista de pontos 3D do modelo
        std::vector<std::vector<int>> triangles;    // Lista de triângulos (índices de vértices)
    };

    /**
     * Gera modelo 3D através de sweep rotacional ou helicoidal
     * @param bezierProfile Pontos do perfil 2D (curva de Bézier)
     * @param numRotationSlices Número de fatias na rotação (resolução angular)
     * @param screenWidth Largura da tela para conversão de coordenadas
     * @param leftAreaWidth Proporção da área esquerda (editor 2D)
     * @param screenHeight Altura da tela para conversão de coordenadas
     * @param enableHelicalSweep Se true, ativa sweep helicoidal
     * @param helicalPitch Altura por volta completa da hélice
     * @param numHelicalTurns Número total de voltas da hélice
     * @return Modelo 3D gerado com vértices e triângulos
     */
    static Model3D generateSweepModel(const std::vector<Vector2>& bezierProfile, 
                                     int numRotationSlices, 
                                     int screenWidth, 
                                     float leftAreaWidth,
                                     int screenHeight,
                                     bool enableHelicalSweep = false,
                                     float helicalPitch = 50.0f,
                                     float numHelicalTurns = 1.0f) {
        
        Model3D model;
        if (bezierProfile.size() < 2 || numRotationSlices < 3) return model;

        // Conversão de coordenadas de tela para unidades do modelo 3D
        float panel_width_pixels = screenWidth * leftAreaWidth;
        float panel_height_pixels = static_cast<float>(screenHeight);
        float min_panel_dim_pixels = std::min(panel_width_pixels, panel_height_pixels);
        float profile_pixels_to_model_units_scale = (min_panel_dim_pixels > 0.001f) ? (200.0f / min_panel_dim_pixels) : 1.0f;

        int slices_per_360_turn = numRotationSlices;
        
        // Calcula número total de segmentos baseado no tipo de sweep
        int total_sweep_segments;
        if (enableHelicalSweep) {
            if (numHelicalTurns <= 0.0f) {
                total_sweep_segments = 0;
            } else {
                total_sweep_segments = static_cast<int>(std::ceil(slices_per_360_turn * numHelicalTurns));
                if (total_sweep_segments == 0 && numHelicalTurns > 0) total_sweep_segments = 1;
            }
        } else {
            total_sweep_segments = slices_per_360_turn;
        }

        if (total_sweep_segments == 0 && bezierProfile.size() >= 2) {
            return model;
        }
        
        // Determina número de fatias de vértices por ponto do perfil
        int num_vertex_slices_per_profile_pt;
        if (enableHelicalSweep) {
            num_vertex_slices_per_profile_pt = total_sweep_segments + 1;
            if (total_sweep_segments == 0) num_vertex_slices_per_profile_pt = 0;
        } else {
            num_vertex_slices_per_profile_pt = slices_per_360_turn;
        }

        if (num_vertex_slices_per_profile_pt < (enableHelicalSweep ? 2 : 1) && total_sweep_segments > 0) {
            return model; 
        }
        if (num_vertex_slices_per_profile_pt == 0) return model;

        // Gera vértices do modelo
        for (int i = 0; i < (int)bezierProfile.size(); ++i) {
            float currentRadius = std::abs(bezierProfile[i].x * profile_pixels_to_model_units_scale);
            float baseProfileHeight = bezierProfile[i].y * profile_pixels_to_model_units_scale;

            for (int j = 0; j < num_vertex_slices_per_profile_pt; ++j) {
                float angle_fraction_of_turn = static_cast<float>(j % slices_per_360_turn) / slices_per_360_turn;
                float angle = PI * 2.0f * angle_fraction_of_turn;
                float currentHeight = baseProfileHeight;

                // Adiciona movimento helicoidal se ativado
                if (enableHelicalSweep) {
                    float turns_completed_at_vertex_j = static_cast<float>(j) / slices_per_360_turn;
                    currentHeight += turns_completed_at_vertex_j * helicalPitch;
                }
                
                // Cria vértice nas coordenadas cilíndricas convertidas para cartesianas
                model.vertices.emplace_back(
                    currentRadius * cos(angle),
                    currentHeight,
                    currentRadius * sin(angle)
                );
            }
        }

        // Gera faces triangulares conectando vértices adjacentes
        int num_profile_points = bezierProfile.size();
        if (num_profile_points < 2) return model;

        int num_quads_to_form = total_sweep_segments;
        if (num_quads_to_form == 0) return model;

        // Conecta pontos adjacentes formando quadriláteros (divididos em triângulos)
        for (int i = 0; i < num_profile_points - 1; ++i) {
            for (int j = 0; j < num_quads_to_form; ++j) {
                int slice_vtx_idx_curr = j;
                int slice_vtx_idx_next;

                if (enableHelicalSweep) {
                    slice_vtx_idx_next = j + 1; 
                } else {
                    slice_vtx_idx_next = (j + 1) % slices_per_360_turn; 
                }

                // Índices dos 4 vértices do quadrilátero atual
                int idx0 = i       * num_vertex_slices_per_profile_pt + slice_vtx_idx_curr;
                int idx1 = i       * num_vertex_slices_per_profile_pt + slice_vtx_idx_next;
                int idx2 = (i + 1) * num_vertex_slices_per_profile_pt + slice_vtx_idx_curr;
                int idx3 = (i + 1) * num_vertex_slices_per_profile_pt + slice_vtx_idx_next;
                
                // Divide quadrilátero em dois triângulos
                model.triangles.push_back({idx0, idx2, idx1});
                model.triangles.push_back({idx1, idx2, idx3});
            }
        }

        return model;
    }
};

#endif // ___MODEL_GENERATOR__H___
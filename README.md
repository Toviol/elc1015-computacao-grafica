# Trabalhos de Computação Gráfica

Este repositório contém os trabalhos práticos desenvolvidos para a disciplina de **Computação Gráfica** ministrada pelo professor **Cesar Tadeu Pozzer** na **UFSM - Universidade Federal de Santa Maria**.

## 📋 Disciplina

- **Disciplina:** Computação Gráfica
- **Professor:** Cesar Tadeu Pozzer
- **Universidade:** UFSM - Universidade Federal de Santa Maria
- **Aluno:** Tobias Viero de Oliveira

## 🎯 Trabalhos Implementados

### Trabalho 1: Editor de Imagens 2D
**Diretório:** `Trab1Tobias/`

Um editor de imagens completo implementado em C++ utilizando OpenGL e Canvas2D.

**Funcionalidades principais:**
- ✅ Carregamento de imagens BMP 24bits sem compactação
- ✅ Sistema de camadas com transparência binária
- ✅ Menu lateral funcional com botões, checkbox e slider
- ✅ Desenho com mouse sobre a camada ativa
- ✅ Identificação visual da camada ativa
- ✅ Seleção de cores para desenho
- ✅ Plano de fundo quadriculado para visualizar transparência
- ✅ Reposicionamento e ocultação de camadas
- ✅ Ferramentas de pincel com diferentes tamanhos
- ✅ Operações de flip horizontal e vertical
- ✅ Interface intuitiva e responsiva

### Trabalho 3: Grand Theft Auto 2D
**Diretório:** `Trab3Tobias/`

Um jogo 2D inspirado no clássico Grand Theft Auto, implementado com física e gráficos 2D.

**Funcionalidades básicas:**
- ✅ Pista criada com curvas B-spline em loop fechado
- ✅ Editor de pista interativo (adicionar/remover pontos de controle)
- ✅ Sistema de colisão do tanque com bordas da pista
- ✅ Movimento contínuo com rotação usando teclas A e D
- ✅ Rotação do topo do tanque seguindo o cursor do mouse
- ✅ Sistema de disparo de projéteis com controle de FPS
- ✅ Colisão de projéteis com alvos e bordas
- ✅ Sistema de vida para alvos e tanque com barras visuais
- ✅ Sistema de pontuação por destruição de alvos
- ✅ Menu de início do jogo

**Extras implementados:**
- 🟢 Efeitos visuais de explosão realistas
- 🟢 Power-ups: Nitro Boost, Rapid Fire, Shield, Super Burst
- 🟢 Sistema de múltiplos níveis com dificuldade progressiva
- 🟢 Scoreboard com ranking de pontuações
- 🟢 Diferentes tipos de inimigos com comportamentos únicos
- 🟢 Sistema de rastro do tanque na pista
- 🟢 Efeitos sonoros e visuais aprimorados

### Trabalho 4: Modelador 3D com Bezier Sweep
**Diretório:** `Trab4Tobias/`

Um modelador 3D avançado que permite criar modelos através de sweep rotacional de curvas de Bézier.

**Funcionalidades implementadas:**
- ✅ Editor de perfil 2D com curvas de Bézier
- ✅ Geração de modelo 3D por sweep rotacional
- ✅ Sweep helicoidal (helical sweep)
- ✅ Visualização wireframe e sólida com Z-buffer
- ✅ Iluminação com modelo Phong (ambiente, difusa, especular)
- ✅ Cálculo e visualização de normais das faces
- ✅ Back-face culling
- ✅ Projeção perspectiva e ortográfica
- ✅ Controle interativo de câmera (órbita e zoom)
- ✅ Transformações 3D do modelo (translação e rotação)
- ✅ Controle interativo da fonte de luz
- ✅ Interface gráfica com informações em tempo real
- ✅ Rasterização de triângulos com interpolação de cores
- ✅ Sistema de coordenadas 3D com eixos de referência

**Controles:**
- **Editor de Perfil:** Mouse esquerdo para mover pontos, direito para adicionar, DEL para remover
- **Câmera:** Setas para orbitar, Page Up/Down para zoom, P para alternar projeção
- **Modelo:** M para alternar modo, WASD+QE para translação, JLIKUO para rotação
- **Parâmetros:** -/= para amostras de Bézier, [/] para fatias de rotação

### Trabalho 5: Sistema Solar 3D
**Diretório:** `Trab5Tobias/`

Uma simulação interativa e realista do Sistema Solar em 3D com texturas, iluminação e física orbital.

**Características:**
- 🌟 Simulação completa do Sistema Solar com todos os planetas
- 🪐 Texturas realistas de alta qualidade (até 8K) para planetas e sol
- 🌍 Movimento orbital e rotacional preciso dos planetas
- 💫 Campo de asteroides animado
- 🧭 Sistema de bússola para navegação 3D
- 🎵 Trilha sonora ambiente
- 📸 Controle de câmera livre em 3D
- ⚡ Otimizações de performance para renderização suave
- 🌙 Detalhes como anéis de Saturno e satélites naturais

## 🛠️ Tecnologias Utilizadas

- **Linguagem:** C++11
- **Gráficos:** OpenGL com FreeGLUT
- **Canvas2D:** Biblioteca customizada para gráficos 2D
- **IDE:** Code::Blocks
- **Formato de Imagens:** BMP 24-bit
- **Áudio:** Suporte a WAV (Trabalho 5)


## 🚀 Como Executar os Trabalhos

**Apenas o Code::Blocks IDE é necessário!** Todas as bibliotecas e dependências já estão incluídas no repositório.

1. **Baixar e Instalar Code::Blocks:**
   - Baixe: `codeblocks-25.03mingw-setup.exe`
   - Execute a instalação completa
   - **Importante:** Selecione **GNU GCC** como compilador durante a configuração

### Passos para Compilação e Execução

1. **Abrir o Projeto:**
   ```
   - Navegue até o diretório do trabalho desejado
   - Abra o arquivo .cbp no Code::Blocks
   ```

2. **Compilar e Executar:**
   ```
   - Build → Rebuild (para recompilar completamente)
   - Build → Build and Run (F9)
   ```

**Pronto!** O projeto deve compilar e executar automaticamente.

**Nota:** Todas as bibliotecas necessárias (FreeGLUT, OpenGL32, GLU32) já estão incluídas no repositório e configuradas nos projetos.

## 📚 Documentação Adicional

- `Manual Canvas2D.pdf` - Manual completo da biblioteca Canvas2D
- `descricaoT*.pdf` - Especificações originais de cada trabalho
- Comentários no código fonte de cada projeto

## 🎓 Objetivos de Aprendizado

Este conjunto de trabalhos aborda conceitos fundamentais de Computação Gráfica:

- **Fundamentos 2D:** Rasterização, transformações, sistemas de coordenadas
- **Processamento de Imagens:** Manipulação de pixels, camadas, transparência
- **Gráficos 3D:** Projeções, iluminação, modelos de sombreamento
- **Geometria Computacional:** Curvas de Bézier, superfícies, normais
- **Física e Simulação:** Movimento orbital, colisões, dinâmica de objetos
- **Interface e Interação:** Controles de mouse/teclado, menus, feedback visual

---

**Desenvolvido com 💻 e ☕ para a disciplina de Computação Gráfica - UFSM**

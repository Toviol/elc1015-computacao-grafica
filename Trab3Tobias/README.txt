Aluno: Tobias Viero de Oliveira  
Disciplina: Computação Gráfica  
Trabalho 3 – Grand Theft Auto 2D  

Resumo da Implementação:
--------------------------

Todos os requisitos **básicos e extras** foram implementados **completamente e com maestria**. O jogo foi desenvolvido utilizando C++ e a API Canvas2D, conforme especificado, e testado com sucesso em ambiente Windows.

Funcionalidades Básicas Implementadas:
---------------------------------------

✔️ Pista criada com curvas B-spline, com loop fechado e largura consistente  
✔️ Editor de pista: o usuário pode **adicionar ou remover pontos de controle** (sempre em pares)  
✔️ Colisão do tanque com as bordas da pista  
✔️ Movimento contínuo para frente com rotação usando as teclas **A** e **D**  
✔️ Rotação do topo do tanque em direção ao cursor do mouse  
✔️ Disparo de projétil a partir do cano do tanque com controle de FPS e delay entre tiros  
✔️ Colisão dos projéteis com alvos e bordas  
✔️ Alvos com barra de vida visível  
✔️ Tanque perde vida ao colidir com alvos ou bordas da pista  
✔️ Barra de vida do tanque fixa na tela  
✔️ Sistema de pontuação por destruição de alvos  
✔️ Controle de FPS e jogabilidade fluida  
✔️ Menu de início do jogo

Funcionalidades Extras Implementadas:
--------------------------------------

🟢 **Efeitos visuais de explosão** realistas e impactantes  
🟢 Diferentes tipos de projéteis  
🟢 Inimigos com comportamento autônomo: **se movimentam, atiram bombas** e possuem barras de vida  
🟢 Implementada uma **shuriken maluca**, com movimento errático e visual único  
🟢 Estilhaços e efeitos secundários após explosões  
🟢 **Power-ups** ativados por teclado:  
  - **Shield (Escudo):** Tecla `'s'`  
  - **Sequência de 3 tiros (Burst):** Tecla `'q'`  
  - **Sequência de 5 tiros:** Tecla `'e'`  
  - **Nitro (aumento de velocidade):** Tecla `'w'`  
🟢 **Background visual aprimorado:**  
  - Geração de **água pseudoaleatória** e **faixa de areia**  
  - Elementos como **rochas, árvores e casas** são posicionados de forma randômica ao longo da borda da pista com base nas B-splines

Instruções de Uso:
-------------------

- Execute o projeto pelo Code::Blocks (MinGW 32 bits), garantindo o controle de FPS.
- Use `A` e `D` para rotacionar o tanque.
- Use o **mouse para mirar** (topo do tanque acompanha o cursor).
- Clique com o botão esquerdo do mouse para atirar.
- Ative poderes com as teclas:
  - `'s'` = Shield  
  - `'q'` = Burst de 3 tiros  
  - `'e'` = Burst de 5 tiros  
  - `'w'` = Nitro  

- No **modo editor**, adicione ou remova pontos da pista sempre em pares.
- Para voltar ao formato padrão da pista, utilize a opção disponível no menu do editor.

Observações Finais:
--------------------

- O projeto foi depurado e **todos os arquivos e funções não utilizados foram removidos**.
- A estrutura do projeto segue exatamente o padrão exigido:  
 `/Trab3Tobias`  
  ├── `src/` (códigos-fonte)  
  └── `trab3.cbp` (projeto Code::Blocks)
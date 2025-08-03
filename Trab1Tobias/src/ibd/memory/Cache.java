/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package ibd.memory;


/**
 * Implementação de uma estrutura LRU para IDs numéricos.
 */
public interface Cache {

    

    /**
     * Obtém um ID do cache, movendo-o para o final da fila se estiver presente.
     * @param id O ID a ser acessado.
     * @return O ID caso esteja no cache, ou -1 caso contrário.
     */
    public int get(int id, boolean putIfMiss);

    /**
     * Insere um ID no cache, removendo o menos recentemente usado se necessário.
     * @param id O ID a ser inserido.
     */
    public int put(int id);
    
    public boolean remove(int id);
    
    public int missCount();
    
    public boolean full();

    /**
     * Exibe o estado atual do cache.
     */
    public void printCache();

    
}

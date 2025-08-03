/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package ibd.memory;

/**
 *
 * @author ferna
 */
import java.util.*;

/**
 * Implementação de uma estrutura LRU para IDs numéricos.
 */
public class LRUCache implements Cache{
    private final int capacity;
    private final Map<Integer, Integer> cache;
    private final Deque<Integer> accessOrder;
    
    private int missCount = 0;

    /**
     * Construtor para definir a capacidade do cache.
     * @param capacity Número máximo de IDs armazenados.
     */
    public LRUCache(int capacity) {
        this.capacity = capacity;
        this.cache = new HashMap<>();
        this.accessOrder = new LinkedList<>();
    }
    
    public boolean full(){
        return cache.size() >= capacity;
    }

    /**
     * Obtém um ID do cache, movendo-o para o final da fila se estiver presente.
     * @param id O ID a ser acessado.
     * @return O ID caso esteja no cache, ou -1 caso contrário.
     */
    public int get(int id, boolean putIfMiss) {
        if (!cache.containsKey(id)) {
            missCount++;
            if (putIfMiss){
                return put(id);
            }
            return -1;
        }
        
        if (putIfMiss){
            put(id);
        }
        // Atualiza a ordem de acesso
        
        
        return cache.get(id);
    }
    
    /**
     * Insere um ID no cache, removendo o menos recentemente usado se necessário.
     * @param id O ID a ser inserido.
     */
    public int put(int id) {
        int removePage = -1;
        if (cache.containsKey(id)) {
            accessOrder.remove(id);
        } else if (cache.size() >= capacity) {
            // Remove o LRU (primeiro da fila)
            removePage = accessOrder.pollFirst();
            cache.remove(removePage);
        }
        // Adiciona o ID ao cache e atualiza a ordem de acesso
        cache.put(id, id);
        accessOrder.addLast(id);
        return removePage;
    }
    
    public boolean remove(int id) {
        boolean exists = accessOrder.remove(id);
        cache.remove(id);
        return exists;
    }

    public int missCount(){
        return missCount;
    }
    
    /**
     * Exibe o estado atual do cache.
     */
    public void printCache() {
        System.out.println("Cache: " + accessOrder);
    }

    
}

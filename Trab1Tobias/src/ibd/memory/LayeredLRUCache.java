/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package ibd.memory;

/**
 *
 * @author ferna
 */

/**
 * Implementação de uma estrutura LRU para IDs numéricos.
 */
public class LayeredLRUCache implements Cache {

    Cache warmCache;
    Cache hotCache;
    int missCount = 0;

    /**
     * Construtor para definir a capacidade do cache.
     */
    public LayeredLRUCache(Cache warmCache, Cache hotCache) {
        this.warmCache = warmCache;
        this.hotCache = hotCache;
    }

    /**
     * Obtém um ID do cache, movendo-o para o final da fila se estiver presente.
     *
     * @param id O ID a ser acessado.
     * @return O ID caso esteja no cache, ou -1 caso contrário.
     */
    public int get(int id, boolean putIfMiss) {

        int page = hotCache.get(id, false);
        if (page != -1) {
            if (putIfMiss) {
                hotCache.put(id);
            }
            return page;

        }

        page = warmCache.get(id, false);
        if (page != -1) {
            if (putIfMiss) {
                upgrade(id);
            }
            return page;
        }

        missCount++;

        if (putIfMiss) {
            if (warmCache.full() && !hotCache.full()) {
                hotCache.put(id);
            } else {
                warmCache.put(id);
            }
            return id;
        }
        return -1;
    }

    private int upgrade(int id) {
        warmCache.remove(id);
        int removedPage = hotCache.put(id);
        if (removedPage != -1) {
            warmCache.put(removedPage);
        }
        return removedPage;
    }

    /**
     * Insere um ID no cache, removendo o menos recentemente usado se
     * necessário.
     *
     * @param id O ID a ser inserido.
     */
    public int put(int id) {
        int page = hotCache.get(id, false);
        if (page != -1) {
            hotCache.put(id);
            warmCache.put(id);
            return -1;

        }

        page = warmCache.get(id, false);
        if (page != -1) {
            return upgrade(id);
        }

        if (warmCache.full() && !hotCache.full()) {
            hotCache.put(id);
            return -1;
        } else {
            return warmCache.put(id);
        }
    }

    public boolean remove(int id) {
        boolean removed = hotCache.remove(id);
        if (!removed) {
            removed = warmCache.remove(id);
        }

        return removed;
    }

    /**
     * Exibe o estado atual do cache.
     */
    public void printCache() {
        //System.out.println("Cache 1: ");
        warmCache.printCache();
        //System.out.println("Cache 2: ");
        hotCache.printCache();
    }

    @Override
    public int missCount() {
        return missCount;
    }

    @Override
    public boolean full() {
        boolean full = hotCache.full();
        if (!full) {
            return false;
        }
        return warmCache.full();
    }

}

/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package ibd.memory;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

/**
 *
 * @author ferna
 */
public class Main {

    public void test1() {
        LRUCache cache = new LRUCache(3);
        cache.put(1);
        cache.put(2);
        cache.put(3);
        cache.printCache(); // Esperado: [1, 2, 3]

        cache.get(2, true); // Acessa 2, movendo-o para o final
        cache.printCache(); // Esperado: [1, 3, 2]

        cache.put(4); // Insere 4, removendo 1 (o LRU)
        cache.printCache(); // Esperado: [3, 2, 4]
    }

    public static void main(String[] args) {
        
        //double FREQUENT_READS_PROBABILITY = 0.9;
        double HOT_LIST_SIZE = 0.5;
        
        for (double FREQUENT_READS_PROBABILITY = 0.0; FREQUENT_READS_PROBABILITY<=1 ; FREQUENT_READS_PROBABILITY+=0.1) {
            List<Integer> frequentPages = new ArrayList();
            for (int i = 0; i < 4; i++) {
                frequentPages.add(i);
            }

            Random r = new Random();
            int bufferSize = 10;
            int hotListSize = (int)(HOT_LIST_SIZE * bufferSize);
            if (hotListSize==0) hotListSize = 1;
            if (hotListSize==bufferSize) hotListSize = bufferSize-1;
            int warmListSize = bufferSize - hotListSize;

            Cache warmCache = new LRUCache(warmListSize);
            Cache hotCache = new LRUCache(hotListSize);

            Cache lruCache = new LRUCache(bufferSize);
            Cache layeredCache = new LayeredLRUCache(warmCache, hotCache);


            int pageCount = 0;
            while (pageCount < 1000) {
                boolean frequent = (r.nextDouble() > (1-FREQUENT_READS_PROBABILITY));
                if (frequent) {
                    int freqPageId = r.nextInt(6);
                    lruCache.get(freqPageId, true);
                    layeredCache.get(freqPageId, true);
                    pageCount++;
                    //System.out.println("page:"+freqPageId);
                    //layeredCache.printCache();
                    //System.out.println("miss count "+layeredCache.missCount());
                } else {
                    int randomPageId = r.nextInt(1000);
                    //System.out.println("sequential read");
                    for (int i = 0; i < bufferSize; i++) {
                        lruCache.get(randomPageId + i, true);
                        layeredCache.get(randomPageId + i, true);
                        pageCount++;
                        //System.out.println("page:"+(randomPageId + i));
                        //layeredCache.printCache();
                        //System.out.println("miss count "+layeredCache.missCount());
                    }
                }
            }
            System.out.printf("\n FREQUENT_READS_PROBABILITY: %.1f\n", FREQUENT_READS_PROBABILITY);
            System.out.println("LRU Cache Number of Page Misses "+lruCache.missCount());
            System.out.println("Layered LRU Cache Number of Page Misses "+layeredCache.missCount());
        }



    }
}

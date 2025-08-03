/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package ibd.table;

/**
 *
 * @author ferna
 */
import java.util.*;

public class IDGenerator {
    
    List<Integer> sortedIds;
    List<Integer> shuffledIds;
    
    public void generate(int lowerValue, int largerValue, int sortedListSize, int shuffledListSize) {

        if (largerValue-lowerValue<(sortedListSize+shuffledListSize))
            throw new RuntimeException("tamanho incompatível"); 
        
        // Conjunto para armazenar IDs únicos
        Set<Integer> uniqueIds = new LinkedHashSet<>();
        Random random = new Random();

        // Gerar IDs exclusivos para a primeira lista (ordenada)
        while (uniqueIds.size() < sortedListSize) {
            uniqueIds.add(lowerValue+random.nextInt(largerValue) + 1);
        }
        sortedIds = new ArrayList<>(uniqueIds);
        Collections.sort(sortedIds); // Ordena a primeira lista

        // Gerar IDs exclusivos para a segunda lista (fora de ordem), sem repetir os da primeira lista
        uniqueIds.clear(); // Limpa o conjunto para reutilizar
        while (uniqueIds.size() < shuffledListSize) {
            int id = lowerValue+random.nextInt(largerValue) + 1;
            if (!sortedIds.contains(id)) { // Garante que não há sobreposição
                uniqueIds.add(id);
            }
        }
        shuffledIds = new ArrayList<>(uniqueIds);
        Collections.shuffle(shuffledIds, random); // Embaralha a segunda lista

    }
    
    public List<Integer> getSortedList(){
        return sortedIds;
    }
    
    public List<Integer> getShuffledList(){
        return shuffledIds;
    }
    
    public static void main(String[] args) {
        IDGenerator gen = new IDGenerator();
        gen.generate(1,200, 10, 5);
    }
}


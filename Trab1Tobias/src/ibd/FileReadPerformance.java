/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package ibd;

import java.io.*;
import java.util.Random;
import java.util.logging.Level;
import java.util.logging.Logger;

public class FileReadPerformance {
    private static final String FILE_NAME = "testfile.dat";
    private static final int FILE_SIZE_MB = 100; // Tamanho do arquivo em MB
    private static final int KB = 1024;
    private static final int MB = KB * 1024;

    public static void main(String[] args) throws IOException {
        createTestFile(FILE_NAME, FILE_SIZE_MB * MB);
        

        measureReadTime("Leitura em blocos de 4 KB", () -> {
            try {
                readInBlocks(FILE_NAME, 4 * KB);
            } catch (IOException ex) {
                Logger.getLogger(FileReadPerformance.class.getName()).log(Level.SEVERE, null, ex);
            }
        });
        
        measureReadTime("Leitura em blocos de 8 KB", () -> {
            try {
                readInBlocks(FILE_NAME, 8 * KB);
            } catch (IOException ex) {
                Logger.getLogger(FileReadPerformance.class.getName()).log(Level.SEVERE, null, ex);
            }
        });
        
     
        
    }

    private static void createTestFile(String fileName, int size) throws IOException {
        Random random = new Random();
        byte[] buffer = new byte[MB];
        try (FileOutputStream fos = new FileOutputStream(fileName)) {
            for (int i = 0; i < size / MB; i++) {
                random.nextBytes(buffer);
                fos.write(buffer);
            }
        }
    }

    private static void readByteByByte(String fileName) throws IOException {
        try (FileInputStream fis = new FileInputStream(fileName)) {
            while (fis.read() != -1) {}
        }
    }

    private static void readInBlocks(String fileName, int blockSize) throws IOException {
        try (FileInputStream fis = new FileInputStream(fileName)) {
            byte[] buffer = new byte[blockSize];
            while (fis.read(buffer) != -1) {}
        }
    }

    private static void readWithBufferedStream(String fileName, int bufferSize) throws IOException {
        try (BufferedInputStream bis = new BufferedInputStream(new FileInputStream(fileName), bufferSize)) {
            while (bis.read() != -1) {}
        }
    }

    private static void measureReadTime(String method, Runnable readMethod) {
        long startTime = System.nanoTime();
        readMethod.run();
        long endTime = System.nanoTime();
        System.out.printf("%s: %.2f ms%n", method, (endTime - startTime) / 1e6);
    }
}


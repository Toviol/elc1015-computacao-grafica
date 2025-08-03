/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package ibd.table;

/**
 *
 * @author ferna
 */
public abstract class Record {

    int id = -1;

    public Record() {
    }
    
    public Record(int id) {
        this.id = id;
    }

    public void setId(int id){
        this.id = id;
    }
    
    // Converte o objeto Record para um array de bytes
    public abstract byte[] toBytes(int textSize);

    // Cria um objeto Record a partir de um array de bytes
    public abstract void fillBytes(byte[] data);
    
    public abstract void update(Record rec);
    
    public abstract String getContent();

    // Converte um inteiro para um array de bytes
    public static byte[] intToBytes(int value) {
        return new byte[]{
            (byte) (value >> 24),
            (byte) (value >> 16),
            (byte) (value >> 8),
            (byte) value
        };
    }

    // Converte um array de bytes para um inteiro
    public static int bytesToInt(byte[] bytes) {
        return ((bytes[0] & 0xFF) << 24)
                | ((bytes[1] & 0xFF) << 16)
                | ((bytes[2] & 0xFF) << 8)
                | (bytes[3] & 0xFF);
    }
}
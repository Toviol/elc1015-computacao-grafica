/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package ibd.table;

import java.util.Arrays;

/**
 *
 * @author ferna
 */
public class IntRecord extends Record{

    Integer value;

    public IntRecord() {
        super();
    }
    
    public IntRecord(int id) {
        super(id);
    }
    
    public IntRecord(int id, int value) {
        this(id);
        this.value = value;
    }
    
    public int getValue(){
        return value;
    }
    
    // Converte o objeto Record para um array de bytes
    @Override
    public byte[] toBytes(int intSize) {
        byte[] data = new byte[4 + intSize];
        System.arraycopy(intToBytes(id), 0, data, 0, 4);
        byte[] intBytes = intToBytes(value);
        System.arraycopy(intBytes, 0, data, 4, Math.min(intBytes.length, intSize));
        return data;
    }

    // Cria um objeto Record a partir de um array de bytes
    @Override
    public void fillBytes(byte[] data) {
        int id_ = bytesToInt(Arrays.copyOfRange(data, 0, 4));
        if (id_ == 0) {
            return;
        }
        id = id_;
        value = bytesToInt(Arrays.copyOfRange(data, 4, 8));
    }
    
    @Override
    public void update(Record rec){
        IntRecord intRecord = (IntRecord)rec;
        this.value = intRecord.value;
    }

    @Override
    public String getContent(){
        return value.toString();
    }
    
}
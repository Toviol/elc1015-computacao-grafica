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
public class StringRecord extends Record{

    String text;

    public StringRecord() {
        super();
    }
    
    public StringRecord(int id) {
        super(id);
    }
    
    public StringRecord(int id, String text) {
        this(id);
        this.text = text;
    }
    
    // Converte o objeto Record para um array de bytes
    public byte[] toBytes(int textSize) {
        byte[] data = new byte[4 + textSize];
        System.arraycopy(intToBytes(id), 0, data, 0, 4);
        byte[] textBytes = text.getBytes();
        System.arraycopy(textBytes, 0, data, 4, Math.min(textBytes.length, textSize));
        return data;
    }

    // Cria um objeto Record a partir de um array de bytes
    public void fillBytes(byte[] data) {
        int id_ = bytesToInt(Arrays.copyOfRange(data, 0, 4));
        if (id_ == 0) {
            return;
        }
        id = id_;
        text = new String(Arrays.copyOfRange(data, 4, data.length)).trim();
    }

    @Override
    public void update(Record rec){
        StringRecord stringRecord = (StringRecord)rec;
        this.text = stringRecord.text;
    }
    
    public String getContent(){
        return text;
    }
    
}
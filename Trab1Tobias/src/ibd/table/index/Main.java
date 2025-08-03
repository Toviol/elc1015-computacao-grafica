/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package ibd.table.index;

import ibd.table.IDGenerator;
import ibd.table.RecordManager;
import static ibd.table.RecordManager.BLOCKS_WRITE;
import static ibd.table.RecordManager.BLOCKS_READ;
import ibd.table.StringRecord;
import ibd.table.Table;
import java.io.IOException;
import java.util.List;

/**
 *
 * @author ferna
 */
public class Main {
    public static void main(String[] args) throws IOException {
        String fileName = "c:\\teste\\records.dat";
        Table table = new IndexedTable(fileName, RecordManager.STRING_TYPE);

        table.createTable(256, 32, 0.5);
        
        table.openTable();
//
//
//
        IDGenerator gen = new IDGenerator();
        gen.generate(0, 10000, 1000, 0);
        List<Integer> ids = gen.getSortedList();
        int lastIdWrote = -1;
        for (int i = 0; i < ids.size(); i++) {
            int id = ids.get(i);
            System.out.println("adding " + id);
            table.insertRecord(new StringRecord(id, "rec:" + id));
            lastIdWrote = id;
        }
//
        
        System.out.println("");

        
        table.openTable();
        BLOCKS_READ = 0;
        System.out.println("searching for the last id:"+lastIdWrote);
        ibd.table.Record rec = table.readRecord(lastIdWrote);
        if (rec!=null){
            System.out.println(rec.getContent());
        }
        else System.out.println("not found");
        
        System.out.println("");
        System.out.println("blocks read: " + BLOCKS_READ);
        table.closeTable();

    }
}

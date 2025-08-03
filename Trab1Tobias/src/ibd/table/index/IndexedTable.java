/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package ibd.table.index;

import ibd.table.Table;


public class IndexedTable extends Table{

    public IndexedTable(String fileName, int dataType) {
        super(fileName, dataType);
        manager = new IndexedRecordManager(fileName, dataType);
    }

    
}

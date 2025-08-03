/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package ibd.table;

import java.io.IOException;
import java.util.List;



public class Table {

    protected RecordManager manager;

    public Table(String fileName, int dataType) {
        manager = new RecordManager(fileName, dataType);
    }

    // Cria um novo arquivo de tabela com tamanho e estrutura especificados
    public void createTable(int blockSize, int recordSize, double splitFactor) throws IOException {

        manager.createTable(blockSize, recordSize, splitFactor);

    }
    
    // Abre um arquivo de tabela existente
    public void openTable() throws IOException {
        manager.openTable();
    }

    // Fecha o arquivo de tabela
    public void closeTable() throws IOException {
        manager.closeTable();
    }


    // Insere um novo registro na tabela
    public void insertRecord(Record record) throws IOException {
        manager.insertRecord(record);
    }

    

    // lê o registro que possui um id específico
    public Record readRecord(int id) throws IOException {
        return manager.readRecord(id);
    }

    // lê todos os registros
    public List<Record> readAllRecords() throws IOException {
        return manager.readAllRecords();
    }

    
}

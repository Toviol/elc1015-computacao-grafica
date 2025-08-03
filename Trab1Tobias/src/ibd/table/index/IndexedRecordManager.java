/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package ibd.table.index;

import ibd.table.Block;
import ibd.table.IntRecord;
import ibd.table.Record;
import ibd.table.RecordManager;
import java.io.*;
import java.util.*;

public class IndexedRecordManager extends RecordManager {

    RecordManager indexManager;
    HashMap<Integer, Integer> blocksMap = new HashMap();

    public IndexedRecordManager(String fileName, int dataType) {
        super(fileName, dataType);
        indexManager = new RecordManager(getIndexFilePath(fileName), INT_TYPE);
    }

    private String getIndexFilePath(String fileName) {
        String suffix = "_idx";

        // Convert to File object
        File file = new File(fileName);

        // Get directory path (if any)
        String parentDir = file.getParent();

        // Get file name and extension
        String name = file.getName();
        int dotIndex = name.lastIndexOf('.');

        String newFileName;
        if (dotIndex != -1) {
            // Insert suffix before extension
            newFileName = name.substring(0, dotIndex) + suffix + name.substring(dotIndex);
        } else {
            // No extension, just append suffix
            newFileName = name + suffix;
        }

        // Construct new file path
        String newFilePath = (parentDir != null) ? parentDir + File.separator + newFileName : newFileName;
        return newFilePath;
    }

    /**
     * Creates a new table with the specified parameters. This method sets up
     * the table with block size, record size, and split factor. It also calls
     * the {@link RecordManager#createTable(int, int, double)} method from the
     * parent class and creates the index table using the {@code indexManager}
     * instance.
     *
     * <p>
     * The specified block size is used to configure the storage of the table,
     * while the record size and split factor control how records are organized
     * and distributed across the blocks.</p>
     *
     * @param blockSize The block size of the table to be created.
     * @param recordSize The size of each record in the table.
     * @param splitFactor The split factor used to control how records are
     * distributed.
     * @throws IOException If an error occurs while creating the table or
     * setting up the index.
     */
    @Override
    public void createTable(int blockSize, int recordSize, double splitFactor) throws IOException {

        super.createTable(blockSize, recordSize, splitFactor);

        indexManager.createTable(blockSize, 8, splitFactor);

    }

    /**
     * Opens the table for access. This method calls the
     * {@link RecordManager#openTable()} method from the parent class to open
     * the base table and also opens the index table by invoking the
     * {@code indexManager.openTable()} method.
     *
     * <p>
     * It ensures that both the main table and the associated index are ready
     * for data operations.</p>
     *
     * @throws IOException If an error occurs while opening the table or the
     * index.
     */
    public void openTable() throws IOException {
        super.openTable();
        indexManager.openTable();
    }

    /**
     * Closes the table and releases the associated resources. This method calls
     * the {@link RecordManager#closeTable()} method from the parent class to
     * close the main table and also closes the index table by invoking
     * {@code indexManager.closeTable()}.
     *
     * <p>
     * It ensures that both the main table and the index are properly closed and
     * resources are released.</p>
     *
     * @throws IOException If an error occurs while closing the table or the
     * index.
     */
    public void closeTable() throws IOException {
        super.closeTable();
        indexManager.closeTable();

    }

    /**
     * Writes a block to the record file and updates the index file if
     * necessary. This method ensures that whenever a block from the record file
     * is updated, the corresponding index file is updated accordingly with the
     * smallest record ID from the block.
     * <p>
     * If the block is new, it inserts the block's smallest record ID into the
     * index. If the block's smallest record ID differs from the previously
     * indexed ID, it updates the index with the new ID.
     * </p>
     *
     * @param block The block to be written to the record file and updated in
     * the index.
     * @throws IOException If an error occurs while writing the block or
     * updating the index.
     */
    protected void writeBlock(Block block) throws IOException {

        super.writeBlock(block);

        //when a block from the record file is updated, we need to update the index file accordingly
        //the smaller id from the block needs to be indexed
        Integer newRecId = block.findMinValue();
        IntRecord rec = new IntRecord(newRecId, block.id);

        //this is the index entry currently associated with the block
        Integer oldRecId = blocksMap.get(block.id);

        //if the block from the record file is new
        if (oldRecId == null) {
            indexManager.insertRecord(rec);
            blocksMap.put(block.id, newRecId);
            return;
        }

        //if the index entry does not need to change
        if (oldRecId.equals(newRecId)) {
            return;
        }

        //the block smallest id is smaller than the index entry
        indexManager.remove(oldRecId);
        indexManager.insertRecord(rec);
        blocksMap.put(block.id, newRecId);

    }

    /**
     * Reads a record from the table based on the given record ID. The method
     * first ensures that the table is open, then uses the index manager to find
     * the largest record ID that is smaller than or equal to the given ID. If a
     * corresponding record is found, it retrieves the block ID and reads the
     * record from that block. If no record is found, it returns null.
     *
     * @param id The ID of the record to be read.
     * @return The record corresponding to the given ID, or null if no such
     * record exists.
     * @throws IOException If an error occurs while reading the record from the
     * table.
     */
    public Record readRecord(int id) throws IOException {
        if (!opened) {
            openTable();
        }

        IntRecord rec = (IntRecord) indexManager.findLargestSmallerOrEqual(id);
        if (rec == null) {
            return null;
        }
        int blockId = rec.getValue();
        return readRecord(id, blockId);

    }

}

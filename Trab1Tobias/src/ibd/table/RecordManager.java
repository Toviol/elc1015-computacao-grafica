/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package ibd.table;

import java.io.*;
import java.util.*;

public class RecordManager {

    private String fileName = "c:\\teste\\records.dat";
    public int blockSize = 0;
    public int recordsSize = 0;
    public static int BLOCKS_WRITE = 0;
    public static int BLOCKS_READ = 0;
    public int blocksCount = 0;
    public double splitFactor = 1;
    protected boolean opened = false;
    RandomAccessFile file;

    int dataType = -1;

    public static final int STRING_TYPE = 1;
    public static final int INT_TYPE = 2;

    /**
     * Constructs a new {@code RecordManager} with the specified file name and
     * data type. This constructor initializes the file name and the data type
     * for the record manager. It prepares the instance for managing records in
     * the specified file.
     *
     * @param fileName The name of the file where records will be managed.
     * @param dataType The type of data being managed, represented by an integer
     * value.
     */
    public RecordManager(String fileName, int dataType) {
        this.fileName = fileName;
        this.dataType = dataType;
    }

    public int getBlocksCount() {
        return blocksCount;
    }

    /**
     * Creates a new table file with the specified configuration. This method
     * initializes the table with the provided block size, record size, and
     * split factor, and then creates an empty file. After initializing the
     * file, it updates the header with the provided configuration values and
     * sets the table as open.
     *
     * @param blockSize The size of each block in the table.
     * @param recordSize The size of each record in the table.
     * @param splitFactor The factor used to split blocks when they become full.
     * @throws IOException If an error occurs while creating or writing to the
     * file.
     */
    public void createTable(int blockSize, int recordSize, double splitFactor) throws IOException {

        this.blockSize = blockSize;
        this.recordsSize = recordSize;
        this.splitFactor = splitFactor;

        file = new RandomAccessFile(fileName, "rw");
        file.setLength(0);

        updateHeader();

        opened = true;

    }

    /**
     * Opens the table file for reading and writing. The method checks if the
     * table is already open, and if not, it opens the file in read-write mode.
     * After opening the file, it reads the file header to initialize the
     * table's configuration.
     *
     * @throws IOException If an error occurs while opening the file or reading
     * the header.
     */
    public void openTable() throws IOException {
        if (!opened) {
            file = new RandomAccessFile(fileName, "rw");
            opened = true;
        }
        readHeader();
    }

    /**
     * Closes the table file if it is currently open. The method checks if the
     * table is open, and if so, it closes the file and updates the status to
     * indicate that the table is no longer open.
     *
     * @throws IOException If an error occurs while closing the file.
     */
    public void closeTable() throws IOException {
        if (opened) {
            file.close();
            opened = false;
        }

    }

    /**
     * Reads the header information from the file. The method retrieves the
     * block size, records size, split factor, and the number of blocks from the
     * file's header (position 0). These values are used to configure the file
     * and subsequent operations.
     *
     * @throws IOException If an error occurs while reading from the file.
     */
    private void readHeader() throws IOException {
        file.seek(0);
        blockSize = file.readInt();
        recordsSize = file.readInt();
        splitFactor = file.readDouble();
        blocksCount = file.readInt();
    }

    /**
     * Updates the header information in the file. The method writes the current
     * values of block size, records size, split factor, and the number of
     * blocks to the file's header (position 0). This ensures that the file's
     * header is up-to-date with the latest configuration.
     *
     * @throws IOException If an error occurs while writing to the file.
     */
    private void updateHeader() throws IOException {
        file.seek(0);
        file.writeInt(blockSize);
        file.writeInt(recordsSize);
        file.writeDouble(splitFactor);
        file.writeInt(blocksCount);
        //BLOCKS_WRITE++;
    }

    /**
     * Reads a block from the file based on its ID. The method calculates the
     * position of the block in the file and retrieves the block's data. If the
     * block ID is invalid or the position exceeds the file's length, it returns
     * {@code null}. Otherwise, it increments the block read counter and returns
     * the block after deserializing its data.
     *
     * @param id The ID of the block to be read.
     * @return The block corresponding to the given ID, or {@code null} if the
     * block does not exist or is invalid.
     * @throws IOException If an error occurs while reading from the file.
     */
    protected Block readBlock(int id) throws IOException {
        if (id == -1) {
            return null;
        }

        long pos = id * blockSize;
        if (file.length() <= pos) {
            return null;
        }

        BLOCKS_READ++;
        file.seek(pos);
        byte[] data = new byte[blockSize];
        file.read(data);
        return Block.fromBytes(data, id, this);
    }

    /**
     * Writes the given block to the file at the appropriate position. The
     * block's position in the file is determined by its ID and the block size.
     * The method seeks to the correct file position, writes the block data, and
     * increments the block write counter.
     *
     * @param block The block to be written to the file.
     * @throws IOException If an error occurs while writing to the file.
     */
    protected void writeBlock(Block block) throws IOException {
        long pos = block.id * blockSize;
        file.seek(pos);
        file.write(block.toBytes());
        BLOCKS_WRITE++;
    }

    /**
     * Inserts a new record into the table. The method locates the appropriate
     * block for insertion, adds the record, and updates the block to maintain
     * the correct data structure. If necessary, it may trigger block splitting
     * to accommodate the new record.
     *
     * @param record The record to be inserted.
     * @throws IOException If an error occurs while reading or writing blocks.
     */
    public void insertRecord(Record record) throws IOException {
        if (!opened) {
            openTable();
        }

        Record rec = readRecord(record.id);
        if (rec != null) {
            return;
        }

        Block block = locateBlockToInsert(record);
        if (block == null) {
            createFirstBlock(Collections.singletonList(record));
        } else {
            insertOrderedRecords(Collections.singletonList(record), block);
        }
    }

    /**
     * Updates an existing record in the table with the provided record. The
     * method searches for a record with the same ID, replaces it, updates the
     * corresponding block, and returns the old record. If no matching record is
     * found, it returns {@code null}.
     *
     * @param rec The new record that will replace the existing one with the
     * same ID.
     * @return The old record that was replaced, or {@code null} if no matching
     * record was found.
     * @throws IOException If an error occurs while reading or writing blocks.
     */
    public Record update(Record rec) throws IOException {
        if (!opened) {
            openTable();
        }
        int blockId = 1;
        while (blockId != -1) {
            Block block = readBlock(blockId);
            if (block == null) {
                break;
            }
            Record record = block.getRecord(rec.id);
            if (record != null) {
                record.update(rec);
                writeBlock(block);
                return record;
            }
            blockId = block.nextBlock;
        }
        return null;
    }

    /**
     * Removes a record with the specified ID from the table. The method
     * searches through the blocks sequentially until it finds the record,
     * removes it, updates the block, and returns the removed record. If the
     * record is not found, it returns {@code null}.
     *
     * @param id The ID of the record to be removed.
     * @return The removed record, or {@code null} if no record with the given
     * ID exists.
     * @throws IOException If an error occurs while reading or writing blocks.
     */
    public Record remove(int id) throws IOException {
        if (!opened) {
            openTable();
        }
        int blockId = 1;
        while (blockId != -1) {
            Block block = readBlock(blockId);
            if (block == null) {
                break;
            }
            Record record = block.removeRecord(id);
            if (record != null) {
                writeBlock(block);
                return record;
            }
            blockId = block.nextBlock;
        }
        return null;
    }

    /**
     * Locates the appropriate block where the given record should be inserted.
     * This method searches for the correct block based on the record's ID,
     * ensuring that the insertion maintains the overall order of records.
     *
     * @param record The record that needs to be inserted.
     * @return The block where the record should be inserted.
     * @throws IOException If an error occurs while accessing or reading blocks.
     */
    private Block locateBlockToInsert(Record record) throws IOException {
        int id = 1;
        Block currentBlock = readBlock(id);
        if (currentBlock == null) {
            return null;
        }
        while (id != -1) {

            currentBlock = readBlock(id);

            int maxValue = currentBlock.findMaxValue();
            if (maxValue >= record.id) {
                break;
            }

            id = currentBlock.nextBlock;
        }
        return currentBlock;
    }

    // Localiza o bloco correto para inserção
    private Block locateBlockToInsertX(Record record) throws IOException {
        int id = 1;
        Block currentBlock = readBlock(id);
        while (currentBlock != null && currentBlock.findMaxValue() < record.id) {
            id++;
            currentBlock = readBlock(id);
        }
        return currentBlock;
    }

    /**
     * Creates the first block and inserts the given list of records into it.
     * The records are stored in sorted order based on their IDs. If the number
     * of records exceeds the block capacity, the overflow records are handled
     * by splitting the block and creating additional blocks as needed.
     *
     * @param allRecords The list of records to be inserted into the first
     * block.
     * @throws IOException If an error occurs while creating or writing the
     * block.
     */
    private void createFirstBlock(List<Record> allRecords) throws IOException {

        blocksCount++;
        updateHeader();

        int newId = blocksCount;
        Block newBlock = new Block(newId, this);
        writeBlock(newBlock);

        insertOrderedRecords(allRecords, newBlock);

    }

    /**
     * Creates a new block and links it to the given block. The new block is
     * initialized and prepared for storing additional records.
     *
     * @param block The current block that may need a new linked block.
     * @return A newly created block linked to the provided block.
     * @throws IOException If an error occurs while creating or writing the new
     * block.
     */
    private Block createNewtBlock(Block block) throws IOException {
        blocksCount++;
        updateHeader();
        int newId = blocksCount;
        Block newBlock = new Block(newId, this);

        Block nextBlock = null;
        if (block.nextBlock != -1) {
            nextBlock = readBlock(block.nextBlock);
        }

        block.nextBlock = newBlock.id;
        newBlock.prevBlock = block.id;
        writeBlock(block);

        if (nextBlock != null) {
            nextBlock.prevBlock = newBlock.id;
            newBlock.nextBlock = nextBlock.id;
            writeBlock(nextBlock);
        }

        writeBlock(newBlock);
        return newBlock;
    }

    /**
     * Inserts a list of records into the specified block while maintaining
     * sorted order by record ID. If the block exceeds its capacity after
     * insertion, it is split according to the {@code splitFactor}, and the
     * overflow records are recursively inserted into a newly created block.
     *
     * @param records The list of records to be inserted.
     * @param block The block where records should be inserted.
     * @throws IOException If an error occurs during block writing.
     */
    private void insertOrderedRecords(List<Record> records, Block block) throws IOException {

        block.records.addAll(records);
        block.records.sort(Comparator.comparingInt(r -> r.id));
        if (block.records.size() > block.capacity()) {
            int splitIndex = (int) (block.capacity() * splitFactor);
            if (splitIndex == 0) {
                splitIndex = 1;
            }
            List<Record> overflow = new ArrayList<>(block.records.subList(splitIndex, block.records.size()));
            block.records = block.records.subList(0, splitIndex);
            writeBlock(block);
            Block nextBlock = createNewtBlock(block);
            insertOrderedRecords(overflow, nextBlock);
        } else {
            writeBlock(block);
        }
    }

    /**
     * Reads a record with the specified ID from the table. This method checks
     * if the table is open, and if not, it opens the table. After opening the
     * table, it retrieves the record with the given ID by calling the
     * overloaded {@link #readRecord(int, int)} method with a block identifier
     * of 1.
     *
     * @param id The ID of the record to be read.
     * @return The record with the specified ID, or {@code null} if the record
     * does not exist.
     * @throws IOException If an error occurs while opening the table or reading
     * the record.
     */
    public Record readRecord(int id) throws IOException {
        if (!opened) {
            openTable();
        }
        return readRecord(id, 1);
    }

    /**
     * Reads a record with the specified record ID starting from a given block.
     * This method iterates through blocks, starting from the given block ID, to
     * find and return the record with the specified ID. If the record is not
     * found, it returns {@code null}. If the block contains a value larger than
     * the requested ID, the search is stopped early to improve efficiency.
     *
     * @param recId The ID of the record to be read.
     * @param startingBlockId The block ID from which to start searching.
     * @return The record with the specified ID, or {@code null} if not found.
     * @throws IOException If an error occurs while reading the block or
     * accessing the file.
     */
    protected Record readRecord(int recId, int startingBlockId) throws IOException {

        int blockId = startingBlockId;
        while (blockId != -1) {
            Block block = readBlock(blockId);
            if (block == null) {
                break;
            }
            Record rec = block.getRecord(recId);
            if (rec != null) {
                return rec;
            }

            if (block.findMaxValue() > recId) {
                return null;
            }
            blockId = block.nextBlock;
        }
        return null;
    }

    /**
     * Finds the largest record that is smaller than or equal to a specified id.
     * If no such element exists, returns null.
     *
     * @param id The value to compare against.
     * @return The record whose id has the largest value that is ≤ id, or null
     * if no such record exists.
     */
    public Record findLargestSmallerOrEqual(int id) throws IOException {
        if (!opened) {
            openTable();
        }

        Block block = readBlock(1);
        if (block == null) {
            return null;
        }

        Record rec = block.getRecord(0);
        return rec;
    }

    /**
     * Reads all records from the table and returns them as a sorted list. This
     * method iterates through the blocks of the table, collects all records,
     * sorts them by their ID, and then returns the sorted list of records. It
     * also logs the number of random reads (when blocks are not sequentially
     * accessed) and the total number of blocks read.
     *
     * @return A list of all records in the table, sorted by their ID.
     * @throws IOException If an error occurs while reading the blocks or
     * accessing the file.
     */
    public List<Record> readAllRecords() throws IOException {
        List<Record> records = new ArrayList();
        if (!opened) {
            openTable();
        }
        int blockId = 1;
        int lastBlockId = 0;
        int randomReads = 0;
        int blocksCount = 0;
        while (blockId != -1) {
            Block block = readBlock(blockId);

            List<Record> blockRecords = new ArrayList();
            for (Record record : block.records) {
                blockRecords.add(record);
            }
            Collections.sort(blockRecords, (r1, r2) -> Integer.compare(r1.id, r2.id));
            records.addAll(blockRecords);
            lastBlockId = blockId;
            blockId = block.nextBlock;
            if (blockId != lastBlockId + 1) {
                randomReads++;
            }
            blocksCount++;
        }
        System.out.println("Random Reads: " + randomReads);
        System.out.println("Blocks count: " + blocksCount);

        return records;
    }

}

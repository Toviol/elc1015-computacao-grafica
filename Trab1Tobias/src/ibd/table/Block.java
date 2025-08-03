/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/Classes/Class.java to edit this template
 */
package ibd.table;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

/**
 *
 * @author ferna
 */
public class Block {

    List<Record> records = new ArrayList<>();
    public int id;
    RecordManager table;
    int prevBlock = -1;
    int nextBlock = -1;

    public Block(int id, RecordManager table) {
        this.id = id;
        this.table = table;
    }

    /**
     * Checks if the current number of records has reached or exceeded a
     * specified percentage of capacity.
     *
     * @param percent The threshold percentage (as a decimal, e.g., 0.8 for 80%)
     * to compare against the capacity.
     * @return {@code true} if the number of records is greater than or equal to
     * the specified percentage of capacity, {@code false} otherwise.
     */
    public boolean isFull(double percent) {
        return records.size() >= (capacity() * percent);
    }

    /**
     * Calculates the maximum number of records that can fit in a block. The
     * calculation considers the block size and subtracts 8 bytes (for
     * metadata), then divides by the size of a single record.
     *
     * @return The maximum number of records that can be stored in a block.
     */
    public int capacity() {
        return (table.blockSize - 8) / table.recordsSize;
    }

    /**
     * Retrieves a record with the specified ID from the list of records.
     *
     * @param id The unique identifier of the record to retrieve.
     * @return The {@code Record} with the given ID if found, or {@code null} if
     * no matching record exists.
     */
    public Record getRecord(int id) {
        for (Record record : records) {
            if (record.id == id) {
                return record;
            }
        }
        return null;
    }

    /**
     * Retrieves the largest record from the list. Assumes that the records are
     * stored in sorted order.
     *
     * @return The largest {@code Record} if the list is not empty, or
     * {@code null} if the list is empty.
     */
    public Record getLargestRecord() {
        if (records.isEmpty()) {
            return null;
        }

        return records.get(records.size() - 1);
    }

    /**
     * Retrieves the largest record whose ID is smaller than or equal to the
     * given ID. If a record with the exact ID exists, it is returned.
     * Otherwise, the largest record with an ID smaller than the given ID is
     * returned. If no such record exists, returns {@code null}.
     *
     * @param id The target ID to search for.
     * @return The record with the largest ID that is smaller than or equal to
     * {@code id}, or {@code null} if no such record exists.
     */
    public Record getLargestSmallerOrEqualRecord(int id) {
        Record prevRecord = null;
        for (Record record : records) {
            if (record.id == id) {
                return record;
            }
            if (record.id > id) {
                return prevRecord;
            }
            prevRecord = record;
        }
        return null;
    }

    /**
     * Finds the maximum ID value among the records.
     *
     * @return The highest {@code id} found in the list of records, or {@code 0}
     * if the list is empty.
     */
    public int findMaxValue() {
        int max = 0;
        for (Record record : records) {
            if (record.id > max) {
                max = record.id;
            }
        }
        return max;
    }

    /**
     * Finds the minimum ID value among the records.
     *
     * @return The lowest {@code id} found in the list of records, or
     * {@code Integer.MAX_VALUE} if the list is empty.
     */
    public int findMinValue() {
        int min = Integer.MAX_VALUE;
        for (Record record : records) {
            if (record.id < min) {
                min = record.id;
            }
        }
        return min;
    }

    /**
     * Removes and returns the record with the specified ID.
     *
     * @param id The ID of the record to be removed.
     * @return The removed {@code Record} if found, otherwise {@code null}.
     */
    public Record removeRecord(int id) {
        for (int i = records.size() - 1; i >= 0; i--) {
            if (records.get(i).id == id) {
                return records.remove(i);
            }
        }
        return null;
    }

    /**
     * Converts the current block of records into a byte array representation.
     * The resulting byte array follows this structure: - First 4 bytes:
     * Previous block reference. - Next 4 bytes: Next block reference. -
     * Remaining bytes: Serialized records.
     *
     * @return A byte array representing the serialized form of this block.
     */
    public byte[] toBytes() {
        byte[] data = new byte[table.blockSize];
        int offset = 0;
        System.arraycopy(Record.intToBytes(prevBlock), 0, data, offset, 4);
        offset += 4;
        System.arraycopy(Record.intToBytes(nextBlock), 0, data, offset, 4);
        offset += 4;
        for (Record record : records) {
            System.arraycopy(record.toBytes(table.recordsSize - 4), 0, data, offset, table.recordsSize);
            offset += table.recordsSize;
        }
        return data;
    }

    /**
     * Constructs a {@code Block} object from a byte array representation. The
     * byte array follows this structure: - First 4 bytes: Previous block
     * reference. - Next 4 bytes: Next block reference. - Remaining bytes:
     * Serialized records.
     *
     * The method iterates over the byte array to extract individual records and
     * adds them to the block.
     *
     * @param data The byte array containing the serialized block data.
     * @param id The identifier of the block.
     * @param table The {@code RecordManager} instance containing metadata about
     * record size and data type.
     * @return A {@code Block} object reconstructed from the given byte array.
     */
    public static Block fromBytes(byte[] data, int id, RecordManager table) {

        Block block = new Block(id, table);

        block.prevBlock = Record.bytesToInt(Arrays.copyOfRange(data, 0, 4));
        block.nextBlock = Record.bytesToInt(Arrays.copyOfRange(data, 4, 8));

        for (int i = 0; i < table.blockSize; i += table.recordsSize) {
            byte[] recordBytes = Arrays.copyOfRange(data, 8 + i, 8 + i + table.recordsSize);
            Record record = null;
            if (table.dataType == RecordManager.INT_TYPE) {
                record = new IntRecord();
            } else {
                record = new StringRecord();
            }
            record.fillBytes(recordBytes);
            if (record.id != -1) {
                block.records.add(record);
            }
        }
        return block;
    }
}

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package ibd.index.btree.generic;

import ibd.index.btree.Key;
import ibd.index.btree.Value;
import ibd.persistent.PersistentPageFile;
import java.nio.file.Paths;
import java.util.List;

/**
 *
 * @author Sergio
 */
public class Main {

    public boolean add(BPlusTreeFileGeneric tree, long value1) {
        Key key = tree.createKey();
        key.setKeys(new Long[]{value1});
        Value value = tree.createValue();
        value.set(0, "record:" + (value1));

        return tree.insert(key, value, true);

    }

    public boolean delete(BPlusTreeFileGeneric tree, long value1) {
        Key key = tree.createKey();
        key.setKeys(new Long[]{value1});

        Value value = tree.delete(key);
        return (value != null);

    }

    public String query(BPlusTreeFileGeneric tree, long value1) {
        Key key = tree.createKey();
        key.setKeys(new Long[]{value1});

        Value value = tree.search(key);
        if (value != null) {
            return value.toString();
        }

        return null;
    }

    
    public void queryInserted(BPlusTreeFileGeneric tree, long value1) {
        Key key = tree.createKey();
        key.setKeys(new Long[]{value1});
        String result = query(tree, value1);
        if (result != null) {
            System.out.println("Success. Found " + result);
        } else {
            System.out.println("Error. Not found " + value1);
        }
    }

    
    public void querySmaller(BPlusTreeFileGeneric tree, long value1) {
        Key key = tree.createKey();
        key.setKeys(new Long[]{value1});

        List<Value> values = tree.searchSmaller(key);
        for (Value value : values) {
            System.out.println(value.toString());
        }

        
    }

    
    public void queryRange(BPlusTreeFileGeneric tree,  long lowerEnd, long higherEnd) {
        Key lowerKey = tree.createKey();
        lowerKey.setKeys(new Long[]{lowerEnd});

        Key higherKey = tree.createKey();
        higherKey.setKeys(new Long[]{higherEnd});

        
        List<Value> values = tree.searchRange(lowerKey, higherKey);
        for (Value value : values) {
            System.out.println(value.toString());
        }

        
    }

    public void queryLarger(BPlusTreeFileGeneric tree,  long value1) {
        Key key = tree.createKey();
        key.setKeys(new Long[]{value1});

        List<Value> values = tree.searchLarger(key);
        for (Value value : values) {
            System.out.println(value.toString());
        }

        
    }

    public void queryDeleted(BPlusTreeFileGeneric tree, long value1) {
        Key key = tree.createKey();
        key.setKeys(new Long[]{value1});
        String result = query(tree, value1);
        if (result == null) {
            System.out.println("Success. Not found " + value1);
        } else {
            System.out.println("Error. Should not find " + value1);
        }
    }

    public static void main(String[] args) {
        try {

            boolean newDatabase = true;

            RowSchema keySchema = new RowSchema(1);
            keySchema.addLongDataType();
            
            RowSchema valueSchema = new RowSchema(1);
            valueSchema.addStringDataType();

            PersistentPageFile p = new PersistentPageFile(4096, Paths.get("c:\\teste\\mtree\\mtree"), newDatabase);
            BPlusTreeFileGeneric tree = new BPlusTreeFileGeneric(p, valueSchema, keySchema);
            tree.open();
            Main test = new Main();

            if (newDatabase) {
                for (long i = 0; i < 100; i++) {
                test.add(tree, i);    
                }
                
                //test.delete(tree, keySchema, 0);
            }

            test.queryLarger(tree, 40);
            //test.queryRange(tree, 40, 80);
            
            //test.queryInserted(tree, 1);
            //test.queryDeleted(tree, 0);
            //test.queryInserted(tree, 2);
//            test.update(tree, 1,

            tree.close();
        } catch (Exception ex) {
            //Logger.getLogger(Test.class.getName()).log(Level.SEVERE, null, ex);
            ex.printStackTrace();
        }
    }

}

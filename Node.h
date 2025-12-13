#pragma once
#include <iostream>
#include <fstream>

class BTreeNode {
private:
    int m;           // order of B-tree
    int* storage;    // Complete node storage (2*m + 1 integers)

public:
    // ==================== CONSTRUCTORS & DESTRUCTORS ====================
    BTreeNode(int m = 5);
    BTreeNode(const BTreeNode& other);
    ~BTreeNode();

    // ==================== ACCESSORS ====================
    int getOrder() const { return m; }
    int getLeafStatus() const { return storage[0]; }
    void setLeafStatus(int status) { storage[0] = status; }

    int getNextFree() const { return storage[1]; }
    void setNextFree(int next) { storage[1] = next; }

    // ==================== NODE INITIALIZATION ====================
    void initAsEmpty(int nextFree = -1);
    void initAsLeaf();
    void initAsNonLeaf();
    void initAsNodeZero(int firstFree = 1);

    // ==================== CAPACITY & STATUS ====================
    bool isEmpty() const;
    int keyCount() const;
    bool isFull() const;
    int getMaxKeys() const;

    // ==================== BASIC OPERATIONS ====================
    int findKeyPosition(int key) const;
    int findInsertPosition(int key) const;
    bool insertKeyRef(int key, int ref);
    bool insertChildKey(int child, int key);

    // ==================== DELETION SUPPORT ====================
    bool deleteKey(int key);
    void deleteAtPosition(int pos);
    void clearPosition(int pos);

    // ==================== FILE I/O ====================
    void writeToFile(std::fstream& file, int nodeIndex) const;
    void readFromFile(std::fstream& file, int nodeIndex);
    static int getNodeSize(int m);

    // ==================== DEBUG/UTILITY ====================
    void printNode(int nodeIndex = -1) const;

private:
    void shiftRight(int fromPos);
    void shiftLeft(int fromPos);
    void clearAll();
};
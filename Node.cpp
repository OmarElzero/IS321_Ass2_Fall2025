#include "Node.h"
#include <cstring>
#include <algorithm>
#include <iomanip>

// ==================== CONSTRUCTORS & DESTRUCTORS ====================
BTreeNode::BTreeNode(int m_) : m(m_) {
    // Exactly 2*m + 1 integers as per assignment
    storage = new int[2 * m + 1];
    for (int i = 0; i < 2 * m + 1; i++) {
        storage[i] = -1;
    }
}

BTreeNode::BTreeNode(const BTreeNode& other) : m(other.m) {
    storage = new int[2 * m + 1];
    std::memcpy(storage, other.storage, sizeof(int) * (2 * m + 1));
}

BTreeNode::~BTreeNode() {
    delete[] storage;
}

// ==================== NODE INITIALIZATION ====================
void BTreeNode::initAsEmpty(int nextFree) {
    for (int i = 0; i < 2 * m + 1; i++) {
        storage[i] = -1;
    }
    storage[1] = nextFree;  // Position 1 is next free node
}

void BTreeNode::initAsLeaf() {
    for (int i = 0; i < 2 * m + 1; i++) {
        storage[i] = -1;
    }
    storage[0] = 0;  // Leaf status at position 0
}

void BTreeNode::initAsNonLeaf() {
    for (int i = 0; i < 2 * m + 1; i++) {
        storage[i] = -1;
    }
    storage[0] = 1;  // Non-leaf status at position 0
}

void BTreeNode::initAsNodeZero(int firstFree) {
    for (int i = 0; i < 2 * m + 1; i++) {
        storage[i] = -1;
    }
    storage[1] = firstFree;  // First free node index at position 1
}

// ==================== CAPACITY & STATUS ====================
bool BTreeNode::isEmpty() const {
    if (storage[0] == -1) return true;

    // Check if all positions (except maybe leaf status) are -1
    for (int i = 1; i < 2 * m + 1; i++) {
        if (storage[i] != -1) return false;
    }
    return true;
}

int BTreeNode::keyCount() const {
    if (storage[0] == -1) return 0;  // Empty node

    int count = 0;
    if (storage[0] == 0) {  // Leaf node
        // Count keys at positions 1, 3, 5, ... (up to 2*m)
        for (int i = 1; i < 2 * m + 1; i += 2) {
            if (storage[i] != -1) count++;
        }
    } else if (storage[0] == 1) {  // Non-leaf node
        // Count keys at positions 2, 4, 6, ... (up to 2*m)
        for (int i = 2; i < 2 * m + 1; i += 2) {
            if (storage[i] != -1) count++;
        }
    }
    return count;
}

bool BTreeNode::isFull() const {
    return keyCount() == m;  // Max m keys for m-order B-tree
}

int BTreeNode::getMaxKeys() const {
    return m;
}

// ==================== BASIC OPERATIONS ====================
int BTreeNode::findKeyPosition(int key) const {
    if (storage[0] == 0) {  // Leaf node
        // Leaf: keys at positions 1, 3, 5, ...
        for (int pos = 0; pos < m; pos++) {
            int idx = 1 + 2 * pos;
            if (idx >= 2 * m + 1) break;

            if (storage[idx] == key) return pos;
            if (storage[idx] == -1) break;
        }
    } else if (storage[0] == 1) {  // Non-leaf node
        // Non-leaf: keys at positions 2, 4, 6, ...
        for (int pos = 0; pos < m; pos++) {
            int idx = 2 + 2 * pos;
            if (idx >= 2 * m + 1) break;

            if (storage[idx] == key) return pos;
            if (storage[idx] == -1) break;
        }
    }
    return -1;  // Key not found
}

int BTreeNode::findInsertPosition(int key) const {
    if (storage[0] == 0) {  // Leaf node
        // Find first position where key should go
        for (int pos = 0; pos < m; pos++) {
            int idx = 1 + 2 * pos;
            if (idx >= 2 * m + 1) return pos;

            if (storage[idx] == -1 || storage[idx] > key) {
                return pos;
            }
        }
    } else if (storage[0] == 1) {  // Non-leaf node
        // For non-leaf, find position for key
        for (int pos = 0; pos < m; pos++) {
            int idx = 2 + 2 * pos;
            if (idx >= 2 * m + 1) return pos;

            if (storage[idx] == -1 || storage[idx] > key) {
                return pos;
            }
        }
    }
    return m;  // Should go at the end
}

bool BTreeNode::insertKeyRef(int key, int ref) {
    if (storage[0] != 0) return false;  // Not a leaf node
    if (isFull()) return false;         // Node is full

    int pos = findInsertPosition(key);
    if (pos < 0 || pos > m) return false;

    // Check if key already exists
    int existingPos = findKeyPosition(key);
    if (existingPos != -1) {
        // Update existing key's reference
        int refIdx = 2 + 2 * existingPos;
        if (refIdx < 2 * m + 1) {
            storage[refIdx] = ref;
            return true;
        }
        return false;
    }

    // Shift elements to the right
    shiftRight(pos);

    // Insert new key-ref pair
    int keyIdx = 1 + 2 * pos;
    int refIdx = keyIdx + 1;

    if (keyIdx < 2 * m + 1 && refIdx < 2 * m + 1) {
        storage[keyIdx] = key;
        storage[refIdx] = ref;
        return true;
    }

    return false;
}

bool BTreeNode::insertChildKey(int child, int key) {
    if (storage[0] != 1) return false;  // Not a non-leaf node
    if (isFull()) return false;         // Node is full

    int pos = findInsertPosition(key);
    if (pos < 0 || pos > m) return false;

    // Shift elements to the right
    shiftRight(pos);

    // Insert child-key pair
    int childIdx = 1 + 2 * pos;      // Child at odd position
    int keyIdx = childIdx + 1;       // Key at even position

    if (childIdx < 2 * m + 1 && keyIdx < 2 * m + 1) {
        storage[childIdx] = child;
        storage[keyIdx] = key;
        return true;
    }

    return false;
}

// ==================== DELETION SUPPORT ====================
bool BTreeNode::deleteKey(int key) {
    int pos = findKeyPosition(key);
    if (pos == -1) return false;  // Key not found

    deleteAtPosition(pos);
    return true;
}

void BTreeNode::deleteAtPosition(int pos) {
    if (pos < 0) return;

    if (storage[0] == 0) {  // Leaf node
        int keyIdx = 1 + 2 * pos;
        int refIdx = keyIdx + 1;

        if (keyIdx < 2 * m + 1) {
            storage[keyIdx] = -1;
            if (refIdx < 2 * m + 1) {
                storage[refIdx] = -1;
            }
            shiftLeft(pos + 1);
        }
    } else if (storage[0] == 1) {  // Non-leaf node
        int childIdx = 1 + 2 * pos;
        int keyIdx = childIdx + 1;

        if (keyIdx < 2 * m + 1) {
            storage[keyIdx] = -1;
            if (childIdx < 2 * m + 1) {
                storage[childIdx] = -1;
            }
            shiftLeft(pos + 1);
        }
    }
}

void BTreeNode::clearPosition(int pos) {
    if (storage[0] == 0) {  // Leaf
        int keyIdx = 1 + 2 * pos;
        int refIdx = keyIdx + 1;

        if (keyIdx < 2 * m + 1) storage[keyIdx] = -1;
        if (refIdx < 2 * m + 1) storage[refIdx] = -1;
    } else if (storage[0] == 1) {  // Non-leaf
        int childIdx = 1 + 2 * pos;
        int keyIdx = childIdx + 1;

        if (childIdx < 2 * m + 1) storage[childIdx] = -1;
        if (keyIdx < 2 * m + 1) storage[keyIdx] = -1;
    }
}

// ==================== FILE I/O ====================
void BTreeNode::writeToFile(std::fstream& file, int nodeIndex) const {
    if (!file.is_open()) return;

    std::streamoff pos = static_cast<std::streamoff>(nodeIndex) * getNodeSize(m);
    file.seekp(pos, std::ios::beg);

    // Write exactly 2*m + 1 integers
    file.write(reinterpret_cast<const char*>(storage), sizeof(int) * (2 * m + 1));
    file.flush();
}

void BTreeNode::readFromFile(std::fstream& file, int nodeIndex) {
    if (!file.is_open()) return;

    std::streamoff pos = static_cast<std::streamoff>(nodeIndex) * getNodeSize(m);
    file.seekg(pos, std::ios::beg);
    file.read(reinterpret_cast<char*>(storage), sizeof(int) * (2 * m + 1));
}

int BTreeNode::getNodeSize(int m) {
    return sizeof(int) * (2 * m + 1);
}

// ==================== DEBUG/UTILITY ====================
void BTreeNode::printNode(int nodeIndex) const {
    if (nodeIndex >= 0) {
        std::cout << "Node " << nodeIndex << ": ";
    }

    // Print node type
    if (storage[0] == -1) std::cout << "EMPTY ";
    else if (storage[0] == 0) std::cout << "LEAF ";
    else if (storage[0] == 1) std::cout << "NON-LEAF ";

    // Print all data (2*m + 1 integers)
    for (int i = 0; i < 2 * m + 1; i++) {
        std::cout << storage[i] << " ";
    }
    std::cout << std::endl;
}

void BTreeNode::clearAll() {
    for (int i = 0; i < 2 * m + 1; i++) {
        storage[i] = -1;
    }
}

// ==================== HELPER METHODS ====================
void BTreeNode::shiftRight(int fromPos) {
    if (storage[0] == 0) {  // Leaf node
        // Shift from the end backwards
        for (int pos = m - 1; pos >= fromPos; pos--) {
            int newKeyIdx = 1 + 2 * pos;
            int newRefIdx = newKeyIdx + 1;

            if (newKeyIdx < 2 * m + 1 && newRefIdx < 2 * m + 1) {
                if (pos > fromPos) {
                    // Shift existing data
                    int oldKeyIdx = 1 + 2 * (pos - 1);
                    int oldRefIdx = oldKeyIdx + 1;
                    storage[newKeyIdx] = storage[oldKeyIdx];
                    storage[newRefIdx] = storage[oldRefIdx];
                } else {
                    // Clear insertion position
                    storage[newKeyIdx] = -1;
                    storage[newRefIdx] = -1;
                }
            }
        }
    } else if (storage[0] == 1) {  // Non-leaf node
        // Similar logic for non-leaf
        for (int pos = m - 1; pos >= fromPos; pos--) {
            int newChildIdx = 1 + 2 * pos;
            int newKeyIdx = newChildIdx + 1;

            if (newChildIdx < 2 * m + 1 && newKeyIdx < 2 * m + 1) {
                if (pos > fromPos) {
                    int oldChildIdx = 1 + 2 * (pos - 1);
                    int oldKeyIdx = oldChildIdx + 1;
                    storage[newChildIdx] = storage[oldChildIdx];
                    storage[newKeyIdx] = storage[oldKeyIdx];
                } else {
                    storage[newChildIdx] = -1;
                    storage[newKeyIdx] = -1;
                }
            }
        }
    }
}

void BTreeNode::shiftLeft(int fromPos) {
    if (storage[0] == 0) {  // Leaf node
        // Shift from fromPos to end
        for (int pos = fromPos; pos < m; pos++) {
            int newKeyIdx = 1 + 2 * pos;
            int newRefIdx = newKeyIdx + 1;

            if (newKeyIdx < 2 * m + 1 && newRefIdx < 2 * m + 1) {
                if (pos < m - 1) {
                    int oldKeyIdx = 1 + 2 * (pos + 1);
                    int oldRefIdx = oldKeyIdx + 1;
                    storage[newKeyIdx] = storage[oldKeyIdx];
                    storage[newRefIdx] = storage[oldRefIdx];
                } else {
                    // Clear last position
                    storage[newKeyIdx] = -1;
                    storage[newRefIdx] = -1;
                }
            }
        }
    } else if (storage[0] == 1) {  // Non-leaf node
        // Similar logic for non-leaf
        for (int pos = fromPos; pos < m; pos++) {
            int newChildIdx = 1 + 2 * pos;
            int newKeyIdx = newChildIdx + 1;

            if (newChildIdx < 2 * m + 1 && newKeyIdx < 2 * m + 1) {
                if (pos < m - 1) {
                    int oldChildIdx = 1 + 2 * (pos + 1);
                    int oldKeyIdx = oldChildIdx + 1;
                    storage[newChildIdx] = storage[oldChildIdx];
                    storage[newKeyIdx] = storage[oldKeyIdx];
                } else {
                    storage[newChildIdx] = -1;
                    storage[newKeyIdx] = -1;
                }
            }
        }
    }
}
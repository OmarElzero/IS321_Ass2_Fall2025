# IS321_Ass2_Fall2025
## B-Tree Index File Implementation (C++ with Binary Files)

### Course: File Management & Processing  
### Semester: Fall 2025  
### Group Members:
- Member 1 – Node design + file read/write utilities + Deletion Support 
- Member 2 – Search  
- Member 3 – Insert (Part 1) & Deletion Support  
- Member 4 – Insert (Splitting & Propagation)  
- Member 5 – Deletion (Core Logic)  

---

## 📌 Project Overview

This project implements a **disk-based B-Tree index** stored inside a binary file using C++.  
The index supports:

- Creating an index file with fixed-length nodes  
- Inserting records  
- Deleting records  
- Searching by key  
- Managing a free-node linked list  
- Displaying file content  

The implementation follows the exact requirements of **IS321 Assignment 2 – Fall 2025**.

---

# 📁 File Structure & Node Format

Each B-Tree node is stored with this fixed structure:

- `LeafFlag` → 0 = leaf, 1 = non-leaf  
- Unused keys and references are stored as `-1`  
- Node 0 is a special system node containing the head of the free-node list  
- All nodes are fixed-size, so random access uses:


---

# 🔧 Required Functions

```cpp
void CreateIndexFile(char* filename, int numberOfRecords, int m);

int InsertNewRecordAtIndex(char* filename, int RecordID, int Reference);
// Returns -1 if no place to insert, or the index where inserted.

void DeleteRecordFromIndex(char* filename, int RecordID);

void DisplayIndexFileContent(char* filename);
// Print each node on a new line.

int SearchARecord(char* filename, int RecordID);
// Returns the reference, or -1 if not found.


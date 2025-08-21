# MyDB  

**Simple B+ Tree-based in-memory database with CLI interface**  

<img width="1059" height="310" alt="image" src="https://github.com/user-attachments/assets/6df187c2-cbc8-477c-9b2f-05b5dd8e9753" />

## Example Usage  
<img width="1036" height="948" alt="image" src="https://github.com/user-attachments/assets/86b6e2d9-cc49-4632-b395-529af92e1ec2" />



## Build & Run

Write these commands one by one in your terminal:

```bash
git clone https://github.com/ardaakmanlar/MyDB
cd MyDB/core
g++ -std=c++14 -o myDB.exe main.cpp Table.cpp BPlusTree.cpp
myDB.exe

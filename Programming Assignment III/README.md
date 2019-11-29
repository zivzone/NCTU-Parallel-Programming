# 注意事項：
1.輸出結果必須與只在一個node上執行的結果相同，即只有一行輸出且答案必須一樣，請注意最初及最後的部份。
   請不要更改輸出格式。
2.Requirement的部份有點小錯誤，command line argument應為3個，如同在Problem Statement所提及和conduction.c所示，分別代表Length, Iteration, Seed。
3.資料經網路交換會有overhead，盡量降低交換資料數有助增強平行效果

# Usage
- Single Machine
```
mpicc -o conduction conduction.c
mpiexec -n 1 conduction 20 100 0
mpiexec -n 4 conduction 20 100 0
```
- Muti Machine
```
/home/PP-f19/MPI/bin/mpicc conduction.c -o conduction
/home/PP-f19/MPI/bin/mpiexec -npernode 1 --hostfile hostfile -n 1 conduction 50000 100 0
/home/PP-f19/MPI/bin/mpiexec -npernode 1 --hostfile hostfile -n 4 conduction 50000 100 0
```

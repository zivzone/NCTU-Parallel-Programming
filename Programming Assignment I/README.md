# Programming Assignment I: Pthreads Programming

- 作業請參考[課程網站](https://people.cs.nctu.edu.tw/~ypyou/courses/PP-f19/)

  - 繳交期限是23:55, November 1, Friday, 2019

- 注意事項：

- 1. 請讓程式能夠接受兩個參數

    - 第一個是CPU數，第二個是執行的次數

    -  e.g.

    - ```$ ./pi 4 100000```

    - 請不要從stdin讀入.

- 2.可使用taskset -c [a numerical list of processors] command測試scalability

    - e.g. 

    - ```$ taskset -c 1,2 ./pi 2 100000```

- 3.請直接輸出結果

    - e.g. 3.1415926

- 4.測試將會以下指令 compile 同學們的程式碼:

    - C++:

```g++ -pthread -std=c++11 -O2 -s pi.cpp -o pi```

    - 或

    - C:

```gcc -pthread -std=gnu99 -O2 -s pi.c -o pi```



**有其他問題可寄信至PP-f19@sslab.cs.nctu.edu.tw**

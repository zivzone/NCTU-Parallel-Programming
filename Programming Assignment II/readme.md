# 說明文件中文檔
==================== 11 / 06 更新 =============================

針對最後的輸出，請確保所有輸出的格式是跟原本一模一樣的，包括任何的換行、空格，

但是計算過程中以及最後輸出的數值如果跟原始版本不一樣是正常的，如果一樣當然也是正常的，

只要確保在「Complete...」之後是輸出「VERIFICATION SUCCESSFUL」，基本上數值在 1.0E-10 的範圍內都是容許的誤差值。

=============================================================== 
## 置頂懶人包注意事項：

- 如果要測試不同的 DATASIZE，make 之前記得要先 make clean。
- 修改過的 cg.c 一定要用到 OpenMP 的 pragma，不然會 0 分。
- 修改過的 cg.c 輸出格式必須和原本的一致，否則 20 分會直接蒸發。
- 修改過的 cg.c 上傳之前記得要改檔名，不然會被扣 10 分。
- 這次作業不是上傳壓縮檔，直接上傳 .c 檔案即可，不然會被扣 10 分。


## 簡單說明：

- 使用 OpenMP 去修改 cg.c，讓程式可以加速。



## 詳細說明：

- 下載附件的 zip 檔案解壓縮後會有 serial 版本的 sample code，能修改的只有 cg.c 這個檔案。
- 請使用 OpenMp 的技術修改 cg.c 來提升程式的效能。
- 輸出的格式請勿更動，但數字不一樣是正常的，並不是要求連輸出的數字都要一模一樣，是「格式」必須一致。
- 修改完的 cg.c 請重新命名成 {學號}.c (例如：0765432.c) 並直接上傳到 e3，不需要壓縮。


## 執行方式：
```
unzip HW2.zip
cd HW2
make clean
make (or) make DATASIZE=LARGE  # 評分時是使用後者
taskset -c 0 ./bin/cg
taskset -c 0,1 ./bin/cg
taskset -c 0,1,2,3 ./bin/cg
```

## 評分方式：

- 85% - Correctness and Scalability - 程式輸出格式必須和原本的一致，並且能按照 taskset 給定的核心數來加速程式。
- 15% - Performance - 依照你的程式使用 taskset -c 0,1,2,3 後在班上的排名，按比例給分。


更詳細的內容請參考附件的 PDF 檔案。

# Description in English version
==================== 11 / 06 Update =============================

For the output, please make sure that the output format of the revised program is the same as the original one, include any newline and space.

If the value is different from the original value, don't worry, this is acceptable. Of course, if the values ​​are the same, it is also acceptable.

You only need to ensure that the message after "Complete..." is "VERIFICATION SUCCESSFUL" and the values ​​will not differ from the original ones by more than 1.0E-10.

================================================================= 

## Note：

- If you want to test for different DATASIZE, remember to execute "make clean" before do "make".
- You must revise cg.c with OpenMP, otherwise the score is 0.
- The output format of the revised program must be the same as the original program, otherwise 20 points will disappear.
- Please rename the revised cg.c before uploading, otherwise you will lose 10 points.
- Please upload .c instead of .zip, or you will lose 10 points.


## Brief description：

- You are required to use OpenMP to improve this program performance.



## Detail：

- The serial sample code is in the attached zip file, only the cg.c is allowed to be revised.
- Please use OpenMP to improve this program performance.
- Please keep the output format (not the output value) of the revised program consistent with the original one.
- Please rename the revised cg.c to {student ID}.c, for example, "0765432.c", and do not zip the .c file.


## Some useful commands：
```
unzip HW2.zip
cd HW2
make clean
make (or) make DATASIZE=LARGE  # the latter will be used when grading
taskset -c 0 ./bin/cg
taskset -c 0,1 ./bin/cg
taskset -c 0,1,2,3 ./bin/cg
```
## Judge：

- 85% - Correctness and Scalability - The output format is the same as original one, and the revised program can correctly improve performance according to the number of cores given by "taskset".
- 15% - Performance - Use "taskset -c 0,1,2,3" to test your program and give points by rank.

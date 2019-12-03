# Programming Assignment IV: CUDA Programming
## 中文解釋
### 置頂懶人包注意事項：
- CUDA 版本的程式碼編譯後，在執行時必須要能吃兩個參數 (point, iteration)
- CUDA 版本的程式碼輸出格式必須和 serial 版本的一致，輸出數字的誤差值不能大於 0.001
- CUDA 版本的程式碼請重新命名為 {自己的學號}.cu (例如：0765432.cu)
- 請直接上傳 CUDA 版本的程式碼，不必壓縮。

### 簡要說明：
- 撰寫 CUDA 版本的程式以利用 GPU 來加速運算。
- 詳細的規範請參照老師的個人網站 - here。

### 執行方式：
- gcc serial_wave.c -o serial_wave -lm
- nvcc cuda_wave.cu -o cuda_wave

### 評分方式：

- 85% - Correctness and Scalability - 程式輸出格式必須和原本的一致，並且與 serial 版本相比能有大幅加速。
- 15% - Performance - 依照程式的執行速度在班上的排名，按比例給分。

## English Note

# 汇编与高级语言性能比较 —— 以插入排序为例

## 一、项目概述
本项目比较了用 C 语言与 x86-64 汇编实现的插入排序在不同数据规模与不同编译优化级别下的执行性能。

## 二、项目环境
- 操作系统：Wsl2 Ubuntu 24.04.3 LTS
- 编译器：GCC
- Python 3.10 + Matplotlib

## 三、项目与文件结构
### `Ocompare.c`  
  - 生成不同规模的带符号整数数组
  - 用C语言和x86-64分别实现插入排序
  - 使用 `<time.h>` 进行计时，结果输出为 CSV
  - 每个实验重复10次，取平均值
### `Ocompare.py`  
  - 读取 CSV 并生成可视化图表（matplotlib实现）
### `Ocompare.sh`  
  - 自动化脚本：对 `Ocompare.c` 以不同编译选项编译并运行，再调用`Ocompare.py`  

## 四、插入排序
插入排序是一种简单直观的排序算法，它的基本思想是：将数组分为“已排序”和“未排序”两部分，每次从未排序部分取出一个元素，插入到已排序部分的合适位置，使得已排序部分始终保持有序。它的过程类似于整理扑克牌的顺序。插入排序在数据量较小或数据本身接近有序时效率较高，具有**稳定性**和**原地排序**的特点，时间复杂度为 O(n²)，空间复杂度为 O(1)。

## 五、核心代码
### 1. 汇编实现插入排序
#### 寄存器与变量说明
| 寄存器/变量    | 含义            |
| ------ | ------------- |
| `%rdi` | 数组首地址 `arr`   |
| `%rcx` | 外层循环索引 `i`    |
| `%rdx` | 内层循环索引 `j`    |
| `%rax` | 当前待插入元素       |
| `%rsi` | 比较元素 `arr[j]` |
| `n`    | 数组长度          |
#### 代码如下
```C
__attribute__((noinline))
void insertion_sort_asm(long *arr, int n) {
    __asm__ volatile (
        "mov $1, %%rcx;"
        "1:;"
        "cmp %[n], %%rcx;"
        "jge 4f;"

        "mov (%%rdi, %%rcx, 8), %%rax;"
        "lea -1(%%rcx), %%rdx;"

        "2:;"
        "cmp $0, %%rdx;"
        "jl 3f;"
        "mov (%%rdi, %%rdx, 8), %%rsi;"
        "cmp %%rax, %%rsi;"
        "jle 3f;"
        "mov %%rsi, 8(%%rdi, %%rdx, 8);"
        "dec %%rdx;"
        "jmp 2b;"

        "3:;"
        "inc %%rdx;"
        "mov %%rax, (%%rdi, %%rdx, 8);"

        "inc %%rcx;"
        "jmp 1b;"

        "4:;"
        :
        : [n]"r"((long)n), "D"(arr)
        : "rax", "rcx", "rdx", "rsi", "memory"
    );
}
```
#### 流程图如下
<img width="2240" height="1132" alt="流程图" src="https://github.com/user-attachments/assets/e30403d5-c9da-42e9-9c2e-a2fbad9c8d04" />
其中左边为外层循环，中间为内层循环，右侧为插入过程。
### 2. 自动化脚本
#### 代码如下
```sh
#!/bin/bash
SRC="Ocompare.c"
OUTPUT="insert"
OPT_LEVELS=("-O0" "-O1" "-O2" "-O3" "-Ofast")

for OPT in "${OPT_LEVELS[@]}"; do
  echo "Compiling: gcc $OPT $SRC -o ${OUTPUT}_${OPT}"
  gcc $OPT $SRC -o "${OUTPUT}_${OPT}" || exit 1

  echo "Running: ${OUTPUT}_${OPT}"
  OPT_LEVEL="${OPT#-}" ./"${OUTPUT}_${OPT}"
done

echo "All tests completed."

python Ocompare.py
```
通过shell脚本实现了自动化以不同编译方式（O0、O1、O2、O3、Ofast）编译运行 `Ocompare.c`，并调用 `Ocompare.py` 实现自动数据可视化操作。使得只需要在Ubuntu环境下的命令行输入`bash Ocompare.sh` 即可完成全部实验流程。

## 六、实验结果
**在O0优化下时间随数据量变化情况如下图**
<img width="2400" height="1800" alt="compare_O0" src="https://github.com/user-attachments/assets/32c4716d-75f1-4d05-b29e-91173081daea" />
情况符合预期。由于C程序可能存在**跨段跳转**、**冗余寄存器操作**等原因，性能远不如ASM实现。

然而，有同组同学实验中出现了汇编实现比C程序更慢的情况。为进一步验证原因，我分别测试了在不同编译优化级别下的性能表现，结果如下：
- **C 程序性能随优化等级变化情况：**
<img width="2400" height="1800" alt="c_compare" src="https://github.com/user-attachments/assets/91a9919d-a495-49a7-b866-73ab523261e0" />
- **汇编程序性能随优化等级变化情况：**
<img width="2400" height="1800" alt="asm_compare" src="https://github.com/user-attachments/assets/de23a5f4-88d3-4524-b462-029403a94eec" />
由结果可看出，**高级别的编译优化对于汇编程序来说甚至起到了负面效果，而对于C程序来说性能提升幅度巨大。**

此外，在O2优化级别下的比较结果如下：
<img width="2400" height="1800" alt="compare_O2" src="https://github.com/user-attachments/assets/1ae93116-0b3d-4657-867a-d68062990cd2" />
由上图可知，C 程序在经过 O2 优化后性能已明显超过汇编版本。

编译器的编译优化是造成同学测试出C程序不如汇编的因素之一。

## 七、实验结论
综上所述：

1. 在无优化（O0）条件下，手写汇编程序由于能够直接控制寄存器与指令序列，避免了编译器生成的冗余操作，性能优于 C 实现。

2. 在高优化（O1~O3）条件下，现代编译器能充分利用 CPU 特性（如指令流水线、寄存器重命名、指令并行化等），生成的机器代码往往比人工汇编更高效。

3. 汇编性能下降的原因在于：编译器在处理内联汇编或独立汇编文件时，无法进行全局优化（如循环展开、寄存器调度等），从而限制了整体优化空间。

在现代编译环境下，编译器的优化能力极强。对于常规算法（如插入排序），C 语言在开启优化后能达到甚至超过手写汇编的性能。因此，除非对底层结构或特殊硬件进行精确控制，使用高级语言结合优化编译器是更高效且可维护的选择。
## 八、汇编应用场景和发展前景
### 1. 应用场景
由于本学期有操作系统课程，这里着重讨论汇编语言在操作系统内核中的应用。其他诸如音视频加速、逆向工程、嵌入式开发等方向本人了解有限，不展开。

在操作系统内核中，汇编语言通常用于实现对硬件的直接控制、处理器上下文切换、中断与异常处理、同步原语等底层功能。下面以自旋锁为例说明汇编的典型应用场景。

在操作系统中，为了保证同时只有一个线程能够进入临界区，会使用到自旋锁。下面展示一个非常基础的自旋锁的实现，其中使用到了内联x86-64汇编。
```C
#define LOCKED   1
#define UNLOCKED 0
int status = UNLOCKED;

void lock() {
    int expected;
    do {
        // Try compare status with expected.
        // If the comparison succeeded, perform
        // an exchange.
        expected = UNLOCKED;
        asm volatile (
            "lock cmpxchgl %2, %1"
            : "+a" (expected) // Value for comparison.
                              // x86 uses eax/rax.
            : "m" (status),   // Memory location.
              "r" (LOCKED)    // Value to be written if
                              // status == expected
            : "memory", "cc"
        );
    } while (expected != UNLOCKED);

void unlock() {
    // To be safer:
    //
    // asm volatile (
    //     "movl %1, %0"
    //     : "=m" (status)
    //     : "r" (UNLOCKED)
    //     : "memory"
    // );

    // But actually we can do this:
    asm volatile("" ::: "memory");
    status = UNLOCKED;
}
```
代码参考出处
```bash
wget -r -np -nH --cut-dirs=2 -R "index.html*" "https://jyywiki.cn/os-demos/concurrency/sum-locked/"
```
- 为了实现自旋锁钥匙的交换，程序中使用到了汇编语言`cmpxchgl`的特性，实现了原子级比较与交换操作，从而保证了多线程同步的正确性。
- 为了实现compile barrier，防止编译器交换语句顺序，程序中使用到了一个空的汇编程序来确保时序一致：
    ```C
    asm volatile("" ::: "memory");
    ```
### 2. 发展前景
随着LLM的发展，自动化编译优化与AI代码生成技术（如基于LLM的代码生成器）可能在未来部分替代写死的编译器或是人工编写汇编，实现更大的性能提升。如在本例中，编译器可以依靠LLM读懂程序要实现排序算法，进而选择时间复杂度更低的实现，如快速排序，极大程度提升性能，可以做到比Ofast优化还快。
## 源代码
已上传至[Github仓库](https://github.com/lzclzclzclzclzclzc/WhichOneFaster_C_vs_ASM)

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

## 四、运行
```bash
bash Ocompare.sh
```

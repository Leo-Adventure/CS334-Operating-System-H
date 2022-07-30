## 期中**quiz**概念题 Solution

> **Author: MQ-Adventure**

[TOC]

### 列出三种从用户模式转向内核模式的方法

- System Call
- Interrupt
- Trap or Exception

### 列出 system call 的每一步

用户进程调用 system call -> trap，mode bit 置零，进入内核态 -> 执行系统调用 -> 调用 return 从内核态返回，修改 mode bit 为 1 -> 回到用户态

### 中断、异常的过程

停止现程序的运行 -> 启动 handler 的执行 -> 处理器通过 IDT 访问 handler -> handler 处理返回

### 对比微内核与宏内核

微内核优点：

- 更加 responsive（由于抢占式）
- 更好的稳定性以及安全性（由于更少代码）
- 更好的对于并发以及分布式 OS 的支持

缺点:

- 需要更多进程间的通信以及上下文切换，导致速度较慢

### fork(), exec(), wait(), exit() 的内核视角

**fork()**

1. 复制父进程的 PCB以及用户空间（user space）
2. 更新 PCB 为 +1，运行时间重置为 0，更新父节点指针
3. 返回值更改为0，父进程返回值是子进程的进程号

**exec()**

1. 清空用户空间的局部变量、动态分配的内存
2. 将全局变量设置为新代码的全局变量，代码部分更改为新代码的部分

**exit()**

1. 将所有分配的内存都释放，将所有开启的文件都关闭
2. 将进程的用户空间全部释放，包括代码以及分配的内存
3. 使用  `SIGCHLD` 通知父进程

**wait()**

1. 为调用 `wait()` 的进程登记一个信号处理路线（signal handling routine）
2. 当 `SIGCHLD`信号来的时候，调用对应的信号处理路线
3. 信号处理路线会接收并移除 `SIGCHLD` 信号，之后在内核空间当中删除子进程的 PCB以及 `task-list`
4. 内核解除父进程对信号处理路线的登记，并将终止的子进程的 `pid`作为返回值

### 描述优先级倒置（Priority Inversion）以及解决方案

高优先级的进程抢占了低优先级的进程运行，但是低优先级的进程拥有锁，导致高优先级的进程不能够进入临界区域运行，所以会导致高优先级的进程空转。

解决方案：调度程序通过**随机**提高 read y线程（在本例中为低优先级锁持有者）的优先级来解决这个问题。低优先级线程运行足够长的时间**退出临界区**，而高优先级线程可以进入临界区。

### 使用信号量（semaphore）为生产者消费者问题（有限缓冲问题）提供一种解决方案

使用二值信号量作为锁来实现**互斥**（mutual exclusion），保证写入的数据不会互相覆盖

使用两个计数信号量来实现**同步**（synchronization），保证不会有(生产者唤醒生产者)或者(消费者唤醒消费者)的情况出现。

### OS 如何避免一个进程访问其他进程的物理内存

利用 **内存虚拟化（Memory Virtualization）**技术

### OS 如何避免一个进程修改内核中的页表

Dual mode

### 列出发生页错误处理的步骤

1. 页错误处理器寻找在物理内存中可用的页帧（如果没有可用的页帧，就需要调用页替换算法，选择一个 to be replaced 的页帧，修改所有相关的 PTE 以及 TLB 的值都设置为 `invalid`，如果该页是脏页就写回磁盘）
2. 将需要的页从磁盘中获取并且存储在内存可用的页帧当中
3. 从页错误异常中返回，并且重新执行引起该异常的指令，重新根据 PTE 建立 TLB 的表项

### 补充：几种算法

**进程调度**：竞争使用 CPU （RR, SJF((non)preemptive), Priority scheduling((non)preemptive)）

**内存分配**：best fit, worst fit, next fit, first fit

**页替换**：MIN, Random, LRU, LFU

**进程内存分配算法**：Equal Allocation, Proportional Allocation, Priority Allocation

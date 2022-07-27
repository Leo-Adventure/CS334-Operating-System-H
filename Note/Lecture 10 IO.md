## Lecture 10 I/O

**设备的两大组成**：

- Interface（status，data，command）
- Internal Structure

**CPU 执行I/O 的两种方式**：

- Polling
- Interrupt

**Polling 的步骤**

1. OS 不断查看设备状态，直到设备进入 Ready
2. OS 通过设备的数据寄存器（data register）向其写入数据
3. OS 通过设备的指令寄存器（command register）写入命令
4. OS 通过 polling 等待设备完成指令

**Interrupt 步骤**：

1. CPU 设备驱动器发起 I/O
2. I/O controller 发起 I/O
3. I/O controller 结束 I/O 之后发起中断
4. CPU 接收中断，进入 Interrupt Handler 进行处理。

**Polling 和 Interrupt 比较**

- 对于 fast device，Polling 更快
- 对于 slow device，Interrupt 更快（因为上下文切换耗费时间）
- 杂交方法（hybrid）：先 poll 一段时间，之后使用 interrupt

**Interrupt 软硬件实现**

- 硬件：Interrupt-request Line 以及 Interrupt-controller hardware
- 软件：Interrupt handler 以及 A table of interrupt vectors 

**DMA 步骤**：

1. CPU 告知设备驱动器传输硬盘数据到内存
2. 设备驱动器告知硬盘控制器传输硬盘数据
3. 硬盘控制器初始化 DMA Transfer，并将每一个字节传输给 DMA 控制器
4. DMA 控制器传输数据到内存，在结束之后通过中断告知 CPU

**设备交互方法（Method of Device Interaction）（Approaches to control devices）**：

- 直接 I/O 指令交互(Explicit I/O instructions)
- 内存映射 I/O(Memory-mapped I/O)

设备驱动器（Device Driver）的两个组件

- Top half.实现了一组标准、跨设备的指令，如 open(), close()...
- Bottom half. 处理中断，在 I/O 结束之后会唤休眠线程


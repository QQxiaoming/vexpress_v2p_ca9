[![CI](https://github.com/QQxiaoming/vexpress_v2p_ca9/actions/workflows/ci.yml/badge.svg?branch=main)](https://github.com/QQxiaoming/vexpress_v2p_ca9/actions/workflows/ci.yml)
[![CodeFactor](https://www.codefactor.io/repository/github/qqxiaoming/vexpress_v2p_ca9/badge)](https://www.codefactor.io/repository/github/qqxiaoming/vexpress_v2p_ca9)
[![License](https://img.shields.io/github/license/qqxiaoming/vexpress_v2p_ca9.svg?colorB=f48041&style=flat-square)](https://github.com/QQxiaoming/vexpress_v2p_ca9)

# vexpress_v2p_ca9

基于arm的vexpress_v2p_ca9x4开发板编写的裸机工程项目，可以使用qemu模拟器运行，旨在深入学习实践启动文件，多核管理，cache，mmu等相关内容以及学习使用Rust语言开发底层驱动。

### 开发记录

 - 2021.03.22 建立工程，基本的串口打印正确。
 - 2021.03.24 增加多核boot代码。
 - 2021.04.11 MMU配置起来，目前是直接平面映射的。
 - 2021.05.03 FreeRTOS初步移植完成（目前仅在单核上运行）。
 - 2021.06.13 初步规划内存映射如下图，初步完成MMU配置，目前整个程序完全使用虚拟地址，不再使用物理地址，为后续实现用户进程准备。
 - 2021.06.20 粗略的编写的任务的页表建立和上下文中页表切换，需改进，后续需要完善页表内存的分配策略、使用ttbr1固化内核部分的页表项、二级页表的规划等。(内存分配有的麻烦，不希望把这个做的太复杂，我又不是想重写一个像linux内核一样的轮子，希望能轻量化实现用户态，另外考虑不使用elf文件做用户态的可执行程序，一切往轻量化走，另外驱动也想设计成灵活的用户态程序，不丢在内核里，目前考虑最好的办法由内核暴露地址映射给用户态驱动，哎，进程通讯又是个麻烦，这个内核代码设计要打破常规，彻底抛弃旧有的用户态，驱动等体系设计)

```
+----------------------+ 0xFFFFFFFF  <------------+
|                      |                          |
|                      |                          |
|    PERIPH            |                          |
|                      |                          |
|                      |                          |
+----------------------+ 0xF0000000               |
|                      |                          |
|                      |                          |
|                      |                          |
+----------------------+                          |
|    SRAM  (32M)       |                          |
+----------------------+ 0xE0000000               | KERNEL(1G)
|                      |                          |
|                      |                          |
| DDR high addr (512M) |                          |
|                      |                          |
|                      |                          |
+----------------------+ 0xD0000000               |
|                      |                          |
|                      |                          |
| DDR low addr  (512M) |                          |
|                      |                          |
|                      |                          |
+----------------------+ 0xC0000000  <------------+
|                      |                          |
|                      |                          |
|                      |                          |
|                      |                          |
|                      |                          |
|                      |                          |
|                      |                          |
|                      |                          |
|                      |                          |
|                      |                          |
|         3G           |                          | USER(3G)
|                      |                          |
|                      |                          |
|                      |                          |
|                      |                          |
|                      |                          |
|                      |                          |
|                      |                          |
|                      |                          |
|                      |                          |
|                      |                          |
+----------------------+ 0x00000000  <------------+
```

 - 2022.01.14 最近打算继续更新这个项目，先移植个简单的CLI程序

 ```
          _______  _______  _______  _______   _____  
|\     /|/ ___   )(  ____ )(  ____ \(  ___  ) / ___ \ 
| )   ( |\/   )  || (    )|| (    \/| (   ) |( (   ) )
| |   | |    /   )| (____)|| |      | (___) |( (___) |
( (   ) )  _/   / |  _____)| |      |  ___  | \____  |
 \ \_/ /  /   _/  | (      | |      | (   ) |      ) |
  \   /  (   (__/\| )      | (____/\| )   ( |/\____) )
   \_/   \_______/|/       (_______/|/     \|\______/ 
2022 by 2014500726@smail.xtu.edu.cn
Welcome debugging vexpress_v2p_ca9 project!
V2PCA9@FreeRTOS>help
help:			Lists all the registered commands
version:		showing version
uname:			Echos uname in turn
echo:			Echos each in turn
ps:			showing the state of each FreeRTOS task
free:			showing the mem of each FreeRTOS heap
V2PCA9@FreeRTOS>
 ```

---
title: Use structure to read/write register's value
layout: post
tags: [Embedded System, C, GCC, Compiler Optimization]
published: true
comments: true
---

這幾天從大神同事那裡聽到這個小技巧, 分享給大家

### Tip: 建議盡量使用structure來存取Register，可以獲得以下好處
    1. 讓compiler對base address計算做最佳化 (with -O1)，讓程式更有效率
    2. 易寫、易讀、易懂！ 

正文開始!

#### 讓compiler對base address計算做最佳化 (with -O1)，讓程式更有效率
基本概念: [Placing C variables at specific addresses to access memory-mapped peripherals](http://infocenter.arm.com/help/index.jsp?topic=%2Fcom.arm.doc.faqs%2Fka3750.html&fbclid=IwAR07VcCJ__NegBh-oGHd_WnzBOFxHiPIcThPQdmd2RanhODYNSOhtOKUbUY)

> **The ARM compiler will normally use a 'base register' plus the immediate offset field available in the load/store instruction to compile struct member or specific array element access.**
> 
> In the ARM instruction set, LDR/STR word/byte instructions have a 4KB range, but LDRH/STRH instructions have a smaller immediate offset of 256 bytes. Equivalent 16-bit Thumb instructions are much more restricted - LDR/STR have a range of 32 words, LDRH/STRH have a range of 32 halfwords and LDRB/STRB have a range of 32 bytes.  However, 32-bit Thumb instructions offer a significant improvement.  **Hence, it is important to group related peripheral registers near to each other if possible. The compiler will generally do a good job of minimising the number of instructions required to access the array elements or structure members by using base registers.**

注意以上粗體部分，大意上是說ARM compiler原本就會使用**base register**加上offset來對struct member與array element來做存取，所以如果我們將一組連續位置的register用struct或array來定義，就可以也套用上述的**base register**存取方式。

聽的霧煞煞對吧XD，直接看例子比較快，如果我們直接用下面這樣的方法去寫A/B/C

```c
#define REG_BASE_ADDR (0x10000000FFFFF00)
#define REG_A (REG_BASE_ADDR + 0x8)
#define REG_B (REG_BASE_ADDR + 0x10)
#define REG_C (REG_BASE_ADDR + 0x18)
#define READ_REG(reg, val) val = *((volatile unsigned long *) (reg))
#define WRITE_REG(reg, val) *((volatile unsigned long *) (reg)) = val

void foo(unsigned long a_val, unsigned long b_val, unsigned long c_val){
    WRITE_REG(REG_A, a_val);
    WRITE_REG(REG_B, b_val);
    WRITE_REG(REG_C, c_val);
}
```

從[Compiler Explorer](https://godbolt.org/)(ARM64 GCC 8.2 **-O2**)測試的assembly結果如下([https://godbolt.org/z/3MRiMJ](https://godbolt.org/z/3MRiMJ))

可以看到需要分別計算A/B/C register的base address(Line2~ 10)才能寫值
```
foo:
        mov     x5, 65288
        mov     x4, 65296
        movk    x5, 0xfff, lsl 16
        movk    x4, 0xfff, lsl 16
        movk    x5, 0x100, lsl 48
        mov     x3, 65304
        movk    x4, 0x100, lsl 48
        movk    x3, 0xfff, lsl 16
        movk    x3, 0x100, lsl 48
        str     x0, [x5]
        str     x1, [x4]
        str     x2, [x3]
        ret
```

而如果改成下面的寫法，利用structure來存取 ([https://godbolt.org/z/g-eJmz](https://godbolt.org/z/g-eJmz))

```c
#define REG_BASE_ADDR (0x10000000FFFFF00)
typedef struct
{
    unsigned long BASE;
    unsigned long REG_A;
    unsigned long REG_B;
    unsigned long REG_C;
} my_register;

#define READ_REG(reg, val) do{ \
    volatile my_register *base = (my_register *) REG_BASE_ADDR; \
    val = base->reg; \
} while(0)

#define WRITE_REG(reg, val) do{ \
    volatile my_register *base = (my_register *) REG_BASE_ADDR; \
    base->reg = val; \
} while(0)

void foo(unsigned long a_val, unsigned long b_val, unsigned long c_val){
    WRITE_REG(REG_A, a_val);
    WRITE_REG(REG_B, b_val);
    WRITE_REG(REG_C, c_val);
}

```

產生的Assembly如下，可以看到只需要去計算Base Address一次，並存在base register **x3**，接著直接透過offset去讀A/B/C，**整整少了一半的指令數！**對於斤斤計較MCPS的Hard Real Time Context來說可是有天壤之別！

```
foo:
        mov     x3, 268435200
        movk    x3, 0x100, lsl 48
        str     x0, [x3, 8]
        str     x1, [x3, 16]
        str     x2, [x3, 24]
        ret
```

#### 易寫、易讀、易懂！

再來看第二個優點，這點對我來說跟甚至比程式效率還重要，而這其實也是Structure本身最大的用途：**用對工程師最友善的方式來描述資料**

例如Register A的Spec如下 (little-endian)


    Bit    | 0 1 2 3 4 5 6 7 8------15-------------- 63 |
    Field  |   X    |   Y   |   Z     |       W         |


假如要對Y寫值(故意挑個中間的!)，以 bit operation操作的話會寫成如下，我想看到`SET_REG_A_Y_FIELD`就知道我的意思了!! 實在是有夠麻煩，當Regitser / Field一多根本沒辦法維護

```c
#define REG_BASE_ADDR (0x10000000FFFFF00)
#define REG_A (REG_BASE_ADDR + 0x8)
#define REG_B (REG_BASE_ADDR + 0x10)
#define REG_C (REG_BASE_ADDR + 0x18)
#define READ_REG(reg, val) val = *((volatile unsigned long *) (reg))
#define WRITE_REG(reg, val) *((volatile unsigned long *) (reg)) = val

#define SET_REG_A_Y_FIELD(val) do { \
	unsigned long cur_val = READ_REG(REG_A, cur_val); \
	cur_val = (cur_val & ~0xF0) | ( (val & 0xF) << 0x4); \
    WRITE_REG(REG_A, cur_val); \
} while (0)

void foo(unsigned char n){
    SET_REG_A_Y_FIELD(n);
}
```

Compile Result (ARM64 GCC 8.2 **-O2**) [https://godbolt.org/z/kftmDw](https://godbolt.org/z/kftmDw) 
```
foo:
        mov     x2, 65288
        ubfiz   x0, x0, 4, 4
        movk    x2, 0xfff, lsl 16
        movk    x2, 0x100, lsl 48
        ldr     x1, [x2]
        and     x1, x1, -241
        orr     x0, x0, x1
        str     x0, [x2]
        ret
```

而Structure的寫法如下，有沒有非常簡單!? 
甚至，能夠指定任意的Field，而且指令數還更少!! 
因為ARM(MIPS /x86也有)有支援直接對Register某一群bit讀/寫值，下面的`bfi`就是，這樣就可以省掉AND與OR操作了。
(Reference: [3.8.1. BFC and BFI](http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0552a/BABIEACC.html))
>BFI copies a bitfield into one register from another register. It replaces width bits in Rd starting at the low bit position lsb, with width bits from Rn starting at bit[0]. Other bits in Rd are unchanged.



```c
#define REG_BASE_ADDR (0x10000000FFFFF00)

typedef struct
{
    unsigned long X:4;
    unsigned long Y:4;
    unsigned long Z:8;
    unsigned long W:48;
} reg_a_t;

typedef struct
{
    unsigned long BASE;
    reg_a_t REG_A;
    unsigned long REG_B;
    unsigned long REG_C;
} my_register;

#define READ_REG(reg, val) do{ \
    volatile my_register *base = (my_register *) REG_BASE_ADDR; \
    val = base->reg; \
} while(0)

#define WRITE_REG(reg, val) do{ \
    volatile my_register *base = (my_register *) REG_BASE_ADDR; \
    base->reg = val; \
} while(0)

#define SET_REG_A_FIELD(field, val) do { \
    WRITE_REG(REG_A.field, val); \
} while (0)


void foo(unsigned char n){
    SET_REG_A_FIELD(Y,n);
}

```
Compile Result (ARM64 GCC 8.2 **-O2**) [https://godbolt.org/z/eIyDNY](https://godbolt.org/z/eIyDNY) 
```
foo:
        mov     x1, 268435200
        movk    x1, 0x100, lsl 48
        ldr     x2, [x1, 8]
        bfi     x2, x0, 4, 4
        str     x2, [x1, 8]
        ret
```

順帶一提，其實近年來compiler的優化已經越做越極致了，在想自幹assembly前先去翻翻[GCC Optimization Option](https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html)還有用Compiler Explorer玩一玩再決定吧!!

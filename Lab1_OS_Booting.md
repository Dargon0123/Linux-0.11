[toc]



# 1、系统启动引导图

其实，系统OS启动的时候，也就对应这个三个文件操作而已，分别是`bootsect.s, setup.s`和`head.s`的执行，你看起来是非常高深莫测的，其实不然，分析之后，会发现系统启动的本质上，是一块块的内存搬运而已。

最初系统编译完之后，整个系统相当于是一堆二进制文件有序存放在系统硬盘里面，下面展示系统代码在编译之后存储到磁盘中相应的位置，其中涉及编译、链接和存储到硬盘的操作。可以看出，硬盘中相继存放着`bootsect, setup, system`模块。

<p align="center">
  <img src="https://raw.githubusercontent.com/Dargon0123/Linux-0.11/Lab1_OS_Boot\L1_Graph\Lab1_os_booting-内存搬运01.png" alt="可爱的猫咪" />
  <br>
  <strong>图1-1</strong>
</p>


接着，这里先放一张全景的内存搬运图。从下面的图片上做一个视觉化的理解，从内存使用上展示系统引导与启动的过程。展示系统在最初启动的时候，就是按照下面这样位置将三个模块进行一次次的搬运操作。

<p align="center">
  <img src="https://raw.githubusercontent.com/Dargon0123/Linux-0.11/Lab1_OS_Boot\L1_Graph\Lab1_os_booting-内存搬运02.png" alt="可爱的猫咪" />
  <br>
  <strong>图1-2</strong>
</p>


再接程序开始执行，进行head.s里面，看一些具体的程序设定操作，这里设置了分别设置了idt、gdt和paging，随后就跳转到main函数里面进行执行操作了。同是，在图中也标出了GDTR、IDTR和CR3着三个寄存器的重要位置。

<p align="center">
  <img src="https://raw.githubusercontent.com/Dargon0123/Linux-0.11/Lab1_OS_Boot\L1_Graph\Lab1_os_booting-head.s.png" alt="可爱的猫咪" />
  <br>
  <strong>图1-3</strong>
</p>




# 2、`BIOS`功能

## 2.1、可寻址空间/物理内存



首先，和BIOS相关的，可能注意到内存最上面的`ROM BIOS映射区`，这个映射地址从`0xF 0000~0x10 0000`总计有`640KB`空间大小，这边地址映射到RAM上，也就是插在电脑上的内存条。为什么需要映射过去呢，直接访问物理内存条不可以吗？

在cpu眼里，插在主板上的内存条并不是都是它眼里的全部可使用的内存。

我们在学习单片机的时候，知道，在CPU看来，其所能使用的内存（可以访问到的内存）是由其地址总线的宽度决定的。例如8086的地址总线是20位，访问最大内存为`2^20=1Mb`，32位的CPU的地址总线是`2^32=4GB` ，这些都是在逻辑上的，地址总线可以访问到的最大内存空间。这你就可以联想到单片机的地址总线和数据总线，多大的宽度就可以访问地址空间多少（当然可以外扩内存空间），数据宽度决定一次读写多少bit。

但是，上面的这个可寻址的内存空间，并不等于我们的物理内存。原因是因为：计算机中，并不是只有插在主板上的内存条需要通过地址总线进行访问，还有很多的外设同样需要通过地址总线来访问，这类设备很多。只好在地址总线上，提前预留出来一些地址空间给这些外设用，比如这片100MB的连续地址用来访问显存，这边100Mb连续地址给硬盘控制器，这片200MB给串口。留够了之后，剩余的地址空间的可用地址，用来访问RAM，也就是插在物理内存上的内存条。

这也就是说，我们平时的安插在主板的内存条并不是全部都被用到了，毕竟需要留下来一些地址空间访问外设。所以8GB的内存条，只作为内存RAM使用的的只有7.8GB的原因，少的0.2GB可能用来访问其它外设的。

<p align="center">
  <img src="https://raw.githubusercontent.com/Dargon0123/Linux-0.11/Lab1_OS_Boot/L1_Graph/ubuntu内存信息.png" alt="可爱的猫咪" />
  <br>
  <strong>图2-1</strong>
</p>


## 2.2、BIOS开始执行

BIOS是存放在ROM中的，是计算机上电执行的第一段程序，对于8086而言，此ROM被映射到8086的1MB内存空间的顶部64KB上，就是上图1-2展示的地址`0xF0000~0xFFFFF`对应的64KB空间上。程序只要访问该64KB地址范围内，由于映射的关系，就是访问到硬盘中的BIOS程序，这个映射是由硬件完成的。

BIOS程序主要是用于计算机开机时执行系统给部分的自检操作，建立系统需要使用的配置表，如中断向量表。硬盘参数表。并且把处理器和系统初始化到一个已知的状态。

在最初的时候，调用的一些中断就是BIOS里面的中断，等系统初始化的时候，再继续将自己的中断处理函数，覆盖掉BIOS这部分。Liunx除了在初始化的时候会利用BIOS所提供的一些系统参数，等到正式运行的时候并不使用BIOS中的功能。





# 3、`bootsect.s`内存搬运

基本搬运规则和搬运流程均在图1-2里面进行详细的展示，通过汇编语言进行整块整块内存上的操作。



# 4、`setup.s`系统设置

这里主要是包括system系统代码的搬运操作，一些基础功能的基础的初始化操作，比如显示信息，磁盘啊等等，包括一些shell里面所打印的信息，都是在这里进行的，还有最初的gdt的初始化设置，随后跳转到head.s里面执行，在head.s里面。



# 5、`head.s`迈向`main`函数

## 5.1、设置setup_idt

这理的作用是，当系统检查到中断来的时候，通过这个idt里面的表，查询对应的中断函数，然后跳过去执行该中断函数。所以现在系统还没有启动完成，这里相当于初始化所有的idt里面的中断函数为ignore_idt，我们先来看看这个idt里面的对应的结构描述符

<p align="center">
  <img src="https://raw.githubusercontent.com/Dargon0123/Linux-0.11/Lab1_OS_Boot\L1_Graph\idt_descr.png" alt="可爱的猫咪" />
  <br>
  <strong>图5-1</strong>
</p>


这里面对应着进行interrupt gate的部分

以及这个初始化是如何填充这个数据结构的，形成下面这样数据内容的

<p align="center">
  <img src="https://raw.githubusercontent.com/Dargon0123/Linux-0.11/Lab1_OS_Boot\L1_Graph\Lab1_os_booting-idt.png" alt="可爱的猫咪" />
  <br>
  <strong>图5-2</strong>
</p>


我们接细细看一下代码

```assembly
setup_idt:
	lea ignore_int,%edx
	movl $0x00080000,%eax
	movw %dx,%ax		/* selector = 0x0008 = cs */
	movw $0x8E00,%dx	/* interrupt gate - dpl=0, present */

	lea idt,%edi
	mov $256,%ecx
rp_sidt:
	movl %eax,(%edi)
	movl %edx,4(%edi)
	addl $8,%edi
	dec %ecx
	jne rp_sidt
	lidt idt_descr
	ret
```

首先是通过`lea ignore_int,%edx`将函数`ignore_int`的地址，加载到32bits的edx寄存器里面，接下来，通过movl命令，将32bits的eax寄存器赋值为`0x00080000`，接下来的`movw %dx,%ax`将`ignore_int`低16bits放到eax的低16bits里面，此时eax的高16bits是0x0008，低16bits则是`&ignore_int[0:15]`。再紧接着通过`movw `

`$0x8E00,%dx`指令，此时的edx的高16bits仍旧是`&ignore_int[16:31]`低16bits被赋值为`0x8E00`。

这样拆解之后，对应上面的idt里面的结构就可以得知，idt里面对应的offset是ignore_int的地址，`Segment Selector`是`0x0008`，这里对应其结构为cs段，`0x8E00`则表示，`Segment Present flag`是存在的，`gate: 1 = 32 bits`;

后面就是循环为剩余的256个idt分别进行如上面同样的数据结构进行赋值操作。也就是上面所说的，将idt都预先初始化为`ignore_idt`中断处理函数。



## 5.2、设置setup_gdt

在setup.s里面有对gdt进行一个初始化3个段，null、代码段和数据段，在这里重新设置gdt里面的256项目，总计2kb=8*256项，的内容大小。后面会新增一些每个task所对应的内容null、TSS和LDT也会存放在GDT里面。

对应部分的代码来看

```assembly
gdt_descr:
	.word 256*8-1		# so does gdt (not that that's any
	.long gdt		# magic number, but it works for me :^)

	.align 8
idt:	.fill 256,8,0		# idt is uninitialized

gdt:	.quad 0x0000000000000000	/* NULL descriptor */
	.quad 0x00c09a0000000fff	/* 16Mb */
	.quad 0x00c0920000000fff	/* 16Mb */
	.quad 0x0000000000000000	/* TEMPORARY - don't use */
	.fill 252,8,0			/* space for LDT's and TSS's etc */
```

对应到实际的内存布局上就是，其中对于quad表示直接是64bits的宽度，用于填充GDT表项。在x86架构中，GDT表项的格式是固定的，每个表项占用8个字节。这个立即数的每一位都被设置为0，表示这是一个空描述符。后面进行依次填入内存里即可。

<p align="center">
  <img src="https://raw.githubusercontent.com/Dargon0123/Linux-0.11/Lab1_OS_Boot\L1_Graph\Lab1_os_booting-setup_gdt.png" alt="可爱的猫咪" />
  <br>
  <strong>图5-3</strong>
</p>




## 5.3、分页setup_paging

看下对应的寄存器的结构

<p align="center">
  <img src="https://raw.githubusercontent.com/Dargon0123/Linux-0.11/Lab1_OS_Boot\L1_Graph\PDE_struct.png" alt="可爱的猫咪" />
  <br>
  <strong>图5-4</strong>
</p>


细节

<p align="center">
  <img src="https://raw.githubusercontent.com/Dargon0123/Linux-0.11/Lab1_OS_Boot\L1_Graph\PDE_strcu_detail.png" alt="可爱的猫咪" />
  <br>
  <strong>图5-5</strong>
</p>


对应的是一个32bits的地址，细致观察，前12bits都是属性类似的，后面的20bits是对应page table/frame的地址，刚好$2^{12}$可以对应4KB，所以page与page之间都是4kb，也就是一个PDE对应着一个4kb的地址空间。

通过GDB进行跟踪一下细节

```shell
0x0000548e in setup_paging ()
(gdb) x /10xw 0x0
0x0 <startup_32>:       0x00001007      0x00002007      0x00003007      0x00004007
0x10 <startup_32+16>:   0x00000000      0x00000000      0x00000000      0x00000000
0x20 <startup_32+32>:   0x00000000      0x00000000
(gdb) p /x $cr0
$11 = 0x13
(gdb) p /x $cr3
$12 = 0x0
```

从内存地址0x0开始的连续4个32bits的地址就是对应的4个PDE页目录项，之后里面PTE页表项都填写之后，就会出现每一个里面其实1024个页表项，比如用0x1007对应的pg0来进行站看里面的内容。（后续第Lab3部分会提到，进程0fork进程1的时候，会仅仅赋值160个页表项）

```shell
(gdb) x /180xw 0x1000
0x1000 <pg0>:   0x00000007      0x00001007      0x00002007      0x00003007
0x1010 <pg0+16>:        0x00004007      0x00005027      0x00006007      0x00007007
0x1020 <pg0+32>:        0x00008007      0x00009007      0x0000a007      0x0000b007
0x1030 <pg0+48>:        0x0000c007      0x0000d007      0x0000e007      0x0000f007
0x1040 <pg0+64>:        0x00010007      0x00011007      0x00012007      0x00013007
0x1050 <pg0+80>:        0x00014007      0x00015007      0x00016007      0x00017007
0x1060 <pg0+96>:        0x00018007      0x00019007      0x0001a007      0x0001b007
0x1070 <pg0+112>:       0x0001c007      0x0001d007      0x0001e007      0x0001f007
0x1080 <pg0+128>:       0x00020007      0x00021007      0x00022007      0x00023007
0x1090 <pg0+144>:       0x00024007      0x00025007      0x00026007      0x00027007
0x10a0 <pg0+160>:       0x00028007      0x00029027      0x0002a007      0x0002b007
0x10b0 <pg0+176>:       0x0002c007      0x0002d007      0x0002e007      0x0002f007
0x10c0 <pg0+192>:       0x00030007      0x00031007      0x00032007      0x00033007
0x10d0 <pg0+208>:       0x00034007      0x00035007      0x00036007      0x00037007
0x10e0 <pg0+224>:       0x00038007      0x00039007      0x0003a007      0x0003b007
0x10f0 <pg0+240>:       0x0003c007      0x0003d007      0x0003e007      0x0003f007
0x1100 <pg0+256>:       0x00040007      0x00041007      0x00042007      0x00043007
0x1110 <pg0+272>:       0x00044007      0x00045007      0x00046007      0x00047007
## 省略部分
```







# 6、细节

## 6.1、实模式-->保护模式

牵扯到地址的转换，就需要说明实模式和保护模式的物理地址的区别，就是程序中，给出一个地址，经过CPU的计算，如何转换到实际在内存中的对应的地址的。

其实，这主要是x86的历史包袱问题，主要是从兼容之前的16bit的cpu，即便现在都已经是32bit、64bits的了。就是从最初的16位的实模式，转为32位的保护模式，下面分别看看两种模式之间的转变。

* 实模式

我们看最初执行的两行代码

```c
	mov	$BOOTSEG, %ax	#将ds段寄存器设置为0x7C0
	mov	%ax, %ds
```

这个就是将`ds`数据段寄存器赋值为`0x07C0`但是，由于x86为了让自己在16bit模式下，能访问到20bit的地址总线，这个历史因素，所以无论是代码段基地址cs和数据段ds来说，都需要左移4bits，这样，以左移过的地址为基地址，刚好可以访问到20bit的地址总线。

所以，上面的ds里面的值，经过这套操作下来，就是0x7c00为基地址了，所以后面所有的数据的访问都是以0x7c00作为偏移的，这就是实模式下的地址访问，比较实在的

比如，接下来就是将boot这部分的代码，从0x7c00搬到0x9000这个位置上（可以**#1.系统启动引导图**），从代码层面上，看看这部分的搬迁流程

```assembly
mov	$256, %cx		#设置移动计数值256字
sub	%si, %si		#源地址	ds:si = 0x07C0:0x0000
sub	%di, %di		#目标地址 es:si = 0x9000:0x0000
rep					#重复执行并递减cx的值
movsw				#从内存[si]处移动cx个字到[di]处
```

其中对于movsw，就是内存级别的移动，其中两个基地址寄存器，ds和es都是左移4位下进行的。si对应着偏移地址。

* 保护模式

当进入保护模式下，地址的计算方式，也就发生了变化，不在是实模式下的仅段基地址左移4bits完成的，变成了ds寄存器里面存储的值，是段选择子，其值对应的结构内容是这样的

<p align="center">
  <img src="https://raw.githubusercontent.com/Dargon0123/Linux-0.11/Lab1_OS_Boot\L1_Graph\segment_selector.png" alt="可爱的猫咪" />
  <br>
  <strong>图6-1</strong>
</p>


其中对应的index部分，在GDT里面的索引index，按照该index，在GDT这张表里面，找到对应位置的代码段或者数据段，这种代码段和数据段的里面包含着段基地址，其对应的结构是下面这样的，

<p align="center">
  <img src="https://raw.githubusercontent.com/Dargon0123/Linux-0.11/Lab1_OS_Boot\L1_Graph\segment_desc.png" alt="可爱的猫咪" />
  <br>
  <strong>图6-2</strong>
</p>


把相应的段基址取出来，加上后面的偏移地址，就是保护模式下的对应的内存中实际地址了。

CPU通过48bit的gdtr寄存器，里面包含着GDT的位置，该寄存器的结构如下

<p align="center">
  <img src="https://raw.githubusercontent.com/Dargon0123/Linux-0.11/Lab1_OS_Boot\L1_Graph\gdtr.png" alt="可爱的猫咪" />
  <br>
  <strong>图6-3</strong>
</p>


在代码中看一下，GDT里面的结构就一目了然了，比如在`setup.s`里面是这样进行初始化操作的

```assembly
lidt	idt_48		# load idt with 0,0
lgdt	gdt_48		# load gdt with whatever appropriate

idt_48:
	.word	0			# idt limit=0
	.word	0,0			# idt base=0L

gdt_48:
	.word	0x800			# gdt limit=2048, 256 GDT entries
	.word   512+gdt, 0x9		# gdt base = 0X9xxxx, 
	# 512+gdt is the real gdt after setup is moved to 0x9020 * 0x10


```

对应的gdt_48这个标签，就是48bit的，3个word组成，从寄存器里面，0x800就是table limit了，下面的两个word写成0x形式就是，0x90200+gdt，对应的gdt也就是setup.s在编译的时候，所对应的一个相对基地址偏移的标签而已。

```assembly
gdt:
	.word	0,0,0,0		# dummy

	.word	0x07FF		# 8Mb - limit=2047 (2048*4096=8Mb)
	.word	0x0000		# base address=0
	.word	0x9A00		# code read/exec
	.word	0x00C0		# granularity=4096, 386

	.word	0x07FF		# 8Mb - limit=2047 (2048*4096=8Mb)
	.word	0x0000		# base address=0
	.word	0x9200		# data read/write
	.word	0x00C0		# granularity=4096, 386
```

我们直接在内存里面，将这部分代码里面的GDT展示划到实际的内存位置里面去逐个对应起来，就是如下了。

<p align="center">
  <img src="https://raw.githubusercontent.com/Dargon0123/Linux-0.11/Lab1_OS_Boot\L1_Graph\Lab1_os_booting-gdt.png" alt="可爱的猫咪" />
  <br>
  <strong>图6-4</strong>
</p>


对应的实际地址转换模式就是这样，最终转变为实际物理地址。

<p align="center">
  <img src="https://raw.githubusercontent.com/Dargon0123/Linux-0.11/Lab1_OS_Boot\L1_Graph\Lab1_os_booting-保护模式地址.png" alt="可爱的猫咪" />
  <br>
  <strong>图6-5</strong>
</p>


* 代码段：比如要访问代码段的内容，此时你的cs寄存器里面的值对应为0x08，0000 1000对应的index=1；也就是gdt[1]；

* 数据段访问：同理如果要访问数据段的内容，此时的ds寄存器里面一定是0x10,0001 0000，对应的index=2，也就是gdt[2]。





## 6.2、向下兼容

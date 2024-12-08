[toc]



# 🥭1、系统调用分析

系统调用是一个桥梁链接用户态和内核态，通过`int $0x80`中断这座桥进行联通。Linus编写这些系统调用函数的原因是在内核加载完之后，会切换到用户模式下，接着做一些初始化工作，然后启动`shell`。而用户模式下的很多工作都需要依赖一些系统调用来完成，因此在内核中实现了这些系统调用API。

比如，我们调用常见的`open、read、exit、fork`这类是用户态函数，程序执行到这儿的时候，通过中断的方式调用内核态里面对应的一系列函数。

这些常使用的函数，linus在0.11系统里面，已经为我们实现好了，并没有看上去的那么神秘，我们接下来通过`open`函数，来正经看看这条调用链路。



## 1.1、从系统自带函数出发

这些给用户态函数调用的API都存放在`lib/`路径下，我们来看看`lib/open.c`里面的`open`函数背后的实现过程。这一类函数，有的是直接调用sys_call的宏定义，比如write，

```c
#define __LIBRARY__
#include <unistd.h>

_syscall3(int,write,int,fd,const char *,buf,off_t,count)

// syscaall宏定义
#define _syscall0(type,name) \
  type name(void) \
{ \
long __res; \
__asm__ volatile ("int $0x80" \
	: "=a" (__res) \
	: "0" (__NR_##name)); \
if (__res >= 0) \
	return (type) __res; \
errno = -__res; \
return -1; \
}
```

有的是直接使用了sys_call里面的内容，比如open函数

```c
int open(const char * filename, int flag, ...)
{
	register int res;
	va_list arg;

	va_start(arg,flag);
	__asm__("int $0x80"
		:"=a" (res)
		:"0" (__NR_open),"b" (filename),"c" (flag),
		"d" (va_arg(arg,int)));
	if (res>=0)
		return res;
	errno = -res;
	return -1;
}
```

对比上面两部分代码，其实展示之后，是同样的。其中最核心的就是`__asm`开始的汇编代码，`int $0x80 `这个为中断入口（准确的说，是异常，编程异常，exception，由于中断比较熟悉，下面都以中断来叙述，具体区分，在细节小节更新✨），后面的是一些寄存器变量的赋值。

那既然所有的系统调用都会产生这个0x80软中断，CPU就接着在IDT表里面，寻找0x80所对应的中断处理函数。带这个问题，那就去寻找在哪里设置的0x80这个中断号的处理函数。

```c
void sched_init(void)
{
	// 省略……
	set_intr_gate(0x20,&timer_interrupt);
	outb(inb_p(0x21)&~0x01,0x21);
	set_system_gate(0x80,&system_call);
}
```

在main的sched_init里面，设置了该中断函数为system_call，接着去调用这个中断处理函数。



## 1.2、system_call中断处理

先来略看一下

```assembly
system_call:
	cmpl $nr_system_calls-1,%eax
	ja bad_sys_call
	push %ds
	push %es
	push %fs
	pushl %edx
	pushl %ecx		# push %ebx,%ecx,%edx as parameters
	pushl %ebx		# to the system call
	movl $0x10,%edx		# set up ds,es to kernel space
	mov %dx,%ds
	mov %dx,%es
	movl $0x17,%edx		# fs points to local data space
	mov %dx,%fs
	call *sys_call_table(,%eax,4)
	pushl %eax
	movl current,%eax
	cmpl $0,state(%eax)		# state
	jne reschedule
	cmpl $0,counter(%eax)		# counter
	je reschedule
```

1. 参数入栈

CPU通过中断进入sys_call函数里面，该由cpu自动保存的寄存器已经入栈了，开始执行函数内部的时候，就接着将`ds,es,fs`数据段寄存器入栈，再将`edx,ecx,ebx`存放着参数的寄存器逆序入栈。

逆序的顺序来看，参数值就是如下内容。

```c
:"0" (__NR_open),"b" (filename),"c" (flag),
// %eax = __NR_open; 系统调用的宏
// %ebx = filename;  文件名
// %ecx = flag; 标志位
```

从这句内联汇编可以看出，Linux0.11系统系统调用，最多只能传递3个参数进去，但是可以扩展✨

2. 调整数据段

<p align="center">
  <img src="F:\Codefield\Code_C\EverNote_typora\Linux0.11总结\L3_Graph\segment_selector.png" alt="可爱的猫咪" />
  <br>
  <strong>图1-1</strong>
</p>

接着将`ds,es`设置为0x10，从数据段寄存器的描述上来看，该数据段对应着GDT[2]数据段。将`fs`设置为0x17，则是指向LDT的用户数据段。所以`fs`寄存器便承担内核空间和用户空间数据交换的桥梁。接着就是真正实现系统调用的`call *sys_call_table(,%eax,4)`，通过查表`sys_call_table`形式，找一个对对应`%eax= __NR_fork`位置的函数，其在函数中定义如下

```c
// 省略……
extern int sys_setregid();
extern int sys_iam();
extern int sys_whoami();
extern int sys_iam2();
extern int sys_whoami2();

fn_ptr sys_call_table[] = { sys_setup, sys_exit, sys_fork, sys_read,
sys_write, sys_open, sys_close, sys_waitpid, sys_creat, sys_link,
sys_unlink, sys_execve, sys_chdir, sys_time, sys_mknod, sys_chmod,
sys_chown, sys_break, sys_stat, sys_lseek, sys_getpid, sys_mount,
sys_umount, sys_setuid, sys_getuid, sys_stime, sys_ptrace, sys_alarm,
sys_fstat, sys_pause, sys_utime, sys_stty, sys_gtty, sys_access,
sys_nice, sys_ftime, sys_sync, sys_kill, sys_rename, sys_mkdir,
sys_rmdir, sys_dup, sys_pipe, sys_times, sys_prof, sys_brk, sys_setgid,
sys_getgid, sys_signal, sys_geteuid, sys_getegid, sys_acct, sys_phys,
sys_lock, sys_ioctl, sys_fcntl, sys_mpx, sys_setpgid, sys_ulimit,
sys_uname, sys_umask, sys_chroot, sys_ustat, sys_dup2, sys_getppid,
sys_getpgrp, sys_setsid, sys_sigaction, sys_sgetmask, sys_ssetmask,
sys_setreuid,sys_setregid, sys_iam, sys_whoami, sys_iam2, sys_whoami2 };
```

对应的就去调用到我们需要的`sys_fork`函数。具体调用分析，我们在`Lab_03`里面详述。



## 1.3、小总结

基本过程：

1. 通过应用程序调用库函数；
2. 库函数通过自己的函数内部触发0x80中断或者调用`sys_call0~3`宏函数来触发中断；
3. 调用中断处理函数`system_call`，通过传进来的`%eax= __NR_fork`找到对应的位置的实际执行函数；
4. `system_call`调用结束之后，通过`%eax`寄存器将返回值传递给库函数；
5. 库函数处理该返回值返回给应用程序。

## 1.4、gdb看下栈信息

使用gdb打印看下系统调用sys_fork的时候栈里面的内容

```shell
# 刚进入函数时候
(gdb) p /x $esp
$2 = 0x2420c

(gdb) p /x $esp
$2 = 0x2420c
(gdb) x /20xw 0x2420c
0x2420c <init_task+4076>:       0x000068e1      0x0000000f      0x00000202      0x00029f10
0x2421c <init_task+4092>:       0x00000017      0x00029f40      0x00000010      0x00000000
```

接着顺序压入之后的栈里面的内容

```shell
(gdb) x /16xw $esp
0x241f0 <init_task+4048>:       0x00007999      0x00023000      0x000055e8      0x00000021
0x24200 <init_task+4064>:       0x00000017      0x00000017      0x00000017      0x000068e1
0x24210 <init_task+4080>:       0x0000000f      0x00000202      0x00029f10      0x00000017
0x24220 <stack_start>:  0x00029f40      0x00000010      0x00000000      0x00000000
```

栈数据示意图如下：

<p align="center">
  <img src="F:\Codefield\Code_C\EverNote_typora\Linux0.11总结\L2_Graph\Lab1_os_booting-fork_stack.png" alt="可爱的猫咪" />
  <br>
  <strong>图1-1</strong>
</p>





# 🍋2、多参数传参机制

仔细看看open函数内部，就可以发现，这中间使用了可变参数的形式，也就是函数内部包含了这样一个参数内容

```c
	__asm__("int $0x80"
		:"=a" (res)
		:"0" (__NR_open),"b" (filename),"c" (flag),
		"d" (va_arg(arg,int))); // 可变参数的传递
```

其实现机制和用发，可参考我们熟悉的`printf`打印函数即可。



# 🍊3、番外

## 2.1、文件交换

* 背景

`Ubunt` 系统和`linux0.11` 系统的文件交换

首先，需要将`linux0.11` 源目录下编译之后，会生成对应的`hdx-0.11.img` 镜像文件，挂载到我们的`ubuntu` 上

```shell
dargon@dd:~/my_linux/Linux-0.11$ ls
books  fs            Image    init    lib       Makefile.header  README.md   System.map
boot   hdc-0.11.img  include  kernel  Makefile  mm               readme.old  tools
```

✨具体生成的`Image` 和`hdc-0.11.img` 两个文件的意义，后续进行补充

* 第一次：失败

尝试在`linux0.11` 的系统`shell` 命令行内，直接进行文件的更改，但是对于该系统运行正在使用的文件，你更改之后，就不能被保存下来，等于是没有改，

且，你使用的编辑器仅有`vi` 一个编辑器，效率很低下，且不能完成目的。

* 第二次：失败

接着直接尝试进行挂载，但是在`WSL2` 中，不支持挂载`minix` 格式的文件系统，你需要是更改`wsl2` 中的系统，比较麻烦，初级选手不建议操作，有风险；

于是，直接转到熟悉的`vmware` 的`ubuntu` 里面。



* 第二次：成功

接下来，就是挂载这样一个`.img` 的系统文件镜像。

1. 首先，本地需要创建一个空的挂载目录

   ```shell
   mkdir /mnt/linux0.11
   ```

2. 使用`mount` 指令将`.img` 挂载到上面所创建的目录上

   ```shell
   sudo mount -o loop,offset=512 -t minix hdc-0.11.img /mnt/linux0.11/
   ```

3. 接着，可以直接访问创建的目录了，其实里面就是一个`minix` 的文件系统了，就可以直接增删改查文件了。更方便的是，你可以直接通过`vscode + ssh` 直接进行登录，操作了。狂喜。

   ```shell
   dargon@dd:/mnt/linux0.11$ ls
   bin  dev  etc  image  mnt  tmp  usr  var
   ```

4. 最后，挂载完文件系统，使用之后，需要卸载掉，就如同你使用U盘一样，拷贝之后，需要拔除操作。另外，需要注意的是，拔除之前，需要关闭所有对这个文件系统的操作，不然，会报如下的`busy` 的错误。

   ```shell
   dargon@dd:~/my_linux/Linux-0.11$ sudo umount /mnt/linux0.11 
   umount: /mnt/linux0.11: target is busy.
   ```

   ```shell
   dargon@dd:~/my_linux/Linux-0.11$ sudo umount /mnt/linux0.11 
   ```






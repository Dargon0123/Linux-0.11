<<<<<<< HEAD
Linux-0.11
==========

The old Linux kernel source ver 0.11 which has been tested under modern Linux,  Mac OSX and Windows.

## 1. Build on Linux

### 1.1. Linux Setup

* a linux distribution: debian , ubuntu and mint are recommended
* some tools: gcc gdb qemu
* a linux-0.11 hardware image file: hdc-0.11.img, please download it from http://www.oldlinux.org, or http://mirror.lzu.edu.cn/os/oldlinux.org/, ant put it in the root directory.
* Now, This version already support the Ubuntu 16.04, enjoy it.

### 1.2. hack linux-0.11
```bash
$ make help		// get help
$ make  		// compile
$ make start		// boot it on qemu
$ make debug		// debug it via qemu & gdb, you'd start gdb to connect it.
```
```gdb
$ gdb tools/system
(gdb) target remote :1234
(gdb) b main
(gdb) c
```

## 2. Build on Mac OS X

### 2.1. Mac OS X Setup

* install cross compiler gcc and binutils
* install qemu
* install gdb. you need download the gdb source and compile it to use gdb because port doesn't provide i386-elf-gdb, or you can use the pre-compiled gdb in the tools directory.
* a linux-0.11 hardware image file: hdc-0.11.img

```bash
$ sudo port install qemu
$ sudo port install i386-elf-binutils i386-elf-gcc
```

optional
```bash
$ wget ftp://ftp.gnu.org/gnu/gdb/gdb-7.4.tar.bz2
$ tar -xzvf gdb-7.4.tar.bz2
$ cd gdb-7.4
$ ./configure --target=i386-elf
$ make
```

### 2.2. hack linux-0.11
same as section 1.2


## 3. Build on Windows
todo...
=======
[toc]



# 1、背景

针对自己在秋招的面试中，根据自己面试情况中，在后面的面试复盘中，针对于面试官要求最后的反问环节的一些注意细节，从反问的角度对于下面问题：
* 我们应该如何进行提问？
* 应该分别针对不同阶段的面试提问哪些内容？
* 提问上应该抱有那些心态？
* ……

自己从每次面试，这里做一次简单的总结分享一下，希望能帮助到各位同学



# 2、番外

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



* 第三次：成功

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





>>>>>>> 384a71b... [代码模块][Lab2_Sys_Call/课程章节][BugID:]

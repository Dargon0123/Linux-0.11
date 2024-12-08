[toc]



# 1、背景

test insert img

<p align="center">
  <img src="https://raw.githubusercontent.com/Dargon0123/Linux-0.11/Lab2_Sys_Call/L2_Graph/segment_desc.png" alt="可爱的猫咪" />
  <br>
  <strong>图1-1</strong>
</p>



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






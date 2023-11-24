# GC JIT 抑制

思路源自抖音技术分享

适配 Android 多版本

通过 hook ConcurrentGCTask 和 JitCompileTask 实现相应的抑制功能，降低启动阶段 CPU 占用，以取得所谓的启动数值上的优化

## 20230702 更新

通过 ab 实验，并没有发现明显收益，不知道抖音大佬们如何做出收益来的···

可能是 inlinehook 的耗时低效掉了，建议直接手写替换函数地址

## 原理

**找到 GC 和 JIT 对应 Task 的 函数符号，然后替换为我们自定义的方法**

有类似的 github 开源库 [gc抑制](https://github.com/RicardoJiang/android-performance/blob/main/startup-optimize/src/main/cpp/StartUpOptimize.cpp)

其实现基本同抖音技术分享，通过查找函数地址，并找虚函数，然后替换为自定义方法。  

**但是其实现有一些问题，一个是 libart 版本路径问题，一个是原方法为成员方法，方法参数根本对不齐的问题**

此处，直接使用 [inlinehook](https://github.com/bytedance/android-inline-hook) 实现方法替换

当然你也可以使用 [xdl](https://github.com/hexhacking/xDL) 来达到上述仓库中 enhanced_dlfcn 相应功能，也不用考虑 libart 路径问题

`注意，inlinehook 和 xdl 只支持 armeabi-v7a v8a`

## 细节
**如何查看函数符号：**
1. 获取 libart.so(各个版本路径不同)  
   可以从模拟器中 pull 出来
2. 查看 libart.so 的符号表  
    mac 机器上可以直接使用 nm 命令，也可以使用 | grep 过滤自己想要查看的符号
3. 如何阅读源码  
   https://cs.android.com/ 可以从这里看，搜索 concurrentGC 就可以找到 gc 相关代码

找到相应的符号后，我们便可以 dlopen + dlsym 获取到对应的函数地址，但是因为 Android N 的限制，需要我们使用类似 enhanced_dlfcn/xdl 等 dlopen 拓展库

同时，如果按照上述 github 方案实现的话，还需要注意 .symtab 和 .dynsym 的问题，enhanced_dlfcn/xdl 均支持

之后，便是函数替换的过程了。

如果是 inlinehook 就绕过了

如果是 enhanced_dlfcn/xdl 需要通过函数地址进一步遍历、读写

## 补充

ConcurrentGCTask 只是众多 GC 类型中的一种，即使我们 hook 住，并且不调用原方法(相当于废掉 ConcurrentGC)，应用也不会 OOM，因为还有其他 GC 可以工作，至少有 Alloc GC

但是要注意 Alloc GC 发生在内存分配时，会阻塞当前线程

且在高端机型下，应用冷启动发生 GC 的次数有限，以我的测试手机来看，平均启动过程中发生 2-3 次 ConcurrentGCTask，可能优化效果并不明显，也许低端机会有一些额外收益？

JIT 发生次数较为频繁，可能收益更多一些

#
觉得 ok 给个 star ⭐️ 把！

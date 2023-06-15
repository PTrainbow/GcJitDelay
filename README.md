# GC JIT 抑制

适配 Android 多版本

通过 hook ConcurrentGCTask 和 JitCompileTask 实现相应的抑制功能，降低启动阶段 CPU 占用，以取得所谓的启动数值上的优化

## 原理

**找到 GC 和 JIT 对应 Task 的 函数符号，然后替换为我们自定义的方法**

有类似的 github 开源库 [gc抑制](https://github.com/RicardoJiang/android-performance/blob/main/startup-optimize/src/main/cpp/StartUpOptimize.cpp)

但是其实现问题很多，一个是 libart 版本路径问题，一个是原方法为成员方法，方法参数根本对不齐的问题

此处，直接使用 [inlinehook](https://github.com/bytedance/android-inline-hook) 实现方法替换

当然你也可以使用 [xdl](https://github.com/hexhacking/xDL) 来达到上述仓库中 enhanced_dlfcn 相应功能，也不用考虑 libart 路径问题

## 补充

ConcurrentGCTask 只是众多 GC 类型中的一种，即使我们 hook 住，并且不调用原方法(相当于废掉 ConcurrentGC)，应用也不会 OOM，因为还有其他 GC 可以工作，至少有 Alloc GC

但是要注意 Alloc GC 发生在内存分配时，会阻塞当前线程

且在高端机型下，应用冷启动发生 GC 的次数有限，以我的测试手机来看，平均启动过程中发生 2-3 次 ConcurrentGCTask，可能优化效果并不明显，也许低端机会有一些额外收益？

JIT 发生次数较为频繁，可能收益更多一些

#
你的 star⭐️  
我的动力

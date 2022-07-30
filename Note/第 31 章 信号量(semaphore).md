### 第31章 信号量(semaphore)

信号量是一个整数值的对象，可以用两个函数来操作它，在 POSIX 标准下，是 `sem_wait()` 以及 `sem_post()`。

`sem_wait()` 将信号量的值 -1，如果信号量的值小于零，那么就进入等待状态。`sem_post()` 是将信号量的值 + 1，如果此时信号量小于等于零，代表有在等待的进程，那么唤醒一个等待中的进程。

#### 信号量的定义

信号量的初始化会决定其行为，使用 `sem_init(&s, 0, 1);` 对其进行初始化，这里将其初始化为 1。

#### 二值信号量（锁）

通过将锁的初始值设置为 1，执行 `sem_wait()`，此时只有开始的进程会能够进入临界区域，其他的进程在调用 `sem_wait()` 的时候会因为信号量的值小于零而进入等待状态。进入临界区域执行完之后，调用 `sem_post()`给信号量的值加一。

```cpp
sem_wait(&s);
cnt++;
sem_post(&s);
```

#### 信号量用作条件变量

通过将信号量的初始值设置为 0，并且在子进程当中调用 `sem_post()` 即可。

两种情况需要考虑

1. 子进程先执行，此时信号量会被增加为 1，父进程执行的时候调用 `sem_wait()` 可以正常执行
2. 父进程先执行，此时调用 `sem_wait()`，将信号量设置为 -1，进入等待状态，进入子进程执行之后，通过 `sem_post()` 的自增方法，将信号量重新变为 0，进入父进程正常执行结束。

#### 生产者/消费者问题

注意在增加互斥的时候要注意避免因为互斥量距离临界区太远而导致的死锁。

解决方案就是将互斥量调整为紧挨着临界区。

#### 读者-写者锁(reader-writer lock)

当有读者的时候，避免所有的写操作。

通过在有第一个读者的时候，获取写锁，此时由于写锁刚好为零，所以当写者需要写入信息的时候，由于此时写锁已经为0，则不能写入。当读者数量为 0 的时候， `sem_post()` 写锁，此时可以正常写入。

#### 哲学家就餐问题

通过特判最后一名哲学家，使其拿起右手边的叉子，即可破除哲学家相互之间的依赖关系。

#### 如何实现信号量

通过使用底层的同步原语（锁和条件变量）来实现信号量

```cpp
#include <semaphore.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

typedef struct _Zem_t{
    int value; // 信号量的值
    pthread_cond_t cond; // 条件变量
    pthread_mutex_t lock; // 锁

} Zem_t;

void Zem_init(Zem_t *s, int value){ // 初始化
    s -> value = value;
    Cond_init(&s -> cond);
    Mutex_init(&s -> lock);
}

void Zem_wait(Zem_t *s){
    Mutex_lock(&s -> lock); // 使用锁 保护临界区域 使得操作原子性
    while(s -> value <= 0){
        Cond_wait(&s -> cond, &s -> lock); // 进入休眠状态，等待唤醒
    }
    s -> value --; // 减少信号量的值
    Mutex_unlock(&s -> lock);
}

void Zem_post(Zem_t *s){
    Mutex_lock(&s -> lock);
    s -> value ++; // 增加信号量的值
    Cond_signal(&s -> cond); // 发送信号唤醒等待的线程
    Mutex_unlock(&s -> lock);
}

```


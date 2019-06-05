/**
 * 生产者-消费者问题
 * 使用互斥量+条件变量
 * PantaSun
 * 2019-06-05
 * 可以多个生产者，多个消费者并发运行
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define WAREHOUSE_COUNT 100                          // 仓库容量，比如一个仓库只能存放这么多商品，仓库满了的时候，生产者应该阻塞
#define TOTAL_COUNT 2000                            // 生产总量，超过后不再生产 
#define PRODUCER_COUNT 6                           // 生产者个数
#define CONSUMER_COUNT 3                          // 消费者个数

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  // 静态分配互斥锁
                                                    // 静态分配条件变量
pthread_cond_t cond_del = PTHREAD_COND_INITIALIZER; // 消费者条件变量
pthread_cond_t cond_add = PTHREAD_COND_INITIALIZER; // 生产者条件变量

                                                    
int prod_count = 0;                                 // 设置一个全局变量，即仓库中产品数量，消费者减少该值，生产者增加该值
int total_count = 0;                                // 总生产量，因为涉及多个生产者，所以该变量也是一个全局变量
//int flag = 0;                                       // 停止生产标志，达到生产总数后不再生产的标识


/**
 * 生产者
 * arg：生产者名字，用区分是谁生产的某件商品
 * return：NULL
 */
void * producer( void * arg){

    char *pname = (char *)arg;                      // 获取生产者名字
    printf("%s START:\n", pname);
    
    while (1)
    {
        
        pthread_mutex_lock(&mutex);                 // 要访问全局变量首先要加锁
       
  
        while (prod_count >= WAREHOUSE_COUNT)        // 如果仓库满了，则进入睡眠，等待消费者消费
        {
            pthread_cond_wait(&cond_add, &mutex);
        }
        if(total_count>=TOTAL_COUNT)                // 判断是否已经停产，若是则退出
        {
            pthread_cond_signal(&cond_del);         // 激活消费者线程
            pthread_mutex_unlock(&mutex);           // 解锁
            break;                                  
        }
        prod_count++;                               // 生产
        total_count++;                              // 总量加一
        

        printf("%s 生产了 1 个产品, 仓库产品数：%d，已经生产：%d\n", pname, prod_count, total_count);
      

        pthread_cond_signal(&cond_del);             // 激活消费者线程
        pthread_mutex_unlock(&mutex);               // 解锁互斥量
        //sleep(1);
    }

    printf("%s EXIT.\n", pname);
    pthread_exit(NULL);    

}


/**
 * 消费者
 * arg：消费者名字，用区分是谁消费的某件商品
 * return：NULL
 */
void * consumer(void * arg){


    char *cname = (char *)arg;                       // 获取消费者名字
    printf("%s START:\n", cname);

    while (1)
    {
        pthread_mutex_lock(&mutex);                  // 加锁

        while(prod_count <= 0)                       // 当仓库为空
        {   if(total_count < TOTAL_COUNT)                            // 仓库为空，且未停止生产
                pthread_cond_wait(&cond_del, &mutex);// 进入随眠，等待生产者生产商品
            else                                     // 若仓库为空，且停产则退出
            {
                pthread_mutex_unlock(&mutex);        // 解锁
                printf("%s Exit\n", cname);
                pthread_exit(NULL);
            }
        }
    
 
        prod_count--;                                // 消费
        printf("%s 消费了一件产品, 仓库产品数量=%d\n", cname, prod_count);
        
        pthread_cond_signal(&cond_add);              // 激活生产者
        pthread_mutex_unlock(&mutex);                // 解锁
        
    }
    printf("%s Exit\n", cname);
    pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
    pthread_t prod[PRODUCER_COUNT];
    pthread_t cons[CONSUMER_COUNT];
   
    char *pName[PRODUCER_COUNT] = {};
    char *cName[CONSUMER_COUNT] = {};
            
    for (int i = 0; i < PRODUCER_COUNT; i++)
    {
        char* pn = NULL; 
        pn = (char*)malloc(sizeof(char) * (strlen("producer") + 2)); 
        sprintf(pn, "producer%d", i+1); 
        pName[i] = pn; 
        pthread_create(&prod[i], NULL, producer, (void*)pName[i]);
    }
    
    for (int i = 0; i < CONSUMER_COUNT; i++)
    {
        char* cn = NULL; 
        cn = (char*)malloc(sizeof(char) * (strlen("consumer") + 2)); 
        sprintf(cn, "consumer%d", i+1); 
        cName[i] = cn; 
        pthread_create(&cons[i], NULL, consumer, (void *)cName[i]);
    }
   
    
   
    for (int i = 0; i < PRODUCER_COUNT; i++)
    {
        pthread_join(prod[i], NULL);
        free(pName[i]);                 // 动态申请空间需要手动释放，下同
    }
    
       
    for (int i = 0; i < CONSUMER_COUNT; i++)
    {
        pthread_join(cons[i], NULL);
        free(cName[i]);
    }
    
    printf("仓库：%d, 总量%d\n", prod_count, total_count);
 
    return 0;
}

 
    /* 
    使用条件变量时，线程在不满足条件时会被挂起。 
    在消费品都消费完毕，生产者线程停止生产后， 
    消费者线程因为缓冲区内容为空，将一直挂起。 
    需设置一个全局变量start，当生产者已经停止时，需告知其他消费者解除挂起的状态。 
    */ 
     
    #include <stdio.h>   
    #include <stdlib.h>   
    #include <string.h>   
    #include <pthread.h>   
    #define NUMS 10000  //表示生产，消费的次数   
    #define CAPACITY 50 //定义缓冲区最大值   
    int capacity = 0; //当前缓冲区的产品个数   
    pthread_mutex_t mylock = PTHREAD_MUTEX_INITIALIZER;//互斥量   
    pthread_cond_t mycond = PTHREAD_COND_INITIALIZER;//条件变量   
     
    int start = 0; 
       
    void *produce(void *args)   
    {   
        int i = 0;   
        for (; i < NUMS; )   
        {   
            pthread_mutex_lock(&mylock);//加锁   
            if (capacity >= CAPACITY) //当前产品个数大于等于缓冲区最大值，则不把产品放入缓冲区。   
            {   
                printf("缓冲区已满，无法放入产品\n");   
                pthread_cond_wait(&mycond, &mylock); 
            } else {//将产品放入缓冲区   
                ++capacity;   
                printf("生产者存入一个产品, 缓冲区大小为:%d\n", capacity);   
                i++;   
            }   
            pthread_mutex_unlock(&mylock);   
            pthread_cond_broadcast(&mycond); 
        }   
         
        pthread_mutex_lock(&mylock);//加锁   
        printf("生产者finish!!!!\n");   
        start = 1; 
        pthread_mutex_unlock(&mylock);  
        pthread_cond_broadcast(&mycond); 
     
        return ((void *) 0);   
    }   
       
    void * consume(void *args)   
    {   
        int i = 0;   
        char *p = (char*)args; 
         
        printf("消费者%s start!!!!!!!!!\n", p);   
        for (; i < NUMS; )   
        {   
            pthread_mutex_lock(&mylock);   
            if (capacity > 0)    
            {   
                --capacity;   
                printf("消费者%s消耗一个产品,缓冲区大小为:%d\n", p, capacity);   
                i++;   
            }  
            else if(capacity <= 0 && start != 1)         
            { //当没获取到资源时，将被wait挂起。当其他线程退出后，将不会有broadcast对wait进行解除。 
              //这将导致循环未结束的线程挂起。所以需引入下一个分支的判断 
      
                printf("%s 缓冲区已空，无法消耗产品\n", p);   
                pthread_cond_wait(&mycond, &mylock); 
                  
            }  
            else if(capacity <= 0 && start == 1)  
            {//start为1，说明生产者进程已经停止，所以应该释放锁，同时，跳出循环，结束线程。 
                pthread_mutex_unlock(&mylock);   
                //pthread_cond_broadcast(&mycond);//可以不加？？？ 
                break; 
            } 
     
            pthread_mutex_unlock(&mylock);   
            pthread_cond_broadcast(&mycond); 
        }   
     
        pthread_mutex_lock(&mylock);   
        printf("消费者%s finish!!!!!!!!!\n", p);   
        pthread_cond_broadcast(&mycond); 
     
        pthread_mutex_unlock(&mylock);   
        return ((void *) 0);   
    }   
       
    #define CNUM 5 
     
    int main(int argc, char** argv) {   
       
        int err;   
        pthread_t produce_tid;   
        pthread_t consume_tid[CNUM];   
        void *ret;   
        err = pthread_create(&produce_tid, NULL, produce, NULL);//创建线程   
        if (err != 0)    
        {   
            printf("线程producer创建失败:%s\n", strerror(err));   
            exit(-1);   
        }   
     
        int i = 0; 
        char *pfree[CNUM] = {0}; 
     
        for(i= 0; i < CNUM; i++) 
        { 
            char* pc = NULL; 
            pc = (char*)malloc(sizeof(char) * (strlen("consumer") + 2)); 
            sprintf(pc, "consumer%d", i); 
            pfree[i] = pc; 
     
            err = pthread_create(consume_tid + i, NULL, consume, (void*)pc);   
            if (err != 0)   
            {   
                printf("线程consumer创建失败:%s\n", strerror(err));   
                continue; 
                //exit(-1);   
            }   
        } 
     
        err = pthread_join(produce_tid, &ret);//主线程等到子线程退出   
        if (err != 0)    
        {   
            printf("生产着线程分解失败:%s\n", strerror(err));   
            exit(-1);   
        }   
     
        for(i= 0; i < CNUM; i++) 
        { 
            free(pfree[i]); 
            err = pthread_join(consume_tid[i], NULL);   
            if (err != 0)   
            {   
                printf("消费者线程分解失败:%s\n", strerror(err));   
                continue;   
            }   
        } 
        return (EXIT_SUCCESS);   
    }   
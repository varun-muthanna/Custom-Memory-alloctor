#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include<string.h>

//
// Created by Varun Muthanna on 25/11/24.
//

typedef char PADD[16];

typedef union header{

    struct {
        bool isFree;
        size_t size;        //unsigned int (avoid negative sizes and compatible between 32 and 64 bit systems)
        union header *next ;
    }s;     //header.s.size vs header.size.

    PADD p;

}header_t; //uniform size of header_t = sizeof largest field that is PADD p

//head and tail for heap alloc
header_t *head=NULL;
header_t *tail=NULL;
pthread_mutex_t mutex;

header_t *get_free_block(size_t size){
   header_t *cur = head;

   while(cur){

       if(cur->s.size>=size && cur->s.isFree==true){
           return cur;
       }

       cur=cur->s.next;
   }

   return NULL;

}

void * my_malloc(size_t size){

    if(!size) return  NULL;

    size_t total_size = sizeof(header_t ) +  size;

    pthread_mutex_lock(&mutex);

    header_t *h = get_free_block(size); //actual size is stored ( as mentioned  by user)

    if(h!=NULL){
        pthread_mutex_unlock(&mutex);
        return (void*)(h+1); // since pointer is initally of type header_t h+1, will offset by sizeof(header_t) (16 bytes) -- then return void* which will be typecasted in main program for use
    }

    void *block =  sbrk(total_size); // brk (address of heap top) + size

    if(block==(void*)-1){
        pthread_mutex_unlock(&mutex);
        return NULL;
    }

    h=block;
    h->s.size=size;
    h->s.isFree=false;
    h->s.next=NULL;

    if(tail){
        tail->s.next=h;
        tail=h;
    }

    if(head==NULL || tail==NULL){
        head=tail=h;
    }

    pthread_mutex_unlock(&mutex);

    return (void*)(h+1);
}

void my_free(void* ptr){

    if(!ptr) return ;

    header_t  *h = (header_t*)(ptr) - 1;

    void *brk = sbrk(0); // top heap address


    pthread_mutex_lock(&mutex);

    if((char*)(ptr) + h->s.size == brk){ //ptr is indexed to h->s.size , since it is char pointer moves only 1 byte per index

        if(head==tail){
            head=tail=NULL;
        }else {
            //move tail one back
            header_t *temp = head;

            while (temp->s.next != tail) {
                temp = temp->s.next;
            }

            temp->s.next = NULL;
            tail = temp;
        }

        size_t totalsize = sizeof(header_t) + h->s.size;
        sbrk(-totalsize); //deallocate (give back to OS)

    }else{
        h->s.isFree=true;
    }

    pthread_mutex_unlock(&mutex);
    return ;
}

void *my_calloc(size_t n, size_t bytes){

    if(!n || !bytes) return NULL;

    size_t size = n*bytes;

    if(size/bytes !=n) {
        return NULL; //overflow of size_t
    }

    void * h = my_malloc(size);

    if(!h) {
        return NULL;
    }

    memset(h,0,size);
    return h;
}

void *my_relloc(void *block , size_t size){

    if(!size || !block){
        return my_malloc(size); //will return NULL
    }

    header_t * h = (header_t*)(block) -1;

    if(h->s.size >=size){ //no change required
        return block;
    }

    void *ret = my_malloc(size);

    if(ret){
        memcpy(ret,block,h->s.size); //copies the  first  h->s.size bytes
        my_free(h);
    }

    return ret;
}



//int main() {
//
//    int *a = my_malloc(5*sizeof(int));
//
//    printf("Testing malloc , enter 5 numbers \n");
//
//    for(int i=0;i<5;i++) scanf("%d",&a[i]);
//
//    for(int i=0;i<5;i++)  printf("%d ",a[i]);
//
//    a=my_relloc(a,10*sizeof(int));
//
//    printf("\nTesting relloc , enter 5 more numbers \n");
//
//    for(int i=5;i<10;i++) scanf("%d",&a[i]);
//
//    for(int i=0;i<10;i++)  printf("%d ",a[i]);
//
//    my_free(a);
//
//    return 0;
//}

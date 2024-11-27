//
// Created by Varun Muthanna on 26/11/24.
//

#include "mymem.h"
#include<stdio.h>
#include<string.h>

int HASH_SIZE = 1024;


typedef struct Node{

    int key;
    int value;
    struct Node *prev; //prev and next only used in cache
    struct Node *next;
    struct Node *hash; //incase of collision we create a linked list of Node
}Node;

typedef struct LRU{ //captues the instance of LRU Cache

    int capacity;
    int size;

    struct Node *head; //most recently used
    struct Node *tail; //least recently used

    Node ** hashmap; //array where each index is linked list of Nodes , so idx = hash(key) and hashmap[idx] contains Node for key;

}LRU;

LRU * LRU_init(const int capacity){

    LRU *  l = (LRU*) my_malloc(sizeof(LRU));

    if(!l) return NULL;

    l->capacity=capacity;
    l->size=0;

    l->head=l->tail=NULL;
    l->hashmap = (Node**) my_calloc( HASH_SIZE , sizeof(Node*));

    if(!l->hashmap) return NULL;

    return l;

}

int Hash(int key){
    return key%HASH_SIZE;
}



void add(LRU* lru,Node * node){

    if(!lru->head){
        lru->head=lru->tail=node;
        return ;
    }

    node->next=lru->head;
    node->prev=NULL;

    lru->head->prev=node;
    lru->head=node; //making node the new head as it is latest get

}


void remNode(LRU * lru,Node *n){
    //no need to remove from hashmap .... always added after

    if(n->prev) {
        n->prev->next=n->next;
    }else{
        lru->head=n->next;
    }

    if(n->next){
        n->next->prev=n->prev;
    }else{
        lru->tail=n->prev;
    }
}



void remTail(LRU *lru){

    if(!lru->tail) {
        return ;
    }

    if(lru->tail==lru->head){
        lru->tail=lru->head=NULL;
        return ;
    }

    int idx=Hash(lru->tail->key);
    //code for removing from hashmap
    Node * h = lru->hashmap[idx];
    Node * prev= NULL;

    while(h){

        if(h==lru->tail){
            if(prev) prev->hash=h->hash;
            else lru->hashmap[idx]=h->hash;

            break ;
        }

        prev=h;
        h=h->hash;
    }

    //code for remove from cache
    Node *temp= lru->tail;
    lru->tail=lru->tail->prev;
    lru->tail->next=NULL;

    my_free(temp);
    --lru->size;
}


int get(LRU *lru,int key){

    int h = Hash(key);
    Node* n = lru->hashmap[h];

    while(n){

        if(n->key==key){
            remNode(lru,n);
            add(lru,n);

            return n->value;
        }
        n=n->hash;
    }

    return -1;
}

void put(LRU *lru,int key,int value){

    int h=Hash(key);

    Node *n = lru->hashmap[h];

    while(n){

        if(n->key==key){
            n->value=value;
            remNode(lru,n);
            add(lru,n);
            return ;
        }

        n=n->hash;
    }

    if(lru->size==lru->capacity) remTail(lru);


    Node * new = (Node*)(my_malloc(sizeof(Node)));
    new->key=key;
    new->value=value;
    new->prev=new->next=new->hash=NULL;

    new->hash=lru->hashmap[h]; //first position in hashmap[h]
    lru->hashmap[h]=new;

    add(lru,new);
    ++lru->size;

    //printf("size= %d\n",lru->size);

}

int main(){
    int capacity;

    printf("Enter capacity of LRU cache\n");
    scanf("%d",&capacity);

    LRU * lru = LRU_init(capacity);

    if(!lru){
        printf("Memory allocation error");
        return 0;
    }

    while(1){
        int op;
        printf("Enter 1 for get,2 for put, 3 for exit\n");

        scanf("%d",&op);

        if(op==1){
            int key;
            scanf("%d",&key);
            printf("%d\n",get(lru,key));
        }else if(op==2){
            int key,value;
            scanf("%d",&key);
            scanf("%d",&value);
            put(lru,key,value);
        }else{
            break;
        }
    }


    return 0;
}




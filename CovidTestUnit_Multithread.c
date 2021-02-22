#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

#define UNIT_NUM 8
#define WAITING_CAPACITY 3
#define MAX_PATIENT 50

void *unit_test(void *unitID);
void *patient(void *patientID);
int unit_select(); //to choose best unit for patient
void randwait(int secs);

int userCount[UNIT_NUM]={0,0,0,0,0,0,0,0};

sem_t unit[UNIT_NUM];// # is 4 
sem_t testMutex;//# is 1
sem_t unit_keeper[UNIT_NUM];//# is 0
int selectedunitID=0;//value of the unit id that keep which unit patient must come in

int main(int argc, char *argv){
    pthread_t Utid[UNIT_NUM];
    pthread_t Ptid[MAX_PATIENT];

    //initializing semaphores.
    for(int i=0; i<UNIT_NUM; i++){
        sem_init(&unit[i],0,WAITING_CAPACITY);
    }
    for(int i=0; i<UNIT_NUM; i++) {
        sem_init(&unit_keeper[i],0,0);
    }
    sem_init(&testMutex,0,1);
    
    printf("A solution to the covid test unit problem using semaphores.\n");
    //thread creation
    for(int i=0; i<UNIT_NUM; i++){
        pthread_create(&Utid[i],NULL,unit_test,(void *)i);            
    }
    for(int i=0; i<MAX_PATIENT; i++){
        pthread_create(&Ptid[i],NULL,patient,(void *)i);     
        randwait(5);
    }

    //thread end
    for(int i=0; i<UNIT_NUM; i++){
        pthread_join(Utid[i], NULL);
    }
    for(int i=0; i<MAX_PATIENT; i++){
        pthread_join(Ptid[i], NULL);
    }
    system("PAUSE");   
    return 0;
}
 
      
void *unit_test(void *unitID){
    int id= (int)unitID;
    int capacity;  //unit capacity which is determined by the semaphore value
    while(true){
        sem_getvalue(&unit[id],&capacity);
        if(capacity==3){//unit is empty
            printf("unit keeper %d ventilating unit %d\n",id+1,id+1);
            sem_wait(&unit_keeper[id]);
            printf("unit keeper %d finished ventilating unit %d\n",id+1,id+1); 
        }
        else{
            if(capacity==0){//unit is full
                printf("unit %d is full\n",id+1);
                //Enter critical region
                sem_wait(&testMutex);
                userCount[id]=userCount[id]+1;
                sem_post(&testMutex);   
                //Leave critical region
                printf("All patients leaving unit %d\n",id+1);
                randwait(5);
                //patients left the unit
                sem_post(&unit[id]);
                sem_post(&unit[id]);
                sem_post(&unit[id]);
                sem_post(&unit[id]);
            }
            else{
                sem_wait(&unit_keeper[id]);
            }
        }                                       
    }
}
void *patient(void *patientID){
    int id = (int)patientID;
    printf("patient %d comes to hospital\n",id+1);
     
    int unitID=unit_select(); //entering critical region in this function
    //and left critical region after function choose best unit
    sem_post(&testMutex);

    printf("patient %d entering unit %d\n",id+1,unitID+1);
    sem_wait(&unit[unitID]);//patient in unit to test
    sem_post(&unit_keeper[unitID]);//unit staff is waking

}

int unit_select(){
    //entering critical region and choosing best unit  
    sem_wait(&testMutex);    
    bool flag=true;
    selectedunitID=0; 
    int count=userCount[0];
    for(int i=0; i<UNIT_NUM; i++){
        if(count>userCount[i]){  
            selectedunitID=i;          
            flag=false;
            return selectedunitID;
        }
    }
    if(flag){
        return selectedunitID;
    }
}

void randwait(int secs) {
     int len = 1; // Generate an arbit number...
     sleep(len);
}
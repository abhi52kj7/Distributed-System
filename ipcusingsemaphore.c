#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

void die(char *msg)
{
  perror(msg);
  exit(1);
}

int main()
{
    int i,j;
    int pid;
    int semid; 
    key_t key ; 
    int semflg = IPC_CREAT | 0666; 
    int nsems = 1; 
    int nsops;
    struct sembuf *sops = (struct sembuf *) malloc(2*sizeof(struct sembuf));
    
    if ((key = ftok("ipcusingsemaphore.c", 'Q')) == -1) 
       die("ftok");
    

    printf("\nsemget: Setting up semaphore: semget(%#lx, %\
                    %#o)\n",key, nsems, semflg);
    if ((semid = semget(key, nsems, semflg)) == -1)
        die("semget: semget failed");


    if ((pid = fork()) < 0)
        die("fork");

    if (pid == 0)
    {
        i = 0;

        while (i  < 3)  
        {

            nsops = 2;

            sops[0].sem_num = 0; 
            sops[0].sem_op = 0; 
            sops[0].sem_flg = SEM_UNDO; 


            sops[1].sem_num = 0;
            sops[1].sem_op = 1;
            sops[1].sem_flg = SEM_UNDO | IPC_NOWAIT;


            printf("\nsemop:Child  Calling semop(%d, &sops, %d) with:", semid, nsops);
            for (j = 0; j < nsops; j++)
            {
                 printf("\n\tsops[%d].sem_num = %d, ", j, sops[j].sem_num);
                 printf("sem_op = %d, ", sops[j].sem_op);
                 printf("sem_flg = %#o\n", sops[j].sem_flg);
            }

            if ((j = semop(semid, sops, nsops)) == -1)
            {
                perror("semop: semop failed");
            }
            else
            {
                printf("\n\nChild Process Taking Control of Track: %d/3 times\n", i+1);
                sleep(5); 

                nsops = 1;

                sops[0].sem_num = 0;
                sops[0].sem_op = -1; 
                sops[0].sem_flg = SEM_UNDO | IPC_NOWAIT; 

                if ((j = semop(semid, sops, nsops)) == -1)
                {
                    perror("semop: semop failed");
                }
                else
                     printf("Child Process Giving up Control of Track: %d/3 times\n", i+1);
                sleep(5); 
            }
            ++i;
        }

    }
    else 
    {
        i = 0;

        while (i  < 3)  
        {

            nsops = 2;

            sops[0].sem_num = 0;
            sops[0].sem_op = 0; 
            sops[0].sem_flg = SEM_UNDO; 


            sops[1].sem_num = 0;
            sops[1].sem_op = 1; 
            sops[1].sem_flg = SEM_UNDO | IPC_NOWAIT; 

             printf("\nsemop:Parent Calling semop(%d, &sops, %d) with:", semid, nsops);
            for (j = 0; j < nsops; j++)
            {
                printf("\n\tsops[%d].sem_num = %d, ", j, sops[j].sem_num);
                printf("sem_op = %d, ", sops[j].sem_op);
                printf("sem_flg = %#o\n", sops[j].sem_flg);
            }

            if ((j = semop(semid, sops, nsops)) == -1)
            {
                perror("semop: semop failed");
            }
            else
            {
                printf("Parent Process Taking Control of Track: %d/3 times\n", i+1);
                sleep(5); 

                nsops = 1;

                sops[0].sem_num = 0;
                sops[0].sem_op = -1; 
                sops[0].sem_flg = SEM_UNDO | IPC_NOWAIT; 

                if ((j = semop(semid, sops, nsops)) == -1)
                {
                    perror("semop: semop failed");
                }
                else
                    printf("Parent Process Giving up Control of Track: %d/3 times\n", i+1);
                sleep(5); 
            }
            ++i;

        }

    }
}



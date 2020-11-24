/*
 * David Rubý
 * fakulta informačních technologií Vysokého učení technického v Brně
 * předmět: IOS
 * projekt 2
 * 26. 04. 2019
 */


#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <stdbool.h>


sem_t *serfs_queue;
sem_t *hacker_queue;
sem_t *mutex;
sem_t *sails;
sem_t *unboarding;
sem_t *captain_unboarding;

int *shared_counter = NULL;
int *hacker_counter = NULL;
int *serfs_counter = NULL;
int *hacker_id = NULL;
int *serfs_id = NULL;
int *unboarded = NULL;
bool *availible = NULL;

int shared_counter_id;
int hacker_counter_id;
int serfs_counter_id;
int hacker_id_id;
int serfs_id_id;
int unboarded_id;
int availible_id;

FILE *output_file;


/*====== function prototypes ================*/
void free_all();
int my_sleep(int milliseconds);
void generated(bool is_hacker, int R, int W, int C);



int main(int argc, char *argv[]){

    //checking the number of arguments
    if(argc != 7){
        fprintf(stderr, "ERROR:\tThe number of arguments is incorrect.\n");
        return 1;
    }

    //creating variables
    int P = atoi(argv[1]);
    int H = atoi(argv[2]);
    int S = atoi(argv[3]);
    int R = atoi(argv[4]);
    int W = atoi(argv[5]);
    int C = atoi(argv[6]);

    /*============================================ arguments value check ============================================*/

    if(!((P >= 2) && ((P % 2) == 0))){
        fprintf(stderr, "ERROR:\tInvalid value of 'P' argument.\nP = %d\tValue must be even and bigger or equal to 2.\n", P);
        return 1;
    }

    if(!((H >= 0) && (H <= 2000))){
        fprintf(stderr, "ERROR:\tInvalid value of 'H' argument.\nP = %d\tValue must be between 0 and 2000 (including).\n", H);
        return 1;
    }

    if(!((S >= 0) && (S <= 2000))){
        fprintf(stderr, "ERROR:\tInvalid value of 'S' argument.\nS = %d\tValue must be between 0 and 2000 (including).\n", S);
        return 1;
    }

    if(!((R >= 0) && (R <= 2000))){
        fprintf(stderr, "ERROR:\tInvalid value of 'R' argument.\nR = %d\tValue must be between 0 and 2000 (including).\n", R);
        return 1;
    }

    if(!((W >= 20) && (W <= 2000))){
        fprintf(stderr, "ERROR:\tInvalid value of 'W' argument.\nW = %d\tValue must be between 20 and 2000 (including).\n", W);
        return 1;
    }

    if(C < 5){
        fprintf(stderr, "ERROR:\tInvalid value of 'C' argument.\nC = %d\tValue must be bigger or equal to 5.\n", C);
        return 1;
    }


/*============================================== file closing ========================================================*/


    //opening up output file and checking successful opening.
    output_file = fopen("proj2.out", "w");
    if(output_file == NULL){
        fprintf(stderr, "ERROR:\tCould not open up an output file.\n");
        return 1;
    }

/*====================================================================================================================*/

/*========================================= shared variables initialization ===========================================*/

    //shared_counter
    if((shared_counter_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666)) == -1){
        fprintf(stderr,"ERROR:\tCould not set shared_counter_id.\n");
        free_all();
        exit(1);
    }

    if((shared_counter = shmat(shared_counter_id, NULL, 0)) == NULL){
        fprintf(stderr,"ERROR:\tCould not create shared_counter.\n");
        free_all();
        exit(1);
    }

    //hacker_counter
    if((hacker_counter_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666)) == -1){
        fprintf(stderr, "ERROR:\tCould not set hacker_counter_id.\n");
        free_all();
        exit(1);
    }

    if((hacker_counter = shmat(hacker_counter_id, NULL, 0)) == NULL){
        fprintf(stderr, "ERROR:\tCould not create hacker_counter.\n");
        free_all();
        exit(1);
    }

    //serfs_counter
    if((serfs_counter_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666)) == -1){
        fprintf(stderr, "ERROR:\tCould not set serfs_counter_id.\n");
        free_all();
        exit(1);
    }

    if((serfs_counter = shmat(serfs_counter_id, NULL, 0)) == NULL){
        fprintf(stderr, "ERROR:\tCould not create serfs_counter.\n");
        free_all();
        exit(1);
    }

    //hacker_id
    if((hacker_id_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666)) == -1){
        fprintf(stderr, "ERROR:\tCould not set hacker_id_id.\n");
        free_all();
        exit(1);
    }

    if((hacker_id = shmat(hacker_id_id, NULL, 0)) == NULL){
        fprintf(stderr, "ERROR:\tCould not create hacker_id.\n");
        free_all();
        exit(1);
    }

    //serfs_id
    if((serfs_id_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666)) == -1){
        fprintf(stderr, "ERROR:\tCould not set serfs_id_id.\n");
        free_all();
        exit(1);
    }

    if((serfs_id = shmat(serfs_id_id, NULL, 0)) == NULL){
        fprintf(stderr, "ERROR:\tCould not create serfs_id.\n");
        free_all();
        exit(1);
    }

    //unboarded
    if((unboarded_id = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666)) == -1){
        fprintf(stderr, "ERROR:\tCould not set unboarded_id.\n");
        free_all();
        exit(1);
    }

    if((unboarded = shmat(unboarded_id, NULL, 0)) == NULL){
        fprintf(stderr, "ERROR:\tCould not create unboarded.\n");
        free_all();
        exit(1);
    }

    //availible
    if((availible_id = shmget(IPC_PRIVATE, sizeof(bool), IPC_CREAT | 0666)) == -1){
        fprintf(stderr, "ERROR:\tCould not set availible_id.\n");
        free_all();
        exit(1);
    }

    if((availible = shmat(availible_id, NULL, 0)) == NULL){
        fprintf(stderr, "ERROR:\tCould not create availible.\n");
        free_all();
        exit(1);
    }


    *shared_counter = 1;
    *hacker_counter = 0;
    *serfs_counter = 0;
    *hacker_id = 0;
    *serfs_id = 0;
    *unboarded = 0;
    *availible = false;




/*====================================== semaphores initialization ===================================================*/

    if((serfs_queue = sem_open("/sem_serfs_queue", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED){
        fprintf(stderr, "ERROR:\tCould not create serfs_queue semaphore.\n");
        free_all();
        return 1;
    }

    if((hacker_queue = sem_open("/sem_hacker_queue", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED){
        fprintf(stderr, "ERROR:\tCould not create hacker_queue semaphore.\n");
        free_all();
        return 1;
    }

    if((mutex = sem_open("/sem_mutex", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED){
        fprintf(stderr, "ERROR:\tCould not create mutex semaphore.\n");
        free_all();
        return 1;
    }

    if((sails = sem_open("/sem_sails", O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED){
        fprintf(stderr, "ERROR:\tCould not create sails semaphore.\n");
        free_all();
        return 1;
    }

    if((unboarding = sem_open("/sem_unboarding", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED){
        fprintf(stderr, "ERROR:\tCould not create unboarding semaphore.\n");
        free_all();
        return 1;
    }

    if((captain_unboarding = sem_open("/sem_captain_unboarding", O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED){
        fprintf(stderr, "ERROR:\tCould not create captain_unboarding semaphore.\n");
        free_all();
        return 1;
    }



/*==========================================================================================================*/


/*======================================== processes generating ==============================================*/


    srand(time(0));

    pid_t pid_hackers_gen;
    pid_t pid_serfs_gen;
    pid_t pid_hackers[P];
    pid_t pid_serfs[P];
    //int status;

    /* ======= generates hackers generator process ============*/
    pid_hackers_gen = fork();
    if(pid_hackers_gen < 0){
        //error message, if the fork did not work
        fprintf(stderr, "ERROR:\tCould not generate hackers_gen process.\n");
        free_all();
        return 1;
    }
        //if the process is a child
    else if(pid_hackers_gen == 0){
        //hackers generator
        for(int i = 0; i < P; i++){
            //generates child hacker
            pid_hackers[i] = fork();
            if(pid_hackers[i] < 0){
                //error code, if the fork did not work
                fprintf(stderr, "ERROR:\tCould not generate child hacker process.\n");
                free_all();
                exit(1);
            }
            else if(pid_hackers[i] == 0){
                //if the process is a child
                generated(true, R, W, C);
                free_all();
                return 0;
            }
            //waits random number of milliseconds between 0 and H
            my_sleep(rand() % (H + 1));
        }
        //parent processes wait for their children and then terminate
        for(int i = 0; i < P; i++){
            waitpid(pid_hackers[i], NULL, 0);
        }
        free_all();
        exit(0);
    }




    /*======== generates serfs generator process =========*/
    pid_serfs_gen = fork();
    if(pid_serfs_gen < 0){
        //error, if the fork did not work
        fprintf(stderr, "ERROR:\tCould not generate serfs_gen process.\n");
        free_all();
        return 1;
    }
        //if the process is a child
    else if(pid_serfs_gen == 0){
        //serfs generator
        for(int i = 0; i < P; i++){
            //generated serf
            pid_serfs[i] = fork();
            if(pid_serfs[i] < 0){
                //error, if the fork did not work
                fprintf(stderr, "ERROR:\tCould not generate child serfs process.\n");
                free_all();
                exit(1);
            }
            else if(pid_serfs[i] == 0){
                //if the process is child
                generated(false, R, W, C);
                free_all();
                return 0;
            }
            //waits random number of milliseconds between 0 and S
            my_sleep(rand() % (S + 1));
        }
        //parent process waits for their child processes and then terminate
        for(int i = 0; i < P; i++){
            waitpid(pid_serfs[i], NULL, 0);
        }
        free_all();
        exit(0);
    }
    waitpid(pid_hackers_gen, NULL, 0);
    waitpid(pid_serfs_gen, NULL, 0);



    free_all();
    return 0;
}

/*======================================= functions definitions ===============================================*/

void generated(bool is_hacker, int R, int W, int C){

    int total_pier;
    bool is_captain = false;

    sem_wait(mutex);
    int id = is_hacker ? ++*hacker_id : ++*serfs_id;
    char *type = is_hacker ? "HACK" : "SERF";
    fprintf(output_file, "%d\t:%s %d\t: starts\n", *shared_counter, type, id);
    fflush(output_file);
    *shared_counter += 1;
    sem_post(mutex);


    //waiting to get to the pier
    do{
        //waiting to get to the pier
        sem_wait(mutex);
        total_pier = *hacker_counter + *serfs_counter;
        sem_post(mutex);
        if(total_pier >= C) {

            sem_wait(mutex);
            fprintf(output_file, "%d\t:%s %d\t: leaves queue\t: %d\t: %d\n", *shared_counter, type, id, *hacker_counter, *serfs_counter);
            fflush(output_file);
            *shared_counter += 1;
            sem_post(mutex);

            my_sleep(rand() % (W - 20 + 1) + 20);    //random number in interval <20; W>

            sem_wait(mutex);
            fprintf(output_file, "%d\t:%s %d\t: is back\n", *shared_counter, type, id);
            fflush(output_file);
            *shared_counter += 1;
            sem_post(mutex);
        }
    }while(total_pier >= C);

    //processes here, finally got to the pier
    sem_wait(mutex);
    if(is_hacker){
        *hacker_counter += 1;
    }
    else{
        *serfs_counter += 1;
    }

    fprintf(output_file, "%d\t:%s %d\t: waits\t\t: %d\t: %d\n", *shared_counter, type, id, *hacker_counter, *serfs_counter);
    fflush(output_file);
    *shared_counter += 1;
    sem_post(mutex);

    //if the combination is right
    //sem_wait(sails);

    if(is_hacker){
        sem_wait(mutex);
        if(*hacker_counter == 4){
            sem_post(hacker_queue);
            sem_post(hacker_queue);
            sem_post(hacker_queue);
            sem_post(hacker_queue);
            *hacker_counter = 0;
            is_captain = true;
        }
        else if((*hacker_counter == 2) && (*serfs_counter >= 2)){
            sem_post(hacker_queue);
            sem_post(hacker_queue);
            sem_post(serfs_queue);
            sem_post(serfs_queue);

            *hacker_counter = 0;
            *serfs_counter -= 2;
            is_captain = true;
        }
        else{
            //sem_post(sails);
        }
        sem_post(mutex);
        sem_wait(hacker_queue);
    }
    else{
        sem_wait(mutex);
        if(*serfs_counter == 4){
            sem_post(serfs_queue);
            sem_post(serfs_queue);
            sem_post(serfs_queue);
            sem_post(serfs_queue);
            *serfs_counter = 0;
            is_captain = true;
        }
        else if((*serfs_counter == 2) && (*hacker_counter >= 2)){
            sem_post(serfs_queue);
            sem_post(serfs_queue);
            sem_post(hacker_queue);
            sem_post(hacker_queue);

            *serfs_counter = 0;
            *hacker_counter -= 2;
            is_captain = true;
        }
        else{
            //sem_post(sails);
        }
        sem_post(mutex);
        sem_wait(serfs_queue);
    }

    //boarding
    if(is_captain){
        //captain boards
        sem_wait(mutex);
        fprintf(output_file, "%d\t:%s %d\t: boards\t: %d\t: %d\n", *shared_counter, type, id, *hacker_counter, *serfs_counter);
        fflush(output_file);
        *shared_counter += 1;
        sem_post(mutex);

        my_sleep(rand() % (R + 1));

        //letting 3 members to get of the boat
        sem_post(unboarding);
        //sem_post(unboarding);
        //sem_post(unboarding);

        //waiting for all 3 members to be of the boat
        sem_wait(captain_unboarding);

        //captain exits print
        sem_wait(mutex);
        fprintf(output_file, "%d\t:%s %d\t: captain exits\t: %d\t: %d\n", *shared_counter, type, id, *hacker_counter, *serfs_counter);
        fflush(output_file);
        *shared_counter += 1;
        //*unboarded = 0;
        sem_post(mutex);

        //another boat can sail
        //sem_post(sails);
    }
    else{
        //if it is not the captain
        //waits for signal from captain to get of the boat... 3 processes will get this signal
        sem_wait(unboarding);
        sem_post(unboarding);

        //printing that the member exits the boat
        sem_wait(mutex);
        fprintf(output_file, "%d\t:%s %d\t: member exits\t: %d\t: %d\n", *shared_counter, type, id, *hacker_counter, *serfs_counter);
        fflush(output_file);
        *shared_counter += 1;

        //counter knows how many members left the boat
        *unboarded += 1;

        //if 3 members left, the captain can leave as well
        if(*unboarded == 3){
            sem_post(captain_unboarding);
            *unboarded = 0;
            sem_post(mutex);
            sem_wait(unboarding);
        }
        else{
            sem_post(mutex);
        }
    }




}

void free_all(){

    /*================================== closing up the file ===============================*/
    if(fclose(output_file) == EOF){
        fprintf(stderr, "ERROR:\tThe output file could not be closed.\n");
        exit(1);
    }

    /*================================ closing semaphores ======================================*/
    if(sem_close(serfs_queue) == -1){
        fprintf(stderr, "ERROR:\tCould not close up serfs_queue semaphore.\n");
        exit(1);
    }
    if(sem_close(hacker_queue) == -1){
        fprintf(stderr, "ERROR:\tCould not close up hacker_queue semaphore.\n");
        exit(1);
    }

    if(sem_close(mutex) == -1){
        fprintf(stderr, "ERROR:\tCould not close up mutex semaphore.\n");
        exit(1);
    }

    if(sem_close(sails) == -1){
        fprintf(stderr, "ERROR:\tCould not close up sails semaphore.\n");
        exit(1);
    }

    if(sem_close(unboarding) == -1){
        fprintf(stderr, "ERROR:\tCould not close up unboarding semaphore.\n");
        exit(1);
    }

    if(sem_close(captain_unboarding) == -1){
        fprintf(stderr, "ERROR:\tCould not close up captain_unboarding semaphore.\n");
        exit(1);
    }


    sem_unlink("/sem_serfs_queue");
    sem_unlink("/sem_hacker_queue");
    sem_unlink("/sem_mutex");
    sem_unlink("/sem_sails");
    sem_unlink("/sem_unboarding");
    sem_unlink("/sem_captain_unboarding");


    /*=================================== freeing shared variables ================================*/


    shmctl(shared_counter_id, IPC_RMID, NULL);
    shmctl(hacker_counter_id, IPC_RMID, NULL);
    shmctl(serfs_counter_id, IPC_RMID, NULL);
    shmctl(hacker_id_id, IPC_RMID, NULL);
    shmctl(serfs_id_id, IPC_RMID, NULL);
    shmctl(unboarded_id, IPC_RMID, NULL);
}

int my_sleep(int milliseconds){
    struct timespec req, rem;

    if(milliseconds > 999){
        req.tv_sec = (int)(milliseconds / 1000);
        req.tv_nsec = (milliseconds - ((long)req.tv_sec * 1000)) * 1000000;
    }
    else{
        req.tv_sec = 0;
        req.tv_nsec = milliseconds * 1000000;
    }

    return nanosleep(&req , &rem);
}
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>

#define MAX_ITEMS 1000

struct part {
    int id;
    char name[64];
};

sem_t empty;
sem_t full;
pthread_mutex_t mutex;

struct part buffer[MAX_ITEMS];
struct part box[MAX_ITEMS][MAX_ITEMS];
struct part partArr[MAX_ITEMS];

int in = 0;
int out = 0;
int proController;
int conController;
int pnt = 0, cnt = 0;
int serial = 0;
int maxPartInBox = 0;
int maximumParts;
int bufferSize;
int partTypes;
int selected;
int ch;
int partsCounter;

void *partProducer(void *arg) {
    proController--;
    sem_wait(&empty);
    pthread_mutex_lock(&mutex);

    if (buffer[in].id == -1) {
        buffer[in] = partArr[selected];
        in = (in + 1) % bufferSize;
    }
    cnt++;
    pthread_mutex_unlock(&mutex);
    sem_post(&full);
    conController++;
    return NULL;
}

void *partConsumer(void *arg) {
    conController--;
    sem_wait(&full);
    pthread_mutex_lock(&mutex);

    struct part item;
    item = buffer[out];
    box[serial][maxPartInBox] = item;
    maxPartInBox++;
    if (maxPartInBox == maximumParts) {
        serial++;
        maxPartInBox = 0;
    }
    buffer[out].id = -1;
    buffer[out].name[0] = '\0';
    out = (out + 1) % bufferSize;
    cnt--;
    proController++;
    partsCounter++;
    pthread_mutex_unlock(&mutex);
    sem_post(&empty);
    return NULL;
}

void createParts() {
    printf("How many types of parts you want to produce: ");
    scanf("%d", &partTypes);
    char name[64];
    for (int i = 0; i < partTypes; i++) {
        printf("Part %d Name: ", i);
        scanf("%s", name);
        struct part p;
        p.id = i;
        strcpy(p.name, name);
        partArr[i] = p;
    }
}

int selectPart() {
    printf("List of Available Parts:\n");
    for (int i = 0; i < partTypes; i++) {
        printf("%d. %s\n", partArr[i].id, partArr[i].name);
    }
    int select;
    printf("Select a part from the list: ");
    scanf("%d", &select);

    while(select > ch){
//       if (select > ch) {
           printf("Please select a valid choice.\n");
           scanf("%d", &select);
       }
    return select;
}

void showBuffer();
void listParts();
void showBoxes();
void countBuffer();

int main() {
    sem_init(&empty, 0, MAX_ITEMS);
    sem_init(&full, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    pthread_t producer_t[MAX_ITEMS], consumer_t[MAX_ITEMS];

    for (int i = 0; i < MAX_ITEMS; i++) {
        buffer[i].id = -1;
        buffer[i].name[0] = '\0';
    }
    for (int i = 0; i < MAX_ITEMS; i++) {
        for (int j = 0; j < MAX_ITEMS; j++) {
            box[i][j].name[0] = '\0';
        }
    }

    printf("Enter Assembly line size (Buffer Size 1 - 1000): ");
    scanf("%d", &bufferSize);
    createParts();

    proController = bufferSize;

    int input;
    printf("\nWelcome to Pak Suzuki Assembly Line Parts Production\n");
    printf("1. Produce Part\n");
    printf("2. Consume Part\n");
    printf("3. Show Assembly Line \n");
    printf("4. Show Available Parts \n");
    printf("5. Show Packed Boxes \n");
    printf("0. Exit the factory\n");
    printf("Enter your option: ");
    scanf("%d", &input);

    while (input != 0) {
        if (input == 1) {
            int numProducer;
            printf("Enter the number of parts you want to produce: ");
            scanf("%d", &numProducer);
            printf("\n");
            ch = numProducer;
            for (int i = 0; i < numProducer; i++) {
                if (proController >= 1) {
		    if(numProducer <= bufferSize){
                        selected = selectPart();
                        pthread_create(&producer_t[i], NULL, partProducer, NULL);
                        printf("Producer %d: produced %s part", i, partArr[selected].name);
                        printf(" Stored at %d of the Buffer\n", in);
		    }
		    else {
                        printf("Maximum Capacity Reached: %d\n", bufferSize);
                        break;
		    }
                } else {
                    printf("Buffer is full\n");
                    break;
                }
            }
        } else if (input == 2) {
            countBuffer();
            int numConsumer;
            if (conController > 0) {
                printf("\nHow many parts do you want to consume: ");
                scanf("%d", &numConsumer);
                if (numConsumer <= partsCounter) {
                   if (maximumParts > 0) {
                        printf("Maximum parts are already stored in a box previously: %d\n", maximumParts);
                    } else {
                        printf("Enter number of parts you want in a Box: ");
                        scanf("%d", &maximumParts);
                    }

                    for (int i = 0; i < numConsumer; i++) {
                        if (conController > 0) {
                            pthread_create(&consumer_t[i], NULL, (void *)partConsumer, NULL);
                            if (i == 0) {
                                printf("%s ", buffer[0].name);
                                printf("consumed from Buffer %d\n", out);
                            } else {
                                printf("%s ", buffer[out + i].name);
                                printf("consumed from Buffer %d\n", out + i);
                            }
                        } else {
                            printf("Buffer is empty\n");
                            break;
                        }
                    }
                } else {
                    printf("Not enough parts in Buffer\n");
                    printf("Available parts in Buffer -> %d\n", partsCounter);
                }
            } else {
                printf("Buffer is empty\n");
            }
        } else if (input == 3) {
            showBuffer();
        } else if (input == 4) {
            listParts();
        } else if (input == 5) {
            showBoxes();
        }
        printf("\nWelcome to Pak Suzuki Assembly Line Parts Production\n");
        printf("1. Produce Part\n");
        printf("2. Consume Part\n");
        printf("3. Show Assembly Line \n");
        printf("4. Show Available Parts \n");
        printf("5. Show Packed Boxes \n");
        printf("0. Exit the factory\n");
        printf("Enter your option: ");
        scanf("%d", &input);
    }

    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&mutex);

    return 0;
}

void showBuffer() {
    printf("Available Parts in the Assembly Line: \n");
    for (int i = 0; i < bufferSize; i++) {
        if (buffer[i].id != -1) {
            printf("%d. %s \n", i, buffer[i].name);
        } else if (buffer[i].id == -1) {
            continue;
        } else {
            printf("Buffer is empty. \n");
            break;
        }
    }
}

void showBoxes() {
    int cnt = 0;
    printf("Total Boxes: %d\n", serial);
    if (serial > 0) {
        for (int i = 0; i < serial; i++) {
            printf("Box %d contains: \n", i);
            for (int j = 0; j < maximumParts; j++) {
                if (box[i][j].id != -1) {
                    int idx = box[i][j].id;
                    printf("%d %s \n", j, partArr[idx].name);
                }
            }
        }
    } else {
        printf("No parts in Box \n");
    }
}

void countBuffer() {
    for (int i = 0; i < bufferSize; i++) {
        if (buffer[i].id != -1) {
            partsCounter++;
        } else if (buffer[i].id == -1) {
            continue;
        } else {
            break;
        }
    }
    printf("Available parts now in Buffer -> %d\n", partsCounter);
}

void listParts() {
    printf("List of Available Parts:\n");
    for (int i = 0; i < partTypes; i++) {
        printf("%d. %s\n", partArr[i].id, partArr[i].name);
    }
}

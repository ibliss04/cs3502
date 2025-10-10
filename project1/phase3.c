
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define NUM_ACCOUNTS 3
#define TRANSACTIONS_PER_TELLER 5
#define NUM_THREADS 5
#define INITIAL_BALANCE 1000

typedef struct {
	int account_id;
	double balance;
	int transaction_count;
	pthread_mutex_t lock;
} Account;

Account accounts[NUM_ACCOUNTS];

void transfer(int from_id, int to_id, double amount){

	printf("Account %ld: Attempting transfer %0.02f from Account %d to %d\n", pthread_self(), amount, from_id, to_id);

	pthread_mutex_lock(&accounts[from_id].lock);
	printf("Thread %ld: Locked account %d\n", pthread_self(), from_id);

	usleep(100);

	printf("Thread %ld: Waiting for account %d\n", pthread_self(), to_id);
	pthread_mutex_lock(&accounts[to_id].lock);
	printf("Thread %ld: Locked account %d\n",pthread_self(), to_id);

	accounts[from_id].balance -= amount;
	accounts[to_id].balance += amount;

	pthread_mutex_unlock(&accounts[to_id].lock);
	pthread_mutex_unlock(&accounts[from_id].lock);
}

void* teller_thread(void* arg) {

	for (int i = 0; i < TRANSACTIONS_PER_TELLER; i++) {

		int first_id = rand()  % NUM_ACCOUNTS;
		int second_id;
		do {
			second_id = rand() % NUM_ACCOUNTS;
		} while (second_id != first_id);

		double amount = (rand() % 2001-1000) / 100.0;

		transfer(first_id, second_id, amount);
	}

	return NULL;

}

int main(void) {

	srand((unsigned)time(NULL));

	for (int i = 0; i < NUM_ACCOUNTS;i++){
		accounts[i].account_id = i;
		pthread_mutex_init(&accounts[i].lock, NULL);
		accounts[i].balance = INITIAL_BALANCE;
		accounts[i].transaction_count = 0;
	}

	pthread_t threads[NUM_THREADS];
	int thread_ids[NUM_THREADS];

	for (int i = 0; i < NUM_THREADS; i++) {
		thread_ids[i] = i;
		pthread_create(&threads[i], NULL, teller_thread, &thread_ids[i]);

	}

	for(int i = 0; i < NUM_THREADS; i++){
		pthread_join(threads[i], NULL);
	}

	for(int i = 0; i < NUM_ACCOUNTS; i++){
		pthread_mutex_destroy(&accounts[i].lock);
	}

	for(int i = 0; i < NUM_ACCOUNTS; i++){
		printf("Account %d: Final Balance %0.2f\n", i, accounts[i].balance);
	}

	return 0;
}

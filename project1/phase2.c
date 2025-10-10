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

double now_ms(void){
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (double)ts.tv_sec * 1000.0 + (double)ts.tv_nsec / 1e6;
}


typedef struct {
	int account_id;
	double balance;
	int transaction_count;
	pthread_mutex_t lock;
} Account;

Account accounts[NUM_ACCOUNTS];

void unsafe_transactions(int teller_id, int idx, double amount){

	double before = accounts[idx].balance;
	usleep(1000);
	accounts[idx].balance = before +  amount;
	accounts[idx].transaction_count++;
	printf("Teller %d: Transaction %d on account %d: %0.2f, new balance = %.2f\n", teller_id, accounts[idx].transaction_count, idx, amount, accounts[idx].balance);

}

void safe_transactions(int teller_id, int idx, double amount){

	pthread_mutex_lock(&accounts[idx].lock);
	double before = accounts[idx].balance;
	usleep(1000);
	accounts[idx].balance = before +  amount;
	accounts[idx].transaction_count++;
	pthread_mutex_unlock(&accounts[idx].lock);
	printf("Teller %d: Transaction %d on account %d: %0.2f, new balance = %.2f\n", teller_id, accounts[idx].transaction_count, idx, amount, accounts[idx].balance);

}

void* teller_thread(void* arg) {
	int teller_id = *(int*)arg;

	//choice = 0 for safe transactions and choice = any other integer for unsafe
	int choice = 0;

	if (choice == 0){
		printf("Safe Transactions\n");
	} else{
		printf("Unsafe Transactions\n");
	}
	for (int i = 0; i < TRANSACTIONS_PER_TELLER; i++) {
		//SELECT RANDOM ACCOUNT
		int idx = rand() % NUM_ACCOUNTS;

		//PERFORM DEPOSIT OR WITHDRAWAL
		double amount = (rand() % 2001-1000) / 100.0;

		if (choice == 0){
			safe_transactions(teller_id, idx, amount);
		} else{
			unsafe_transactions(teller_id, idx, amount);
		}
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

	double start = now_ms();

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

	double end = now_ms();
	printf("Elapsed time: %0.2f ms\n", end - start);

	return 0;
}

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

typedef struct {
	int account_id;
	double balance;
	int transaction_count;
} Account;

Account accounts[NUM_ACCOUNTS];

void* teller_thread(void* arg) {
	int teller_id = *(int*)arg;


	for (int i = 0; i < TRANSACTIONS_PER_TELLER; i++) {
		//SELECT RANDOM ACCOUNT
		int idx = rand() % NUM_ACCOUNTS;

		//PERFORM DEPOSIT OR WITHDRAWAL
		double amount = (rand() % 2001-1000) / 100.0;

		double before = accounts[idx].balance;
		usleep(1000);
		accounts[idx].balance = before +  amount;
		accounts[idx].transaction_count++;

		printf("Teller %d: Transaction %d on account %d: %0.2f, new balance = %.2f\n", teller_id, accounts[idx].transaction_count, idx, amount, accounts[idx].balance);
	}

	return NULL;

}

int main(void) {

	srand((unsigned)time(NULL));

	for (int i = 0; i < NUM_ACCOUNTS;i++){
		accounts[i].account_id = i;
		accounts[i].balance = 1000.0;
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
		printf("Account %d: Final Balance %0.2f\n", i, accounts[i].balance);
	}

	return 0;
}

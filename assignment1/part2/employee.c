#include <stdio.h>
#include <string.h>

int main() {

	char name[50];
	int employee_id;
	float hours_worked;

	printf("Owltech Employee Regristration\n");
	printf("==============================\n");

	printf("Enter your name: ");
	fgets(name, sizeof(name), stdin);

	name[strcspn(name, "\n")] = '\0';

	printf("Enter your employee ID: ");
	scanf("%d", &employee_id);

	printf("Hours worked this week: ");
	scanf("%f", &hours_worked);

	printf("\nEmployee Summary:\n");
	printf("Name: %s\n", name);
	printf("Hours: %.2f\n", hours_worked);

	return 0;
}

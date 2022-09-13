#include <stdio.h>
#include <string.h>

#define CONST 12 //constant for xor

char* xor_password(char str[100]) //xor func
{
	static char ret[100];
	for (int i = 0; i < strlen( str ); i++)ret[i] = str[i] ^ CONST;
	return ret;
}

int password_requier() //checking password function
{
	char password[100];
	char buffer[100];
	printf("Enter the passsword : ");
	scanf("%s", &password);
	FILE* file = fopen("password.txt", "r");
	fgets(buffer, 100, file);
	if (strcmp(password, xor_password(buffer)) == 0)return 1;
	return 0;
}

int main() {
	if (password_requier() == 1)printf("\nAcces granted");
	else printf("\nAcces denied");
	while(1){}
}
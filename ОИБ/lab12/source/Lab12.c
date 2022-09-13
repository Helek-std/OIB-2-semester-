#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <aclapi.h>
#include <securitybaseapi.h>
#include <stdint.h>
#include <stdbool.h>
#include <windows.h>
#include <accctrl.h>
#include <string.h>

char* showbits(unsigned int x)
{
	static char bits[33];
	int i = 0;
	for (i = 31; i >= 0; i--)
	{
		bits[31 - i] = (char)(x & (1u << i) ? '1' : '0');
	}
	bits[32] = '\0';
	return bits;
}

int* read_file_atributes(char* filename)
{
	static int ret[4];
	PACL info = NULL;
	PSECURITY_DESCRIPTOR psd = NULL;
	ACCESS_ALLOWED_ACE* pACE = NULL;
	GetNamedSecurityInfoA(filename,
		SE_FILE_OBJECT,
		DACL_SECURITY_INFORMATION,
		NULL,
		NULL,
		&info,
		NULL,
		&psd);
	GetAce(info, 0, &pACE);
	LocalFree((HLOCAL)psd);
	ret[0] = showbits(pACE->Mask)[25] - 48;//полный доступ
	ret[1] = showbits(pACE->Mask)[15] - 48;//удаление
	ret[2] = showbits(pACE->Mask)[24] - 48;//чтение
	ret[3] = showbits(pACE->Mask)[23] - 48;//запись
	return ret;
}

void print_file_atributes(char* filename)
{
	printf("Full access : %d\n", read_file_atributes(filename)[0]);
	printf("Delete : %d\n", read_file_atributes(filename)[1]);
	printf("Read : %d\n", read_file_atributes(filename)[2]);
	printf("Write : %d\n", read_file_atributes(filename)[3]);
}

void write_to_file(char* filename)
{
	FILE* fp;
	if( (fp = fopen(filename, "a+")) != NULL)
	{
		char* string = (char*)malloc(1000);
		string[0] = '\0';
		printf("String to put into file :");
		scanf("%s", string);
		fputs(string, fp);
		fclose(fp);
	}
	else 
	{
		printf("Error opening file, searching for problem...\n");
		if (read_file_atributes(filename)[4] == 0)printf("No premission to write into this file\n");
	}
		
}

void read_file(char* filename)
{
	FILE* fp;
	if ((fp = fopen(filename, "r")) != NULL)
	{
		char* string = (char*)malloc(1000);
		string[0] = '\0';
		while (fgets(string, 999, fp))
			printf("%s", string);
		fclose(fp);
	}
	else
	{
		printf("Error opening file, searching for problem...\n");
		if (read_file_atributes(filename)[3] == 0)printf("No premission to read this file\n");
	}

}

int* get_new_permissions(char* filename)
{
	print_file_atributes(filename);
	static int new_flags[4];
	printf("\nFull access(0/1) : ");
	scanf("%d", &new_flags[0]);
	if (new_flags[0] == 1)
		printf("\nFull access");
	else
	{
		printf("\nDelete(0/1) : ");
		scanf("%d", &new_flags[1]);
		printf("\nRead(0/1) : ");
		scanf("%d", &new_flags[2]);
		printf("\nWrite(0/1) : ");
		scanf("%d", &new_flags[3]);
	}
	return new_flags;

}


void change_permissions(char* filename)
{
	int* flags = get_new_permissions(filename);
	char* command = (char*)malloc(1000);
	command[0] = '\0';
	strcat(command, "powershell C:\\Users\\Eugen\\source\\repos\\Lab12\\Lab12\\Code.ps1 ");
	if (flags[0] == 1)
		strcat(command, "FullControl");
	else
	{
		if (flags[1] == 1)
			strcat(command, "Delete, ");
		if (flags[2] == 1)
			strcat(command, "Read, ");
		if (flags[3] == 1)
			strcat(command, "Write, ");
		command[strlen(command) - 2] = '\0';
	}
	strcat(command, " ");
	strcat(command, filename);
	system(command);
}


void create_file(char* filename) 
{
	FILE* f = fopen(filename, "a+");
	fclose(f);
	char* command = (char*)malloc(1000);
	command[0] = '\0';
	strcat(command, "powershell C:\\Users\\Eugen\\source\\repos\\Lab12\\Lab12\\Code2.ps1 ");
	strcat(command, filename);
	printf("|%s|", command);
	system(command);
	system(command);
	change_permissions(filename);
	printf("File %s has been created\n", filename);
}



void fnc()
{
	char input;
	char* filename = (char*)malloc(100);
	while (1) 
	{
		memset(filename, '\0', 100);
		printf("\n1 to read from file\n2 to write to file\n3 to print file atributes\n4 to change file permissions\n5 to create file\n");
		scanf("%c", &input);
		if(input == '\n')scanf("%c", &input);
		switch (input) 
		{
		case '1':
			printf("Enter filename : ");
			scanf("%s",filename);
			read_file(filename);
			break;
		case '2':
			printf("Enter filename : ");
			scanf("%s", filename);
			write_to_file(filename);
			break;
		case '3':
			printf("Enter filename : ");
			scanf("%s", filename);
			print_file_atributes(filename);
			break;
		case '4':
			printf("Enter filename : ");
			scanf("%s", filename);
			change_permissions(filename);
			break;
		case '5':
			printf("Enter filename : ");
			scanf("%s", filename);
			create_file(filename);
		default :
			printf("Wrong input\n");
			break;
		}
		printf("\n");
	}
}

int main() {
	system("chcp1251");
	system("cls");
	fnc();
}	
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define OUTPUT "obfuscated.c"
#define INPUT "pass.c"
#define PARAMETRS "pars.txt"
#define BUFFER "buffer.c"
#define min(a,b) (a < b) ? a : b

char* slice(char* str, int start, int end)	 
{
	char* ret = (char*)malloc(end - start + 1);
	strncpy(ret, (str + start), end - start);
	ret[end - start] = '\0';
	return ret;
}

void copy_file(char* input, char* output) 
{
	FILE* inp = fopen(input, "r"); //file from which we are reading
	FILE* outp = fopen(output, "w"); //file we'r writting in

	char buff[200];

	int i;

	while (fgets(buff, sizeof(buff), inp) != NULL)
		for (i = 0; i < strlen(buff); i++)
			fputc(buff[i], outp);

	fclose(inp);
	fclose(outp);
}

void no_tabs(char* input, char* output)
{
	printf("deliting tabs...\n");

	FILE* inp = fopen(input, "r"); //file from which we are reading
	FILE* outp = fopen(output, "w"); //file we'r writting in

	int i;
	char buff[200]; //buffer for strings

	while (fgets(buff, sizeof(buff), inp) != NULL)
	{
		if (buff[0] == '#') 
		{
			fputs(buff, outp);
			continue;
		}
		for (i = 0; i < strlen(buff); i++)
		{
			if (buff[i] == '\t' || buff[i] == '\n')fputc(' ', outp);
			else fputc(buff[i], outp);
		}
	}
	fclose(inp);
	fclose(outp);
}

void delete_comments(char* input, char* output)
{
	printf("deliting comments...\n");

	FILE* inp = fopen(input, "r"); //file from which we are reading
	FILE* outp = fopen(output, "w"); //file we'r writting in

	int i;
	char buff[200]; //buffer for strings

	while (fgets(buff, sizeof(buff), inp) != NULL)
	{

		for (i = 0; i < strlen(buff); i++)
		{
			if (buff[i] == '/' || buff[i + 1] == '/')
			{
				fputc('\n', outp);
				break;
			}
			else fputc(buff[i], outp);
		}
	}

	fclose(inp);
	fclose(outp);
}

char* find_vars(char* input)
{
	FILE* inp = fopen(input, "r");

	char* buffer = (char*)malloc(300);
	static char result[200];

	int flag = 0;
	int mn;

	char* type = (char*)malloc(7);
	char spliter;


	char* types = (char*)malloc(100);
	types[0] = '\0';
	strcat(types, "int ,int* ,char ,char* ,FILE* ,");

	while (strchr(types, ',') != NULL)
	{
		strcpy(type, slice(types, 0, strchr(types, ',') - types));
		types = strchr(types, ',');
		types++;

		while (fgets(buffer, 299, inp) != NULL)
		{
			if (strstr(buffer, type) != NULL)
			{
				buffer = strstr(buffer, type) + strlen(type);
				mn = min(abs(strchr(buffer, ';') - buffer), abs(strchr(buffer, '[') - buffer));
				mn = min(abs(strchr(buffer, ' ') - buffer), mn);
				mn = min(abs(strchr(buffer, '(') - buffer), mn);
				spliter = buffer[mn];
				strcat(result, slice(buffer, 0, strchr(buffer, spliter) - buffer));
				strcat(result, " ");
			}
		}
		fclose(inp);
		inp = fopen(input, "r");
	}
	return result;
}

char* generate_name(int len)
{
	char* var_name = (char*)malloc(len + 1);

//	srand(time(NULL));
	for (int i = 0; i < len; i++) 
		var_name[i] = (char)(rand()%26 + 97);
	var_name[len] = '\0';
	return var_name;
	
}

void change_names(char* input, char* output)
{
	printf("changing names...\n");
	FILE* inp = fopen(input, "r");
	FILE* outp = fopen(output, "w");


	char* buffer = (char*)malloc(200);
	char* cur_name = (char*)malloc(10);
	char* var_names = (char*)malloc(100);
	char* generated_name = (char*)malloc(10);

	char separators[] = " ,[]();+-=*/&";
	char* text_buffer = (char*)malloc(10000);
	char* second_buffer = (char*)malloc(10000);


	text_buffer[0] = '\0';
	second_buffer[0] = '\0';


	strcpy(var_names, find_vars(input));

	while (fgets(buffer, 200, inp) != NULL)strcat(text_buffer, buffer);

	int j = 0;

	while (strchr(var_names, ' ') != NULL)
	{
		strcpy(cur_name, slice(var_names, 0, strchr(var_names, ' ') - var_names));
		var_names = strchr(var_names, ' ');
		
		strcpy(generated_name, generate_name(9));
		var_names++;
		if (strcmp(cur_name, "main") == 0)continue;

		for(int i = 0; i < strlen(text_buffer);i++)
		{
			if (strncmp(slice(text_buffer,i,strlen(text_buffer)), cur_name, strlen(cur_name)) == 0) 
			{
				if (strchr(separators, text_buffer[i - 1]) != NULL && strchr(separators, text_buffer[i + strlen(cur_name)]) != NULL) 
				{
					strcat(second_buffer,generated_name);

					i+= strlen(cur_name) - 1;
					continue;
				}
			}
			strcat(second_buffer, slice(text_buffer,i,i+1));
		}
		strcpy(text_buffer, second_buffer);
		memset(second_buffer, '\0', strlen(second_buffer));
	}
	fputs(text_buffer, outp);
	fclose(inp);
	fclose(outp);
	
}

void add_vars(char* input, char* output) 
{
	printf("adding vars...\n");
	FILE * inp = fopen(input, "r");
	FILE * outp = fopen(output, "w");

	int flag = 0;
	char* buff = (char*)malloc(200);
	char* text = (char*)malloc(200);
	char* result = (char*)malloc(5000);

	buff[0] = '\0';
	text[0] = '\0';
	result[0] = '\0';

	while (fgets(text, 200, inp) != NULL) 
	{

		for (int i = 0; i < strlen(text); i++)
		{
			if (text[i] == '{')flag = 1;
			if (text[i] == '}')flag = 0;
			strcat(result, slice(text, i, i + 1));
		}
		if (flag == 1)

			if ((rand() % 2) == 1)
			{
				switch (rand() % 4)
				{
				case 0:
					sprintf(buff, "\nint %s;\n", generate_name(5 + (rand() % 4)));
					break;
				case 1:
					sprintf(buff, "\nint %s = %d;\n", generate_name(5 + (rand() % 4)), rand() % 37);
					break;
				case 2:
					sprintf(buff, "\nchar %s = '%c';\n", generate_name(5 + (rand() % 4)), 97 + rand() % 25);
					break;
				case 3:
					sprintf(buff, "\nchar %s[] = \"%s\";\n", generate_name(5 + (rand() % 4)), generate_name(rand() % 20));
					break;
				}
				strcat(result, buff);
			}
	}
	fputs(result, outp);

	fclose(inp);
	fclose(outp);
}

void obfuscate()
{
	int change_name;
	int add_var;
	int del_tabs;
	int del_comments;

	FILE* pars = fopen(PARAMETRS, "r");

	char parametrs[5];

	//reading parametrs for obfuscator
	fgets(parametrs, 5, pars);
	change_name = parametrs[0] - 48;
	add_var = parametrs[1] - 48;
	del_tabs = parametrs[2] - 48;
	del_comments = parametrs[3] - 48;

	fclose(pars);
	
	copy_file(INPUT, BUFFER);

	if (del_comments == 1)
	{
		delete_comments(BUFFER, OUTPUT);
		copy_file(OUTPUT, BUFFER);
	}
	
	if (change_name == 1)
	{
		change_names(BUFFER, OUTPUT);
		copy_file(OUTPUT, BUFFER);
	}

	if (add_var == 1)
	{
		add_vars(BUFFER, OUTPUT);
		copy_file(OUTPUT, BUFFER);
	}

	if (del_tabs == 1)
	{
		no_tabs(BUFFER, OUTPUT); 
		copy_file(OUTPUT, BUFFER);
	}
}

int main() 
{
	obfuscate();
	
}
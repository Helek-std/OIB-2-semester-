#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define OUTPUT "output.txt"
#define SIMPLE_TXT "simple_text.txt"
#define INPUT "input.txt"
#define LEN 1000000
#define STATS "stats_rus.txt"
#define OFFSET_ENG (int)'a'
#define OFFSET_RU (int)'а'

char* get_txt(char* name)
{
	char* text = (char*)malloc(LEN);
	FILE* fp = fopen(name, "r");
	int index = 0;
	while (fgets(text + index, 1000, fp) != NULL) index = strlen(text);
	return text;
	fclose(fp);
}

void print_crypt()
{
	char* text = get_txt(OUTPUT);
	printf("%s", text);
}

void cesar(int step) 
{
	char* txt = (char*)malloc(LEN);
	txt = get_txt(SIMPLE_TXT);

	FILE* fp = fopen(INPUT, "w");

	for (int i = 0; i < strlen(txt); i++) 
	{
		if ((int)txt[i] >= (int)'a' && (int)txt[i] <= (int)'z')txt[i] = (char)((int)txt[i] - (int)'a' + step)%26 + (int)'a';
		else if (((int)txt[i] >= (int)'A' && (int)txt[i] <= (int)'Z')) txt[i] = (char)((int)txt[i] - (int)'A' + step) % 26 + (int)'A';
		putc(txt[i],fp);

		//printf("%c", txt[i]);
	}
	fclose(fp);

}

typedef struct
{
	char symbol;
	float stat;
	int count;
}data;

typedef struct {
	char coded;
	char uncoded;
	data arr[33];

}encode;

data* get_stats() 
{
	static data stats[33];
	char* buff = (char*)malloc(30);
	FILE* fp = fopen(STATS, "r");
	int index = 0;

	while (fgets(buff, LEN, fp) != NULL) 
	{
		stats[index].symbol = buff[0];
		
		sscanf(buff+2,"%f", &stats[index].stat);
		
		index++;
	}

	fclose(fp);
	return stats;

}

data* get_text_stats()
{
	char* txt = (char*)malloc(LEN);
	static data stats[33];
	for (int i = 0; i < 33; i++)
	{
		stats[i].symbol = (char)(OFFSET_RU+ i);
		stats[i].count = 0;
	}
	txt = get_txt(INPUT);

	for (int i = 0; i < strlen(txt); i++)
	{

		if (((int)txt[i] >= (int)'а' && (int)(txt[i]) <= (int)'я') || ((int)txt[i] >= (int)'А' && (int)(txt[i]) <= (int)'Я'))
		{
			for (int j = 0; j < 33; j++) if (stats[j].symbol == txt[i] || stats[j].symbol == (char)((int)txt[i] + ('а' - 'А')) && stats[j].symbol != '\0')stats[j].count++;
		}
	}
	int sum = 0;
	for (int i = 0; i < 33; i++)sum += stats[i].count;

	for (int i = 0; i < 33; i++)stats[i].stat = ((float)stats[i].count / sum) * 100;

	int i, j;
	data buff;

	for (i = 32; i > 0; i--)
		for (j = 0; j < i; j++)
			if (stats[j].stat < stats[j + 1].stat)
			{
				buff = stats[j];
				stats[j] = stats[j + 1];
				stats[j + 1] = buff;
			}
	return stats;
}

char* possible_switches(char symbol) {
	data* stats = get_stats();
	data* text_stats = get_text_stats();
	int i, j,k;

	float text_stat = 0.0;
	for (i = 0; i < 33; i++)
		if (text_stats[i].symbol == symbol)
	{
		text_stat = text_stats[i].stat;
		break;
	}
	
	data buff;
//	for (i = 0; i < 33; i++)printf("%c", stats[i].symbol);


	for (j = 0; j < 32; j++)
		for (k = 0; k < 33 - j - 1; k++)
			if (fabs(stats[k].stat - text_stat) > fabs(stats[k + 1].stat - text_stat)) 
			{
				//printf("%c   %f  ----> %c   %f \n", stats[k].symbol, stats[k].stat, stats[k + 1].symbol, stats[k + 1].stat);
				buff = stats[k];
				stats[k] = stats[k + 1];
				stats[k + 1] = buff;
			}
	//for (i = 0; i < 33; i++)printf("%c", stats[i].symbol);
	static char ret[33];	
	for (i = 0; i < 33; i++)ret[i] = stats[i].symbol;

	return ret;
	}

void print_switches(char symbol) {
	if (symbol != '\n') {
		printf("\n Возможные замены для %c: ", symbol);
		char* buff = possible_switches(symbol);
		for (int i = 0; i < 33; i++)printf("%c ", buff[i]);
		printf("\n");
	}
}

void print_stats(data* stats) {
	printf("========STATS TABLE========\n");
	printf("|Letters|Number|Percent|\n");
	for (int i = 0; i < 32; i++) {printf("|%7c|%6d|%7f|\n", stats[i].symbol,stats[i].count,stats[i].stat);}
	printf("========STATS TABLE========\n");
}

void switch_char(char coded, char uncoded) 
{
	char* input = get_txt(OUTPUT);
	printf("%c для замены \n", uncoded);
	FILE* fp = fopen(OUTPUT, "w");
	for(int i = 0; i<strlen(input);i++)
	{
		if (input[i] == (char)((int)coded-32))
		{
			putc(uncoded, fp);
			continue;
		}
		putc(input[i], fp);
	}
	fclose(fp);
	printf("Замена %c на %c произведена\n",coded,uncoded);
}

int main(int argc, char* argv[])
{
	system("chcp 1251");
	system("cls");


	char input;
	printf("1.вывод статистических таблиц\n2.для очистки консоли\n3.для возможных замен буквы\n4.для замены буквы\n5.Для отката изменений\n6.Для вывода криптограммы\n");
	while (1)
	{
		input = getchar();
		if (input == '1')
		{
			print_stats(get_stats());
			print_stats(get_text_stats());
			continue;
		}
		else if (input == '2') {
			system("cls");
			printf("1.вывод статистических таблиц\n2.для очистки консоли\n3.для возможных замен буквы\n4.для замены буквы\n5.Для отката изменений\n6.Для вывода криптограммы\n");
		}
		else if (input == '3')
		{
			printf("буква для замены - ");
			char letter = getchar();
			while (letter == '\n')letter = getchar();
			if (letter != '\n')
			{
				print_switches(letter);
				continue;
			}

		}
		else if (input == '4') 
		{
			printf("буква для замены - ");
			char letter = getchar();
			while (letter == '\n')letter = getchar();
			if (letter != '\n')
			{
				print_switches(letter);
				printf("буква, на которую заменим - ");
				char swtch = getchar();
				while (swtch == '\n')swtch = getchar();
				if (swtch != '\n') 
				{
					switch_char(letter, swtch);
					continue;
				}
			}
		}
		else if (input == '5') 
		{
			char* input = get_txt(INPUT);
			FILE* fp = fopen(OUTPUT, "w");
			for (int i = 0; i < strlen(input); i++) { putc(input[i], fp); }
			fclose(fp);
			continue;
		}
		else if (input == '6') 
		{
			print_crypt();
			continue;
		}
	}
}
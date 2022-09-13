#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#define LEN 200
#define INPUT "input.txt"
#define CODED "16.txt"
#define DECODED "decoded.txt"

#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

bool isPowerOfTwo(int x) {
	return x && (!(x & (x - 1)));
}

char* slice(char* str, int start, int end)
{
	char* ret = (char*)malloc(end - start + 1);
	strncpy(ret, (str + start), end - start);
	ret[end - start] = '\0';
	return ret;
}

char* dec_to_bin(int number) 
{
	int buff = number;
	static char ret[9];
	for (int i = 0; i < 8; i++)ret[i] = '0';
	for (int i = 0; buff > 0; i++)
	{
		ret[7 - i] = buff % 2 + 48;
		buff = buff / 2;
	}
	ret[8] = '\0';
	return ret;
}

int bin_to_dec(char* bits)
{
	int res = 0;
	for (int i = 0; i < 8; i++)
	{
		res += pow(2, (7 - i)) * ((int)bits[i] - 48);
	}
	return res;
}

char count_code_bit(int bit, char* block)
{
	int step = bit + 1;
	int len = strlen(block);
	int j;
	int sum = 0;
	int i = bit;
	while (i < len) {
		j = i;
		while (j < i + step && j < len) {
			if (isPowerOfTwo(j + 1) == 0) {

				sum += (int)block[j] - 48;
			}
			j++;
		}
		i += step * 2;
	}
	return (char)sum % 2 + 48;
}

char* code_block(char* block)
{
	size_t size = strlen(block);
	size_t counter = 0;
	for (size_t i = 0; i < size; i++)
		if (isPowerOfTwo((int)(i + 1)) == 1)
		{
			size++;
		}
	char* block_result = (char*)malloc(size + 1);
	block_result[0] = '\0';
	size_t j = 0;

	for (size_t i = 0; i < size; i++)
	{
		if (isPowerOfTwo((int)(j + 1)) == 0) {
			block_result[j] = block[i];
		}
		else {
			block_result[j] = '0';
			i--;
		}
		j++;
	}

	size += counter;
	block_result[size] = '\0';

	for (size_t i = 0; i < size; i++)
	{
		if (isPowerOfTwo(i + 1) == 1)
		{
			block_result[i] = count_code_bit(i, block_result);
		}
	}
	return block_result;
}

int check_mistake(char* block)
{
	int sum = 0;
	for (int i = 0; i < strlen(block); i++)
	{
		if (isPowerOfTwo(i + 1) == 1) {
			if (count_code_bit(i, block) != block[i])sum += i + 1;

		}
	}
	if (sum != 0)return sum;
	return -1;
}

char* decode_block(char* bits)
{
	int mistake = check_mistake(bits);
	char* result = (char*)malloc(strlen(bits));
	result[0] = '\0';
	int j = 0;
	for (int i = 0; i < strlen(bits); i++)
	{
		if (isPowerOfTwo(i + 1) == 0)
		{
			if (i == mistake - 1)
				switch (bits[i])
				{
				case '0':
					result[j++] = '1';
					break;
				case '1':
					result[j++] = '0';
					break;
				}
			else
				result[j++] = bits[i];
		}
	}
	result[j] = '\0';
	return result;
}

void read_by_bits(char* filename, char* destiny)
{
	FILE* f = fopen(filename, "r");
	int symb;
	while ((symb = fgetc(f)) != EOF)
	{
		strcat(destiny, dec_to_bin(symb));
	}
	fclose(f);
}

void code_string(char* input, char* destiny, int bits)
{
	int ln = strlen(input) / bits;
	for (int i = 0; i < ln; i++) 
	{
		strcat(destiny, code_block(slice(input, i * bits, (i + 1) * bits)));
		printf("%d\n", strlen(destiny));
	}


	if (ln % bits != 0)
	{
		strcat(destiny, code_block(slice(input, ln * bits, strlen(input))));
	}
}

int get_block_len(int bits) 
{
	for (int i = 0; i < bits; i++) 
		if (isPowerOfTwo(i) == 1)
			bits++;
	return bits;
}

void write_by_bits(char* filename, char* input)
{
	FILE* f = fopen(filename, "w");
	int ln = strlen(input) / 8;
	if (strlen(input) % 8 != 0)
	{
		char* zeros = (char*)malloc(8);
		zeros[0] = '\0';
		memset(zeros, '0', 8 - strlen(input) % 8);
		zeros[8 - strlen(input) % 8] = '\0';
		strcat(input, zeros);
		ln += 1;
	}
	int i = 0;
	for (i = 0; i < ln; i++) {
		fputc(bin_to_dec(slice(input, i * 8, (i + 1) * 8)), f);
		printf("%d %c ", i,bin_to_dec(slice(input, i * 8, (i + 1) * 8)));
	}
	fclose(f);
}

void decode_string(char* input, char* output, int bits)
{

	char* test = (char*)malloc(bits + 1);
	memset(test, '1', bits);
	test[bits] = '\0';
	int block_ln = strlen(code_block(test));//длинна bits битов после кодировки
	while (strlen(input) > block_ln)
	{
		strcat(output, decode_block(slice(input, 0, block_ln)));
		input += block_ln;
	}
	if (strlen(input) != 0)
	{
		strcat(output, decode_block(slice(input, 0, strlen(input))));
	}
	output[strlen(output) / 8 * 8] = '\0';
}

int count_symbols(char* filename) 
{
	FILE* f = fopen(filename, "r");
	fseek(f, 0, SEEK_END);
	int n = ftell(f);
	fclose(f);
	return n;
}

void code_text()
{
	int bits;
	printf("Bits in block : ");
	scanf("%d", &bits);

	int block_len = get_block_len(bits);

	char* input_bits = (char*)malloc(count_symbols((char*)INPUT) * bits + 1);
	input_bits[0] = '\0';

	read_by_bits((char*)INPUT, input_bits);

	char* coded_bits = (char*)malloc(count_symbols((char*)INPUT) * block_len + 1);
	coded_bits[0] = '\0';

	code_string(input_bits, coded_bits, bits);

	write_by_bits((char*)CODED, coded_bits);

	printf("Coded\n");
}

void decode_text()
{
	int bits;

	printf("Bits in block : ");
	scanf("%d", &bits);


	int block_ln = get_block_len(bits);//длинна bits битов после кодировки

	char* coded_bits = (char*)malloc(count_symbols((char*)CODED) * block_ln + 1);
	coded_bits[0] = '\0';

	read_by_bits((char*)CODED, coded_bits);

	char* decoded_bits = (char*)malloc(count_symbols((char*)CODED) * bits + 1);
	decoded_bits[0] = '\0';

	decode_string(coded_bits, decoded_bits, bits);
	write_by_bits((char*) DECODED, decoded_bits);
	printf("Decoded\n");
}

int main()
{
	printf("%d",get_block_len(32));
}
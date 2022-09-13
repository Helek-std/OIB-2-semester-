#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define OUTPUT_TEXT "text_output.txt"
#define OUTPUT "new_image.bmp"
#define INPUT "input.bmp"
#define LEN 10000
#define TEXT "input.txt"

typedef struct {
	unsigned char name[2];
	unsigned int size;
	int garbage; //not required
	unsigned int offset; //offset from where image starts in file
} BITMAP_header;

unsigned char* dec_to_bin(int number) {
	int buff = number;
	static unsigned char ret[8];
	for (int i = 0; i < 8; i++)ret[i] = '0';
	for (int i = 0; buff > 0; i++)
	{
		ret[7 - i] = (unsigned char)buff % 2 + 48;
		buff = buff / 2;
	}
	return ret;
}

int bin_to_dec(char bits[8])
{
	int res = 0;
	for (int i = 0; i < 8; i++)
	{
		res += pow(2, (7 - i)) * ((int)bits[i] - 48);
	}
	return res;
}

unsigned char* dec_to_bin_len(int number)
{
	int buff = number;
	static unsigned char ret[16];
	for (int i = 0; i < 16; i++)ret[i] = '0';
	for (int i = 0; buff > 0; i++)
	{
		ret[15 - i] = (unsigned char)buff % 2 + 48;
		buff = buff / 2;
	}
	return ret;
} //Terrible crutch

int bin_to_dec_len(unsigned char bits[16]) {
	int res = 0;
	for (int i = 0; i < 16; i++)
	{
		res += pow(2, (15 - i)) * ((int)bits[i] - 48);
	}
	return res;
}//One more terrible crutch

int change_byte(int byte, char* bit)
{
	char bits[9];
	int new_byte = byte;
	memcpy(bits, dec_to_bin(new_byte), 8);
	for (int i = 8 - strlen(bit); i < 8; i++)
		bits[i] = *bit++;
	return bin_to_dec(bits);
}

char* get_txt(char* name)
{
	char* text = (char*)malloc(LEN);
	FILE* fp = fopen(name, "r");
	int index = 0;
	while (fgets(text + index, 1000, fp) != NULL) index = strlen(text);
	return text;
	fclose(fp);
}

unsigned char* cut(unsigned char* text_bits,int density) 
{
	int i;
	unsigned char* ret = (unsigned char*)malloc(8);
	for (i = 0; i < density; i++)
		if (*text_bits != '\0')
			ret[i] = *text_bits++;
		else ret[i] = '0';
	ret[density] = '\0';
	return ret;

}

int make_image(int density)
{
	int i,j,k;
	int cnt = 0;
	unsigned char c;
	char* temp = (char*)malloc(9);

	FILE* image = fopen(INPUT, "r");
	FILE* new_image = fopen(OUTPUT, "w");

	BITMAP_header header;

	fread(header.name, 2, 1, image);
	fread(&header.size, 3 * sizeof(unsigned int), 1, image);

	if (header.name[0] != 'B' || header.name[1] != 'M') 
	{
		printf("Error reading\n");
		return 1;
	}

	fseek(image, 0L, SEEK_SET);

	for (i = 0; i < 54; i++)fputc(fgetc(image), new_image);


	char* text = (char*)malloc(LEN);
	text = get_txt(TEXT);
	int length = strlen(text);

	if (length * 8 / density > header.size - header.offset - 24) 
	{
		printf("Error, message too large\n");
		return 2;
	}

	unsigned char* len = (unsigned char*)malloc(17);//buffers
	unsigned char* symb = (unsigned char*)malloc(2);//buffers

	memcpy(len, dec_to_bin_len(length), 16);
	
	len[16] = '\0';
	while (*len != '\0')
	{
		memcpy(symb, len++, 1);
		symb[1] = '\0';
		fputc(change_byte(fgetc(image),symb),new_image);
	}//writing length of message

	unsigned char* buff = (unsigned char*)malloc(9);
	memcpy(buff, dec_to_bin(density), 8);
	buff[8] = '\0';
	while (*buff != '\0') {
		memcpy(symb, buff++, 1);
		fputc(change_byte(fgetc(image), symb), new_image);
	}//writing density of message

	unsigned char* text_bits = (unsigned char*)malloc(length * 8 + 1);
	text_bits[0] = '\0';

	while (*text) 
		strcat(text_bits, dec_to_bin((int)*text++));
	text_bits[length * 8] = '\0';
	//creating string that contains text bits
	for (i = 0; i < header.size - header.offset - 24; i++) 
	{
		c = fgetc(image);
		if (*text_bits != '\0')
		{
			//printf("to code - %s ", dec_to_bin(c));
			fputc(change_byte(c, cut(text_bits, density)), new_image);
			text_bits += density;
			//printf("coded - %s\n", dec_to_bin(change_byte(c, cut(text_bits, density))));
		}
		else fputc(c, new_image);
	}

	fclose(image);
	fclose(new_image);
}

void text_from_image()
{
	FILE* text = fopen(OUTPUT_TEXT, "w");
	FILE* image = fopen(OUTPUT, "r");

	int cnt = 0;
	int i;
	int c;

	unsigned char* len_buff = (unsigned char*)malloc(16);
	unsigned char* buff = (unsigned char*)malloc(8);

	fseek(image, 54, SEEK_CUR);
	for (i = 0; i < 16; i++)
	{
		c = fgetc(image);
		len_buff[i] = dec_to_bin(c)[7];
	}
	int len = bin_to_dec_len(len_buff);
	printf("Len is -> %d\n", len);
	for (i = 0; i < 8; i++)
	{
		c = fgetc(image);
		buff[i] = dec_to_bin(c)[7];
	}
	int density = bin_to_dec(buff);
	printf("Density is -> %d\n", density);

	unsigned char* text_bits = (unsigned char*)malloc(len * 8 + 1);
	text_bits[0] = '\0';
	for (i = 0; i < len * 8 / density; i++) 
	{
		c = fgetc(image);
		strcat(text_bits,(dec_to_bin(c) + 8 - density));
	}
	unsigned char* symb = malloc(9);
	while(*text_bits!='\0')
	{
		memcpy(symb, text_bits, 8);
		symb[8] = '\0';
		text_bits += 8;
		fputc((char)bin_to_dec(symb), text);
	}

}

int main()
{
	make_image(1);
	text_from_image();
}
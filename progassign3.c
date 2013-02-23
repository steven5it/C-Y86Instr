#include <stdio.h>
#include <stdlib.h>

void fileType (FILE* file_name);
void printContents (unsigned char *buffer, int maximum);
unsigned short loadAddress (FILE* file_name, int c1);
unsigned short byteCount (FILE* file_name);
int fourBytes (int i);
void decode(int number);
unsigned char* buff;

int main (int argc, char** argv)
{
	char* filename = argv[1];
	buff =  calloc(135000, sizeof(char));
	unsigned short address;
	unsigned short count;
	int max = 0;
	/*ensure there is enough memory*/
	if (buff == NULL)
	{
		printf("Out of memory.\n");
		return 1;
	}

	FILE* fp;	/*declare file pointer*/
	fp = fopen(filename, "rb");
	/*if file is NULL, does not exist*/
	if (fp == NULL)
	{
		printf("File does not exist.\n");
		return 1;
	}


	/*check if valid object file */
	int c1;

	fileType (fp);
	while (1)
	{
		int i;
		/*if EOF reached after reading a block, then file reading is complete*/
		if ((c1 = fgetc(fp)) == EOF) break;
		address = loadAddress(fp, c1);
		count = byteCount (fp);
		/*if address values exceed range*/
		if (address + count > max)
		{
			max = address + count;
		}
		if (address + count > 0x1fffe)
		{
			printf("The Y86 Object file is corrupted.\n");
			return 1;
		}
		/*loop through contents*/
		for (i = 0; i < count; i++)
		{
			c1 = fgetc(fp);
			/*if EOF reached before byteCount full, invalid file*/
			if (c1 == EOF)
			{
				printf("The Y86 Object file is corrupted.\n");
				return 1;
			}
			else
				buff[address++] = (unsigned char) c1;
		}
	}
	printContents(buff, max);
	fclose(fp);
	free (buff);
	return 0;
}

void fileType (FILE * file_name)
{
	int c1 = fgetc(file_name);
	int c2 = fgetc(file_name);
	/*check if file is Y86 object*/
	if (!(c1 == 0x79 && c2 == 0x62))
	{
		printf ("The file is not a Y86 object file.\n");
		exit (EXIT_FAILURE);
	}
}

unsigned short loadAddress(FILE* file_name, int c1)
{
	unsigned short s;
	unsigned char c2 = fgetc(file_name);
	/*if there is a partial load address without any data*/
	if ((int)c2 == EOF)
	{
		printf("The Y86 Object file is corrupted.\n");
		exit (EXIT_FAILURE);
	}
	/*return the load address in positive format*/
	s = ((((unsigned char)c1) << 8) | c2);
	return s;
}

unsigned short byteCount (FILE* file_name)
{
	short s;
	unsigned char c1 = fgetc(file_name);
	if ((int)c1 == EOF)
	{
		printf("The Y86 Object file is corrupted.\n");
		exit (EXIT_FAILURE);
	}
	unsigned char c2 = fgetc(file_name);
	/*if there is a partial bytecount without any data*/
	if ((int)c2 == EOF)
	{
		printf("The Y86 Object file is corrupted.\n");
		exit (EXIT_FAILURE);
	}
	/*return the byteCount*/
	s = ((c1 << 8) | c2);
	return (unsigned) s;
}

/*interpret 4 bytes with little endianness*/
int fourBytes (int i)
{
	int num;
	int j;
	num = 0;
	for (j = 3; j >= 0; j--)
	{
		num = (num <<  8) | buff[i + j];
	}
	return num;
}

void decode (int number)
{
	switch (number)
	{
		case 0:
		{
			printf("halt");
			break;
		}
		case 0x10:
		{
			printf("nop");
			break;
		}
		case 0x20:
		{
			printf("rrmovl");
			break;
		}
		case 0x21:
		{
			printf("cmovle");
			break;
		}
		case 0x22:
		{
			printf("cmovl");
			break;
		}
		case 0x23:
		{
			printf("cmove");
			break;
		}
		case 0x24:
		{
			printf("cmovne");
			break;
		}
		case 0x25:
		{
			printf("cmovge");
			break;
		}
		case 0x26:
		{
			printf("cmovg");
			break;
		}
		case 0x30:
		{
			printf("irmovl");
			break;
		}
		case 0x40:
		{
			printf("rmmovl");
			break;
		}
		case 0x50:
		{
			printf("mrmovl");
			break;
		}
		case 0x60:
		{
			printf("addl");
			break;
		}
		case 0x61:
		{
			printf("subl");
			break;
		}
		case 0x62:
		{
			printf("andl");
			break;
		}
		case 0x63:
		{
			printf("xorl");
			break;
		}
		case 0x70:
		{
			printf("jmp");
			break;
		}
		case 0x71:
		{
			printf("jle");
			break;
		}
		case 0x72:
		{
			printf("jl");
			break;
		}
		case 0x73:
		{
			printf("je");
			break;
		}
		case 0x74:
		{
			printf("jne");
			break;
		}
		case 0x75:
		{
			printf("jge");
			break;
		}
		case 0x76:
		{
			printf("jg");
			break;
		}
		case 0x80:
		{
			printf("call");
			break;
		}
		case 0x90:
		{
			printf("ret");
			break;
		}
		case 0xA0:
		{
			printf("pushl");
			break;
		}
		case 0xB0:
		{
			printf("popl");
			break;
		}
		/*invalid opcode*/
		default:
		{
			printf("invalid");
			break;
		}
	}
}

void printContents (unsigned char *buffer, int maximum)
{
	int i;
	for (i = 0; i < maximum; i++)
	{
		int instruct = fourBytes(i);
		printf ("%04X %02X ",i , buffer[i]);
		printf ("%08X ", instruct);
		printf ("%11d ", instruct);
		decode(buffer[i]);
		printf("\n");
	}
}

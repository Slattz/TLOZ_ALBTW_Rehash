#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "TLOZ_ALBTW_Rehash.h"

#define SIZE1 0xDFC //Checksum: 4 bytes @ 0xDFC
#define SIZE2 0x5FC //Checksum: 4 bytes @ 0x13FC
#define OFFSET1 0xDFC
#define OFFSET2 0x13FC

void wait_to_exit(void)
{
    printf("\nPress Any Key To Exit.\n");
    getchar(); 
    return;
}

void help_text(void)
{
    printf("TLOZ A Link Between Worlds 3DS Checksum Fixer v1.0\n");
    printf("By Slattz\n");
    printf("Usage: 'TLOZ_ALBTW_Rehash.exe filename', or drag-and-drop a file.\n\n");
}

u32 Fix_TLOZ_ALBTW_Checksum(u8* data, u32 length)
{
    u32 checksum = 0xFFFFFFFF;
    if (length > 0)
    {
        int i;
        u32 j;
        int offset = -1;
        u8 byte = 0;
        u32 unk;

        for (i = length>>1; i > 0; i--)
        {
            for (j = 0; j < 2; j++)
            {
                byte = (u8)(data[offset+1+j]);
                byte ^= checksum;
                byte &= 0xFF;
                unk = CRC_TABLE[ (byte<<2)/4 ];
                checksum = unk ^ (checksum>>8);
            }
            offset += 2;
        }
    }

    return ~checksum;
}

int main(int argc, char* argv[])
{
    help_text();
    if(argc < 2)
    {
        printf("Specify a filename or drag-and-drop a file.\n");
        wait_to_exit();
        return 0;
    }

    FILE *save = fopen(argv[1], "r+b");
    if (save == NULL)
    {
        printf("Error opening file!\n");
        wait_to_exit();
        return 0; 
    }
    
    fseek(save, 0, SEEK_END);
    u32 savesize = ftell(save);

    if (savesize != 0x1604)
    {
        printf("File isn't the correct size (0x1604)!\n");
        wait_to_exit();
        return 0;
    }

    u8 *buffer = malloc(SIZE1);
    
    //Checksum 1
    rewind(save);
    fread(buffer, 1, SIZE1, save);
    u32 checksum = Fix_TLOZ_ALBTW_Checksum(buffer, SIZE1);
    fseek(save, OFFSET1, SEEK_SET);
    fwrite(&checksum, 1, 4, save);
    free(buffer);

    //Checksum 2
    buffer = malloc(SIZE2);
    fseek(save, OFFSET1+4, SEEK_SET);
    fread(buffer, 1, SIZE2, save);
    checksum = Fix_TLOZ_ALBTW_Checksum(buffer, SIZE2);
    fseek(save, OFFSET2, SEEK_SET);
    fwrite(&checksum, 1, 4, save);
    free(buffer);

    printf("Save Fixed\n");
    fclose(save);
    return 0;
}

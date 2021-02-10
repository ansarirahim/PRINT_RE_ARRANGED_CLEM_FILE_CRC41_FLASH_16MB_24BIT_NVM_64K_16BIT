

//A. R. Ansari listed code to read line by line
//gcc read_ClemFile_Print_CRC41_FLASH_16M_NVM64K_R01.c -o read_ClemFile_Print_CRC41_FLASH_16M_NVM64K_R01
//Ver 1.00 Rev1.00
//For read clemfile nvm  64K and flash file 24bit 16MB
// AND PRINT CRC41 OF THE CLEM FILE EXAMPLE
//21,2
// WHERE 21=> CRC41 OF THE NVM 64K FILE
// 2=> CRC41 OF THE FLASH 24BIT 16MB FILE
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
//24bit all ff crc41=178
//16bit all ff crc41=58

/*
struct pageData{
//    unsigned int pageNumber;
    unsigned char page[256];

};*/
typedef unsigned char BYTE;
//pageData nPageData[256];
#define _BLANK_ 0
#define _NOT_BLANK_ 1
#define FLASH_SIZE (0x10000UL)
typedef struct Flashpage
{
    BYTE dataProperty;

    unsigned char page[256];
    /// unsigned int pageNo;

} flashPg;

flashPg fpages[FLASH_SIZE];
#define NVM_SIZE (0x100UL)
typedef struct NVMpage
{
    BYTE dataProperty;

    unsigned char page[256];
    unsigned int pageNo;

} nvmPg;

nvmPg npages[NVM_SIZE];

BYTE getNVM_CRC(BYTE *CKBlock, int Length, BYTE Seed)
{
    BYTE val, y, crc, bbit1;
    int k;
    crc = Seed;
    for (k = 0; k < Length; k++)
    {
        val = CKBlock[k];
        crc = crc ^ ((val << 4));
        for (y = 0; y < 8; y++)
        {
            bbit1 = crc & 0x80;
            if (bbit1)
            {
                crc = crc ^ 74;
                bbit1 = 1;
            }
            else
                bbit1 = 0;
            crc = ((crc << 1) + bbit1);
        }
        crc = crc ^ 82;
        crc = crc ^ (val >> 4);
    }

    return (crc);
}

BYTE CharToHex(char ch)
{
    // Convert to upper case
    ch = toupper(ch);

    // Parse nibble
    if (ch >= '0' && ch <= '9')
    {
        return (ch - '0');
    }
    else if (ch >= 'A' && ch <= 'F')
    {
        return (ch - 'A' + 10);
    }
    else
    {
        // Bad input character
        //    assert(false);
        return 0; // to suppress warning C4715
    }
}
BYTE ParseByte(const char *str) //const char *str)
{

    char highNibbleChar = str[0];
    char lowNibbleChar = str[1];

    BYTE highNibble = CharToHex(highNibbleChar);
    BYTE lowNibble = CharToHex(lowNibbleChar);

    return (lowNibble + (highNibble << 4));
}
char hexASCII_addr[2];
char hexASCII_index[2];
char hexASCII_data[64];
#define ADDRESS_LENGTH 12

unsigned long clemFullAddress = 0;
BYTE hexASCII_full_address[ADDRESS_LENGTH];
#define START_ADDRESS_INDEX 0
typedef struct addressSturture
{
    unsigned int address;
    int addresslength;
} memoryAddress;
memoryAddress clemFileMemoryAddress;
#define MINMUM_ADDRESS_LENGTH 4
#define MAXIMUM_ADDRESS_LENGTH 6
// static inline void swapObjects(char *o1, char *o2, int size)
// {
//     for (int k = 0; k < size; k++, o1++, o2++)
//     {
//         char tmp = *o1;
//         *o1 = *o2;
//         *o2 = tmp;
//     }
// }
void rvereseArray(BYTE arr[], int start, int end)
{
    int temp;
    while (start < end)
    {
        temp = arr[start];
        arr[start] = arr[end];
        arr[end] = temp;
        start++;
        end--;
    }
}
#define MAX_DATA_BYTES 64
typedef struct dataBytes
{
    BYTE data[MAX_DATA_BYTES];
    int data_len;
} dataBytes;
dataBytes dBytes;

int asciiHexDataToBytes(char *s)
{
    int i = 0;
    if (*s == '\0')
        return -1;
    dBytes.data_len = strlen(s);
    BYTE add[2];
    memset(dBytes.data, 0xff, MAX_DATA_BYTES);
    for (i = 0; i < dBytes.data_len / 2; i++)
    {

        add[0] = *s;
        *s++;
        add[1] = *s;

        dBytes.data[i] = ParseByte(add);

        *s++;

        if (*s == '\0')
            break;
    }
    dBytes.data_len /= 2;
    return 0;
}
int memoryType = 0;
#define MEMORY_FLASH 2
#define MEMORY_NVM 0
int asciitoFullAddress(char *s)
{
    ///unsigned long fullAddress;
    unsigned int i = 0;
    ///unsigned int b = 0;
    BYTE add[2];
    BYTE address;
    clemFileMemoryAddress.address = 0;

    clemFileMemoryAddress.addresslength = strlen(s);
    if (clemFileMemoryAddress.addresslength < MINMUM_ADDRESS_LENGTH)
    {
        printf("\nNACK"); //INVALID ADDRESS LENGTH");
        return 0;
    }
    if (clemFileMemoryAddress.addresslength > MAXIMUM_ADDRESS_LENGTH)
    {
        if (*s == '2')
        {
            s++;
            s++;
            memoryType = MEMORY_FLASH;
            clemFileMemoryAddress.addresslength -= 2;
        }
        else
        {
            //default is considered as NVM
            memoryType = MEMORY_NVM;
        }
    }
    else
        memoryType = MEMORY_NVM;

    //while (*s != '\0')
    memset(hexASCII_full_address, '\0', ADDRESS_LENGTH);
    for (i = 0; i < clemFileMemoryAddress.addresslength / 2; i++)
    {

        add[0] = *s;
        *s++;
        add[1] = *s;
        address = ParseByte(add);
        //hexASCII_full_address[(clemFileMemoryAddress.addresslength/2)-1-i]=ParseByte(add);///address;
        hexASCII_full_address[i] = ParseByte(add);
        *s++;

        ///    printf("\nParse byte ad=%.4X", hexASCII_full_address[i]); ///
        if (*s == '\0')
            break;
    }
    ////  printf("\nnormal=");
    clemFileMemoryAddress.address = 0;
    //unsigned int i=0;
    for (i = 0; i < clemFileMemoryAddress.addresslength / 2; i++)
    {
        clemFileMemoryAddress.address |= hexASCII_full_address[i] << (8 * ((clemFileMemoryAddress.addresslength / 2 - 1) - i));
        ///  printf("%.2X", hexASCII_full_address[i]);
    }

    switch (clemFileMemoryAddress.addresslength)
    {
    case 4:
        // printf("\nLength address=%.4X", clemFileMemoryAddress.address);
        break;
    case 6:
        ///////////// printf("\nLength 6address=%.6X", clemFileMemoryAddress.address);
        break;
    case 8:
        ///////////// printf("\nLength 8address=%.8X", clemFileMemoryAddress.address);
        break;
    case 10:;
        /////////////printf("\nLength 10address=%.10X", clemFileMemoryAddress.address);
    }

    return 1;
}
void splitLine(char *str, int init_size) /// nvmPg *nvmpage,char *str,int init_size)
{
    int k = 0;
  //  BYTE currentPage;
    unsigned int currentPage;
    BYTE currentByte;
    int length = 0;
    int length1 = 0;
    int i = 0;
    unsigned char pageIndex = 0;
    unsigned int higherByte;
    unsigned int lowerByte;
    unsigned int address;

    //int init_size = strlen(str);
    char delim[] = " ";
    //hexASCII_adress[8];
    //har hexAscc
    char *ptr = strtok(str, delim);
    ///currentPage=ParseByte(ptr+2);
    while (ptr != NULL)
    {
        switch (k)
        {
        case 0:

            if (asciitoFullAddress(ptr))
            {
                ; ///printf("\n&&&&&&&\n");
            }
            else
            {
                printf("\nNACK"); //
                exit(0);
            }
            break;
        case 1:
            ///  printf("\n*****\n");
            if (asciiHexDataToBytes(ptr) < 0)
            {
                printf("\nNACK");
                exit(0);
            }
            else
            {

                currentPage = clemFileMemoryAddress.address >> 8;
                pageIndex = clemFileMemoryAddress.address;
                k = dBytes.data_len + pageIndex;
                ///////////printf("\nAddress=%.4X,Cp=%.2X, pageIndex=%.2X,total index=%d\n",clemFileMemoryAddress.address,currentPage,pageIndex,k);
                for (i = 0; i < dBytes.data_len; i++) //
                {

                    if ((i + pageIndex) < 256)
                    { ///printf("%.2X", dBytes.data[i]);

                        switch (memoryType)
                        {
                        case MEMORY_NVM:
                            npages[currentPage].page[(pageIndex + i) % 256] = dBytes.data[i % 256];
                            break;

                        case MEMORY_FLASH:
                            //  printf("\nFlash data");
                            fpages[currentPage].page[(pageIndex + i) % 256] = dBytes.data[i % 256];
                            break;
                        }
                    }
                    else
                    {
                        ///printf("\nPI=%.2X=>data=%.2X\n", currentPage+1,dBytes.data[i]);
                        switch (memoryType)
                        {
                        case MEMORY_NVM:
                            npages[currentPage + 1].page[(pageIndex + i) % 256] = dBytes.data[i];
                            break;

                        case MEMORY_FLASH:
                            fpages[currentPage + 1].page[(pageIndex + i) % 256] = dBytes.data[i];
                            break;
                        }
                    }
                }
                k = 0;
            }
        }
        //	 printf("\nlength2=%d\n",length1);
        ptr = strtok(NULL, delim);
        k++;
    }
}
void splitLineBasic(char *str, int init_size)
{
    //int init_size = strlen(str);
    char delim[] = " ";

    char *ptr = strtok(str, delim);

    while (ptr != NULL)
    {
        ///printf("'%s'\n", ptr);
        ptr = strtok(NULL, delim);
    }
}
char *readline(FILE *fp, char *buffer)
{
    int ch;
    int i = 0;
    size_t buff_len = 0;

    buffer = malloc(buff_len + 1);
    if (!buffer)
        return NULL; // Out of memory

    while ((ch = fgetc(fp)) != '\n' && ch != EOF)
    {
        buff_len++;
        void *tmp = realloc(buffer, buff_len + 1);
        if (tmp == NULL)
        {
            free(buffer);
            return NULL; // Out of memory
        }
        buffer = tmp;

        buffer[i] = (char)ch;
        i++;
    }
    buffer[i] = '\0';

    // Detect end
    if (ch == EOF && (i == 0 || ferror(fp)))
    {
        free(buffer);
        return NULL;
    }
    if ((buffer[0] == ';') || (buffer[0] == '#'))
    {
        //	buffer++;//=buffer+1;
        //splitLine(buffer,i);
    }
    else
        splitLine(buffer, i);
    return buffer;
}

void lineByline(FILE *file)
{

    //// int linenumber = 0;
    char *s;
    //int j=0;
    while ((s = readline(file, 0)) != NULL)
    {
        ///puts(s);
        free(s);
        ////// linenumber++
        ///  printf("\nline number completed=%d", linenumber++);
    }
}

void Init()
{
    int i = 0;
    int j = 0;
    //char cmd_buffer[80];char cmd_buffer[80];
    for (j = 0; j < NVM_SIZE; j++)
    {
        npages[j].dataProperty = _BLANK_;
        for (i = 0; i < 256; i++)
            npages[j].page[i] = 0xff;
    }

    for (j = 0; j < FLASH_SIZE; j++)
    {
        fpages[j].dataProperty = _BLANK_;
        for (i = 0; i < 256; i++)
            fpages[j].page[i] = 0xff;
    }
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int checkFileExist(char *s)
{

    {

        int i = 1;

        unsigned int len = 0;
        {
            while (*(s + len))
                len++;
            printf("\nlen of finame=%u", len); //
            if (len > 0)
            {
                char buff[100] = "find /root/PROGRAM/CLemFile/003812.j19  -type f -exec grep -l 'searchstring' {} \\;";

                sprintf(buff, "find /root/PROGRAM/CLemFile/%s  -type f -exec grep -l 'searchstring' {} \\; > /dev/null 2>&1", s); //
                i = system(buff);
            }
            else
                i = 1;
        }
        switch (i)
        {

        case 0:
            printf("ACK\r\n");
            break;
        default:
        {
            printf("NACK\r\n");
            exit(0);
        }
        }
        //printf("system()=%d\n",i);//(system(buff));//
        return 0;
    }
}
char buff1[100];
int checkScript(char *s)
{
    sprintf(buff1, "CHECK_CLEM_FILE_NAME_FULL_PATH %s > /dev/null 2>&1", s);

    ////printf("\ncmd=%s\n\n",buff1);
    return (system(buff1));
}
///="/root/PROGRAM/CLemFile/TEST_files/ClemFile_nvm_64k";
#define __FIRMWARE_VERSION__ "VER 1.00 REV 1.00\n"

bool checkData(BYTE *pagebuffer)
{
    unsigned int i = 0;
    bool x = _BLANK_;
    for (i = 0; i < 256; i++)
    {
        if (pagebuffer[i] < 0xff)
        {
            x = _NOT_BLANK_;

            break;
        }
    }
    return x;
}

char debugFilePath[100]; 
void printTFileWithMemoryInf(int memorytype,int newline, unsigned char *buff, unsigned int pageNUMBER, int crc) //,int len)
{
    int i = 0;
    /*
unsigned char x[256];
for(i=0;i<256;i++)
{
sprintf(x,"%.2X",buff[i];
}*/
    /* File pointer to hold reference of input file */

    FILE *fPtr;
    //char filePath[100];

    //    char dataToAppend[BUFFER_SIZE];
    /*  Open all file in append mode. */
    fPtr = fopen(debugFilePath, "a");

    if (fPtr == NULL)
    {
        /* Unable to open file hence exit */
        printf("\nUnable to open '%s' file.\n", debugFilePath);
        printf("Please check whether file exists and you have write privilege.\n");
        exit(EXIT_FAILURE);
    }
switch(memorytype)
{
    case MEMORY_FLASH :
    fprintf(fPtr, "\nFlash PageNo(Hex)=02%.8X", pageNUMBER); ///fprintf(fPtr, "\n",i);
    for (i = 0; i < 256; i++)
    {

        if (i % (newline) == 0)
            fprintf(fPtr, "\n", "");

        fprintf(fPtr, "%.2X", buff[i]);
    }
    fprintf(fPtr, "\nFlash CRC(Decimal)=%d", crc);
    fclose(fPtr);
    break;
    case MEMORY_NVM :
    fprintf(fPtr, "\nNVM PageNo(Hex)=%.8X", pageNUMBER); ///fprintf(fPtr, "\n",i);
    for (i = 0; i < 256; i++)
    {

        if (i % (newline) == 0)
            fprintf(fPtr, "\n", "");

        fprintf(fPtr, "%.2X", buff[i]);
    }
    fprintf(fPtr, "\nNVM CRC(Decimal)=%d", crc);
    fclose(fPtr);
    break;
}
    
}

                                                           ///="/root/PROGRAM/CLemFile/TEST_files/ClemFile_nvm_64k";
void printTFile(int newline, unsigned char *buff, unsigned int pageNUMBER, int crc) //,int len)
{
    int i = 0;
    /*
unsigned char x[256];
for(i=0;i<256;i++)
{
sprintf(x,"%.2X",buff[i];
}*/
    /* File pointer to hold reference of input file */

    FILE *fPtr;
    //char filePath[100];

    //    char dataToAppend[BUFFER_SIZE];
    /*  Open all file in append mode. */
    fPtr = fopen(debugFilePath, "a");

    if (fPtr == NULL)
    {
        /* Unable to open file hence exit */
        printf("\nUnable to open '%s' file.\n", debugFilePath);
        printf("Please check whether file exists and you have write privilege.\n");
        exit(EXIT_FAILURE);
    }

    fprintf(fPtr, "\nPageNo(Hex)=%.8X", pageNUMBER); ///fprintf(fPtr, "\n",i);
    for (i = 0; i < 256; i++)
    {

        if (i % (newline) == 0)
            fprintf(fPtr, "\n", "");

        fprintf(fPtr, "%.2X", buff[i]);
    }
    fprintf(fPtr, "\nCRC(Decimal)=%d", crc);
    fclose(fPtr);
}


#define __DEBUG_FILE_PATH_INDEX__ 2
#define __CLEM_FILE_PATH_INDEX__ 1
int main(int argc, char **argv)
{

    int x = 0;
    int page = 0;
    int crc64K;
    char ch = 0;
    int crc16M = 0;
    char cmd_buffer[80];

    memset(hexASCII_full_address, '\0', ADDRESS_LENGTH);

    Init();
    if (argc == 1)
    {
        printf(__FIRMWARE_VERSION__);
        exit(0);
    }


   for (ch = 0; ch < 100; ch++)
    {
        if (argv[__DEBUG_FILE_PATH_INDEX__][ch] == '\0')
            break;
        debugFilePath[ch] = argv[__DEBUG_FILE_PATH_INDEX__][ch]; //
    }
    ch = 0;
    // fp=freopen(NULL,"w",fp);
    fclose(fopen(debugFilePath, "w"));
    FILE *file = fopen(argv[__CLEM_FILE_PATH_INDEX__], "r");

    lineByline(file);

    crc64K = 80;
    x = 0;

    for (page = 0; page < NVM_SIZE; page++)
    {

        //for(x=0;x<256;x++)
        {
            crc64K = getNVM_CRC(npages[page].page, 256, crc64K);
             npages[page].dataProperty = checkData(fpages[page].page);
            if (npages[page].dataProperty == _NOT_BLANK_)
            {
                printTFileWithMemoryInf(MEMORY_NVM,32, npages[page].page, page, crc64K);
            }

            ///   if(npages[page].dataProperty==_NOT_BLANK_)
            {
                ///z      printTFile(newl, npages[page].page, page, crc64K);
                // x++;
            }
        }
    }

    // printf("\n%d,", crc64K);
    crc16M = 80;
    x = 0;

    for (page = 0; page < FLASH_SIZE; page++)
    {

        //for(x=0;x<256;x++)
        {
            crc16M = getNVM_CRC(fpages[page].page, 256, crc16M);
            fpages[page].dataProperty = checkData(fpages[page].page);
            if (fpages[page].dataProperty == _NOT_BLANK_)
            {
                printTFileWithMemoryInf(MEMORY_FLASH,32, fpages[page].page, page, crc16M);
            }

            ///   if(npages[page].dataProperty==_NOT_BLANK_)
            {
                ///z      printTFile(newl, npages[page].page, page, crc64K);
                // x++;
            }
        }
    }

     FILE * fPtr = fopen(debugFilePath, "a");

    if (fPtr == NULL)
    {
        /* Unable to open file hence exit */
        printf("\nUnable to open '%s' file.\n", debugFilePath);
        printf("Please check whether file exists and you have write privilege.\n");
        exit(EXIT_FAILURE);
    }
     fprintf(fPtr, "\nNVM_CRC(Decimal)=%d FLASH_CRC(Decimal)=%d", crc64K, crc16M);
    fclose(fPtr);
    printf("%d,%d\n", crc64K, crc16M);

    return 0;
}

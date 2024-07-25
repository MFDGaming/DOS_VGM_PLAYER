#include "vgm.h"
#include <stdio.h>
#include <stdlib.h>

#define VGM_DOS

#ifdef VGM_DOS
#include <dos.h>

void write_reg(unsigned short port, unsigned char reg, unsigned char data, unsigned char second) {
    int i;
    if (second != 0) {
        port += 2;
    }
    outp(port, reg);
    i = 6;
    while (i--) inp(port);
    outp(port + 1, data);
    i = 35;
    while (i--) inp(port);
}
#else
void write_reg(unsigned short port, unsigned char reg, unsigned char data, unsigned char second) {
}

void delay(unsigned int ms) {}
#endif

int main() {
    FILE *fp = NULL;
    size_t len = 0;
    vgm_header_t *header = NULL;
    unsigned short samples = 0;
    unsigned char reg = 0;
    unsigned char reg_data = 0;
    unsigned long i = 0;
    unsigned char *data = NULL;
    unsigned char command = 0;
    unsigned char do_loop = 0;
    
    fp = fopen("test.vgm", "rb");
    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    printf("FILE LENGTH: %u\n", len);
    fseek(fp, 0, SEEK_SET);

    data = malloc(len);
    fread(data, 1, len, fp);

    fclose(fp);
    
    header = (vgm_header_t *)data;

    printf("VERSION: %X\n", header->version);
    printf("CLOCK_RATE: %u\n", header->ymf262_clock);
    printf("DATA_OFFSET: %u\n", header->vgm_data_offset);
    
    for (i = header->vgm_data_offset + 52; i < header->eof_offset;) {
        command = data[i++];
        if (command == 0x5E) {
            reg = data[i++];
            reg_data = data[i++];

#ifdef VGM_DEBUG
            printf("[PORT0] REGISTER (%u) <- %u\n", reg, reg_data);
#endif
            write_reg(0x388, reg, reg_data, 0);
        } else if (command == 0x5F) {
            reg = data[i++];
            reg_data = data[i++];
#ifdef VGM_DEBUG
            printf("[PORT0] REGISTER (%u) <- %u\n", reg, reg_data);
#endif
            write_reg(0x388, reg, reg_data, 1);
        } else if (command == 0x61) {
            samples = *((unsigned short *)(&data[i]));

            i += 2;

#ifdef VGM_DEBUG
            printf("[L] WAIT for %u samples\n", samples);
#endif
            delay((unsigned long)(((double)samples / 44100.0) * 1000));
        } else if (command == 0x62) {
#ifdef VGM_DEBUG
            printf("WAIT for 735 samples\n");
#endif
            delay((unsigned long)(((double)735 / 44100.0) * 1000));
        } else if (command == 0x63) {
#ifdef VGM_DEBUG
            printf("WAIT for 882 samples\n");
#endif
            delay((unsigned long)(((double)882 / 44100.0) * 1000));
        } else if (command == 0x66) {
            printf("END OF SOUND DATA\n");
            if (header->loop_offset == 0 || do_loop >= 1) {
                break;
            }
            i = header->loop_offset + 0x1c;
            do_loop = 1;
        } else if (command >= 0x70 || command <= 0x7f) {
#ifdef VGM_DEBUG
            printf("[S] WAIT for %u samples\n", command & 0x0f);
#endif
            delay((unsigned long)((((double)(command & 0x0f)) / 44100.0) * 1000));
        } else {
            printf("UNKNOWN COMMAND\n");
        }
    }

    return 0;
}
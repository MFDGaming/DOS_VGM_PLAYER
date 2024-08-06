#include "vgm.h"
#include "vgm_opl3.h"
#include <stdio.h>
#include <stdlib.h>

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

void decode_opl3_register(U8 reg, U8 data, U8 second) {
    printf("| reg [%X] [PORT %u] |\n", reg, second & 1);
    if (reg == 0x01 && second == 0) {
        printf("|_> LSI TEST: %u\n", VGM_OPL3_GET_LSITEST1(data));
    } else if (reg == 0x01 && second == 0) {
        printf("|_> LSI TEST: %u\n", VGM_OPL3_GET_LSITEST2(data));
    } else if (reg == 0x02 && second == 0) {
        printf("|_> TIMER1: %u\n", VGM_OPL3_GET_TIMER1(data));
    } else if (reg == 0x03 && second == 0) {
        printf("|_> TIMER2: %u\n", VGM_OPL3_GET_TIMER2(data));
    } else if (reg == 0x04 && second == 0) {
        printf("|_> RST: %u\n", VGM_OPL3_GET_RST(data));
        printf("|_> MT1: %u\n", VGM_OPL3_GET_MT1(data));
        printf("|_> MT2: %u\n", VGM_OPL3_GET_MT2(data));
        printf("|_> ST2: %u\n", VGM_OPL3_GET_ST2(data));
        printf("|_> ST1: %u\n", VGM_OPL3_GET_ST1(data));
    } else if (reg == 0x04 && second == 1) {
        printf("|_> CONNECTION SELECT: %u\n", VGM_OPL3_GET_CONNECTIONSEL(data));
    } else if (reg == 0x05 && second == 1) {
        printf("|_> NEW: %u\n", VGM_OPL3_GET_NEW(data));
    } else if (reg == 0x08 && second == 0) {
        printf("|_> NTS: %u\n", VGM_OPL3_GET_NTS(data));
    } else if (reg >= 0x20 && reg <= 0x35) {
        printf("|_> AM: %u\n", VGM_OPL3_GET_AM(data));
        printf("|_> VIB: %u\n", VGM_OPL3_GET_VIB(data));
        printf("|_> EGT: %u\n", VGM_OPL3_GET_EGT(data));
        printf("|_> KSR: %u\n", VGM_OPL3_GET_KSR(data));
        printf("|_> MULT: %u\n", VGM_OPL3_GET_MULT(data));
    } else if (reg >= 0x40 && reg <= 0x55) {
        printf("|_> KSL: %u\n", VGM_OPL3_GET_KSL(data));
        printf("|_> TL: %u\n", VGM_OPL3_GET_TL(data));
    } else if (reg >= 0x60 && reg <= 0x75) {
        printf("|_> AR: %u\n", VGM_OPL3_GET_AR(data));
        printf("|_> DR: %u\n", VGM_OPL3_GET_DR(data));
    } else if (reg >= 0x80 && reg <= 0x95) {
        printf("|_> SL: %u\n", VGM_OPL3_GET_SL(data));
        printf("|_> RR: %u\n", VGM_OPL3_GET_RR(data));
    } else if (reg >= 0xa0 && reg <= 0xa8) {
        printf("|_> F NUMBER L: %u\n", VGM_OPL3_GET_FNUML(data));
    } else if (reg >= 0xb0 && reg <= 0xb8) {
        printf("|_> KON: %u\n", VGM_OPL3_GET_KON(data));
        printf("|_> BLOCK: %u\n", VGM_OPL3_GET_BLOCK(data));
        printf("|_> F NUMBER H: %u\n", VGM_OPL3_GET_FNUMH(data));
    } else if (reg == 0xbd && second == 0) {
        printf("|_> DAM: %u\n", VGM_OPL3_GET_DAM(data));
        printf("|_> DVB: %u\n", VGM_OPL3_GET_DVB(data));
        printf("|_> RYT: %u\n", VGM_OPL3_GET_RYT(data));
        printf("|_> BD: %u\n", VGM_OPL3_GET_BD(data));
        printf("|_> SD: %u\n", VGM_OPL3_GET_SD(data));
        printf("|_> TOM: %u\n", VGM_OPL3_GET_TOM(data));
        printf("|_> TC: %u\n", VGM_OPL3_GET_TC(data));
        printf("|_> HH: %u\n", VGM_OPL3_GET_HH(data));
    } else if (reg >= 0xc0 && reg <= 0xc8) {
        printf("|_> EX1: %u\n", VGM_OPL3_GET_EX1(data));
        printf("|_> EX0: %u\n", VGM_OPL3_GET_EX0(data));
        printf("|_> STL: %u\n", VGM_OPL3_GET_STL(data));
        printf("|_> STR: %u\n", VGM_OPL3_GET_STR(data));
        printf("|_> FB: %u\n", VGM_OPL3_GET_FB(data));
        printf("|_> CNT: %u\n", VGM_OPL3_GET_CNT(data));
    } else if (reg >= 0xe0 && reg <= 0xf5) {
        printf("|_> WS: %u\n", VGM_OPL3_GET_WS(data));
    } else {
        printf("|_> UNKNOWN\n");
    }
}

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

    data = (unsigned char *)malloc(len);
    fread(data, 1, len, fp);

    fclose(fp);
    
    header = (vgm_header_t *)data;

    printf("VERSION: %X\n", header->version);
    printf("CLOCK_RATE: %u\n", header->ymf262_clock);
    printf("DATA_OFFSET: %u\n", header->vgm_data_offset);
    
    for (i = header->vgm_data_offset + 52; i < header->eof_offset;) {
        command = data[i++];
        if (command == 0x5E || command == 0x56) {
            reg = data[i++];
            reg_data = data[i++];

#ifdef VGM_DEBUG
            decode_opl3_register(reg, reg_data, 1);
#endif
            write_reg(0x388, reg, reg_data, 0);
        } else if (command == 0x5F || command == 0x57) {
            reg = data[i++];
            reg_data = data[i++];
#ifdef VGM_DEBUG
            decode_opl3_register(reg, reg_data, 1);
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
        } else if (command >= 0x70 && command <= 0x7f) {
#ifdef VGM_DEBUG
            printf("[S] WAIT for %u samples\n", command & 0x0f);
#endif
            delay((unsigned long)((((double)(command & 0x0f)) / 44100.0) * 1000));
        } else {
            printf("UNKNOWN COMMAND %x\n", command);
            return 0;
        }
    }
    if (data) {
        free(data);
    }

    return 0;
}
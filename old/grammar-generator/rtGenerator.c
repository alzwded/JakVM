#include <rtGenerator.h>

/*
    input:  byte stream from parser
    output: c-source for an executable that takes one parameter
            (program.b binary file) and executes it
            this output program relies on processor.h and everything else
            to build an executable

    structure of output H file:
    #include <processor.h>
    int MNEMONICsIZE_PARAM1_PARAM2(typed_t* i);
    e.g.:
    void MOV1_AD_imed(long imed);
    void MVR1_AD_ED();
    void LOD4_AX(long b1, long b2, long b3);
    void CLZ();

    structure of output C file:
    #include <OUTPUT.h>
    #include <processor.h>
    void inc_ip() {
        // increment IP register
    }
    void execute(unisnged char byteStream[]) {
        long lopcode1, lopcode2, lopcode3, lopcode4;
        int pos = 0;

        try_read(byteStream, &pos, lopcode2); // calls inc_ip();
        try_read(byteStream, &pos, lopcode2);
        switch(lopcode1) {
        case * generated *:
            * either require reading 3 additional immediate values *
            * or this is enough to execute *
            * or switch remaining opcode2 *
        case 0x01:
            MOV_1_AD_imed(lopcode2);
            break;
        case 0x1D:
            switch(lopcode2) {
            case 0x09:
                MVR_1_AB_EC();
                break;
            }
            break;
        }
        case 0xE0:
            try_read(byteStream, &pos, lopcode3);
            try_read(byteStream, &pos, lopcode4);
            LOD1_AD_add(lopcode2, lopcode3, lopcode4);
            break;
    }
*/

void generateRTFile()
{
}

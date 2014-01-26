#include <asmGenerator.h>

/*
    input:  byte stream from parser
    output: c-source for an executable that takes one parameter
            (program.a file) and generates binary for VM (program.b)

    structure of output C source file:
    #include <OUTPUT.h>
    void check_parameters(argc, argv) {
        // if(incorrect) abort();
    }
    void assemble(argv[0]) {
        // compute output filename
        char mnemonic[5], param1[3], param2[20], opcode1[3], opcode2[3], opcode3[3], opcode4[3];
        short opcodeSize;
        do {
            try_read_mnemonic(mnemonic);

            * for_each(typed_t* i in parsedList) generate: *
            if(strncmp(mnemonic, * generated *) == 0) {
                * consider reading parameter 1 based on type *
                * consider reading parameter 2 based on type *
            } else if( * idem *) {
            } else {
                * handle error *
            }

            if(mnemonic != NULL) {
                write_instruction(opcodeSize, * opcode1, opcode2, opcode3, opcode4 *);
            } else {
                assert(0);
            }
        } while(!feof(inFile));
    }
    void write_instruction(int, char*, char*, char*, char*) {
        // write binary
    }
    int main(int argc, char* argv[]) {
        check_parameters(argc, argv);
        assemble(argv[0]);
    }

    
*/

void generateAsmFile()
{
}

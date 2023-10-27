#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>

std::vector<uint8_t> readBinFile(std::string name) {
    FILE* file;
    auto err = fopen_s(&file, name.data(), "rb");
    if (err != 0) {
        throw std::runtime_error("Could not open file");
    }

    fseek(file, 0, SEEK_END);
    long sz = ftell(file);
    fseek(file, 0, SEEK_SET);

    std::vector<uint8_t> buffer(sz);
    fread(buffer.data(), 1, sz, file);

    fclose(file);
    return buffer;
}

void parseOut(FILE* out, const std::vector<uint8_t>& bin) {
    auto sz = bin.size();
    for (size_t i = 0; i + 1 < sz; i += 2) {
        uint8_t ab = bin[i];
        uint8_t cd = bin[i + 1]; 

        uint8_t a = (ab & 0xF0) / 0x10;
        uint8_t b = ab & 0x0F;
        uint8_t c = (cd & 0xF0) / 0x10;
        uint8_t d = cd & 0x0F;

        // fprintf(out, "0x%X%X%X%X\n", a, b, c, d);

        switch (a) {
        case 0x0: {
            if (d == 0x0 && b == 0x0 && c == 0xE) {
                fprintf(out, "CLS\n");
                break;
            }
            if (d == 0xE && b == 0x0 && c == 0xE) {
                fprintf(out, "RET\n");
                break;
            }

            fprintf(out, "SYS %x\n", b * 0x100 + cd);
        } break;
        case 0x1: {
            fprintf(out, "JP %x\n", b * 0x100 + cd);
        } break;
        case 0x2: {
            fprintf(out, "CALL %x\n", b * 0x100 + cd);
        } break;
        case 0x3: {
            fprintf(out, "SE V%x, %x\n", b, cd);
        } break;
        case 0x4: {
            fprintf(out, "SNE V%x, %x\n", b, cd);
        } break;
        case 0x5: {
            if (d != 0x0) {
                fprintf(out, "Not real instruction 0x%X%X\n", ab, cd);
                break;
            }
            fprintf(out, "SE V%x, V%x\n", b, c);
        } break;
        case 0x6: {
            fprintf(out, "LD V%x, %x\n", b, cd);
        } break;
        case 0x7: {
            fprintf(out, "ADD V%x, %x\n", b, cd);
        } break;
        case 0x8: {
            switch (d) {
            case 0x0: {
                fprintf(out, "LD V%x, V%x\n", b, c);
            } break;
            case 0x1: {
                fprintf(out, "OR V%x, V%x\n", b, c);
            } break;
            case 0x2: {
                fprintf(out, "AND V%x, V%x\n", b, c);
            } break;
            case 0x3: {
                fprintf(out, "XOR V%x, V%x\n", b, c);
            } break;
            case 0x4: {
                fprintf(out, "ADD V%x, V%x\n", b, c);
            } break;
            case 0x5: {
                fprintf(out, "SUB V%x, V%x\n", b, c);
            } break;
            case 0x6: {
                fprintf(out, "SHR V%x, V%x\n", b, c);
            } break;
            case 0x7: {
                fprintf(out, "SUBN V%x, V%x\n", b, c);
            } break;
            case 0xE: {
                fprintf(out, "SHL V%x, V%x\n", b, c);
            } break;
            default:
                fprintf(out, "Not real instruction 0x%X%X\n", ab, cd);
            }
        } break;
        case 0x9: {
            if (d != 0x0) {
                fprintf(out, "Not real instruction 0x%X%X\n", ab, cd);
                break;
            }
            fprintf(out, "SNE V%x, V%x\n", b, c);
        } break;
        case 0xA: {
            fprintf(out, "LD I, %x\n", b * 0x100 + cd);
        } break;
        case 0xB: {
            fprintf(out, "JP V0, %x\n", b * 0x100 + cd);
        } break;
        case 0xC: {
            fprintf(out, "RND V%x, %x\n", b, cd);
        } break;
        case 0xD: {
            fprintf(out, "DRW V%x, V%x, nibble (%x)\n", b, c, d);
        } break;
        case 0xE: {
            if (c == 0x9 && d == 0xE) {
                fprintf(out, "SKP V%x\n", b);
                break;
            }
            if (c == 0xA && d == 0x1) {
                fprintf(out, "SKNP V%x\n", b);
                break;
            }

            fprintf(out, "Not real instruction 0x%X%X\n", ab, cd);
        } break;
        case 0xF: {
            switch (cd) {
            case 0x07: {
                fprintf(out, "LD V%x, DT\n", b);
            } break;
            case 0x0A: {
                fprintf(out, "LD V%x, K\n", b);
            } break;
            case 0x15: {
                fprintf(out, "LD DT, V%x\n", b);
            } break;
            case 0x18: {
                fprintf(out, "LD ST, V%x\n", b);
            } break;
            case 0x1E: {
                fprintf(out, "ADD I, V%x\n", b);
            } break;
            case 0x29: {
                fprintf(out, "LD F, V%x\n", b);
            } break;
            case 0x33: {
                fprintf(out, "LD B, V%x\n", b);
            } break;
            case 0x55: {
                fprintf(out, "LD [I], V%x\n", b);
            } break;
            case 0x65: {
                fprintf(out, "LD V%x, [I]\n", b);
            } break;
            default:
                fprintf(out, "Not real instruction 0x%X%X\n", ab, cd);
            }
        } break;
        }
    }
}

int main() {
    auto binFile = readBinFile("bin/keyboard.ch8");
    // if (binFile.size() % 2 != 0) {
    //     throw std::runtime_error("CHIP8 instructions are 2 byte, but file size is not divisible by 2");
    // }

    parseOut(stdout, binFile);
    
    return 0;
}
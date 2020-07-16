
#include <fstream>
#include <iostream>
#include <cmath>
#include <time.h>
using namespace std;
int rom[1 << 18];
unsigned int pc = 0;
unsigned int regs[32];
bool flag = true;
//ofstream out;


enum INSTRUCTIONS {
    LUI,
    AUIPC,
    JAL,
    JALR,
    BEQ,
    BNE,
    BLT,
    BGE,
    BLTU,
    BGEU,
    LB,
    LH,
    LW,
    LBU,
    LHU,
    SB,
    SH,
    SW,
    ADDI,
    SLTI,
    SLTIU,
    XORI,
    ORI,
    ANDI,
    SLLI,
    SRLI,
    SRAI,
    ADD,      //R
    SUB,
    SLL,
    SLT,
    SLTU,
    XOR,
    SRL,
    SRA,
    OR,
    AND
};
/*
void print_thing(int i)
*/

int X_to_D(char* s, int X_len)
{
    char* tmp;
    tmp = new char[X_len];
    for (int i = 0;i < X_len;++i)
        tmp[i] = '0';
    int result = 0, k = 1;
    for (int i = 0;i < X_len;++i)
    {
        if (s[i] < 'A') tmp[i] = s[i] - '0';
        else tmp[i] = s[i] - 'A' + 10;
        result += (long long)(tmp[i]) * k;
        k *= 16;
    }

    return result;
}
bool read_in()
{

    static char ch = cin.get();
    if (ch == EOF)
        return false;
    while (ch != EOF)
    {
        char add[8];
        if (ch == '@')
        {
            for (int i = 0;i < 8;++i)
            {
                ch = cin.get();
                add[7 - i] = ch;
            }
            pc = X_to_D(add, 8);
        }

        else if (ch == '\n')
        {
            ch = cin.get();
            continue;
        }
        else if (ch == ' ')
        {
            ch = cin.get();
            continue;
        }
        else
        {
            char ch0 = cin.get();
            char chs[2];
            chs[0] = ch0;
            chs[1] = ch;
            rom[pc] = X_to_D(chs, 2);
            pc += 1;
        }
        ch = cin.get();
    }
    pc = 0;
    if (ch == EOF)
        return false;
    return true;
}
unsigned int get_inst()
{
    
    unsigned int ans = 0;
    ans += rom[pc + 3];
    ans = ans << 8;
    ans += rom[pc + 2];
    ans = ans << 8;
    ans += rom[pc + 1];
    ans = ans << 8;
    ans += rom[pc];


    return ans;

}

struct instruction
{
    unsigned int bin;
    INSTRUCTIONS op;
    char type;
    unsigned int rs1, rs2, rd, imm = 0, adress = 0, rom_content = 0, calc_res;
    instruction(unsigned int num)
    {
        bin = num;
        unsigned int func3 = (num >> 12) % 8;
        unsigned int opcode = num % (1 << 7);
        unsigned int func7 = (num >> 25) % (1 << 7);
        switch (opcode)
        {

        case 0b0110111:
            op = LUI;
            break;
        case 0b1101111:
            op = JAL;
            break;
        case 0b1100111:
            op = JALR;
            break;
        case 0b1100011:
        {
            switch (func3)
            {
            case 0b000:
                op = BEQ;
                break;
            case 0b001:
                op = BNE;
                break;
            case 0b100:
                op = BLT;
                break;
            case 0b101:
                op = BGE;
                break;
            case 0b110:
                op = BLTU;
                break;
            case 0b111:
                op = BGEU;
                break;
            }
            break;
        }
        case 0b0000011:
        {
            switch (func3)
            {
            case 0b000:
                op = LB;
                break;
            case 0b001:
                op = LH;
                break;
            case 0b010:
                op = LW;
                break;
            case 0b100:
                op = LBU;
                break;
            case 0b101:
                op = LHU;
                break;
            }
            break;

        }
        case 0b0100011:
        {
            switch (func3)
            {
            case 0b000:
                op = SB;
                break;
            case 0b001:
                op = SH;
                break;
            case 0b010:
                op = SW;
                break;
            }
            break;

        }
        case 0b0010011:
        {
            switch (func3)
            {
            case 0b000:
                op = ADDI;
                break;
            case 0b010:
                op = SLTI;
                break;
            case 0b011:
                op = SLTIU;
                break;
            case 0b100:
                op = XORI;
                break;
            case 0b110:
                op = ORI;
                break;
            case 0b111:
                op = ANDI;
                break;
            case 0b001:
                op = SLLI;
                break;
            case 0b101:
            {
                if (func7 == 0b0000000)
                {
                    op = SRLI;
                }

                else if (func7 == 0b0100000)
                {
                    op = SRAI;
                }

                break;
            }

            }
            break;

        }
        case 0b0110011:
        {
            switch (func3)
            {
            case 0b000:
            {
                if (func7 == 0b0000000)
                {
                    op = ADD;
                }

                else if (func7 == 0b0100000)
                {
                    op = SUB;
                }

                break;
            }
            case 0b001:
                op = SLL;
                break;
            case 0b010:
                op = SLT;
                break;
            case 0b011:
                op = SLTU;
                break;
            case 0b100:
                op = XOR;
                break;
            case 0b101:
            {
                if (func7 == 0b0000000)
                {
                    op = SRL;
                }

                else if (func7 == 0b0100000)
                {
                    op = SRA;
                }

                break;
            }
            case 0b110:
                op = OR;
                break;
            case 0b111:
                op = AND;
                break;
            }
            break;

        }
        }
        if (op == LUI)type = 'U';
        else if (op == ADDI || op == SLTI || op == SLTIU || op == ANDI || op == ORI || op == XORI || op == SLLI || op == SRLI || op == SRAI || op == JALR)type = 'I';
        else if (op == ADD || op == SLT || op == SLTU || op == AND || op == OR || op == XOR || op == SLL || op == SRL || op == SUB || op == SRA)type = 'R';
        else if (op == JAL)type = 'J';
        else if (op == BEQ || op == BNE || op == BLT || op == BLTU || op == BGE || op == BGEU)type = 'B';
        else if (op == LW || op == LH || op == LHU || op == LB || op == LBU)type = 'I';
        else if (op == SW || op == SH || op == SB)type = 'S';
        //        cout <<' ' << op << '\t';
    }

    void decode()
    {
        if (type == 'U')
        {
            rd = (bin >> 7) % 32;
            imm = (bin & 0xFFFFF000UL);
        }
        else if (type == 'I')
        {
            rs1 = (bin >> 15) % 32;
            rd = (bin >> 7) % 32;
            if ((bin >> 31u) == 1u)
                imm |= 0xFFFFF800;
            imm |= (bin >> 20u) & 2047u;
        }
        else if (type == 'R')
        {
            rs1 = (bin >> 15) % 32;
            rs2 = (bin >> 20) % 32;
            rd = (bin >> 7) % 32;
        }
        else if (type == 'J')
        {
            rd = (bin >> 7) % 32;

            if ((bin >> 31u) == 1u)
                imm |= 0xFFF00000UL;
            imm |= bin & 0x000ff000UL;
            imm |= ((bin >> 20u) & 1u) << 11u;
            imm |= ((bin >> 21u) & 1023u) << 1u;
        }
        else if (type == 'B')
        {
            rs1 = (bin >> 15) % 32;
            rs2 = (bin >> 20) % 32;
            if ((bin >> 31u) == 1u)
                imm |= 0xfffff000;
            imm |= ((bin >> 7u) & 1u) << 11u;
            imm |= ((bin >> 25u) & 63u) << 5u;
            imm |= ((bin >> 8u) & 15u) << 1u;
        }
        else if (type == 'S')
        {
            rs1 = (bin >> 15) % 32;
            rs2 = (bin >> 20) % 32;
            if ((bin >> 31u) == 0x1u)
                imm |= 0xfffff800;
            imm |= ((bin >> 25u) & 63u) << 5u;
            imm |= ((bin >> 8u) & 15u) << 1u;
            imm |= (bin >> 7u) & 1u;
        }
    }
    void execute()
    {
        if (type == 'I')
        {
            if (op == JALR)
            {
                if (rd != 0)
                    calc_res = pc + 4;
                pc = (int)((imm + regs[rs1]) >> 1) << 1;
                pc -= 4;
            }
            else if (op == ADDI)
            {
                calc_res = regs[rs1] + imm;
            }
            else if (op == SLTI)
            {
                calc_res = ((int)regs[rs1] < (int)imm);
            }
            else if (op == SLTIU)
            {
                calc_res = ((int)regs[rs1] < (int)imm);
            }
            else if (op == ANDI)
            {
                calc_res = regs[rs1] & imm;
            }
            else if (op == ORI)
            {
                calc_res = regs[rs1] | imm;
            }
            else if (op == XORI)
            {
                calc_res = regs[rs1] ^ imm;
            }
            else if (op == SLLI)
            {
                calc_res = regs[rs1] << (imm & 31UL);
            }
            else if (op == SRLI)
            {
                calc_res = regs[rs1] >> (imm & 31UL);
            }
            else if (op == SRAI)
            {
                calc_res = (int)regs[rs1] >> (imm & 31UL);
            }
            else
            {
                adress = regs[rs1] + (int)imm;
            }
        }
        else if (type == 'B')
        {
            if (op == BEQ)
            {

                if (regs[rs1] == regs[rs2])
                {
                    pc += imm;
                    pc -= 4;
                }
            }
            else if (op == BNE)
            {
                if (regs[rs1] != regs[rs2])
                {
                    pc += imm;
                    pc -= 4;
                }
            }
            else if (op == BLT)
            {
                if ((int)regs[rs1] < (int)regs[rs2])
                {
                    pc += imm;
                    pc -= 4;
                }
            }
            else if (op == BLTU)
            {
                if (regs[rs1] < regs[rs2])
                {
                    pc += imm;
                    pc -= 4;
                }
            }
            else if (op == BGE)
            {
                if ((int)regs[rs1] >= (int)regs[rs2])
                {
                    pc += imm;
                    pc -= 4;
                }
            }
            else if (op == BGEU)
            {
                if (regs[rs1] >= regs[rs2])
                {
                    pc += imm;
                    pc -= 4;
                }
            }
        }
        else if (type == 'R')
        {
            if (op == ADD)
            {
                calc_res = regs[rs1] + regs[rs2];
            }
            else if (op == SUB)
            {
                calc_res = regs[rs1] - regs[rs2];
            }
            else if (op == SLL)
            {
                calc_res = regs[rs1] << (regs[rs2] & 31UL);
            }
            else if (op == SRL)
            {
                calc_res = regs[rs1] >> (regs[rs2] & 31UL);
            }
            else if (op == SRA)
            {
                calc_res = ((int)regs[rs1]) >> (regs[rs2] & 31UL);
            }
            else if (op == SLT)
            {
                calc_res = ((int)regs[rs1] < (int)regs[rs2]);
            }
            else if (op == SLTU)
            {
                calc_res = (regs[rs1] < regs[rs2]);
            }
            else if (op == AND)
            {
                calc_res = regs[rs1] & regs[rs2];
            }
            else if (op == OR)
            {
                calc_res = regs[rs1] | regs[rs2];
            }
            else if (op == XOR)
            {
                calc_res = regs[rs1] ^ regs[rs2];
            }
        }
        else if (type == 'S')
        {
            adress = regs[rs1] + (int)imm;
        }
        else if (type == 'J')
        {
            if (op == JAL)
            {
                if (rd != 0)
                    calc_res = pc + 4;
                pc += (int)imm;
                pc -= 4;
            }
        }
        pc += 4;
    }
    void memory()
    {
        if (type == 'I' && rd == 0)
            return;
        if (type == 'I')
        {
            if (op == LB)
            {
                rom_content += (char)rom[adress];
            }
            else if (op == LH)
            {
                rom_content += (char)rom[adress + 1];
                rom_content=rom_content << 8;
                rom_content += (char)rom[adress];
            }
            else if (op == LW)
            {
                rom_content += rom[adress + 3];
                rom_content=rom_content << 8;
                rom_content += rom[adress + 2];
                rom_content=rom_content << 8;
                rom_content += rom[adress + 1];
                rom_content=rom_content << 8;
                rom_content += rom[adress];
            }
            else if (op == LBU)
            {
                rom_content += rom[adress];
            }
            else if (op == LHU)
            {
                rom_content += rom[adress + 1];
                rom_content=rom_content << 8;
                rom_content += rom[adress];
            }
        }
        else if (type == 'S')
        {
            if (op == SB)
            {
                rom[adress] = (regs[rs2] % (1 << 8));
            }
            else if (op == SH)
            {
                rom[adress] = (regs[rs2] % (1 << 8));
                rom[adress + 1] = ((regs[rs2] >> 8) % (1 << 8));
            }
            else if (op == SW)
            {
                rom[adress] = (regs[rs2] % (1 << 8));
                rom[adress + 1] = ((regs[rs2] >> 8) % (1 << 8));
                rom[adress + 2] = ((regs[rs2] >> 16) % (1 << 8));
                rom[adress + 3] = ((regs[rs2] >> 24) % (1 << 8));
            }
        }
    }
    void write_back()
    {
        if (rd == 0)
            return;
        if (type == 'I')
        {
            if (op == LB || op == LH || op == LW)
            {
                regs[rd] = (int)rom_content;
            }
            else if (op == LBU || op == LHU)
            {
                regs[rd] = rom_content;
            }
            else
            {
                regs[rd] = calc_res;
            }
        }
        else if (type == 'U')
        {
            if (op == LUI)
            {
                regs[rd] = imm;
            }
        }
        else if (type == 'R' || type == 'J')
        {
            regs[rd] = calc_res;
        }

    }
};
int main()
{
    char operation[32];
    for (int i = 0;i < (1 << 18);++i)
        rom[i] = 0;

    //    out.open("res.txt");
    read_in();
    
    while (1)
    {
        
        unsigned int operation = get_inst();
        
        if (operation == 0x0ff00513)
        {
            cout << (((unsigned int)regs[10]) & 255u);
            break;
        }
        else
        {
            instruction inst(operation);
            inst.decode();
            inst.execute();
            if(inst.type=='I'||inst.type=='S')
                    inst.memory();
            inst.write_back();
        }
    }

}
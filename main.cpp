
#include <fstream>
#include <iostream>
#include <cmath>
using namespace std;
int *rom;
unsigned int pc = 0;
unsigned int regs[32];
bool flag = true;
//ofstream out;
int tick = 1;

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
    ADD,      
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

void print_thing(int i)
{
    switch (i)
    {
    case 0:
        cout << "LUI";
        break;
    case 1:
        cout << "AUIPC";
        break;
    case 2:
        cout << "JAL";
        break;
    case 3:
        cout << "JALR";
        break;
    case 4:
        cout << "BEQ";
        break;
    case 5:
        cout << "BNE";
        break;
    case 6:
        cout << "BLT";
        break;
    case 7:
        cout << "BGE";
        break;
    case 8:
        cout << "BLTU";
        break;
    case 9:
        cout << "BGEU";
        break;
    case 10:
        cout << "LB";
        break;
    case 11:
        cout << "LH";
        break;
    case 12:
        cout << "LW";
        break;
    case 13:
        cout << "LBU";
        break;
    case 14:
        cout << "LHU";
        break;
    case 15:
        cout << "SB";
        break;
    case 16:
        cout << "SH";
        break;
    case 17:
        cout << "SW";
        break;
    case 18:
        cout << "ADDI";
        break;
    case 19:
        cout << "SLTI";
        break;
    case 20:
        cout << "SLTIU";
        break;
    case 21:
        cout << "XORI";
        break;
    case 22:
        cout << "ORI";
        break;
    case 23:
        cout << "ANDI";
        break;
    case 24:
        cout << "SLLI";
        break;
    case 25:
        cout << "SRLI";
        break;
    case 26:
        cout << "SRAI";
        break;
    case 27:
        cout << "ADD";
        break;
    case 28:
        cout << "SUB";
        break;
    case 29:
        cout << "SLL";
        break;
    case 30:
        cout << "SLT";
        break;
    case 31:
        cout << "SLTU";
        break;
    case 32:
        cout << "XOR";
        break;
    case 33:
        cout << "SRL";
        break;
    case 34:
        cout << "SRA";
        break;
    case 35:
        cout << "OR";
        break;
    case 36:
        cout << "AND";
        break;
    }
}


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
    delete[] tmp;
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
    char ch[8];
    ch[1] = rom[pc] / 16 + '0';
    ch[0] = rom[pc] % 16 + '0';
    ch[3] = rom[pc + 1] / 16 + '0';
    ch[2] = rom[pc + 1] % 16 + '0';
    ch[5] = rom[pc + 2] / 16 + '0';
    ch[4] = rom[pc + 2] % 16 + '0';
    ch[7] = rom[pc + 3] / 16 + '0';
    ch[6] = rom[pc + 3] % 16 + '0';
    unsigned int ans = 0;
    for (int i = 0;i < 8;i++) {
        if (ch[i] < 'A') ch[i] = ch[i] - '0';
        else ch[i] = ch[i] - 'A' + 10;
        ans |= ((unsigned int)(ch[i]) << (i << 2));
    }
    pc += 4;
    //    cout << hex << ans << '\t' << dec;
    return ans;

}

struct instruction
{
    unsigned int bin;
    INSTRUCTIONS op;
    char type;
    unsigned int rs1=255, rs2=255, rd=255, imm = 0, adress = 0, rom_content = 0, calc_res = 0;
    bool has_ex = 0,has_id=0;
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
        
    }
    ~instruction(){}
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
            if (op == ADDI)
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

                if (regs[rs1] == regs[rs2])calc_res = 1;
                else calc_res=0;
            }
            else if (op == BNE)
            {
                if (regs[rs1] != regs[rs2])calc_res = 1;
                else calc_res = 0;
            }
            else if (op == BLT)
            {
                if ((int)regs[rs1] < (int)regs[rs2])calc_res = 1;
                else calc_res = 0;
            }
            else if (op == BLTU)
            {
                if (regs[rs1] < regs[rs2])calc_res = 1;
                else calc_res = 0;
            }
            else if (op == BGE)
            {
                if ((int)regs[rs1] >= (int)regs[rs2])calc_res = 1;
                else calc_res = 0;
            }
            else if (op == BGEU)
            {
                if (regs[rs1] >= regs[rs2])calc_res = 1;
                else calc_res = 0;
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
            calc_res = regs[rs2];
        }
        
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
                rom_content = rom_content << 8;
                rom_content += (char)rom[adress];
               
            }
            else if (op == LW)
            {
                rom_content += rom[adress + 3];
                rom_content = rom_content << 8;
                rom_content += rom[adress + 2];
                rom_content = rom_content << 8;
                rom_content += rom[adress + 1];
                rom_content = rom_content << 8;
                rom_content += rom[adress];
            }
            else if (op == LBU)
            {
                rom_content += rom[adress];
            }
            else if (op == LHU)
            {
                rom_content += rom[adress + 1];
                rom_content = rom_content << 8;
                rom_content += rom[adress];
            }
        }
        else if (type == 'S')
        {
            if (op == SB)
            {
                rom[adress] = (calc_res % (1 << 8));
            }
            else if (op == SH)
            {
                rom[adress] = (calc_res % (1 << 8));
                rom[adress + 1] = ((calc_res >> 8) % (1 << 8));
            }
            else if (op == SW)
            {
                rom[adress] = (calc_res % (1 << 8));
                rom[adress + 1] = ((calc_res >> 8) % (1 << 8));
                rom[adress + 2] = ((calc_res >> 16) % (1 << 8));
                rom[adress + 3] = ((calc_res >> 24) % (1 << 8));
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

struct streamline
{
    instruction* IF, * ID, * EX, * MEM, * WB, * sleep;
    bool final_order = 0, has_sleep = 0;
    unsigned int mem_tick = 0;
    int branch_buffer = 0;
    int correct_prediction=0, total_prediction=0;
    struct saver
    {
        int predict_res;
        unsigned int pc_alternative;
    };
    saver branch;
    bool branch_prediction()
    {
        if (branch_buffer == 0)return 0;
        else if (branch_buffer == 1)return 0;
        else if (branch_buffer == 2)return 1;
        else if (branch_buffer == 3)return 1;
    }
    bool need_mem(instruction* EX)
    {
        if (EX && (EX->type == 'S' || EX->op == LW || EX->op == LB || EX->op == LH || EX->op == LBU || EX->op == LHU))
            return 1;
        else
            return 0;
    }
    void get_branch()
    {
        branch.predict_res = branch_prediction();
        if (branch.predict_res)
            branch.pc_alternative = pc;
        else
            branch.pc_alternative = pc + ID->imm - 4;
        if (branch.predict_res)
            pc += ID->imm - 4;
        
    }
    void test_branch()
    {
        if (EX && EX->type == 'B' && EX->calc_res != branch.predict_res)
        {
            total_prediction += 1;
            if (branch_buffer == 0)branch_buffer = 1;
            else if (branch_buffer == 1)branch_buffer = 2;
            else if (branch_buffer == 2)branch_buffer = 1;
            else if (branch_buffer == 3)branch_buffer = 2;
            delete IF;
            delete ID;
            IF = NULL;
            ID = NULL;
            pc = branch.pc_alternative;
        }
        else if (EX && EX->type == 'B' && EX->calc_res == branch.predict_res)
        {
            total_prediction += 1;
            correct_prediction += 1;
            if (branch_buffer == 0)branch_buffer = 0;
            else if (branch_buffer == 1)branch_buffer = 0;
            else if (branch_buffer == 2)branch_buffer = 3;
            else if (branch_buffer == 3)branch_buffer = 3;
        }

    }
    void instruction_fetch()
    {
        if (!final_order&&!IF)
        {
            int num = get_inst();
            if (num == 0x0ff00513)
            {
                final_order = 1;
                IF = NULL;
            }
            else
                IF = new instruction(num);
        }
    }
    void instruction_decode()
    {
        if (ID&&ID->has_id == 1)
            return;
        if (ID)
            ID->has_id = 1;
        if (ID)ID->decode();
        if (ID && ID->op == JALR)
        {
            ID->calc_res = pc;
            pc = (int)((ID->imm + regs[ID->rs1]) >> 1) << 1;
        }
        else if (ID && ID->op == JAL)
        {
            ID->calc_res = pc;
            pc += (int)(ID->imm);
            pc -= 4;
        }
        else if (ID && ID->type == 'B')
        {
            get_branch();
        }
    }
    void execute()
    {
        if (EX&&EX->has_ex == 1)
            return;
        if (EX)
            EX->has_ex = 1;
        if (EX)EX->execute();
        test_branch();
    }
    void hazard_deal()
    {
        has_sleep = 1;
        sleep = EX;
        EX = NULL;
    }
    void end_suspension()
    {
        EX = sleep;
        sleep = NULL;
    }
    void carry_out()//WB->hazard_deal->MEM->suspend/end_suspend->EX->ID->IF
    {
        //WB&MEM
        if (WB)WB->write_back();
        if ((EX&&MEM)&&((EX->rs1 == MEM->rd && MEM->rd !=255) || (EX->rs2 == MEM->rd && MEM->rd != 255) || (EX->rd == MEM->rd && MEM->rd != 255)))//hazard
        {
            hazard_deal();
        }
        if (MEM)
        {
            if(mem_tick == 0)
                MEM->memory();
            mem_tick += 1;
            if (mem_tick <= 0)
                return;
            else
                mem_tick = 0;
        }
        if (has_sleep)
            return;


        //EX
        execute();

        //ID
        instruction_decode();

        //IF
        instruction_fetch();
        
    }
    void next_round()
    {
        //have_sleep=1表示应该等待mem操作完成再继续流水

        
        if (WB)
        {
            delete WB;
            WB = NULL;
        }
        if(has_sleep)
        {
            int i = 0;
        }
        /*
        if (mem_tick == 0 && !has_sleep && EX&& !need_mem(EX)&&!MEM)
        {
            WB = EX;
            EX = ID;
            ID = IF;
            IF = NULL;
        }
        else if (mem_tick == 0 && !has_sleep && EX && !need_mem(EX) && MEM)
        {
            WB = MEM;
        }
        else if (mem_tick == 0 && !has_sleep && EX && need_mem(EX))
        {
            WB = MEM;
            MEM = EX;
            EX = ID;
            ID = IF;
            IF = NULL;
        }
        else if (has_sleep && mem_tick == 0)
        {
            WB = MEM;
            MEM = NULL;
            has_sleep = 0;
        }
        else if (mem_tick == 0 && MEM)
        {
            WB = MEM;
            MEM = NULL;
        }
        else if(mem_tick!=0&&!has_sleep&&(EX&&!need_mem(EX)))
        {
            WB = EX;
            EX = ID;
            ID = IF;
            IF = NULL;
        }
        else if(!(mem_tick!=0&&!has_sleep&&EX&&need_mem(EX)))
        {
            EX = ID;
            ID = IF;
            IF = NULL;
        }
        */                                             
        /*
        if (mem_tick == 0)
        {
            WB = MEM;
            MEM = NULL;
            if (!has_sleep)
            {
                MEM = EX;
                EX = ID;
                ID = IF;
                IF = NULL;
            }
            has_sleep = 0;
        }
        */

        
        if (MEM)//有MEM
        {
            
            if (EX && need_mem(EX))//EX要用mem
            {
                if (mem_tick == 0)//MEM做完了
                {
                    WB = MEM;
                    MEM = EX;
                    EX = ID;
                    ID = IF;
                    IF = NULL;
                
                    has_sleep = 0;
                }
                //MEM没做完继续做
            }
            else if (has_sleep)//EX在睡眠
            {
                if (mem_tick == 0)//MEM做完了
                {
                    WB = MEM;
                    MEM = NULL;
                    has_sleep = 0;//起床
                }
                //MEM没做完继续做
            }
            else if (!need_mem(EX))//EX不要用mem
            {
                if (mem_tick == 0)//MEM做完了
                {
                    WB = MEM;
                    MEM = NULL;
                }
                else//MEM没做完
                {
                    WB = EX;
                    EX = ID;
                    ID = IF;
                    IF = NULL;
                }
            }  
        }
        else//没有MEM操作，显然没人睡觉
        {
            if (EX && need_mem(EX))//EX要用mem
            {
                MEM = EX;
                EX = ID;
                ID = IF;
                IF = NULL;
            }
            else if (!need_mem(EX))//EX不要用mem或者没用EX
            {
                WB = EX;
                EX = ID;
                ID = IF;
                IF = NULL;
            }
        }


        if (!has_sleep && sleep)
        {
            end_suspension();
        }

    }
    bool has_end()
    {
        return ( WB == NULL && MEM == NULL && EX == NULL && WB == NULL && final_order);
    }
    streamline()
    {
        IF= NULL;
        ID = NULL;
        EX = NULL;
        MEM = NULL;
        WB = NULL;
        sleep = NULL;
    }
};

int main()
{
    char operation[32];
    rom = new int[1 << 20];
    for (int i = 0;i < (1 << 20);++i)
        rom[i] = 0;

    //    out.open("res.txt");
    read_in();

    
    streamline st;

    
    while (!st.has_end())
    {
        st.carry_out();
        st.next_round();
        tick += 1;
    }
    cout << (((unsigned int)regs[10]) & 255u) << endl;
    //cout << st.correct_prediction << '/' << st.total_prediction;
    delete[] rom;
   


}
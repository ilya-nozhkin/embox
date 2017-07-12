//
//  import.c
//
//  Created by Andrey Terekhov on 2/25/14.
//  Copyright (c) 2014 Andrey Terekhov. All rights reserved.
//

//#define ROBOT 1
//#include <unistd.h>
#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sensor_map.h>
//#include <ruthreads>

#include "Defs.h"

#define I2CBUFFERSIZE 50

#define index_out_of_range  1
#define wrong_kop           2
#define wrong_arr_init      3
#define wrong_motor_num     4
#define wrong_motor_pow     5
#define wrong_digsensor_num 6
#define wrong_ansensor_num  7
#define wrong_robot_com     8
#define wrong_number_of_elems 9
#define zero_devide         10
#define float_zero_devide   11
#define mem_overflow        12
#define sqrt_from_negat     13
#define log_from_negat      14
#define log10_from_negat    15
#define wrong_asin          16
#define wrong_string_init   17

int g, iniproc, maxdisplg, wasmain;
int reprtab[MAXREPRTAB], rp, identab[MAXIDENTAB], id, modetab[MAXMODETAB], md;
int mem[MAXMEMSIZE], functions[FUNCSIZE], funcnum;
int threads[NUMOFTHREADS], curthread, upcurthread;
int procd, iniprocs[INIPROSIZE], base = 0;
FILE *input;

#ifdef ROBOT
FILE *f1, *f2;   // файлы цифровых датчиков
const char* JD1 = "/sys/devices/platform/da850_trik/sensor_d1";
const char* JD2 = "/sys/devices/platform/da850_trik/sensor_d2";

int rungetcommand(const char *command)
{
    FILE *fp;
    int x = -1;
    char path[100] = {'\0'};
    
    /* Open the command for reading. */
    fp = popen(command, "r");
    if (fp == NULL)
        runtimeerr(wrong_robot_com, 0,0);
    
    /* Read the output a line at a time - output it. */
    while (fgets(path, sizeof(path)-1, fp) != NULL)
    {
        x = strtol(path, NULL, 16);
        printf("[%s] %d\n", path, x);
    }
    pclose(fp);
    return x;                   // ??????
}

#endif

int szof(int type)
{
    return type == LFLOAT ? 2 :
    (type > 0 && modetab[type] == MSTRUCT) ? modetab[type + 1] : 1;
}

void printf_char(int wchar)
{
    if (wchar<128)
        printf("%c", wchar);
    else
    {
        unsigned char first = (wchar >> 6) | /*0b11000000*/ 0xC0;
        unsigned char second = (wchar & /*0b111111*/ 0x3F) | /*0b10000000*/ 0x80;

        printf("%c%c", first, second);
    }
}

void fprintf_char(FILE *f, int wchar)
{    if (wchar<128)
    fprintf(f, "%c", wchar);
    else
    {
        unsigned char first = (wchar >> 6) | /*0b11000000*/ 0xC0;
        unsigned char second = (wchar & /*0b111111*/ 0x3F) | /*0b10000000*/ 0x80;

        fprintf(f, "%c%c", first, second);
    }
}

int getf_char()
{
    // reads UTF-8
    
    unsigned char firstchar, secondchar;
    
    if (scanf(" %c", &firstchar) == EOF)
        return EOF;
    else
        if ((firstchar & /*0b11100000*/0xE0) == /*0b11000000*/0xC0)
        {
            scanf("%c", &secondchar);
            return ((int)(firstchar & /*0b11111*/0x1F)) << 6 | (secondchar & /*0b111111*/0x3F);
        }
        else
            return firstchar;
}

void runtimeerr(int e, int i, int r)
{
    switch (e)
    {
        case index_out_of_range:
            printf("индекс %i за пределами границ массива %i\n", i, r-1);
            break;
        case wrong_kop:
            printf("команду %i я пока не реализовал\n", i);
            break;
        case wrong_arr_init:
            printf("массив с %i элементами инициализируется %i значениями\n", i, r);
            break;
        case wrong_string_init:
            printf("строковая переменная с %i элементами инициализируется строкой с %i литерами\n", i, r);
            break;
        case wrong_motor_num:
            printf("номер силового мотора %i, а должен быть от 1 до 4\n", i);
            break;
        case wrong_motor_pow:
            printf("задаваемая мощность мотора %i равна %i, а должна быть от -100 до 100\n", i, r);
            break;
        case wrong_digsensor_num:
            printf("номер цифрового сенсора %i, а должен быть 1 или 2\n", i);
            break;
        case wrong_ansensor_num:
            printf("номер аналогового сенсора %i, а должен быть от 1 до 6\n", i);
            break;
        case wrong_robot_com:
            printf("робот не может исполнить команду\n");
            break;
        case wrong_number_of_elems:
            printf("количество элементов в массиве по каждому измерению должно быть положительным, а тут %i\n", r);
            break;
        case zero_devide:
            printf("целое деление на 0\n");
            break;
        case float_zero_devide:
            printf("вещественное деление на 0\n");
            break;
        case mem_overflow:
            printf("переполнение памяти, скорее всего, нет выхода из рекурсии\n");
            break;
        case sqrt_from_negat:
            printf("попытка вычисления квадратного корня из отрицательного числа %\n");
            break;
        case log_from_negat:
            printf("попытка вычисления натурального логарифма из 0 или отрицательного числа\n");
            break;
        case log10_from_negat:
            printf("попытка вычисления десятичного логарифма из 0 или отрицательного числа\n");
            break;
        case wrong_asin:
            printf("аргумент арксинуса должен быть в отрезке [-1, 1]\n");
            break;
            
            
        default:
            break;
    }
    exit(3);
}
/*
void prmem()
{
    int i;
    printf("mem=\n");
    for (i=g; i<=x; i++)
        printf("%i ) %i\n",i, mem[i]);
    printf("\n");
    
}
*/
void auxprint(int beg, int t, char before, char after)
{
    double rf;
    int r = mem[beg];
    
    if (before)
        printf("%c", before);
    
    if (t == LINT)
        printf("%i", r);
    else if (t == LCHAR)
        printf_char(r);
    else if (t == LFLOAT)
    {
        memcpy(&rf, &mem[beg], sizeof(double));
        printf("%f", rf);
    }
    else if (t == LVOID)
        printf(" значения типа ПУСТО печатать нельзя\n");
    
    // здесь t уже точно положительный
    else if (modetab[t] == MARRAY)
    {
        int rr = r, i, type = modetab[t+1], d;
        d = szof(type);
        
        if (modetab[t+1] > 0)
            for (i=0; i<mem[rr-1]; i++)
                auxprint(rr + i * d, type, 0, '\n');
        else
            for (i=0; i<mem[rr-1]; i++)
                auxprint(rr + i * d, type, 0, (type == LCHAR ? 0 : ' '));
    }
    else if (modetab[t] == MSTRUCT)
    {
        int cnt = modetab[t+2], i;
        printf("{");
        for (i=2; i<=cnt; i+=2)
        {
            int type = modetab[t+i+1];
            if (type < 0)
                auxprint(beg, type, (i == 2 ? 0 : ' '), (i == cnt ? 0 : ','));
            else
                auxprint(beg, type, '\n', '\n');
            beg += szof(type);
        }
        printf("}");
    }
    else
        printf(" значения типа ФУНКЦИЯ и указателей печатать нельзя\n");
    
    if (after)
        printf("%c", after);
}

void auxget(int beg, int t)
{
    double rf;
//     printf("beg=%i t=%i\n", beg, t);
    if (t == LINT)
        scanf(" %i", &mem[beg]);
    else if (t == LCHAR)
    {
        mem[beg] = getf_char();
    }
    else if (t == LFLOAT)
    {
        scanf(" %lf", &rf);
        memcpy(&mem[beg], &rf, sizeof(double));
    }
    else if (t == LVOID)
        printf(" значения типа ПУСТО вводить нельзя\n");
    
    // здесь t уже точно положительный
    else if (modetab[t] == MARRAY)
    {
        int rr = mem[beg], i, type = modetab[t+1], d;
        d = szof(type);
        for (i=0; i<mem[rr-1]; i++)
            auxget(rr + i * d, type);
    }
    else if (modetab[t] == MSTRUCT)
    {
        int cnt = modetab[t+2], i;
        for (i=2; i<=cnt; i+=2)
        {
            int type = modetab[t+i+1];
            auxget(beg, type);
            beg += szof(type);
        }
    }
    else
        printf(" значения типа ФУНКЦИЯ и указателей вводить нельзя\n");
}

void interpreter(int);

void genarr(int N, int curdim, int d, int adr, int procnum, int *x, int *pc, int bounds[])
{
    int c0, i, curb = bounds[curdim], oldpc = *pc;
    mem[++(*x)] = curb;
    c0 = ++(*x);

    *x += curb * (curdim < N ? 1 : d) - 1;
    if (*x >= upcurthread)
        runtimeerr(mem_overflow, 0, 0);
    
    if(curdim == N)
    {
        if (procnum)
        {
            int curx = *x, oldbase = base;
            for (i=c0; i<=curx; i+=d)
            {
                *pc = procnum;   // вычисление границ очередного массива в структуре
                base = i;
                interpreter(0);
            }
            *pc = oldpc;
            base = oldbase;
        }
    }
    else
    {
        for (i=0; i < curb; i++)
        {
            genarr(N, curdim + 1, d, c0 + i, procnum, x, pc, bounds);
        }
    }
    mem[adr] = c0;
}


void rec_init_arr(int where, int from, int N, int d)
{
    int b = mem[where-1], i, j;
    for (i=0; i<b; i++)
        if (N == 1)
        {
            for (j=0; j<d; j++)
                mem[where++] = mem[from++];
        }
        else
            rec_init_arr(mem[where++], from, N-1, d);
}

int check_zero_int(int r)
{
    if (r == 0)
        runtimeerr(zero_devide, 0, 0);
    return r;
    
}

double check_zero_float(double r)
{
    if (r == 0)
        runtimeerr(float_zero_devide, 0, 0);
    return r;
}

int dsp(int di, int l)
{
    return di < 0 ? g - di : l + di;
}

void interpreter(int numthr)
{
    int l, x, pc, cur0;
    int *curth;
    int N, bounds[100], d,from, prtype;
    int i,r, flagstop = 1, entry, num, di, di1, len, n;
    double lf, rf;
    int membeg = threads[numthr];
    l = mem[membeg+1];
    x = mem[membeg+2];
    pc = mem[membeg+3];

    flagstop = 1;
    while (flagstop)
    {
        memcpy(&rf, &mem[x-1], sizeof(double));
        
        switch (mem[pc++])
        {

            case STOP:
                flagstop = 0;
                break;
/*                
            case CREATEC:
                numthr = mem[pc++];
                threads[numthr] = cur0 = MAXMEMTHREAD * numthr;
                upcurthread = cur0 + MAXMEMTHREAD;
                mem[cur0] = numthr;
                mem[cur0+1] = cur0 + 4;    // l
                mem[cur0+2] = cur0 + 6;    // x
                mem[cur0+3] = pc;
                mem[cur0+4] = g;
                mem[cur0+5] = 0;
                mem[cur0+6] = 0;
                r = t_create(interpreter, curth);
                if (r != numthr)
                    printf("bad t_create %i %i\n", r, numthr);
                break;
            case JOINC:
                t_join(mem[x--]);
                break;
            case SLEEPC:
                t_sleep(mem[x--]);
                break;
            case EXITC:
                t_exit();
                break;
            case SEMCREATEC:
                mem[x] = t_sem_create(mem[x]);
                break;
            case SEMPOSTC:
                t_sem_post(mem[x--]);
                break;
            case SEMWAITC:
                t_sem_wait(mem[x--]);
                break;
            case MSGRECEIVEC:
                mem[++x] = t_msg_receive();
                break;
            case MSGSENDC:
                r = mem[x--];
                t_msg_send(mem[x--], &r);
                break;
*/       
            case SETMOTORC:
                r = mem[x--];
                n = mem[x--];
                servo_settings(n - 1, r);
                break;
            case GETDIGSENSORC:
                n = mem[x - 1];
                mem[x - 1] = read_digital_sensor(n - 1);
                break;
                
            case GETANSENSORC:
                n = mem[x];
                mem[x] = read_analog_sensor(n - 1);
                break;
            case FUNCBEG:
                pc = mem[pc+1];
                break;
            case PRINT:
            {
                int t = mem[pc++];
                x -= szof(t);
                auxprint(x+1, t, 0, '\n');
            }
                break;
            case PRINTID:
                i = mem[pc++];              // ссылка на identtab
                prtype = identab[i+2];
                r = identab[i+1] + 2;       // ссылка на reprtab
                do
                    printf_char(reprtab[r++]);
                while (reprtab[r] != 0);
                
                if (prtype > 0 && modetab[prtype] == MARRAY && modetab[prtype+1] > 0)
                    auxprint(dsp(identab[i+3], l), prtype, '\n', '\n');
                else
                    auxprint(dsp(identab[i+3], l), prtype, ' ', '\n');
                break;
            case GETID:
                i = mem[pc++];              // ссылка на identtab
                prtype = identab[i+2];
                r = identab[i+1] + 2;       // ссылка на reprtab
                do
                    printf_char(reprtab[r++]);
                while (reprtab[r] != 0);
                printf("\n");
                auxget(dsp(identab[i+3], l), prtype);
                break;
            case ABSIC:
                mem[x] = abs(mem[x]);
                break;
            case ABSC:
                rf = fabs(rf);
                memcpy(&mem[x-1], &rf, sizeof(double));
                break;
            case SQRTC:
                if (rf < 0)
                    runtimeerr(sqrt_from_negat, 0, 0);
                rf = sqrt(rf);
                memcpy(&mem[x-1], &rf, sizeof(double));
                break;
            case EXPC:
                rf = exp(rf);
                memcpy(&mem[x-1], &rf, sizeof(double));
                break;
            case SINC:
                rf = sin(rf);
                memcpy(&mem[x-1], &rf, sizeof(double));
                break;
            case COSC:
                rf = cos(rf);
                memcpy(&mem[x-1], &rf, sizeof(double));
                break;
            case LOGC:
                if (rf <= 0)
                    runtimeerr(log_from_negat, 0, 0);
                rf = log(rf);
                memcpy(&mem[x-1], &rf, sizeof(double));
                break;
            case LOG10C:
                if (rf <= 0)
                    runtimeerr(log10_from_negat, 0, 0);
                rf = log10(rf);
                memcpy(&mem[x-1], &rf, sizeof(double));
                break;
            case ASINC:
                if (rf < -1 || rf > 1)
                    runtimeerr(wrong_asin, 0, 0);
                rf = asin(rf);
                memcpy(&mem[x-1], &rf, sizeof(double));
                break;
            case RANDC:
                rf = (double)rand() / RAND_MAX;
                memcpy(&mem[++x], &rf, sizeof(double));
                ++x;
                break;
            case ROUNDC:
                mem[x] = (int)(rf+0.5);
                break;
                
            case STRUCTWITHARR:
            {
                int oldpc, oldbase = base, procnum;
                base = (di = mem[pc++], di<0) ? g-di : l+di;
                procnum = mem[pc++];
                oldpc = pc;
                pc = procnum;
                interpreter(0);
                pc = oldpc;
                base = oldbase;
                flagstop = 1;
            }
                break;
            case DEFARR:      // N, d, displ, proc     на стеке N1, N2, ... , NN
            {
                int N = mem[pc++];
                int d = mem[pc++];
                int curdsp = mem[pc++];
                int proc = mem[pc++];
                for (i=abs(N); i>0; i--)
                    if ((bounds[i] = mem[x--]) <= 0)
                        runtimeerr(wrong_number_of_elems, 0, bounds[i]);
                
    genarr(abs(N), 1, d, N > 0 ? (curdsp < 0 ? g - curdsp : l + curdsp) : base + curdsp, proc, &x, &pc, bounds);
                flagstop = 1;
            }
                break;
            case LI:
                mem[++x] = mem[pc++];
                break;
            case LID:
                memcpy(&mem[++x], &mem[pc++], sizeof(double));
                ++x;
                ++pc;
                break;
            case LOAD:
                mem[++x] = mem[dsp(mem[pc++], l)];
                break;
            case LOADD:
                memcpy(&mem[++x], &mem[dsp(mem[pc++], l)], sizeof(double));
                ++x;
                break;
            case LAT:
                mem[x] = mem[mem[x]];
                break;
            case LATD:
                r = mem[x];
                mem[x++] = mem[r];
                mem[x] = mem[r+1];
                break;
            case LA:
                mem[++x] = dsp(mem[pc++], l);
                break;
            case CALL1:
                mem[l+1] = ++x;
                mem[x++] = l;
                mem[x++] = 0;    // следующая статика
                mem[x] = 0;      // pc в момент вызова
                break;
            case CALL2:
                i = mem[pc++];
                entry = functions[i > 0 ? i : mem[l-i]];
                l = mem[l+1];
                x = l + mem[entry+1] - 1;
                if (x >= upcurthread)
                    runtimeerr(mem_overflow, 0, 0);
                mem[l+2] = pc;
                pc = entry + 3;
                break;
            case RETURNVAL:
                d = mem[pc++];
                pc = mem[l+2];
                r = l;
                l = mem[l];
                mem[l+1] = 0;
                from = x-d;
                x = r-1;
                for (i=0; i<d; i++)
                    mem[++x] = mem[++from];
                break;
            case RETURNVOID:
                pc = mem[l+2];
                x = l-1;
                l = mem[l];
                mem[l+1] = 0;
                break;
            case NOP:
                ;
                break;
            case B:
            case STRING:
                pc = mem[pc];
                break;
            case BE0:
                pc = (mem[x--]) ?  pc + 1 : mem[pc];
                break;
            case BNE0:
                pc = (mem[x--]) ? mem[pc] : pc + 1;
                break;
            case SELECT:
                mem[x] += mem[pc++];   // ident displ
                break;
            case COPY00:
                di = dsp(mem[pc++], l);
                di1 = dsp(mem[pc++], l);
                len = mem[pc++];
                for (i=0; i<len; i++)
                    mem[di+i] =  mem[di1+i];
                break;
            case COPY01:
                di = dsp(mem[pc++], l);
                len = mem[pc++];
                di1 = mem[x--];
                for (i=0; i<len; i++)
                    mem[di+i] =  mem[di1+i];
                break;
            case COPY10:
                di =  mem[x--];
                di1 = dsp(mem[pc++], l);
                len = mem[pc++];
                for (i=0; i<len; i++)
                    mem[di+i] =  mem[di1+i];
                break;
            case COPY11:
                di1 = mem[x--];
                di =  mem[x--];
                len = mem[pc++];
                for (i=0; i<len; i++)
                    mem[di+i] =  mem[di1+i];
                break;
            case COPY0ST:
                di = dsp(mem[pc++], l);
                len = mem[pc++];
                for (i=0; i<len; i++)
                    mem[++x] =  mem[di+i];

                break;
            case COPY1ST:
                di = mem[x--];
                len = mem[pc++];
                for (i=0; i<len; i++)
                    mem[++x] =  mem[di+i];
                break;
            case COPY0STASS:
                di = dsp(mem[pc++], l);
                len = mem[pc++];
                x -= len;
                for (i=0; i<len; i++)
                    mem[di+i] = mem[x+i+1];
                break;
            case COPY1STASS:
                len = mem[pc++];
                x -= len;
                di = mem[x--];
                for (i=0; i<len; i++)
                    mem[di+i] = mem[x+i+2];
                break;
                
            case SLICE:
                d = mem[pc++];
                i = mem[x--];        // index
                r = mem[x];          // array
                if (i < 0 || i >= mem[r-1])
                    runtimeerr(index_out_of_range, i, mem[r-1]);
                mem[x] = r + i * d;
                break;
            case STRINGINIT:
                di = mem[pc++];
                r = mem[di < 0 ? g - di : l + di];
                N = mem[r-1];
                from = mem[x--];
                d = mem[from-1];     // d - кол-во литер в строке-инициаторе
                if (N != d)
                    runtimeerr(wrong_string_init, N, d);
                for (i=0; i<N; i++)
                    mem[r+i] = mem[from+i];
                break;
            case ARRINIT:
                N = mem[pc++];       // N - размерность
                d = mem[pc++];       // d
                x -= mem[pc++];      // сколько всего слов во всех элементах инициализации
                from = x + 1;
                rec_init_arr(mem[dsp(mem[pc++], l)], from, N, d);
                break;
            case WIDEN:
                rf = (double)mem[x];
                memcpy(&mem[x++], &rf, sizeof(double));
                break;
            case WIDEN1:
                mem[x+1] = mem[x];
                mem[x] = mem[x-1];
                rf = (double)mem[x-2];
                memcpy(&mem[x-2], &rf, sizeof(double));
                ++x;
                break;
            case _DOUBLE:
                r = mem[x];
                mem[++x] = r;
                break;
                
                
            case ASS:
                mem[dsp(mem[pc++], l)] = mem[x];
                break;
            case REMASS:
                r = mem[dsp(mem[pc++], l)] %= check_zero_int(mem[x]);
                mem[x] = r;
                break;
            case SHLASS:
                r = mem[dsp(mem[pc++], l)] <<= mem[x];
                mem[x] = r;
                break;
            case SHRASS:
                r = mem[dsp(mem[pc++], l)] >>= mem[x];
                mem[x] = r;
                break;
            case ANDASS:
                r = mem[dsp(mem[pc++], l)] &= mem[x];
                mem[x] = r;
                break;
            case EXORASS:
                r = mem[dsp(mem[pc++], l)] ^= mem[x];
                mem[x] = r;
                break;
            case ORASS:
                r = mem[dsp(mem[pc++], l)] |= mem[x];
                mem[x] = r;
                break;
            case PLUSASS:
                r = mem[dsp(mem[pc++], l)] += mem[x];
                mem[x] = r;
                break;
            case MINUSASS:
                r = mem[dsp(mem[pc++], l)] -= mem[x];
                mem[x] = r;
                break;
            case MULTASS:
                r = mem[dsp(mem[pc++], l)] *= mem[x];
                mem[x] = r;
                break;
            case DIVASS:
                r = mem[dsp(mem[pc++], l)] /= check_zero_int(mem[x]);
                mem[x] = r;
                break;
                
            case ASSV:
                mem[dsp(mem[pc++], l)] = mem[x--];
                break;
            case REMASSV:
                mem[dsp(mem[pc++], l)] %= check_zero_int(mem[x--]);
                break;
            case SHLASSV:
                mem[dsp(mem[pc++], l)] <<= mem[x--];
                break;
            case SHRASSV:
                mem[dsp(mem[pc++], l)] >>= mem[x--];
                break;
            case ANDASSV:
                mem[dsp(mem[pc++], l)] &= mem[x--];
                break;
            case EXORASSV:
                mem[dsp(mem[pc++], l)] ^= mem[x--];
                break;
            case ORASSV:
                mem[dsp(mem[pc++], l)] |= mem[x--];
                break;
            case PLUSASSV:
                mem[dsp(mem[pc++], l)] += mem[x--];
                break;
            case MINUSASSV:
                mem[dsp(mem[pc++], l)] -= mem[x--];
                break;
            case MULTASSV:
                mem[dsp(mem[pc++], l)] *= mem[x--];
                break;
            case DIVASSV:
                mem[dsp(mem[pc++], l)] /= check_zero_int(mem[x--]);
                break;
                
            case ASSAT:
                r = mem[mem[x-1]] = mem[x];
                mem[--x] = r;
                break;
            case REMASSAT:
                r = mem[mem[x-1]] %= check_zero_int(mem[x]);
                mem[--x] = r;
                break;
            case SHLASSAT:
                r = mem[mem[x-1]] <<= mem[x];
                mem[--x] = r;
                break;
            case SHRASSAT:
                r = mem[mem[x-1]] >>= mem[x];
                mem[--x] = r;
                break;
            case ANDASSAT:
                r = mem[mem[x-1]] &= mem[x];
                mem[--x] = r;
                break;
            case EXORASSAT:
                r = mem[mem[x-1]] ^= mem[x];
                mem[--x] = r;
                break;
            case ORASSAT:
                r = mem[mem[x-1]] |= mem[x];
                mem[--x] = r;
                break;
            case PLUSASSAT:
                r = mem[mem[x-1]] += mem[x];
                mem[--x] = r;
                break;
            case MINUSASSAT:
                r = mem[mem[x-1]] -= mem[x];
                mem[--x] = r;
                break;
            case MULTASSAT:
                r = mem[mem[x-1]] *= mem[x];
                mem[--x] = r;
                break;
            case DIVASSAT:
                r = mem[mem[x-1]] /= check_zero_int(mem[x]);
                mem[--x] = r;
                break;
                
            case ASSATV:
                mem[mem[x-1]] = mem[x];
                x--;
                break;
            case REMASSATV:
                mem[mem[x-1]] %= check_zero_int(mem[x]);
                x--;
                break;
            case SHLASSATV:
                mem[mem[x-1]] <<= mem[x];
                x--;
                break;
            case SHRASSATV:
                mem[mem[x-1]] >>= mem[x];
                x--;
                break;
            case ANDASSATV:
                mem[mem[x-1]] &= mem[x];
                x--;
                break;
            case EXORASSATV:
                mem[mem[x-1]] ^= mem[x];
                x--;
                break;
            case ORASSATV:
                mem[mem[x-1]] |= mem[x];
                x--;
                break;
            case PLUSASSATV:
                mem[mem[x-1]] += mem[x];
                x--;
                break;
            case MINUSASSATV:
                mem[mem[x-1]] -= mem[x];
                x--;
                break;
            case MULTASSATV:
                mem[mem[x-1]] *= mem[x];
                x--;
                break;
            case DIVASSATV:
                mem[mem[x-1]] /= check_zero_int(mem[x]);
                x--;
                break;
                
            case LOGOR:
                mem[x-1] = mem[x-1] || mem[x];
                x--;
                break;
            case LOGAND:
                mem[x-1] = mem[x-1] && mem[x];
                x--;
                break;
            case LOR:
                mem[x-1] |= mem[x];
                x--;
                break;
            case LEXOR:
                mem[x-1] ^= mem[x];
                x--;
                break;
            case LAND:
                mem[x-1] &= mem[x];
                x--;
                break;
            case LSHR:
                mem[x-1] >>= mem[x];
                x--;
                break;
            case LSHL:
                mem[x-1] <<= mem[x];
                x--;
                break;
            case LREM:
                mem[x-1] %= mem[x];
                x--;
                break;
            case EQEQ:
                mem[x-1] = mem[x-1] == mem[x];
                x--;
                break;
            case NOTEQ:
                mem[x-1] = mem[x-1] != mem[x];
                x--;
                break;
            case LLT:
                mem[x-1] = mem[x-1] < mem[x];
                x--;
                break;
            case LGT:
                mem[x-1] = mem[x-1] > mem[x];
                x--;
                break;
            case LLE:
                mem[x-1] = mem[x-1] <= mem[x];
                x--;
                break;
            case LGE:
                mem[x-1] = mem[x-1] >= mem[x];
                x--;
                break;
            case LPLUS:
                mem[x-1] += mem[x];
                x--;
                break;
            case LMINUS:
                mem[x-1] -= mem[x];
                x--;
                break;
            case LMULT:
                mem[x-1] *= mem[x];
                x--;
                break;
            case LDIV:
                mem[x-1] /= check_zero_int(mem[x]);
                x--;
                break;
            case POSTINC:
                mem[++x] = mem[r=dsp(mem[pc++], l)];
                mem[r]++;
                break;
            case POSTDEC:
                mem[++x] = mem[r=dsp(mem[pc++], l)];
                mem[r]--;
                break;
            case INC:
                mem[++x] = ++mem[dsp(mem[pc++], l)];
                break;
            case DEC:
                mem[++x] = --mem[dsp(mem[pc++], l)];
                break;
            case POSTINCAT:
                mem[x] = mem[r=mem[x]];
                mem[r]++;
                break;
            case POSTDECAT:
                mem[x] = mem[r=mem[x]];
                mem[r]--;
                break;
            case INCAT:
                mem[x] = ++mem[mem[x]];
                break;
            case DECAT:
                mem[x] = --mem[mem[x]];
                break;
            case INCV:
            case POSTINCV:
                mem[dsp(mem[pc++], l)]++;
                break;
            case DECV:
            case POSTDECV:
                mem[dsp(mem[pc++], l)]--;
                break;
            case INCATV:
            case POSTINCATV:
                mem[mem[x--]]++;
                break;
            case DECATV:
            case POSTDECATV:
                mem[mem[x--]]--;
                break;
                
            case UNMINUS:
                mem[x] = -mem[x];
                break;
 
            case ASSR:
                mem[r=dsp(mem[pc++], l)] = mem[x-1];
                mem[r+1] = mem[x];
                break;
            case PLUSASSR:
                memcpy(&lf, &mem[i=dsp(mem[pc++], l)], sizeof(double));
                lf += rf;
                memcpy(&mem[x-1], &lf, sizeof(double));
                memcpy(&mem[i], &lf, sizeof(double));
                break;
            case MINUSASSR:
                memcpy(&lf, &mem[i=dsp(mem[pc++], l)], sizeof(double));
                lf -= rf;
                memcpy(&mem[x-1], &lf, sizeof(double));
                memcpy(&mem[i], &lf, sizeof(double));
                break;
            case MULTASSR:
                memcpy(&lf, &mem[i=dsp(mem[pc++], l)], sizeof(double));
                lf *= rf;
                memcpy(&mem[x-1], &lf, sizeof(double));
                memcpy(&mem[i], &lf, sizeof(double));
                break;
            case DIVASSR:
                memcpy(&lf, &mem[i=dsp(mem[pc++], l)], sizeof(double));
                lf /= check_zero_float(rf);
                memcpy(&mem[x-1], &lf, sizeof(double));
                memcpy(&mem[i], &lf, sizeof(double));
                break;
                
            case ASSATR:
                r = mem[x-2];
                mem[r] = mem[x-2] = mem[x-1];
                mem[r+1] = mem[x-1] = mem[x];
                x--;
                break;
            case PLUSASSATR:
                memcpy(&lf, &mem[i=mem[x-=2]], sizeof(double));
                lf += rf;
                memcpy(&mem[++x], &lf, sizeof(double));
                memcpy(&mem[i], &lf, sizeof(double));
                break;
            case MINUSASSATR:
                memcpy(&lf, &mem[i=mem[x-=2]], sizeof(double));
                lf -= rf;
                memcpy(&mem[++x], &lf, sizeof(double));
                memcpy(&mem[i], &lf, sizeof(double));
                break;
            case MULTASSATR:
                memcpy(&lf, &mem[i=mem[x-=2]], sizeof(double));
                lf *= rf;
                memcpy(&mem[++x], &lf, sizeof(double));
                memcpy(&mem[i], &lf, sizeof(double));
                break;
            case DIVASSATR:
                memcpy(&lf, &mem[i=mem[x-=2]], sizeof(double));
                lf /= check_zero_float(rf);
                memcpy(&mem[++x], &lf, sizeof(double));
                memcpy(&mem[i], &lf, sizeof(double));
                break;
 
            case ASSRV:
                r = dsp(mem[pc++], l);
                mem[r+1] = mem[x--];
                mem[r] = mem[x--];
                break;
            case PLUSASSRV:
                memcpy(&lf, &mem[i=dsp(mem[pc++], l)], sizeof(double));
                lf += rf;
                memcpy(&mem[i], &lf, sizeof(double));
                x -= 2;
                break;
            case MINUSASSRV:
                memcpy(&lf, &mem[i=dsp(mem[pc++], l)], sizeof(double));
                lf -= rf;
                memcpy(&mem[i], &lf, sizeof(double));
                x -= 2;
                break;
            case MULTASSRV:
                memcpy(&lf, &mem[i=dsp(mem[pc++], l)], sizeof(double));
                lf *= rf;
                memcpy(&mem[i], &lf, sizeof(double));
                x -= 2;
                break;
            case DIVASSRV:
                memcpy(&lf, &mem[i=dsp(mem[pc++], l)], sizeof(double));
                lf /= check_zero_float(rf);
                memcpy(&mem[i], &lf, sizeof(double));
                x -= 2;
                break;
            
            case ASSATRV:
                r = mem[x-2];
                mem[r+1] = mem[x--];
                mem[r] = mem[x--];
                break;
            case PLUSASSATRV:
                memcpy(&lf, &mem[i=mem[x-=2]], sizeof(double));
                lf += rf;
                memcpy(&mem[i], &lf, sizeof(double));
                --x;
                break;
            case MINUSASSATRV:
                memcpy(&lf, &mem[i=mem[x-=2]], sizeof(double));
                lf -= rf;
                memcpy(&mem[i], &lf, sizeof(double));
                --x;
                break;
            case MULTASSATRV:
                memcpy(&lf, &mem[i=mem[x-=2]], sizeof(double));
                lf *= rf;
                memcpy(&mem[i], &lf, sizeof(double));
                --x;
                break;
            case DIVASSATRV:
                memcpy(&lf, &mem[i=mem[x-=2]], sizeof(double));
                lf /= check_zero_float(rf);
                memcpy(&mem[i], &lf, sizeof(double));
                --x;
                break;
                
            case EQEQR:
                memcpy(&lf, &mem[x-=3], sizeof(double));
                mem[x] = lf == rf;
                break;
            case NOTEQR:
                memcpy(&lf, &mem[x-=3], sizeof(double));
                mem[x] = lf != rf;
                break;
            case LLTR:
                memcpy(&lf, &mem[x-=3], sizeof(double));
                mem[x] = lf < rf;
                break;
            case LGTR:
                memcpy(&lf, &mem[x-=3], sizeof(double));
                mem[x] = lf > rf;
                break;
            case LLER:
                memcpy(&lf, &mem[x-=3], sizeof(double));
                mem[x] = lf <= rf;
                break;
            case LGER:
                memcpy(&lf, &mem[x-=3], sizeof(double));
                mem[x] = lf >= rf;
                break;
            case LPLUSR:
                memcpy(&lf, &mem[x-=3], sizeof(double));
                lf += rf;
                memcpy(&mem[x++], &lf, sizeof(double));
                break;
            case LMINUSR:
                memcpy(&lf, &mem[x-=3], sizeof(double));
                lf -= rf;
                memcpy(&mem[x++], &lf, sizeof(double));
                break;
            case LMULTR:
                memcpy(&lf, &mem[x-=3], sizeof(double));
                lf *= rf;
                memcpy(&mem[x++], &lf, sizeof(double));
                break;
            case LDIVR:
                memcpy(&lf, &mem[x-=3], sizeof(double));
                lf /= check_zero_float(rf);
                memcpy(&mem[x++], &lf, sizeof(double));
                break;
            case POSTINCR:
                memcpy(&rf, &mem[i=dsp(mem[pc++], l)], sizeof(double));
                memcpy(&mem[x+1], &rf, sizeof(double));
                x += 2;
                ++rf;
                memcpy(&mem[i], &rf, sizeof(double));
                break;
            case POSTDECR:
                memcpy(&rf, &mem[i=dsp(mem[pc++], l)], sizeof(double));
                memcpy(&mem[x+1], &rf, sizeof(double));
                x += 2;
                --rf;
                memcpy(&mem[i], &rf, sizeof(double));
                break;
            case INCR:
                memcpy(&rf, &mem[i=dsp(mem[pc++], l)], sizeof(double));
                ++rf;
                memcpy(&mem[x+1], &rf, sizeof(double));
                x += 2;
                memcpy(&mem[i], &rf, sizeof(double));
                break;
            case DECR:
                memcpy(&rf, &mem[i=dsp(mem[pc++], l)], sizeof(double));
                --rf;
                memcpy(&mem[x+1], &rf, sizeof(double));
                x += 2;
                memcpy(&mem[i], &rf, sizeof(double));
                break;
            case POSTINCATR:
                memcpy(&rf, &mem[i=mem[x]], sizeof(double));
                memcpy(&mem[x+1], &rf, sizeof(double));
                x+=2;
                ++rf;
                memcpy(&mem[i], &rf, sizeof(double));
                break;
            case POSTDECATR:
                memcpy(&rf, &mem[i=mem[x]], sizeof(double));
                memcpy(&mem[x+1], &rf, sizeof(double));
                x+=2;
                --rf;
                memcpy(&mem[i], &rf, sizeof(double));
                break;
            case INCATR:
                memcpy(&rf, &mem[i=mem[x]], sizeof(double));
                ++rf;
                memcpy(&mem[x+1], &rf, sizeof(double));
                x += 2;
                memcpy(&mem[i], &rf, sizeof(double));
                break;
            case DECATR:
                memcpy(&rf, &mem[i=mem[x]], sizeof(double));
                --rf;
                memcpy(&mem[x+1], &rf, sizeof(double));
                x += 2;
                memcpy(&mem[i], &rf, sizeof(double));
                break;
            case INCRV:
            case POSTINCRV:
                memcpy(&rf, &mem[i=dsp(mem[pc++], l)], sizeof(double));
                ++rf;
                memcpy(&mem[i], &rf, sizeof(double));
                break;
            case DECRV:
            case POSTDECRV:
                memcpy(&rf, &mem[i=dsp(mem[pc++], l)], sizeof(double));
                --rf;
                memcpy(&mem[i], &rf, sizeof(double));
                break;
            case INCATRV:
            case POSTINCATRV:
                memcpy(&rf, &mem[i=mem[x--]], sizeof(double));
                ++rf;
                memcpy(&mem[i], &rf, sizeof(double));
                break;
            case DECATRV:
            case POSTDECATRV:
                memcpy(&rf, &mem[i=mem[x--]], sizeof(double));
                --rf;
                memcpy(&mem[i], &rf, sizeof(double));
                break;
                
            case UNMINUSR:
                rf = -rf;
                memcpy(&mem[x-1], &rf, sizeof(num));
                break;
            case LNOT:
                mem[x] = ~mem[x];
                break;
            case LOGNOT:
                mem[x] = !mem[x];
                break;
                
            default:
                runtimeerr(wrong_kop, mem[pc-1], 0);
        }
    }
}

void ruc_import(const char *filename)
{
    int i, l, pc, x;
    
#ifdef ROBOT
    f1 = fopen(JD1, "r");                       // файлы цифровых датчиков
    f2 = fopen(JD2, "r");
    printf("stage 1\n");
    system("i2cset -y 2 0x48 0x10 0x1000 w");   // инициализация силовых моторов
    system("i2cset -y 2 0x48 0x11 0x1000 w");
    system("i2cset -y 2 0x48 0x12 0x1000 w");
    system("i2cset -y 2 0x48 0x13 0x1000 w");
#endif
    
    input = fopen(filename, "r");
    
    fscanf(input, "%d%d%d%d%d%d%d", &pc, &funcnum, &id, &rp, &md, &maxdisplg, &wasmain);

    for (i=0; i<pc; i++) {
        fscanf(input, "%d", &mem[i]);
	}
    
    for (i=0; i<funcnum; i++)
        fscanf(input, "%d", &functions[i]);
    
    for (i=0; i<id; i++)
        fscanf(input, "%d", &identab[i]);
    
    for (i=0; i<rp; i++)
        fscanf(input, "%d", &reprtab[i]);
    
    for (i=0; i<md; i++)
        fscanf(input, "%d", &modetab[i]);
    
    fclose(input);
    
    l = g = pc;
    threads[0] = 0;
    upcurthread = MAXMEMTHREAD;
    mem[g] = mem[g+1] = 0;
    x = g + maxdisplg;
    pc = 4;
    mem[1] = l;
    mem[2] = x;
    mem[3] = pc;
//    t_initAll();
//    mem[0] = curthread = t_create(interpreter, curth);
//    t_destroyAll();
    mem[0] = curthread = 0;                // номер нити главной программы 0
    interpreter(0);
    
#ifdef ROBOT
    system("i2cset -y 2 0x48 0x10 0 w");   // отключение силовых моторов
    system("i2cset -y 2 0x48 0x11 0 w");
    system("i2cset -y 2 0x48 0x12 0 w");
    system("i2cset -y 2 0x48 0x13 0 w");
    fclose(f1);
    fclose(f2);
#endif
    
    
}

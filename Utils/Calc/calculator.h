#ifndef CALCULATOR_H_INCLUDED
#define CALCULATOR_H_INCLUDED

//https://github.com/Kexin-Tang/Calculator

#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include "sys.h"

typedef struct Data_Struct //数据栈
{
    double inf[100];
    int top;
} Data;

typedef struct Symbol_Struct //操作符栈
{
    char inf[100];
    int top;
} Sym;

typedef struct TAB{
        int number;
        char ch;
}Book;

/****************************************************/

typedef void(*CalcResultUpdateHandler)(char *enter_str, double result);

/***********************Define ADT****************************/
void Init_Data_Struct(Data *data);  //初始化数据栈
void Init_Sym_Struct(Sym *sym);     //初始化操作符栈
int Push_Data(Data *data,double num);//数据压入栈
int Push_Sym(Sym *sym, char op);    //操作符入栈
double Pop_Data(Data *data);         //数据出栈
char Pop_Sym(Sym *sym);             //操作数出栈
double Read_Data(Data *data);        //读数据栈
char Read_Sym(Sym *sym);            //读操作符栈
int Judge(char ch);                 //判断是否为操作符
double Calculate(double num1,char op,double num2); //计算
char Op_Compare(char op1,char op2); //操作符优先级比较
double Output(void);                 //最终输出

void Init_Calc_Book(CalcResultUpdateHandler handler);		//初始化键盘数据表
void Calc_Run(u8 num);				//计算器运行，传入参数，按键索引值
void Calc_Exit(void);

#endif // CALCULATOR_H_INCLUDED

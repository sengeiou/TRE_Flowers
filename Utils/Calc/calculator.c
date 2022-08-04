#include "calculator.h"

#define wrong NULL    //定义错误
#define right 99     //定义正确
u8 wrong_falg = 0;

#define DEBUG_CALC 0

double ans = 0;	//保存上一次结果
double result = 0;
u8 over  = 0;	//标记是否按下=, 输入结束
u8 ce    = 0;	//标记是否按下CE, 结果清零
u8 flag	= 0; //是否完成一次输入

Book book[20];
static char enter[100]; //存放输入
//static char out[100];

//定义键盘功能
//  = -> =
//  ~ -> CE
//  # -> ANS
char sym[20]="()~/789*456-123+.0#=";    

int open_paren_cnt = 0;
static CalcResultUpdateHandler update_handler;

/***********************Define ADT****************************/
void Init_Data_Struct(Data *data);  //初始化数据栈
void Init_Sym_Struct(Sym *sym);     //初始化操作符栈
int Push_Data(Data *data,double num);//数据压入栈
int Push_Sym(Sym *sym, char op);    //操作符入栈
double Pop_Data(Data *data);         //数据出栈
char Pop_Sym(Sym *sym);             //操作数出栈
double Read_Data(Data *data);        //读数据栈
char Read_Sym(Sym *sym);            //读操作数栈
int Judge(char ch);                 //判断是否为操作符
double Calculate(double num1,char op,double num2); //进行计算
char Op_Compare(char op1,char op2); //两个操作数优先级比较
double Output(void);                 //最终计算输出

static int ii =0;

static void calc_var_init()
{
	ii = 0;
	over = 0;
	ce = 0;
	flag = 0;
	wrong_falg = 0;
	open_paren_cnt = 0;
	result = 0;

}
/***********************Implement ADT*************************/

void Init_Calc_Book(CalcResultUpdateHandler handler)
{
	int i=0;
	for(;i<20;i++)
	{
		book[i].number=i;
		book[i].ch=sym[i];
	}
	
	update_handler = handler;
}

void Calc_Exit()
{
	calc_var_init();
	memset(enter, 0, 100);
}

void Calc_Run(u8 num)
{
	
	char key = book[num].ch;
#if DEBUG_CALC
	printf("Pressed key: (%d, %c)\n", num, key);
#endif

	if(key == '='){over = 1;} //按下 =
	if(key == '~'){ce = 1;} //按下 CE
	
	if(over == 0 && ce == 0)
	{
		
		if(flag)
		{
			//已完成一次计算，清除屏幕上次记录
			calc_var_init();
			memset(enter, 0, 100);
			//printf("CALC Clear screen\n");
			update_handler(enter, result);
		}
		
		if(key == '(')
		{
			open_paren_cnt++;
		}
		
		if(key == ')')
		{
			if(open_paren_cnt == 0)
			{
				return;
			}else{
				open_paren_cnt--;
				if(open_paren_cnt < 0){open_paren_cnt = 0;}
			}
		}
		
		//未结束输入并且未清零，则添加输入
		enter[ii++] = key;
		if(book[num].ch == '#')
		{
			calc_var_init();
			memset(enter, 0, 100);
			//显示ans
			//printf("%1f\n", ans);
			update_handler("Ans", ans);
		}else
		{
			//printf("%s\n", enter);
			update_handler(enter, result);
		}
	}
	
	//输入正常没有按 CE
	if(over == 1 && ce == 0)
	{
		
		if(over)
		{
			//匹配括号是否为偶数
			for(u8 i=0;i<open_paren_cnt;i++)
			{
				enter[ii++] = book[1].ch;
			}
			enter[ii++] = key;
#if DEBUG_CALC
			printf("final enter: %s\n", enter);
#endif
		}
		
		result = Output();
		ans = result;
		
		if(wrong_falg)
		{
			calc_var_init();
			
			//printf("CALC Input error\n");
			memset(enter, 0, 100);
			update_handler("CALC Input error", result);
			return;
		}else
		{
			//sprintf(out, "%lf", ans);
			//printf("%s\n", out);
			update_handler(enter, result);
			
			//重置，准备下次输入
			calc_var_init();
			flag = 1;
		}
		
		memset(enter, 0, 100);
		
	}else if(ce == 1) //按下 CE
	{
		//printf("CALC Clear screen\n");
		calc_var_init();
		memset(enter, 0, 100);
		
		update_handler(enter, result);
	}
	
}

//初始化数据栈
void Init_Data_Struct(Data *data)
{
    data->top = -1;
}

//初始化操作符栈
void Init_Sym_Struct(Sym *sym)
{
    sym->top = -1;
}

//数据压入栈
int Push_Data(Data *data,double num)
{
    if(data->top == 100){
				wrong_falg = 1;
        return wrong;   //出现了溢出
		}
    data->top ++;
    data->inf[data->top] = num;
    return right;
}

//操作符入栈
int Push_Sym(Sym * sym,char op)
{
    if(sym->top == 100){
				wrong_falg = 1;
        return wrong;
		}
    sym->top ++;
    sym->inf[sym->top] = op;
    return right;
}

//数据出栈
double Pop_Data(Data *data)
{
    double num;
    num = data->inf[data->top];
    data->top --;
    return num;
}

//操作数出栈
char Pop_Sym(Sym * sym)
{
    char op;
    op = sym->inf[sym->top];
    sym->top--;
    return op;
}

//读数据栈
double Read_Data(Data * data)
{
    return data->inf[data->top];
}

//读操作数栈
char Read_Sym(Sym *sym)
{
    return sym->inf[sym->top];
}

//判断是否为操作符
int Judge(char ch)
{
    if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '(' || ch == ')' || ch == '=' || ch == '~' || ch == '#')
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//进行计算
double Calculate(double num1, char op, double num2)
{
    double res;  //结果
    switch(op)
    {
        case '+':
        {
            res = num1 + num2;      break;
        }

        case '-':
        {
            res = num1 - num2;      break;
        }

        case '*':
        {
            res = num1 * num2;      break;
        }

        case '/':
        {
            if(!num2)   //不能除0
            {
								wrong_falg = 1;
                return wrong;
            }
            res = num1 / num2;      break;
        }
    }
    return res;
}

/*********************************************
*   op1     操作符栈顶操作符                  
*   op2     与栈顶比较的操作符，即新取的操作符
*********************************************/
//操作符优先级比较
char Op_Compare(char op1,char op2)
{
    switch(op2)
    {
        case '(':                   //(一般都要直接压入栈
            if(op1 == ')')
            {
								wrong_falg = 1;
                return wrong;
            }
            else
            {
                return '<';
            }

        case '+':
        case '-':                   //+、-是倒数第二优先级
            return (op1 == '(' || op1 == '=') ?  '<' : '>';
        
        case '*':
        case '/':                   //*、/有较高的优先级
            return (op1 == '*' || op1 == '/' || op1 == ')') ? '>' : '<';
        
        case '=':                   //出现=的情况即压入一个），而要一直Pop符号栈直到出现（
            {
                switch(op1)
                {
                    case '=':return '=';
                    case '(':{wrong_falg = 1;return wrong;};
                    default :return '>';
                }
            }

        case ')':
            {
                switch(op1)
                {
                    case '(':return '=';
                    case '=':{wrong_falg=1;return wrong;};
                    default :return '>';
                }
            }
    }
}



char ch;    //为什么ch一定要放成全局变量？？？不科学啊？？？
//进行最终的出栈入栈和计算工作
double Output()
{
    int i=0;
    Data data;  //声明两个栈
    Sym  symbol;
    int cnt = 0;                //记录输入的个数
    double num1,num2;        //num为操作数


    Init_Data_Struct(&data);
    Init_Sym_Struct(&symbol);
    
    Push_Sym(&symbol,'='); 
    
    ch = enter[i];
    
    while((symbol.top != -1))
    {
        if(Judge(ch) == 0)     //即如果输入的为数字，则不断接收直到出现操作符，然后把数据atof存入数据栈
        {
//            char *str = (char *)malloc(sizeof(char) * 50); //用于存放键盘输入的数据
//            do
//            {
//								printf("%c\n", ch);
//                *str = ch;
//                str++;
//                cnt++;
//                i++;
//                ch = enter[i];
//            } while (Judge(ch) == 0);
//            *str = '\0';    //插入\0来标志输入的结束
//            str = str-cnt;
//            Push_Data(&data,atof(str));
//            free(str);
//            cnt = 0;
					
						char temp_str[50];
						do{
							//printf("a %c\n", ch);
							temp_str[cnt++] = ch;
							i++;
							ch = enter[i];
						}while(Judge(ch) == 0);
						temp_str[cnt] = '\0';
#if DEBUG_CALC
						printf("temp_f: %f\n", atof(temp_str));
#endif
						Push_Data(&data,atof(temp_str));
						cnt = 0;
        }
        
        else if(ch == '#')  //如果输入ans
        {
						//printf("b %c\n", ch);
            Push_Data(&data,ans);
            i++;
            ch=enter[i];
        }

        else                    //如果输入为操作符，则判断优先级来控制出入栈
        {
						//printf("c %c\n", ch);
            switch(Op_Compare(symbol.inf[symbol.top],ch))
            {
                case '>':
                {
                    num2=Pop_Data(&data);   //由于先入后出，所以先取出的是num2！！
                    num1=Pop_Data(&data);
                    num1=Calculate(num1,Pop_Sym(&symbol),num2);
                    //if(num1 == wrong)
										if(wrong_falg)
                    {
												//wrong_falg = 1;
                        return wrong;
                    }
                    else
                        Push_Data(&data,num1);    //把计算的值压入栈
                    //此时不要getchar()，因为弹出栈顶后还要比较新的栈顶的优先级
                    break;
                }
                case '<':
                {
                    Push_Sym(&symbol,ch);
                    i++;
                    ch = enter[i];
                    break;
                }
                case '=':
                {
                    Pop_Sym(&symbol);      //出现=是因为栈内有（，而匹配到）时弹出（并收取下一个输入
                    i++;
                    ch = enter[i];
                    break;
                }
                
                case wrong:
                {
										wrong_falg = 1;
                    return wrong;
                }
            }
        }

    }
    return data.inf[data.top];
}

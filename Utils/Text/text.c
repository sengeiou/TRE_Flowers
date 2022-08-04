#include "text.h"
#include "malloc.h"
#include <math.h>

#define TEXT_PAGE_BUFF_SIZE			(8*1024) //缓存8K
#define TEXT_DISP_BUFF_SIZE			(512) //每次显示最大字节数
#define TEXT_READ_LINES_MAX			(8) //每次最大读取行数

static FIL fp;
static FRESULT fres;
static uint32_t readBytes;

char* page_buff;
char* disp_buff;

int32_t page_buff_pointer;
int32_t read_line_count;
FSIZE_t file_pointer;

/***********************************************/
static void vars_init(void);

/***********************************************/
static void vars_init()
{
	readBytes = 0; 
	page_buff_pointer = 0;
	read_line_count = 0; 
	file_pointer = 0; 
	memset(page_buff, 0, TEXT_PAGE_BUFF_SIZE); 
	memset(disp_buff, 0, TEXT_DISP_BUFF_SIZE); 
}

/**
 *@name text_read_page
 *@desc 从文件读取数据到页缓存区
 */
static void text_read_page(uint8_t* pdir)
{
	//页缓存指针有效
	if(page_buff_pointer > 0 && page_buff_pointer < strlen(page_buff)) return;
	
	FSIZE_t ofs = (*pdir == TEXT_READ_PREV) ? f_tell(&fp)-TEXT_PAGE_BUFF_SIZE : f_tell(&fp) + TEXT_PAGE_BUFF_SIZE;
	ofs = fmax(ofs, 0);
	printf("ofs: %ld\n", ofs);
	
	fres = f_lseek (&fp, ofs);
	if(fres != FR_OK) return;
	
	memset(page_buff, 0, TEXT_PAGE_BUFF_SIZE);
	fres = f_read(&fp, page_buff, TEXT_PAGE_BUFF_SIZE, &readBytes);
	page_buff_pointer = 0; //重置
	
	if(f_eof(&fp) != 0 || ofs <= 0)
	{
		printf("file end\n");
	}
}

static void text_read_prev()
{
	read_line_count = 0;
	char* p;
	int read_bytes = 0;
	
	do{
	
		
		
	}while(read_line_count < TEXT_READ_LINES_MAX);
}

static void text_read_next()
{
	read_line_count = 0;
	char* p = (page_buff + page_buff_pointer); //记录初始位置
	char* pn;
	int read_bytes = 0;
	int str_len = 0;
	
	do{
		pn = strchr(p, '\n');
		
		if(pn == NULL && page_buff_pointer < TEXT_PAGE_BUFF_SIZE)
		{
			//剩余部分不够一行，需要从文件中读取
			break;
		}
		
		str_len = (pn + 1 - p);
		read_bytes += str_len;
		
		if(read_bytes > TEXT_DISP_BUFF_SIZE)
		{
			read_bytes -= str_len;
			break;
		}
		
		//偏移指针
		page_buff_pointer += str_len;
		p = (page_buff + page_buff_pointer);
		
		read_line_count++;
	}while(read_line_count < TEXT_READ_LINES_MAX);
	
	//拷贝缓冲区数据
	memcpy(disp_buff, p-read_bytes, read_bytes);
}

/**
 *@name text_read
 *@desc 从页缓存区读取到显示缓存区
 */
void text_read(uint8_t dir)
{	
	text_read_page(&dir);
	
	if(dir == TEXT_READ_PREV)
	{
		//向前读
		text_read_prev();
	}
	else
	{
		//向后读
		text_read_next();
	}
}

bool text_load(const char* path, FSIZE_t ofs)
{
	f_close(&fp);
	vars_init();
	
	//移动文件指针
	file_pointer = ofs;
	fres = f_lseek(&fp, file_pointer);
	if(fres == !FR_OK) return false;
	
	return true;
}

void text_close()
{
	vars_init();
}

bool text_init()
{
	page_buff = mymalloc(SRAMIN, TEXT_PAGE_BUFF_SIZE);
	disp_buff = mymalloc(SRAMIN, TEXT_DISP_BUFF_SIZE);
	
	if(page_buff == NULL || disp_buff == NULL) return false;
	vars_init();
}










/*
 * 数据字典管理接口
 * 作者：beastsam
 * 日期：2018.10.8
 * 功能：将数据字典配置文件初始化读入内存，实现关键字的增删改查
 * 返回值：
 *       如成功执行返回0
 *       若执行失败返回错误代码  
 * 备注：
 *       引用linux内核文件list.h
 */
 
#ifndef  DATADICTIONARYMANAGE_H
#define  DATADICTIONARYMANAGE_H

//#define OFD_DDM_DEFAULT_FILE 	"./ofd_dic.dat"
#define OFD_DDM_DEFAULT_FILE 	"./1.txt"
#define OFD_DDM_MAX_DATA_LEN 	256
#define MAX_KEYWORD_LEN 256	                 //关键字最大长度
#define MAX_HASHTABLE_SIZE 1009              //哈希表的大小

#define ERROR_OPEN_FILE 11
#define ERROR_DICFILE_FORMAT 12
#define ERROR_FREE_MEMORY 13
#define ERROR_FIND_PARAM 14
#define ERROR_NOT_FOUND 15
#define ERROR_PARAM 16


#define ERROR_KEYWORD_LEN 11            //关键字长度超出范围


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

typedef struct HashTable                          //哈希表头
{
	struct hlist_head stHead[MAX_HASHTABLE_SIZE]; 	    
}stHeads;

typedef struct HashBody
{
	char 	cKeyWord[MAX_KEYWORD_LEN]; 	//关键字
	unsigned int 	uiLevel;						      //关键字安全等级
	unsigned int 	uiRead;
	unsigned int 	uiPrint;
	unsigned int 	uiCopy;
	struct hlist_node stNode; 						   //哈希链表节点
}stBody;

int read_dic_file(stHeads *pstHT);
int find_dic_keyword(stHeads *pstHT, char *cKeyWord, unsigned int *uiLevel,unsigned int *uiRead, unsigned int *uiPrint, unsigned int *uiCopy);
int free_memory(stHeads *pstHT);
int add_dic_keyword(stHeads *pstHT, char *cKeyWord, unsigned int *uiLevel,unsigned int *uiRead, unsigned int *uiPrint, unsigned int *uiCopy);
int del_dic_keyword(stHeads *pstHT, char *cKeyWord);

#endif

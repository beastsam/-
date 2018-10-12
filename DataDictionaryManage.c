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
 
#include "DataDictionaryManage.h"

/* 
 * static unsigned int BKDRHash(const char* str, unsigned int len)
 * 功能：哈希函数
 * 参数：
 *     str[in]           输入字符串
 *     len[in]           字符串长度
 * 返回值：
 *      hash % MAX_HASHTABLE_SIZE
 * 备注：
 */

static unsigned int BKDRHash(const char* str, unsigned int len)
{
   unsigned int seed = 131; /* 31 131 1313 13131 131313 etc.. */
   unsigned int hash = 0;
   unsigned int i    = 0;

   for(i = 0; i < len; str++, i++)
   {
      hash = (hash * seed) + (*str);
   }

   return hash % MAX_HASHTABLE_SIZE;
}

/* 
 * static int hash_head_init(stHeads *pstHT)
 * 功能：哈希表头初始化
 * 参数：
 *     pstHTN[in]           哈希表头结点指针
 * 返回值：
 *       如成功执行返回0
 *       若执行失败返回错误代码  
 * 备注：
 */
static int hash_head_init(stHeads *pstHT)
{
	int i = 0;
	if (pstHT == NULL){
		return ERROR_PARAM;
	}
	
	for (i = 0; i < MAX_HASHTABLE_SIZE; i++)
	{
		INIT_HLIST_HEAD(&(pstHT->stHead[i]));
	}
	
	return 0;
}

/* 
 * static int dic_add_memory(stHeads *pstHT, char *cKeyWord, unsigned int *uiLevel, 
 *                            unsigned int *uiRead, unsigned int *uiPrint, unsigned int *uiCopy)
 * 功能：数据字典信息存入内存
 * 参数：
 *     pstHTN[in]           哈希表头结点指针
 *     cKeyWord[in]         关键字
 *     uiLevel[in]          数据等级
 *     uiRead[in]           读权限
 *     uiPrint[in]          打印权限 
 *     uiCopy[in]           复印权限
 * 返回值：
 *       如成功执行返回0
 *       若执行失败返回错误代码  
 * 备注：
 */

static int dic_add_memory(stHeads *pstHT, char *cKeyWord, unsigned int *uiLevel, 
                          unsigned int *uiRead, unsigned int *uiPrint, unsigned int *uiCopy)
{
     //输入参数检查
    if(pstHT == NULL || cKeyWord == NULL || strlen(cKeyWord)>MAX_KEYWORD_LEN){
        return ERROR_PARAM;    
   }
    
     //数据域结构体初始化
    stBody *pstHB = NULL;
    pstHB = (stBody *)malloc(sizeof(stBody));
    memset(pstHB->cKeyWord, 0,MAX_KEYWORD_LEN);
    memcpy(pstHB->cKeyWord, cKeyWord, strlen(cKeyWord));
    pstHB->uiLevel = *uiLevel;
    pstHB->uiRead = *uiRead;
    pstHB->uiPrint = *uiPrint;
    pstHB->uiCopy = *uiCopy;
    INIT_HLIST_NODE(&(pstHB->stNode));

     //计算关键字哈希下标地址，写入数据
    unsigned int hashAddr = 0;
    hashAddr = BKDRHash(cKeyWord, strlen(cKeyWord));
    hlist_add_head(&(pstHB->stNode), &(pstHT->stHead[hashAddr]));
    
    return 0;
}

/* 
 * int read_dic_file(stHeads *pstHTN)
 * 功能：读取数据字典配置文件内容，并将数据存入内存
 * 参数：
 *     pstHTN[out]       哈希表头结点指针
 * 返回值：
 *       如成功执行返回0
 *       若执行失败返回错误代码  
 * 备注：
 */
 
int read_dic_file(stHeads *pstHTN)
{
    char cBuffer[OFD_DDM_MAX_DATA_LEN];
	 char cKeyWord[MAX_KEYWORD_LEN];
	 memset(cBuffer, 0, OFD_DDM_MAX_DATA_LEN);
	 memset(cKeyWord, 0, MAX_KEYWORD_LEN);
	 unsigned int uiLevel = 0;
	 unsigned int uiRead = 0;
	 unsigned int uiPrint = 0;
	 unsigned int uiCopy = 0;
	 int result = 0;                             //存储节点加入函数执行结果被
	 
	 /* 
      * 初始化哈希表头，哈希表头为结构体数组，
      * 每个数组的下标为关键字的哈希值，数组内容存储指向节点的指针
      */
    stHeads *pstHT = NULL;
	 pstHT = (stHeads *)malloc(sizeof(stHeads));        
    hash_head_init(pstHT);
	 
	 /* 读取数据字典配置文件，每读一行就将数据存入内存一行 */
    FILE *fpDic = NULL;
    fpDic = fopen(OFD_DDM_DEFAULT_FILE,"r");
    if(fpDic == NULL){
        return ERROR_OPEN_FILE;                        //配置文件打开失败，返回值 11
    }
    
    while (fgets(cBuffer, OFD_DDM_MAX_DATA_LEN, fpDic))
	{
	    //判断跳过文件头部注释
		if ((cBuffer[0] == '#') || (cBuffer[0] == '\n')){
			continue; 
		}
		
		//匹配数据字典中以keyword开头的行
		if (memcmp(cBuffer, "KeyWord", strlen("KeyWord")) == 0){
			sscanf(cBuffer, "%*[^=]=%[^,],%*[^=]=%d,%*[^=]=%d,%*[^=]=%d,%*[^=]=%d",
					 cKeyWord, &uiLevel, &uiRead, &uiPrint, &uiCopy);
		} else{
		    return ERROR_DICFILE_FORMAT;                //配置文件格式错误,返回值 12
		}
		
		//判断内存加入是否成功，失败则返回错误码
		result = dic_add_memory(pstHT, cKeyWord, &uiLevel, &uiRead, &uiPrint, &uiCopy);
		if(result != 0){
		    return result;
		}
		
		memset(cBuffer, 0, OFD_DDM_MAX_DATA_LEN);
		memset(cKeyWord, 0, OFD_DDM_MAX_DATA_LEN);
	}
    
    //返回调用者生成的哈希头结点
    *pstHTN = *pstHT;
    fclose(fpDic);
    if(fpDic != NULL){
        fpDic = NULL;
    }
    return 0;
}

/* 
 * int free_memory(stHeads *pstHT)
 * 功能：放开辟内存
 * 参数：
 *     pstHTN[in]       哈希表头结点指针
 * 返回值：
 *       如成功执行返回0
 *       若执行失败返回错误代码  
 * 备注：
 */

int free_memory(stHeads *pstHT)
{
   if(pstHT == NULL){
        return ERROR_FREE_MEMORY;
   }

    struct hlist_node *tmp1 = NULL;
    struct hlist_node *tmp2 = NULL;
    stBody *pstHB = NULL;
    int i;
    for(i=0;i<MAX_HASHTABLE_SIZE;i++){
        hlist_for_each_entry_safe(pstHB, tmp1, tmp2, &(pstHT->stHead[i]),stNode){
            if(pstHB){
                free(pstHB);
            }
            (pstHT->stHead[i]).first = NULL;
        }
    }
    return 0;
}

/* 
 * int find_dic_keyword(stHeads *pstHT, char *cKeyWord, unsigned int *uiLevel,
 *                      unsigned int *uiRead, unsigned int *uiPrint, unsigned int *uiCopy) 
 * 功能：查找关键字
 * 参数：
 *     pstHTN[in]           哈希表头结点指针
 *     cKeyWord[in]         关键字
 *     uiLevel[in]          数据等级
 *     uiRead[in]           读权限
 *     uiPrint[in]          打印权限 
 *     uiCopy[in]           复印权限
 * 返回值：
 *       如成功执行返回0
 *       若执行失败返回错误代码  
 * 备注：
 */

int find_dic_keyword(stHeads *pstHT, char *cKeyWord, unsigned int *uiLevel,
                     unsigned int *uiRead, unsigned int *uiPrint, unsigned int *uiCopy) 
{
   if(cKeyWord == NULL || strlen(cKeyWord)>MAX_KEYWORD_LEN || pstHT == NULL){
        return ERROR_FIND_PARAM;
    }
    
    int iaddr = 0;                     //存储哈希节点下标
    struct hlist_node *tmp1 = NULL;
    struct hlist_node *tmp2 = NULL;
    stBody *pstHB = NULL;
    iaddr = BKDRHash(cKeyWord, strlen(cKeyWord));
    
     //遍历节点，找到符合关键字的相关信息
    hlist_for_each_entry_safe(pstHB, tmp1, tmp2, &(pstHT->stHead[iaddr]),stNode){
       if(strcmp(pstHB ->cKeyWord, cKeyWord) == 0){
            *uiLevel =  pstHB ->uiLevel;
            *uiRead =  pstHB ->uiRead;
            *uiPrint =  pstHB ->uiPrint;
            *uiCopy =  pstHB ->uiCopy;
            return 0;
       }       
    }
    return ERROR_NOT_FOUND;
}

/* 
 * int add_dic_keyword(stHeads *pstHT, char *cKeyWord, unsigned int *uiLevel,
 *                     unsigned int *uiRead, unsigned int *uiPrint, unsigned int *uiCopy)
 * 功能：添加关键字
 * 参数：
 *     pstHTN[in]           哈希表头结点指针
 *     cKeyWord[in]         关键字
 *     uiLevel[in]          数据等级
 *     uiRead[in]           读权限
 *     uiPrint[in]          打印权限 
 *     uiCopy[in]           复印权限
 * 返回值：
 *       如成功执行返回0
 *       若执行失败返回错误代码  
 * 备注：
 */

int add_dic_keyword(stHeads *pstHT, char *cKeyWord, unsigned int *uiLevel,
                    unsigned int *uiRead, unsigned int *uiPrint, unsigned int *uiCopy)
{
   if(pstHT == NULL){
        return ERROR_PARAM;    
   }
   if(cKeyWord == NULL || strlen(cKeyWord)>MAX_KEYWORD_LEN){
        return ERROR_PARAM;
   }
   if(uiLevel == NULL){
        return ERROR_PARAM;
   }
   if(uiRead == NULL){
        return ERROR_PARAM; 
   }
   if(uiPrint == NULL){
        return ERROR_PARAM;
   }
   if(uiCopy == NULL){
        return ERROR_PARAM; 
   }
     
    int result = 0; 
    result = dic_add_memory(pstHT, cKeyWord, uiLevel, uiRead, uiPrint, uiCopy);
    return result;
}

/* 
 * int del_dic_keyword(stHeads *pstHT, char *cKeyWord)
 * 功能：删除节点
 * 参数：
 *     pstHTN[in]           哈希表头结点指针
 *     cKeyWord[in]         关键字
 * 返回值：
 *       如成功执行返回0
 *       若执行失败返回错误代码  
 * 备注：
 */

int del_dic_keyword(stHeads *pstHT, char *cKeyWord)
{
   if(pstHT == NULL || cKeyWord == NULL || strlen(cKeyWord)>MAX_KEYWORD_LEN){
        return ERROR_PARAM;    
   }
    
    int iaddr = 0;
    struct hlist_node *tmp1 = NULL;
    struct hlist_node *tmp2 = NULL;
    stBody *pstHB = NULL;
    iaddr = BKDRHash(cKeyWord, strlen(cKeyWord));
    hlist_for_each_entry_safe(pstHB, tmp1, tmp2, &(pstHT->stHead[iaddr]),stNode){
       if(strcmp(pstHB ->cKeyWord, cKeyWord) == 0){
            hlist_del(&(pstHB->stNode));
            return 0;
         }
     }
   return ERROR_NOT_FOUND;
}



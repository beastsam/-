#include <stdlib.h>
#include <stdio.h>
#include "DataDictionaryManage.h"

int main(int argc,char *argv[])
{
    char cKeyWord[256] = "公开"; 	                          //关键字
	unsigned int 	uiLevel = 0;						      //关键字安全等级
	unsigned int 	uiRead = 0;
	unsigned int 	uiPrint = 0;
	unsigned int 	uiCopy = 0;
   stHeads *pstHT = NULL;
   pstHT = (stHeads *)malloc(sizeof(stHeads));
   
   
   read_dic_file(pstHT);
   find_dic_keyword(pstHT, cKeyWord, &uiLevel, &uiRead, &uiPrint, &uiCopy);    
   printf("level:%d\n",uiLevel);
   printf("read:%d\n",uiRead);
   printf("print:%d\n",uiPrint);
   printf("copy:%d\n",uiCopy);
    
    
    char cKeyWord1[256] = "超级秘密";	                          //关键字
	unsigned int 	uiLevel1 = 5;						      //关键字安全等级
	unsigned int 	uiRead1 = 5;
	unsigned int 	uiPrint1 = 5;
	unsigned int 	uiCopy1 = 5;

    add_dic_keyword(pstHT, cKeyWord1, &uiLevel1,&uiRead1, &uiPrint1, &uiCopy1);
    int c;
    c = find_dic_keyword(pstHT, cKeyWord1, &uiLevel, &uiRead, &uiPrint, &uiCopy);    
    
    printf("c:%d\n",c);
    
    printf("level1:%d\n",uiLevel);
    printf("read1:%d\n",uiRead);
    printf("print1:%d\n",uiPrint);
    printf("copy1:%d\n",uiCopy);


    int f,s;
    f = del_dic_keyword(pstHT, cKeyWord1 );

    s = find_dic_keyword(pstHT, cKeyWord1, &uiLevel, &uiRead, &uiPrint, &uiCopy);    

    printf("f:%d\n,s:%d\n",f,s);







    char str[100];
    printf("close:");
    scanf("%s",str);
    if(strcmp(str,"close") == 0){
       free_memory(pstHT); 
    }

   getchar();
   
   return 0;
}














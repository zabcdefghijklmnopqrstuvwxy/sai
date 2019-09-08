#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ini_rw.h"

#define SIZE_LINE		1024	//每行最大长度
#define SIZE_FILENAME		256		//文件名最大长度
#define min(x, y)		(x <= y) ? x : y

#define CTRL_LF			0x0A			//回车ASCII码
#define	CTRL_CR			0x0D			//换行ASCII码

/**
 * @brief ini 行解析的枚举  
 */
typedef enum _ELineType_ {
	LINE_IDLE,		/**< 未处理行 */
	LINE_ERROR,		/**< 错误行 */
	LINE_EMPTY,		/**< 空白行或注释行 */
	LINE_SECTION,	/**< 节定义行 */
	LINE_VALUE		/**< 值定义行 */
} ELineType ;
	
static int8_t gFilename[SIZE_FILENAME];   /* 文件名数组 */
static int8_t *gcBuffer;                   /* 读取文件缓存指针 */
static int32_t gnBuflen;			/* 读取文件长度 */ 

static int32_t gnRstStrFlag;		/* 字符串复位标记 */
static int32_t gnRstFloatFlag;        /* 浮点数复位标记 */
static int32_t gnRstIntFlag;          /* 整形复位标记 */

/**
 * @brief 去除首尾空格
 * @param[in] s 待处理的字符串指针
 * @note 将字符串中的空格去掉，会修改传入的字符串数据
 * @retval 返回修改后的字符串指针
 */
static int8_t *StrHtStrip(int8_t *s)
{
	size_t size;
	int8_t *p1, *p2;
	size = strlen(s);

	if (!size)
   	{
   		return s;
   	}

   	p2 = s + size - 1;

   	while ((p2 >= s) && isspace(*p2))
   	{
   		p2--;
   	}

 	*(p2 + 1) = ' ';
	 p1 = s;

 	while (*p1 && isspace(*p1))
 	{
 		p1++;
 	}

 	if (s != p1)
	{
 		memmove(s, p1, p2 - p1 + 2);
 	}

 	return s;
}
 
/**
 * @brief 去处字符串中的空格 
 * @param[in] s 待处理的字符串指针
 * @note 将字符串中的空格去掉，会修改传入的字符串数据
 * @retval 返回修改后的字符串指针
 */
static int8_t *StrStrip(int8_t *s)
{
	int32_t nIndex = 0;
	size_t size;
	int8_t *p = NULL;
	int8_t cStr[256] = {0};

	p = s;
	size = strlen(s);

	while(*p != '\0')
	{
		if(!isspace(*p))
		{
			cStr[nIndex++] = *p; 
		}
		p++;
	}

	cStr[nIndex] = '\0';

	if(strlen(cStr) < size)
	{
		memmove(s,cStr,nIndex + 1);
	}

	return s;
}

/**
 * @brief 比较字符串 
 * @param[in] s1 比较的第一个字符串，s2比较的第二个字符串
 * @note 不区分大小写进行比较
 * @retval 返回0表示两个字符串相同，不为0表示字符串不相同
 */
static int32_t StriCmp(const int8_t *s1, const int8_t *s2)
{
	int32_t ch1, ch2;
	do
	{
		ch1 = (  int8_t)*(s1++);
		if ((ch1 >= 'A') && (ch1 <= 'Z'))
			ch1 += 0x20;
		ch2 = (  int8_t)*(s2++);
		if ((ch2 >= 'A') && (ch2 <= 'Z'))
			ch2 += 0x20;
	} while ( ch1 && (ch1 == ch2) );
	return(ch1 - ch2);
}

/**
 * @brief 获取一行ini字符串
 * @param[in] buf ini文件数据指针
 * @param[out] content 有效内容串，rem1 注释首，rem2 注释尾，nextline 下一行首 
 * @note 注释 
 * @retval 返回行检测码
 */
static int32_t INI_GetLine(int8_t *buf, int32_t buflen, int8_t *content, int8_t **rem1, int8_t **rem2, int8_t **nextline)
{
	int8_t *cont1, *cont2;
	int32_t cntblank, cntCR, cntLF;		//连续空格、换行符数量
	int8_t isQuot1, isQuot2;			//引号
	int32_t i;
	int8_t *p;
	//首先断行断注释，支持如下换行符：\r \n
	cntblank = 0;
	cntCR = cntLF = 0;
	isQuot1 = isQuot2 = 0;
	cont1 = *rem1 = 0;
	content[0] = 0;

	for (i = 0, p = buf; i < buflen; i++, p++)
	{
		if (*p == 0) 
		{
			p++;
			break;
		}
		//2个CR或LF，行结束
		if (cntCR == 2 || cntLF == 2) 
		{
			p--;	//回溯1
			break;
		}
		//CR或LF各1个之后任意字符，行结束
		if (cntCR + cntLF >= 2) 
		{
			break;
		}
		//CR或LF之后出现其它字符，行结束
		if ((cntCR || cntLF) && *p != CTRL_LF && *p != CTRL_CR)
		{
			break;
		}

		switch (*p) 
		{
			case CTRL_CR:
				{
					cntCR++;
				}break;
			case CTRL_LF:
				{
					cntLF++;
				}break;
			case '\'':
				if (!isQuot2)
				{
					isQuot1 = 1 - isQuot1;
				}break;
			case '\"':
				if (!isQuot1)
				{
					isQuot2 = 1 - isQuot2;
				}break;
			case ';':

			case '#':
				if (isQuot1 || isQuot2)
				{ 
					break;
				} 
				if (*rem1 == NULL)
				{
					*rem1 = p - cntblank;
				}break;
			default:
				if (isspace((int8_t)*p))
				{
					cntblank++;

				} 
				else 
				{
					cntblank = 0;
					if ((*rem1 == NULL) && (cont1 == NULL))
					{ 
						cont1 = p;
					}
				}break;
		}
	}

	*nextline = p;
	*rem2 = p - cntCR - cntLF;

	if (*rem1 == NULL)
	{ 
		*rem1 = *rem2;
	}

	cont2 = *rem1 - cntblank;

	if (cont1 == NULL) 
	{
		cont1 = cont2;
		return LINE_EMPTY;
	}

	i = (int32_t)(cont2 - cont1);

	if (i >= SIZE_LINE)
	{ 
		return LINE_ERROR;
	}

	//内容头尾已无空格
	memcpy(content, cont1, i);
	content[i] = 0;

	if (content[0] == '[' && content[i - 1] == ']')
	{
		return LINE_SECTION;
	}

	if (strchr(content, '=') != NULL)
	{
		return LINE_VALUE;
	}

	return LINE_ERROR;
}

/**
 * @brief 查找节是否存在
 * @param[in] section 待查找的节名称指针
 * @param[out] sect1 节名称首指针，sect2 节名称尾，cont1 节内容首，cont2 节内容尾，nextsect 下一节首 
 * @note None 
 * @retval None
 */
static int32_t INI_FindSection(const int8_t *section, int8_t **sect1, int8_t **sect2, int8_t **cont1, int8_t **cont2, int8_t **nextsect)
{
	int32_t type;
	int8_t content[SIZE_LINE];
	int8_t *rem1, *rem2, *nextline;
	int8_t *p;
	int8_t *empty;
	int32_t uselen = 0;
	int8_t found = 0;

	if (gcBuffer == NULL) 
	{
		return 0;
	}

	while (gnBuflen - uselen > 0) 
	{
		p = gcBuffer + uselen;
		type = INI_GetLine(p, gnBuflen - uselen, content, &rem1, &rem2, &nextline);
		uselen += (int32_t)(nextline - p);

		if (LINE_SECTION == type) 
		{
			if (found || section == NULL)
			{	
				break;		//发现另一section
			}
			content[strlen(content) - 1] = 0;	//去尾部]
			StrStrip(content + 1);		        //去首尾空格

			if (StriCmp(content + 1, section) == 0) 
			{
				found = 1;
				*sect1 = p;
				*sect2 = rem1;
				*cont1 = nextline;
			}
			empty = nextline;
		} 
		else if (LINE_VALUE == type) 
		{
			if (!found && section == NULL) 
			{
				found = 1;
				*sect1 = p;
				*sect2 = p;
				*cont1 = p;
			}
			empty = nextline;
		}
	}

	if (!found) 
	{
		return 0;
	}

	*cont2 = empty;
	*nextsect = nextline;

	return 1;
}

/**
 * @brief 获取键值 
 * @param[in] content 内容串
 * @param[out] key 键名，value 键值
 * @note content 会被修改，从一行中获取键值
 * @retval None
 */
static void INI_GetKeyValue(int8_t *content, int8_t **key, int8_t **value)
{
	int8_t *p;
	p = strchr(content, '=');

	*p = 0;
	StrStrip(content);
	StrStrip(p + 1);

	*p = '\0';

	*key = content;
	*value = p + 1;
}

/**
 * @brief 释放ini文件资源 
 * @param None
 * @note None
 * @retval None
 */
void INI_FreeFile(void)
{
	if (gcBuffer != NULL) 
	{
		free(gcBuffer);
		gcBuffer = 0;
		gnBuflen = 0;
	}
}

/**
 * @brief 加载ini文件
 * @param[in] filename 加载的ini文件名
 * @note 每次加载文件名时会调用INI_FreeFile释放资源
 * @retval None
 */
ini_err_t INI_LoadFile(const int8_t *filename)
{
	FILE *file;
	int32_t len;

	INI_FreeFile();
	if (strlen(filename) >= sizeof(gFilename))
	{
		return ERR_FILENAME_TOOLONG;
	}

	strcpy(gFilename, filename);
	file = fopen(gFilename, "rb");

	if (file == NULL) 
	{
		return ERR_OPENFILE_FAILED;
	}

	fseek(file, 0, SEEK_END);
	len = ftell(file);
	gcBuffer = malloc(len);

	if (gcBuffer == NULL) 
	{
		fclose(file);
		return ERR_MALLOC_FAILED;
	}

	fseek(file, 0, SEEK_SET);
	len = fread(gcBuffer, 1, len, file);
	fclose(file);
	gnBuflen = len;

	return ERR_OK;

}

/**
 * @brief 获取键值
 * @param[in] section 节名，key 键名，value 键值，maxlen 键值最大长度，defvalue 默认数值
 * @note None
 * @retval None
 */
static ini_err_t INI_GetValue(const int8_t *section, const int8_t *key, int8_t *value, int32_t maxlen)
{
	ini_err_t err = ERR_OK;
	int32_t type;
	int8_t content[SIZE_LINE];
	int8_t *rem1, *rem2, *nextline;
	int8_t *key0, *value0;
	int8_t *p;
	int32_t uselen = 0;
	int8_t found = 0;
	int32_t len;
	int8_t cFindSign = 0;

	memset(content,0,sizeof(content));

	if (gcBuffer == NULL) 
	{
		return ERR_MALLOC_FAILED;
	}

	while (gnBuflen - uselen > 0) 
	{
		p = gcBuffer + uselen;
		type = INI_GetLine(p, gnBuflen - uselen, content, &rem1, &rem2, &nextline);
		uselen += (int32_t)(nextline - p);

		if (LINE_SECTION == type) 
		{
			if (found || section == NULL) 
			{
				err = ERR_LOOKUP_SECT_FAILED;
				break;		//发现另一section
			}

			content[strlen(content) - 1] = 0;			//去尾部]
			StrStrip(content + 1);						//去首尾空格
			if (StriCmp(content + 1, section) == 0) 
			{
				found = 1;
			}

		} 
		else if (LINE_VALUE == type) 
		{
			if (!found && section == NULL) 
			{
				found = LINE_SECTION;
			}

			if (!found)
			{ 
				continue;
			}

			INI_GetKeyValue(content, &key0, &value0);

			if (StriCmp(key0, key) == 0) 
			{
				len = strlen(value0);
				if (len == 0) 
				{
					break;		//空值视为无效
				}
				if (value != NULL) 
				{
					len = min(len, maxlen - 1);

					if(len > maxlen)
					{
						err =  ERR_ARRAY_BOUND;
						break;
					}
					else
					{
						strncpy(value, value0, len);
						value[len] = 0;
					}
				}
				found = LINE_VALUE;
				break;
			}
		}
	}
	//未发现键值

	if(LINE_VALUE != found)
	{
		err = ERR_LOOKUP_KEY_FAILED;
	}

	return err;
}

/**
 * @brief 获取字符串
 * @param[in] section 节名，key 键名，value 键值，maxlen 键值最大长度，defvalue 默认数值
 * @note None
 * @retval None
 */
ini_err_t INI_GetString(const int8_t *section, const int8_t *key, int8_t *value, int32_t maxlen)
{
	int32_t ret;
	int32_t len;

	if(NULL == section || NULL == key || NULL == value)
	{
		return ERR_INVALID;
	}


	ret = INI_GetValue(section, key, value, maxlen);
	if (ret)
	{ 
		return ret;
	} 
	//去首尾空格

	len = strlen(value);
	if (value[0] == '\'' && value[len - 1] == '\'') 
	{
		value[len - 1] = 0;
		memmove(value, value + 1, len - 1);
	} 
	else if (value[0] == '\'' && value[len - 1] == '\"') 
	{
		value[len - 1] = 0;
		memmove(value, value + 1, len - 1);
	}

	return ret;
}

/**
 * @brief 获取整形数据
 * @param[in] section 节名，key 键名，defvalue 默认数据
 * @note None
 * @retval 返回整形数据 
 */
ini_err_t INI_GetInt(const int8_t *section, const int8_t *key, int32_t *value)
{
	ini_err_t err = ERR_OK;
	int8_t valstr[64];

	if(NULL == section || NULL == key || NULL == value)
	{
		return ERR_INVALID;
	}

	if (ERR_OK == (err = INI_GetValue(section, key, valstr, sizeof(valstr))))
	{ 
		*value = (int32_t)strtol(valstr, NULL, 0);
	}

	return err;
}

/**
 * @brief 获取浮点数据
 * @param[in] section 节名，key 键名，defvalue 默认数据
 * @note None
 * @retval 返回浮点数据 
 */
ini_err_t INI_GetFloat(const int8_t *section, const int8_t *key, float *value)
{
	ini_err_t err = ERR_OK;
	int8_t valstr[64];

	if(NULL == section || NULL == key || NULL == value)
	{
		return ERR_INVALID;
	}

	if(ERR_OK == (err = INI_GetValue(section, key, valstr, sizeof(valstr))))
	{ 
		*value = atof(valstr);
	}

	return err;
}

/**
 * @brief 获取枚举数据
 * @param[in] section 节名，key 键名，value 键值，整形数组指针, maxlen键值数组最大长度
 * @note None
 * @retval 返回ini解析错误码
 */
ini_err_t INI_GetEnum(const int8_t *section, const int8_t *key, int32_t *value,int32_t *len,int32_t maxlen)
{
	ini_err_t err = ERR_OK;
	int8_t valstr[128] = {0};
	int8_t tempstr[16] = {0};
	int32_t nLen = 0;
	int32_t *pValue = value;
	int8_t *pStr = NULL;
	int8_t cIndex = 0;
	int32_t nCnt = 0;

	if(NULL == section || NULL == key || NULL == value)
	{
		return ERR_INVALID;
	}

	
	if(ERR_OK == (err = INI_GetValue(section, key, valstr, sizeof(valstr))))
	{
		nLen = strlen(valstr) + 1;
		pStr = valstr;		

		while(nLen--)
		{
			if(*pStr != ',' && *pStr != '\0')
			{
				tempstr[cIndex++] = *pStr;
			}
			else
			{
				*pValue = (int32_t)strtol(tempstr, NULL, 0);
				pValue++;
				memset(tempstr,0,sizeof(tempstr));
				cIndex = 0;
				nCnt++;
			}
			pStr++;
		}

	}
	
	*len = nCnt;

	return err;
}

/**
 * @brief 设置字符串
 * @param[in] section 节名，key 键名，value 字符串数值
 * @note 如果匹配查找成功后，会将键值进行修改，如果value为NULL则删除这一行包括注释，如果匹配查找失败后会增加一行
 * @retval 返回ini解析错误码
 */
ini_err_t INI_SetData(const int8_t *section, const int8_t *key, const int8_t *value)
{
	FILE *file;
	int8_t *sect1, *sect2, *cont1, *cont2, *nextsect;
	int8_t *p;
	int32_t len, type;
	int8_t content[SIZE_LINE];
	int8_t *key0, *value0;
	int8_t *rem1, *rem2, *nextline;
	int8_t cLookUpFlag = 0;

	if (gcBuffer == NULL) 
	{
		return ERR_MALLOC_FAILED;
	}

	if(INI_FindSection(section, &sect1, &sect2, &cont1, &cont2, &nextsect) == 0)
	{
		//未找到节
		//value无效则返回
		if (value == NULL)
		{	
			return ERR_SETVALUE_ILLEGAL;
		}

		//在文件尾部添加
		file = fopen(gFilename, "ab");

		if (file == NULL) 
		{
			return ERR_OPENFILE_FAILED;
		}

		fprintf(file,"[%s]%s=%s",section,key,value);
		fclose(file);
		INI_LoadFile(gFilename);
		return ERR_OK;
	}

	//找到节，则节内查找key
	p = cont1;
	len = (int32_t)(cont2 - cont1);

	while (len > 0) 
	{
		type = INI_GetLine(p, len, content, &rem1, &rem2, &nextline);

		if (LINE_VALUE == type) 
		{
			INI_GetKeyValue(content, &key0, &value0);
			if (StriCmp(key0, key) == 0) 
			{
				//找到key
				file = fopen(gFilename, "wb");
				if (file == NULL) 
				{ 
					return ERR_OPENFILE_FAILED;
				}

				len = (int32_t)(p - gcBuffer);
				fwrite(gcBuffer, 1, len, file);					//写入key之前部分
				if (value == NULL) 
				{
					//value无效，删除
					len = (int32_t)(nextline - gcBuffer);			//整行连同注释一并删除
				} 
				else 
				{
					//value有效，改写
					fprintf(file, "%s=%s", key, value);
					len = (int32_t)(rem1 - gcBuffer);			//保留尾部原注释!
				}

				fwrite(gcBuffer + len, 1, gnBuflen - len, file);	//写入key所在行含注释之后部分
				fclose(file);
				INI_LoadFile(gFilename);
				
				cLookUpFlag = 1;
				break;
			}	
		}
		len -= (int32_t)(nextline - p);
		p = nextline;
	}

	//未找到key
	//value无效则返回
	if(0 == cLookUpFlag)
	{
		if (value == NULL) 
		{ 
			return ERR_SETVALUE_ILLEGAL;
		}

		//在文件尾部添加
		file = fopen(gFilename, "wb");
		if (file == NULL) 
		{ 
			return ERR_OPENFILE_FAILED;
		}

		len = (int32_t)(cont2 - gcBuffer);
		fwrite(gcBuffer, 1, len, file);					//写入key之前部分
		fprintf(file, "%s=%s", key, value);
		fwrite(gcBuffer + len, 1, gnBuflen - len, file);	//写入key之后部分
		fclose(file);
		INI_LoadFile(gFilename);
	}

	return ERR_OK;
}

/**
 * @brief 设置字符串
 * @param[in] section 节名，key 键名，value 字符串数值，base表示进制10表示十进制，16表示十六进制，8表示八进制
 * @note 如果匹配查找成功后，会将键值进行修改，如果value为NULL则删除这一行包括注释，如果匹配查找失败后会增加一行
 * @retval 返回错误码
 */
ini_err_t INI_SetInt(const int8_t *section, const int8_t *key, int32_t value, int32_t base)
{
	int8_t valstr[64];
	int8_t orikey[64];
	int32_t orivalue = 0;
	ini_err_t err = ERR_OK;

	sprintf(orikey,"%s%s","ori-",key);
	err = INI_GetInt(section, orikey, &orivalue);

	if(err == ERR_LOOKUP_KEY_FAILED)
	{
		memset(valstr,0,sizeof(valstr));
		sprintf(valstr,"%d",&orivalue);
		err = INI_SetData(section, orikey, valstr);
		if(err != ERR_OK)
		{
			return err;	
		}
	}

	switch (base) 
	{
		case 16:
			sprintf(valstr, "0x%x", value);
			return INI_SetData(section, key, valstr);
		case 8:
			sprintf(valstr, "0%o", value);
			return INI_SetData(section, key, valstr);
		default:	//10
			sprintf(valstr, "%d", value);
			return INI_SetData(section, key, valstr);
	}
}

/**
 * @brief 设置浮点数值 
 * @param[in] section 节指针，key 键名指针，value 键值
 * @note 如果匹配查找成功后，会将键值进行修改，如果value为NULL则删除这一行包括注释，如果匹配查找失败后会增加一行
 * @return 返回ini解析错误码
 */
ini_err_t INI_SetFloat(const int8_t *section, const int8_t *key, float value)
{
	int8_t valstr[64];
	int8_t orikey[64] = {0};
	float orivalue = 0.0;
	ini_err_t err = ERR_OK;

	sprintf(orikey,"%s%s","ori-",key);
	err = INI_GetFloat(section, orikey, &orivalue);

	if(err == ERR_LOOKUP_KEY_FAILED)
	{
		memset(valstr,0,sizeof(valstr));
		sprintf(valstr,"%f",orivalue);
		err = INI_SetData(section, orikey, valstr);
		if(err != ERR_OK)
		{
			return err;
		}
	}

	memset(valstr,0,sizeof(valstr));
	sprintf(valstr,"%f",value);
	return INI_SetData(section,key,valstr);
}

/**
 * @brief 设置字符串
 * @param[in] section 节名，key 键名，value 字符串数值,len 字符串长度
 * @note 如果匹配查找成功后，会将键值进行修改，如果value为NULL则删除这一行包括注释，如果匹配查找失败后会增加一行
 * @retval 返回ini解析错误码
 */
ini_err_t INI_SetString(const int8_t *section, const int8_t *key, const int8_t *value,int32_t len)
{
	int8_t valstr[256] = {0};
	int8_t orikey[64] = {0};
	ini_err_t err = ERR_OK;


	if(NULL == section || NULL == key)
	{
		return ERR_INVALID;
	}

	if(len > FILE_MAX_LEN)
	{
		return ERR_OUT_FILE_LENGTH;
	}


	sprintf(orikey,"%s%s","ori-",key);
	err = INI_GetString(section, orikey, valstr, sizeof(valstr));

	if(err == ERR_LOOKUP_KEY_FAILED)
	{
		err = INI_SetData(section, orikey, valstr);
		if(err != ERR_OK)
		{
			return err;
		}
	}
	err = INI_SetData(section, key, value);

	return err;
}


/**
 * @brief 设置枚举值
 * @param[in] section 节指针，key 键名指针，value 键值,len 枚举长度
 * @note 如果匹配查找成功后，会将键值进行修改，如果value为NULL则删除这一行包括注释，如果匹配查找失败后会增加一行
 * @return 返回ini解析错误码
 */
ini_err_t INI_SetEnum(const int8_t *section, const int8_t *key, int32_t *value,int32_t len)
{
	int32_t nIndex = 0;
	int32_t nCnt = 0;
	int32_t nLen = 0;
	int8_t valstr[256] = {0};
	int32_t tempstr[16] = {0};
	int8_t orikey[64] = {0};
	ini_err_t err = ERR_OK;

	sprintf(orikey,"%s%s","ori-",key);
	err = INI_GetString(section, orikey,valstr,sizeof(valstr));

	if(err == ERR_LOOKUP_KEY_FAILED)
	{
		memset(valstr,0,sizeof(valstr));
		err = INI_GetString(section, key, valstr, sizeof(valstr));
		if(ERR_OK != err)
		{
			return err;
		}

		err = INI_SetData(section, orikey, valstr);
		if(err != ERR_OK)
		{
			return err;
		}
	}

	memset(valstr,0,sizeof(valstr));

	for(nIndex = 0; nIndex < len; nIndex++)	
	{
		memset(tempstr,0,sizeof(tempstr));
		sprintf(tempstr,"%d",value[nIndex]);
		strcat(valstr,tempstr);

		nLen = strlen(valstr);
		if(nLen > 240)
		{
			err = ERR_ARRAY_BOUND;
			break;
		}

		if(nIndex != (len-1))
		{
			strcat(valstr,",");
		}
		else
		{
			strcat(valstr,"\0");
		}
	}

	if(err != ERR_OK)
	{
		return err;
	}

	err = INI_SetData(section, key, valstr);

	return err;

}


/**
* @brief 恢复字符串的原始初值 
* @param[in] section 节指针，key 键名指针
* @note 将键名对应的键值恢复
* @return 返回ini解析错误码
*/
ini_err_t INI_ResetString(const int8_t *section,const int8_t *key)
{
	int32_t nLen = 0;
	int8_t valstr[256] = {0};
	int8_t orikey[64] = {0};
	ini_err_t err = ERR_OK;

	nLen = strlen(key);
	if(nLen > 60)
	{
		return ERR_ARRAY_BOUND;
	}

	sprintf(orikey,"%s%s","ori-",key);

	if(ERR_OK != (err= INI_GetString(section, orikey, valstr, sizeof(valstr))))
	{
		return err;
	}

	err = INI_SetData(section,key,valstr);

	if(ERR_OK != err)
	{
		return err;
	}

	err = INI_SetData(section,orikey,NULL);
	
	return err;
}

/**
* @brief 恢复整形的原始初值 
* @param[in] section 节指针，key 键名指针
* @note 将键名对应的键值恢复
* @return 返回ini解析错误码
*/
ini_err_t INI_ResetInt(const int8_t *section,const int8_t *key)
{
	int8_t orikey[64] = {0};
	int32_t nValue = 0;
	ini_err_t err = ERR_OK;

	int32_t nLen = 0;
	nLen = strlen(key);
	if(nLen > 60)
	{
		return ERR_ARRAY_BOUND;
	}

	sprintf(orikey,"%s%s","ori-",key);

	if(ERR_OK != (err= INI_GetInt(section, orikey, &nValue)))
	{
		return err;
	}

	err = INI_SetFloat(section,key,nValue);

	if(ERR_OK != err)
	{
		return err;
	}

	err = INI_SetData(section,orikey,NULL);
	
	return err;
}

/**
* @brief 恢复浮点数的原始初值 
* @param[in] section 节指针，key 键名指针
* @note 将键名对应的键值恢复
* @return 返回ini解析错误码
*/
ini_err_t INI_ResetFloat(const int8_t *section,const int8_t *key)
{
	int8_t orikey[64] = {0};
	float fValue = 0.0;
	ini_err_t err = ERR_OK;

	int32_t nLen = 0;
	nLen = strlen(key);
	if(nLen > 60)
	{
		return ERR_ARRAY_BOUND;
	}

	sprintf(orikey,"%s%s","ori-",key);

	if(ERR_OK != (err= INI_GetFloat(section, orikey, &fValue)))
	{
		return err;
	}

	err = INI_SetFloat(section,key,fValue);

	if(ERR_OK != err)
	{
		return err;
	}

	err = INI_SetData(section,orikey,NULL);
	
	return err;
}

/**
* @brief 恢复浮点数的原始初值 
* @param[in] section 节指针，key 键名指针
* @note 将键名对应的键值恢复
* @return 返回ini解析错误码
*/
ini_err_t INI_ResetEnum(const int8_t *section,const int8_t *key)
{
	int8_t orikey[64] = {0};
	int32_t value[16] = {0};
	int32_t nCnt = 0;
	ini_err_t err = ERR_OK;

	int32_t nLen = 0;
	nLen = strlen(key);
	if(nLen > 60)
	{
		return ERR_ARRAY_BOUND;
	}

	sprintf(orikey,"%s%s","ori-",key);

	if(ERR_OK != (err= INI_GetEnum(section, orikey, value, &nCnt, sizeof(value))))
	{
		return err;
	}

	err = INI_SetEnum(section,key,value,nCnt);

	if(ERR_OK != err)
	{
		return err;
	}

	err = INI_SetData(section,orikey,NULL);
	
	return err;
}


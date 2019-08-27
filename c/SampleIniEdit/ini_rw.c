#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ini_rw.h"

#define SIZE_LINE		1024	//ÿ����󳤶�
#define SIZE_FILENAME		256		//�ļ�����󳤶�
#define min(x, y)		(x <= y) ? x : y

#define CTRL_LF			0x0A			//�س�ASCII��
#define	CTRL_CR			0x0D			//����ASCII��

/**
 * @brief ini �н�����ö��  
 */
typedef enum _ELineType_ {
	LINE_IDLE,		/**< δ������ */
	LINE_ERROR,		/**< ������ */
	LINE_EMPTY,		/**< �հ��л�ע���� */
	LINE_SECTION,	/**< �ڶ����� */
	LINE_VALUE		/**< ֵ������ */
} ELineType ;
	
static int8_t gFilename[SIZE_FILENAME];   /* �ļ������� */
static int8_t *gcBuffer;                   /* ��ȡ�ļ�����ָ�� */
static int32_t gnBuflen;			/* ��ȡ�ļ����� */ 

static int32_t gnRstStrFlag;		/* �ַ�����λ��� */
static int32_t gnRstFloatFlag;        /* ��������λ��� */
static int32_t gnRstIntFlag;          /* ���θ�λ��� */

/**
 * @brief ȥ����β�ո�
 * @param[in] s ��������ַ���ָ��
 * @note ���ַ����еĿո�ȥ�������޸Ĵ�����ַ�������
 * @retval �����޸ĺ���ַ���ָ��
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
 * @brief ȥ���ַ����еĿո� 
 * @param[in] s ��������ַ���ָ��
 * @note ���ַ����еĿո�ȥ�������޸Ĵ�����ַ�������
 * @retval �����޸ĺ���ַ���ָ��
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
 * @brief �Ƚ��ַ��� 
 * @param[in] s1 �Ƚϵĵ�һ���ַ�����s2�Ƚϵĵڶ����ַ���
 * @note �����ִ�Сд���бȽ�
 * @retval ����0��ʾ�����ַ�����ͬ����Ϊ0��ʾ�ַ�������ͬ
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
 * @brief ��ȡһ��ini�ַ���
 * @param[in] buf ini�ļ�����ָ��
 * @param[out] content ��Ч���ݴ���rem1 ע���ף�rem2 ע��β��nextline ��һ���� 
 * @note ע�� 
 * @retval �����м����
 */
static int32_t INI_GetLine(int8_t *buf, int32_t buflen, int8_t *content, int8_t **rem1, int8_t **rem2, int8_t **nextline)
{
	int8_t *cont1, *cont2;
	int32_t cntblank, cntCR, cntLF;		//�����ո񡢻��з�����
	int8_t isQuot1, isQuot2;			//����
	int32_t i;
	int8_t *p;
	//���ȶ��ж�ע�ͣ�֧�����»��з���\r \n
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
		//2��CR��LF���н���
		if (cntCR == 2 || cntLF == 2) 
		{
			p--;	//����1
			break;
		}
		//CR��LF��1��֮�������ַ����н���
		if (cntCR + cntLF >= 2) 
		{
			break;
		}
		//CR��LF֮����������ַ����н���
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

	//����ͷβ���޿ո�
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
 * @brief ���ҽ��Ƿ����
 * @param[in] section �����ҵĽ�����ָ��
 * @param[out] sect1 ��������ָ�룬sect2 ������β��cont1 �������ף�cont2 ������β��nextsect ��һ���� 
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
				break;		//������һsection
			}
			content[strlen(content) - 1] = 0;	//ȥβ��]
			StrStrip(content + 1);		        //ȥ��β�ո�

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
 * @brief ��ȡ��ֵ 
 * @param[in] content ���ݴ�
 * @param[out] key ������value ��ֵ
 * @note content �ᱻ�޸ģ���һ���л�ȡ��ֵ
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
 * @brief �ͷ�ini�ļ���Դ 
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
 * @brief ����ini�ļ�
 * @param[in] filename ���ص�ini�ļ���
 * @note ÿ�μ����ļ���ʱ�����INI_FreeFile�ͷ���Դ
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
 * @brief ��ȡ��ֵ
 * @param[in] section ������key ������value ��ֵ��maxlen ��ֵ��󳤶ȣ�defvalue Ĭ����ֵ
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
				break;		//������һsection
			}

			content[strlen(content) - 1] = 0;			//ȥβ��]
			StrStrip(content + 1);						//ȥ��β�ո�
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
					break;		//��ֵ��Ϊ��Ч
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
	//δ���ּ�ֵ

	if(LINE_VALUE != found)
	{
		err = ERR_LOOKUP_KEY_FAILED;
	}

	return err;
}

/**
 * @brief ��ȡ�ַ���
 * @param[in] section ������key ������value ��ֵ��maxlen ��ֵ��󳤶ȣ�defvalue Ĭ����ֵ
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
	//ȥ��β�ո�

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
 * @brief ��ȡ��������
 * @param[in] section ������key ������defvalue Ĭ������
 * @note None
 * @retval ������������ 
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
 * @brief ��ȡ��������
 * @param[in] section ������key ������defvalue Ĭ������
 * @note None
 * @retval ���ظ������� 
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
 * @brief ��ȡö������
 * @param[in] section ������key ������value ��ֵ����������ָ��, maxlen��ֵ������󳤶�
 * @note None
 * @retval ����ini����������
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
 * @brief �����ַ���
 * @param[in] section ������key ������value �ַ�����ֵ
 * @note ���ƥ����ҳɹ��󣬻Ὣ��ֵ�����޸ģ����valueΪNULL��ɾ����һ�а���ע�ͣ����ƥ�����ʧ�ܺ������һ��
 * @retval ����ini����������
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
		//δ�ҵ���
		//value��Ч�򷵻�
		if (value == NULL)
		{	
			return ERR_SETVALUE_ILLEGAL;
		}

		//���ļ�β�����
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

	//�ҵ��ڣ�����ڲ���key
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
				//�ҵ�key
				file = fopen(gFilename, "wb");
				if (file == NULL) 
				{ 
					return ERR_OPENFILE_FAILED;
				}

				len = (int32_t)(p - gcBuffer);
				fwrite(gcBuffer, 1, len, file);					//д��key֮ǰ����
				if (value == NULL) 
				{
					//value��Ч��ɾ��
					len = (int32_t)(nextline - gcBuffer);			//������ͬע��һ��ɾ��
				} 
				else 
				{
					//value��Ч����д
					fprintf(file, "%s=%s", key, value);
					len = (int32_t)(rem1 - gcBuffer);			//����β��ԭע��!
				}

				fwrite(gcBuffer + len, 1, gnBuflen - len, file);	//д��key�����к�ע��֮�󲿷�
				fclose(file);
				INI_LoadFile(gFilename);
				
				cLookUpFlag = 1;
				break;
			}	
		}
		len -= (int32_t)(nextline - p);
		p = nextline;
	}

	//δ�ҵ�key
	//value��Ч�򷵻�
	if(0 == cLookUpFlag)
	{
		if (value == NULL) 
		{ 
			return ERR_SETVALUE_ILLEGAL;
		}

		//���ļ�β�����
		file = fopen(gFilename, "wb");
		if (file == NULL) 
		{ 
			return ERR_OPENFILE_FAILED;
		}

		len = (int32_t)(cont2 - gcBuffer);
		fwrite(gcBuffer, 1, len, file);					//д��key֮ǰ����
		fprintf(file, "%s=%s", key, value);
		fwrite(gcBuffer + len, 1, gnBuflen - len, file);	//д��key֮�󲿷�
		fclose(file);
		INI_LoadFile(gFilename);
	}

	return ERR_OK;
}

/**
 * @brief �����ַ���
 * @param[in] section ������key ������value �ַ�����ֵ��base��ʾ����10��ʾʮ���ƣ�16��ʾʮ�����ƣ�8��ʾ�˽���
 * @note ���ƥ����ҳɹ��󣬻Ὣ��ֵ�����޸ģ����valueΪNULL��ɾ����һ�а���ע�ͣ����ƥ�����ʧ�ܺ������һ��
 * @retval ���ش�����
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
 * @brief ���ø�����ֵ 
 * @param[in] section ��ָ�룬key ����ָ�룬value ��ֵ
 * @note ���ƥ����ҳɹ��󣬻Ὣ��ֵ�����޸ģ����valueΪNULL��ɾ����һ�а���ע�ͣ����ƥ�����ʧ�ܺ������һ��
 * @return ����ini����������
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
 * @brief �����ַ���
 * @param[in] section ������key ������value �ַ�����ֵ,len �ַ�������
 * @note ���ƥ����ҳɹ��󣬻Ὣ��ֵ�����޸ģ����valueΪNULL��ɾ����һ�а���ע�ͣ����ƥ�����ʧ�ܺ������һ��
 * @retval ����ini����������
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
 * @brief ����ö��ֵ
 * @param[in] section ��ָ�룬key ����ָ�룬value ��ֵ,len ö�ٳ���
 * @note ���ƥ����ҳɹ��󣬻Ὣ��ֵ�����޸ģ����valueΪNULL��ɾ����һ�а���ע�ͣ����ƥ�����ʧ�ܺ������һ��
 * @return ����ini����������
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
* @brief �ָ��ַ�����ԭʼ��ֵ 
* @param[in] section ��ָ�룬key ����ָ��
* @note ��������Ӧ�ļ�ֵ�ָ�
* @return ����ini����������
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
* @brief �ָ����ε�ԭʼ��ֵ 
* @param[in] section ��ָ�룬key ����ָ��
* @note ��������Ӧ�ļ�ֵ�ָ�
* @return ����ini����������
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
* @brief �ָ���������ԭʼ��ֵ 
* @param[in] section ��ָ�룬key ����ָ��
* @note ��������Ӧ�ļ�ֵ�ָ�
* @return ����ini����������
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
* @brief �ָ���������ԭʼ��ֵ 
* @param[in] section ��ָ�룬key ����ָ��
* @note ��������Ӧ�ļ�ֵ�ָ�
* @return ����ini����������
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


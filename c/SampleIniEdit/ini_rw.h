#ifndef _INI_RW_H_
#define _INI_RW_H_
#ifdef __cplusplus
extern "C" {
#endif


typedef 	int 		ini_err_t;          /* ini错误码数据类型定义 */

#define         FILE_MAX_LEN		2048        /**< 文件缓存最大长度 */


#define		ERR_OK			0	       /**< ini处理正常 */
#define		ERR_INVALID		1              /**< 参数值非法无效 */
#define		ERR_OPENFILE_FAILED	2	       /**< 打开ini文件失败 */
#define		ERR_MALLOC_FAILED	3	       /**< 内存分配失败 */
#define		ERR_FILENAME_TOOLONG    4	       /**< ini加载文件名过长 */
#define		ERR_SETVALUE_ILLEGAL	5	       /**< 设置键值非法 */
#define		ERR_LOOKUP_SECT_FAILED  6              /**< 查找节名失败 */
#define		ERR_LOOKUP_KEY_FAILED   7              /**< 查找键名失败 */
#define		ERR_ARRAY_BOUND		8	       /**< 数组越界 */
#define         ERR_OUT_FILE_LENGTH     9              /**< 超出文件长度 */


/**
 * @brief 加载ini文件
 * @param[in] filename 加载的ini文件名
 * @note 每次加载文件名时会调用INI_FreeFile释放资源
 * @retval None
 */
ini_err_t INI_LoadFile(const int8_t *filename);

/**
 * @brief 释放ini文件资源 
 * @param None
 * @note None
 * @retval None
 */
void INI_FreeFile();

/**
 * @brief 获取字符串
 * @param[in] section 节名，key 键名，value 键值，maxlen 键值最大长度
 * @note None
 * @retval None
 */
ini_err_t INI_GetString(const int8_t *section, const int8_t *key, int8_t *value, int32_t maxlen);

/**
 * @brief 获取整形数据
 * @param[in] section 节名，key 键名，value 键值，整形数据指针
 * @note None
 * @retval 返回整形数据 
 */
ini_err_t INI_GetInt(const int8_t *section, const int8_t *key, int32_t *value);

/**
 * @brief 获取浮点数据
 * @param[in] section 节名，key 键名，value 键值，浮点数据指针
 * @note None
 * @retval 返回ini解析错误码
 */
ini_err_t INI_GetFloat(const int8_t *section, const int8_t *key, float *value);

/**
 * @brief 获取枚举数据
 * @param[in] section 节名，key 键名，value 键值，整形数组指针
 * @note None
 * @retval 返回ini解析错误码
 */
ini_err_t INI_GetEnum(const int8_t *section, const int8_t *key, int32_t *value,int32_t *len,int32_t maxlen);

/**
 * @brief 设置字符串
 * @param[in] section 节名，key 键名，value 字符串数值,len 字符串长度
 * @note 如果匹配查找成功后，会将键值进行修改，如果value为NULL则删除这一行包括注释，如果匹配查找失败后会增加一行
 * @retval 返回
 */
ini_err_t INI_SetString(const int8_t *section, const int8_t *key, const int8_t *value, int32_t len);

/**
 * @brief 设置字符串
 * @param[in] section 节名，key 键名，value 字符串数值，base表示进制10表示十进制，16表示十六进制，8表示八进制
 * @note 如果匹配查找成功后，会将键值进行修改，如果value为NULL则删除这一行包括注释，如果匹配查找失败后会增加一行
 * @retval 返回错误码
 */
ini_err_t INI_SetInt(const int8_t *section, const int8_t *key, int32_t value, int32_t base);

/**
 * @brief 设置浮点数值 
 * @param[in] section 节指针，key 键名指针，value 键值
 * @note 如果匹配查找成功后，会将键值进行修改，如果value为NULL则删除这一行包括注释，如果匹配查找失败后会增加一行
 * @return 返回ini解析错误码
 */
ini_err_t INI_SetFloat(const int8_t *section, const int8_t *key, float value);

/**
 * @brief 设置枚举值
 * @param[in] section 节指针，key 键名指针，value 键值, len设置枚举数量
 * @note 如果匹配查找成功后，会将键值进行修改，如果value为NULL则删除这一行包括注释，如果匹配查找失败后会增加一行
 * @return 返回ini解析错误码
 */
ini_err_t INI_SetEnum(const int8_t *section, const int8_t *key, int32_t *value, int32_t len);

/**
* @brief 恢复原始初值 
* @param[in] section 节指针，key 键名指针
* @note 将键名对应的键值恢复
* @return 返回ini解析错误码
*/
ini_err_t INI_ResetString(const int8_t *section,const int8_t *key);

/**
* @brief 恢复整形的原始初值 
* @param[in] section 节指针，key 键名指针
* @note 将键名对应的键值恢复
* @return 返回ini解析错误码
*/
ini_err_t INI_ResetInt(const int8_t *section,const int8_t *key);

/**
* @brief 恢复浮点数的原始初值 
* @param[in] section 节指针，key 键名指针
* @note 将键名对应的键值恢复
* @return 返回ini解析错误码
*/
ini_err_t INI_ResetFloat(const int8_t *section,const int8_t *key);

/**
* @brief 恢复浮点数的原始初值 
* @param[in] section 节指针，key 键名指针
* @note 将键名对应的键值恢复
* @return 返回ini解析错误码
*/
ini_err_t INI_ResetEnum(const int8_t *section,const int8_t *key);


#ifdef __cplusplus
}
#endif
#endif


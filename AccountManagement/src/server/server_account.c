/**
 * @file server_account.c
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-10
 * @modified  ���� 2015-11-11 18:51:05 �й���׼ʱ��
 */

#ifdef __cplusplus
extern "C"
{
#endif

static struct {
	uint8_t grade;
	uint32_t id;
	char username[64];
} s_account_login;

#ifdef __cplusplus
}
#endif

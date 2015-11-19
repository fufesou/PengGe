/**
 * @file account_login.c
 * @brief  
 * @author cxl, <shuanglongchen@yeah.net>
 * @version 0.1
 * @date 2015-11-16
 * @modified  Fri 2015-11-20 00:14:16 (+0800)
 */

#include  <malloc.h>
#include  <stdint.h>
#include  <stdio.h>
#include  <string.h>
#include    "../../common/config_macros.h"
#include    "../../common/macros.h"
#include    "../../common/list.h"
#include    "../../common/utility_wrap.h"
#include    "../common/account.h"
#include    "../common/account_file.h"
#include    "account_login.h"

#ifdef __cplusplus
extern "C"
{
#endif



#ifdef __cplusplus
}
#endif


void am_login_add(
			struct list_head* node_head,
			const struct account_data_t* account,
			const void* data_verification,
			uint32_t len_verification)
{
    struct list_login_t* node_login = NULL;

    node_login = (struct list_login_t*)malloc(sizeof(struct list_login_t));
	cs_memcpy(&node_login->account_sock.account, sizeof(node_login->account_sock.account), account, sizeof(*account));

	node_login->account_sock.data_verification = malloc(len_verification) + 1;
    cs_memcpy(node_login->account_sock.data_verification, len_verification + 1, data_verification, len_verification + 1);
    node_login->account_sock.size_verification = len_verification + 1;

    list_add(&node_login->listnode, node_head);
}

int am_login_remove(
			struct list_head* node_head,
			uint32_t id_account,
			const void* data_verification,
			uint32_t len_verification)
{
	struct list_login_t* node_login = NULL;
	struct account_login_t* account_login = am_login_find(node_head, id_account);

	if (account_login == NULL) {
		return 1;
	}

	if (memcmp(account_login->data_verification, data_verification, len_verification) != 0) {
		return 2;
	}

    node_login = container_of(account_login, struct list_login_t, account_sock);

    list_del(&node_login->listnode);
	free(account_login->data_verification);
	free(node_login);

	return 0;
}

struct account_login_t* am_login_find(const struct list_head* node_head, uint32_t id_account)
{
    struct list_login_t* login_data = NULL;
    struct list_head* node_login = node_head->next;

    while (node_login != node_head) {
        login_data = container_of(node_login, struct list_login_t, listnode);
        if (login_data->account_sock.account.id == id_account) {
            return &login_data->account_sock;
        }
        node_login = node_login->next;
    }

	return NULL;
}

int am_login_exist(
			const struct list_head* node_head,
			const struct account_data_t* account,
			const void* data_verification,
			uint32_t len_verification)
{
    int login_status = 0;
    struct list_login_t* login_data = NULL;
    struct list_head* node_login = node_head->next;

    while (node_login != node_head) {
        login_data = container_of(node_login, struct list_login_t, listnode);
        if (login_data->account_sock.account.id == account->id) {
            login_status = 1;

            if (memcmp(login_data->account_sock.data_verification, data_verification, len_verification) == 0) {
                login_status = 2;
                return login_status;
            }
        }
        node_login = node_login->next;
    }

    return login_status;
}

int am_login_write(const struct list_head* node_head)
{
    struct list_login_t* login_data = NULL;
    struct list_head* node_login = node_head->next;

    while (node_login != node_head) {
        login_data = container_of(node_login, struct list_login_t, listnode);
        if (am_account_write(&login_data->account_sock.account) != 0) {
			return 1;
		}

        node_login = node_login->next;
    }

	return 0;
}

int am_login_tryadd(struct list_head* node_head, const struct account_data_t* account, const void* data_verification, uint32_t len_verification)
{
    int login_status = am_login_exist(node_head, account, data_verification, len_verification);
    if (login_status == 0) {
        am_login_add(node_head, account, data_verification, len_verification);
    }
    return login_status;
}

void am_login_clear(struct list_head* node_head)
{
    struct list_login_t* node_login = NULL;
    struct list_head* delnode = node_head->next;

    while (delnode != node_head) {
        node_login = container_of(delnode, struct list_login_t, listnode);
        free(node_login->account_sock.data_verification);
        free(node_login);
        delnode = delnode->next;
    }

    node_head->prev = node_head;
    node_head->next = node_head;
}

/*
 * Sample showing how to do SSH2 connect.
 *
 * The sample code has default values for host name, user name, password
 * and path to copy, but you can specify them on the command line like:
 *
 * "ssh2 host user password [-p|-i|-k]"
 */

#ifndef TESTSET_SSH2
#define TESTSET_SSH2

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#ifdef WIN32
#define close closesocket
#pragma comment (lib, "Ws2_32.lib")
#endif

enum
{
    SSH_TYPE_SSH,
    SSH_TYPE_SFTP,
};

typedef int(*ssh_data_callback)(void *param, const char *data, int data_len);

typedef int(*ssh_worker_callback)(void *param);

typedef struct _tag_my_ssh_param
{
    int type;       // 0-ssh,1-sftp
    int run;        // 0-no,1-running
    void *param;    // client socket
    void *param1;   // local listent port, local client port
    void *param2;   // remote port
    void *param3;
    void *session;
    void *channel;

    const char *server_addr;
    unsigned short server_port;
    const char *username;
    const char *password;

    ssh_data_callback data_cb;      // ssh���ݻص�
    ssh_worker_callback worker_cb;  // ssh���̻߳ص�

}my_ssh_param, *p_my_ssh_param;

/**
 *\fn        void ssh_thread_func(void *param)
 *\brief     ssh���߳�
 *\param[in] void * param p_my_ssh_param
 *\return    void 0�ɹ�,����ʧ��
 */
void ssh_thread_func(void *param);

/**
 *\fn        int ssh_send_data(my_ssh_param *param, const char *data, int len)
 *\brief     ssh��������
 *\param[in] my_ssh_param * param ssh��������
 *\param[in] const char * data Ҫ���͵�����
 *\param[in] int len Ҫ���͵����ݳ�
 *\return    int 0�ɹ�,����ʧ��
 */
int ssh_send_data(my_ssh_param *param, const char *data, int len);

/**
 *\fn        int ssh_recv_data(my_ssh_param *param, char *data, int max)
 *\brief     ssh��������
 *\param[in] my_ssh_param * param ssh��������
 *\param[in] char * data Ҫ���յ�����
 *\param[in] int max �������ݻ���������
 *\return    int ���յ����ݳ�
 */
int ssh_recv_data(my_ssh_param *param, char *data, int max);

/**
 *\fn           int ssh_set_size(my_ssh_param *param, int width, int height)
 *\brief        ������Ļ���
 *\param[in]    my_ssh_param * param ssh��������
 *\param[in]    int width ��Ļ��
 *\param[in]    int height ��Ļ��
 *\return       int 0�ɹ�,����ʧ��
 */
int ssh_set_size(my_ssh_param *param, int width, int height);

/**
 *\fn        int get_server_addr(const char *addr, int port, struct sockaddr_in *srv_addr)
 *\brief     �õ������ַ
 *\param[in] const char *addr ��������ַ
 *\param[in] int port �������˿�
 *\param[in] struct sockaddr_in *srv_addr ��������ַ�ṹ
 *\return    int 0�ɹ�,����ʧ��
 */
int get_server_addr(const char *addr, int port, struct sockaddr_in *srv_addr);

/**
 *\fn        void print_msg(my_ssh_param *param, const char *data, int len)
 *\brief     sftp��������
 *\param[in] my_ssh_param * param ssh��������
 *\param[in] const char * data ���յ�������
 *\param[in] int len ���յ������ݳ�
 *\return    void 0�ɹ�,����ʧ��
 */
int print_msg(my_ssh_param *param, const char *data, int len);

#endif

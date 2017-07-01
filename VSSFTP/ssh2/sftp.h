/*
 * Sample showing how to do SFTP write transfers.
 *
 * The sample code has default values for host name, user name, password
 * and path to copy, but you can specify them on the command line like:
 *
 * "sftp 192.168.0.1 user password sftp_write.c /tmp/secrets"
 */

#ifndef TESTSET_SFTP
#define TESTSET_SFTP

struct _tag_my_ssh_param;
typedef struct _tag_my_ssh_param my_ssh_param, *p_my_ssh_param;

/**
 *\fn        void sftp_thread_func(void *param)
 *\brief     ssh���߳�
 *\param[in] void *param p_my_ssh_param
 *\return    void 0�ɹ�,����ʧ��
 */
void sftp_thread_func(void *param);

/**
 *\fn        int sftp_make_dir(my_ssh_param *param, const char *dir)
 *\brief     ����Ŀ¼
 *\param[in] my_ssh_param * param sftp����
 *\param[in] const char * dir Ŀ¼
 *\return    int 0�ɹ�,����ʧ��
 */
int sftp_make_dir(my_ssh_param *param, const char *dir);

/**
 *\fn           int sftp_rm_dir(my_ssh_param *param, const char *dir)
 *\brief        ɾ��Ŀ¼
 *\param[in]    my_ssh_param * param sftp����
 *\param[in]    const char * dir Ŀ¼
 *\return       int 0�ɹ�,����ʧ��
 */
int sftp_rm_dir(my_ssh_param *param, const char *dir);

/**
 *\fn           int sftp_rm(my_ssh_param *param, const char *filename)
 *\brief        ɾ���ļ�
 *\param[in]    my_ssh_param * param sftp����
 *\param[in]    const char * filename �ļ���
 *\return       int 0�ɹ�,����ʧ��
 */
int sftp_rm(my_ssh_param *param, const char *filename);

/**
 *\fn        int sftp_get_dir(my_ssh_param *param, const char *path, char *dir, int dir_max_len)
 *\brief     �õ�Ŀ¼�ṹ
 *\param[in] my_ssh_param * param sftp����
 *\param[in] const char * path Զ��·��
 *\param[in] char * dir Ŀ¼�ṹ, �ļ���Ŀ¼����(1Byte),�ļ���Ŀ¼����(1Byte),�ļ���Ŀ¼��
 *\param[in] int dir_max_len Ŀ¼�ṹ��������
 *\return    int 0�ɹ�,����ʧ��
 */
int sftp_get_dir(my_ssh_param *param, const char *path, char *dir, int dir_max_len);

/**
 *\fn        int sftp_rename(my_ssh_param *param, const char *sourcefile, const char *destfile)
 *\brief     �������ļ�
 *\param[in] my_ssh_param * param sftp����
 *\param[in] const char * sourcefile Դ�ļ�
 *\param[in] const char * destfile Ŀ���ļ�
 *\return    int 0�ɹ�,����ʧ��
 */
int sftp_rename(my_ssh_param *param, const char *sourcefile, const char *destfile);

/**
 *\fn           int sftp_get_link_path(my_ssh_param *param, const char *link, char *path, int path_max_len)
 *\brief        �õ����ӵ�Ŀ��·��
 *\param[in]    my_ssh_param * param sftp����
 *\param[in]    const char * link �����ļ�·��
 *\param[out]   char * path Ŀ��·��
 *\param[in]    int path_max_len Ŀ��·����󳤶�
 *\return       int 0-�ļ�����,1-Ŀ¼����
 */
int sftp_get_link_path(my_ssh_param *param, const char *link, char *path, int path_max_len);

/**
 *\fn        int sftp_upload_file(my_ssh_param *param, const char *localFilename, 
                                  const char *remoteFilename)
 *\brief     �ϴ��ļ�
 *\param[in] my_ssh_param * param sftp����
 *\param[in] const char * localFilename �����ļ���
 *\param[in] const char * remoteFilename Զ���ļ���
 *\return    int 0�ɹ�,����ʧ��
 */
int sftp_upload_file(my_ssh_param *param, const char *localFilename, const char *remoteFilename);

/**
 *\fn        int sftp_download_file(my_ssh_param *param, const char *localFilename, 
                                    const char *remoteFilename)
 *\brief     �����ļ�
 *\param[in] my_ssh_param * param sftp����
 *\param[in] const char * localFilename �����ļ���
 *\param[in] const char * remoteFilename Զ���ļ���
 *\return    int 0�ɹ�,����ʧ��
 */
int sftp_download_file(my_ssh_param *param, const char *localFilename, const char *remoteFilename);

#endif

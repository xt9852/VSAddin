// Package.h

#pragma once

#include <atlstr.h>
#include <VSLCommandTarget.h>
#include <time.h>
#include "resource.h"       // main symbols
#include "Guids.h"
#include "..\VSSFTPUI\Resource.h"
#include "..\VSSFTPUI\CommandIds.h"
#include "src\SSHConfig.h"
#include "src\VSProcess.h"

extern "C"
{
#include "ssh2\ssh2.h"
#include "ssh2\sftp.h"
#include "ssh2\zmodem.h"
};

using namespace std;
using namespace VSL;

#define SSHINIT 666
#define CAPTION "VSSFTP"

typedef map<string, my_ssh_param>  MAP_STR_SSH;

time_t          g_last = 0;
char            g_buff[1024*1024];
bool            init_ = false;
Config          conf_;
VSProcess       proc_;
MAP_STR_SSH     ssh_;
MAP_STR_SERVER  *server_;
MAP_STR_PROJECT *project_;

static CRITICAL_SECTION g_cs;

extern "C"
{
    int outputCallback(void *, const char *data, int)
    {
        proc_.outputString(CAPTION, data);
        return 0;
    }
}

int send_cmd(p_my_ssh_param p, const char *cmd, int sleep, const char *local_filename, const char *remote_filename)
{
    EnterCriticalSection(&g_cs);

    int ret = ssh_send_data(p, cmd, strlen(cmd));

    if (ret < 0)
    {
        sprintf_s(g_buff, "%s error ret:%d\n", cmd, ret);
        proc_.outputString(CAPTION, g_buff);
        LeaveCriticalSection(&g_cs);
        return ret;
    }

    ssh_send_data(p, "\r", 1);
    Sleep(sleep);
    ret = ssh_recv_data(p, g_buff, sizeof(g_buff) - 1);

    int t1;
    int t2;
    int len;

    if (0 == strncmp(cmd, "rz", 2))
    {
        char *ptr = strchr(g_buff, '*');

        if (NULL != ptr)
        {
            strcpy_s(ptr, 10240, "\n");
            proc_.outputString(CAPTION, g_buff);

            t1 = time(NULL);

            ret = file_put(local_filename, remote_filename, &len);

            t2 = time(NULL);

            t1 = (t1 == t2) ? 1 : (t2 - t1);

            sprintf_s(g_buff, "Starting zmodem transfer.Press Ctrl + C to cancel.\n"
                "Transferring %s\n"
                "  100%%\t%d B\t%d B/s\t%d Second\t%d Errors\n\n",
                remote_filename, 
                len,
                len / t1,
                t1,
                ret);

            proc_.outputString(CAPTION, g_buff);

            ssh_recv_data(p, g_buff, sizeof(g_buff) - 1);
        }
    }
    else if (0 == strncmp(cmd, "sz", 2))
    {
        char *ptr = strchr(g_buff, '*');

        if (NULL != ptr)
        {
            strcpy_s(ptr, 10240, "");

            if (0 != strncmp(g_buff, cmd, strlen(cmd)) && (ptr = strstr(g_buff, "\r\n")) != NULL)
            {
                char *ptr1 = ptr + 3;
                strcpy_s(ptr, 10240, ptr1);
            }

            proc_.outputString(CAPTION, g_buff);

            t1 = time(NULL);

            ret = file_get(local_filename, &len);

            t2 = time(NULL);

            t1 = (t1 == t2) ? 1 : (t2 - t1);

            sprintf_s(g_buff, "Starting zmodem transfer.Press Ctrl + C to cancel.\n"
                "Transferring %s\n"
                "  100%%\t%d B\t%d B/s\t%d Second\t%d Errors\n\n",
                local_filename,
                len,
                len / t1,
                t1,
                ret);

            proc_.outputString(CAPTION, g_buff);

            ssh_recv_data(p, g_buff, sizeof(g_buff) - 1);
        }
    }

    g_last = time(NULL);
    proc_.outputString(CAPTION, g_buff);
    LeaveCriticalSection(&g_cs);
    return ret;
}

int sshCallback(void *param)
{
    g_last = time(NULL);
    VCT_CMD::iterator cmd_iter;
    p_my_ssh_param p = (p_my_ssh_param)param;
    MAP_STR_SERVER::iterator server_iter = server_->find(p->server_addr);

    for (cmd_iter = server_iter->second.cmd.begin(); cmd_iter != server_iter->second.cmd.end(); ++cmd_iter)
    {
        send_cmd(p, cmd_iter->cmd.c_str(), cmd_iter->sleep, NULL, NULL);
    }

    p->param3 = (void*)SSHINIT;

    while (p->run)
    {
        if ((time(NULL) - g_last) > 60)
        {
            EnterCriticalSection(&g_cs);

            if (ssh_send_data(p, "\r", 1) > 0)
            {
                Sleep(100);
                ssh_recv_data(p, g_buff, sizeof(g_buff) - 1);
            }

            g_last = time(NULL);
            LeaveCriticalSection(&g_cs);
        }

        Sleep(100);
    }

    proc_.outputString(CAPTION, "ssh exit\n");
    return 0;
}

class ATL_NO_VTABLE CVSSFTPPackage :
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<CVSSFTPPackage, &CLSID_VSSFTP>,
    public IVsPackageImpl<CVSSFTPPackage, &CLSID_VSSFTP>,
    public IOleCommandTargetImpl<CVSSFTPPackage>,
    public ATL::ISupportErrorInfoImpl<&__uuidof(IVsPackage)>
{
public:
    BEGIN_COM_MAP(CVSSFTPPackage)
        COM_INTERFACE_ENTRY(IVsPackage)
        COM_INTERFACE_ENTRY(IOleCommandTarget)
        COM_INTERFACE_ENTRY(ISupportErrorInfo)
    END_COM_MAP()

    VSL_DECLARE_NOT_COPYABLE(CVSSFTPPackage)

public:
    CVSSFTPPackage()
    {
    }

    ~CVSSFTPPackage()
    {
    }

    static const LoadUILibrary::ExtendedErrorInfo& GetLoadUILibraryErrorInfo()
    {
        static LoadUILibrary::ExtendedErrorInfo errorInfo(IDS_E_BADINSTALL);
        return errorInfo;
    }

    static HRESULT WINAPI UpdateRegistry(BOOL /*bRegister*/)
    {
        return S_OK;
    }

    VSL_BEGIN_COMMAND_MAP()
        VSL_COMMAND_MAP_ENTRY(CLSID_VSSFTPCmdSet, cmdidsftpupload, NULL, CommandHandler::ExecHandler(&OnUpload))
        VSL_COMMAND_MAP_ENTRY(CLSID_VSSFTPCmdSet, cmdidsftpdownload, NULL, CommandHandler::ExecHandler(&OnDownload))
    VSL_END_VSCOMMAND_MAP()

    int init()
    {
        if (init_)
        {
            return 0;
        }

        int count = 0;

        // 停止线程
        for (MAP_STR_SSH::iterator iter = ssh_.begin(); iter != ssh_.end(); ++iter)
        {
            if (iter->second.run)
            {
                iter->second.run = FALSE; // 停止线程
                count++;
            }
        }

        if (count > 0)
        {
            Sleep(1000);
        }

        // 加载DTE
        _DTE *dte = NULL;
        this->GetVsSiteCache().QueryService(SID_SDTE, &dte);
        proc_.init(dte);

        // 加载配置文件
        string path = proc_.getSolutionPath() + "\\ssh.xml";
        if (0 != conf_.openFile(path.c_str()))
        {
            path += "加载失败,请选中文件后再操作.\n";
            proc_.statusbarString(path.c_str());
            proc_.outputString(CAPTION, path.c_str());
            return -1;
        }

        // 清空数据
        ssh_.clear();
        server_ = conf_.getServerInfo();
        project_ = conf_.getProjectInfo();

        // 设置配置参数
        for (MAP_STR_SERVER::iterator iter = server_->begin(); iter != server_->end(); ++iter)
        {
            my_ssh_param param;
            memset(&param, 0, sizeof(param));
            param.run = FALSE;
            param.type = iter->second.type; // SSH_TYPE_SSH,SSH_TYPE_SFTP
            param.server_addr = iter->second.addr.c_str();
            param.server_port = (unsigned short)iter->second.port;
            param.username = iter->second.username.c_str();
            param.password = iter->second.password.c_str();
            param.data_cb = outputCallback;
            param.worker_cb = sshCallback;
            ssh_[iter->first] = param;
        }

        init_ = true;
        path += "加载成功.\n";
        proc_.outputString(CAPTION, path.c_str());
        InitializeCriticalSection(&g_cs);
        return 0;
    }

    my_ssh_param* run_worker(const char *projectName)
    {
        MAP_STR_PROJECT::iterator project_iter = project_->find(projectName);

        if (project_->end() == project_iter)
        {
            return NULL;
        }

        MAP_STR_SSH::iterator ssh_iter = ssh_.find(project_iter->second.server);

        if (ssh_.end() == ssh_iter)
        {
            return NULL;
        }

        if (!ssh_iter->second.run)
        {
            _beginthread((SSH_TYPE_SSH == ssh_iter->second.type) ? ssh_thread_func : sftp_thread_func, 0, &(ssh_iter->second));
        }

        while (ssh_iter->second.param3 != (void*)SSHINIT)
        {
            Sleep(100);
        }

        zmodem_set(&(ssh_iter->second), (SEND_CALLBACK)ssh_send_data, (RECV_CALLBACK)ssh_recv_data);

        return (&ssh_iter->second);
    }

    void OnUpload(CommandHandler* /*pSender*/, DWORD /*flags*/, VARIANT* /*pIn*/, VARIANT* /*pOut*/)
    {
        if (0 != init())
        {
            return;
        }

        char info[512];
        string projectName = proc_.getCurProjectName();
        ProjectInfo *project = conf_.getProjectInfo(projectName.c_str());

        if (NULL == project)
        {
            sprintf_s(info, "配置文件中没有找到项目(%s)\n", projectName.c_str());
            proc_.statusbarString(info);
            proc_.outputString(CAPTION, info);
            return;
        }

        my_ssh_param *ssh = run_worker(projectName.c_str());

        if (NULL == ssh)
        {
            sprintf_s(info, "没有找到项目(%s)对应的服务器\n", projectName.c_str());
            proc_.statusbarString(info);
            proc_.outputString(CAPTION, info);
            return;
        }

        int ok = 0;
        MAP_STR_STR files;
        proc_.getSelectFile(files);

        if (SSH_TYPE_SSH == ssh->type)
        {
            for (MAP_STR_STR::iterator iter = files.begin(); iter != files.end(); ++iter)
            {
                sprintf_s(info, "%s%s", project->remotePath.c_str(), iter->second.c_str());
                ok += (0 == send_cmd(ssh, "rz -y", 100, iter->first.c_str(), info));
            }
        }
        else
        {
            for (MAP_STR_STR::iterator iter = files.begin(); iter != files.end(); ++iter)
            {
                sprintf_s(info, "%s%s", project->remotePath.c_str(), iter->second.c_str());
                ok += (0 == sftp_upload_file(ssh, iter->first.c_str(), info));
            }
        }

        sprintf_s(info, "上传文件(%s:%d)完成(%d/%d)", ssh->server_addr, ssh->server_port, ok, files.size());
        proc_.statusbarString(info);
    }

    void OnDownload(CommandHandler* /*pSender*/, DWORD /*flags*/, VARIANT* /*pIn*/, VARIANT* /*pOut*/)
    {
        if (0 != init())
        {
            return;
        }

        char info[512];
        string projectName = proc_.getCurProjectName();
        ProjectInfo *project = conf_.getProjectInfo(projectName.c_str());

        if (NULL == project)
        {
            sprintf_s(info, "配置文件中没有找到项目(%s)\n", projectName.c_str());
            proc_.statusbarString(info);
            proc_.outputString(CAPTION, info);
            return;
        }

        my_ssh_param *ssh = run_worker(projectName.c_str());

        if (NULL == ssh)
        {
            sprintf_s(info, "没有找到项目(%s)对应的服务器\n", projectName.c_str());
            proc_.statusbarString(info);
            proc_.outputString(CAPTION, info);
            return;
        }

        int ok = 0;
        MAP_STR_STR files;
        proc_.getSelectFile(files);

        if (SSH_TYPE_SSH == ssh->type)
        {
            for (MAP_STR_STR::iterator iter = files.begin(); iter != files.end(); ++iter)
            {
                sprintf_s(info, "sz %s%s", project->remotePath.c_str(), iter->second.c_str());
                ok += (0 == send_cmd(ssh, info, 100, iter->first.c_str(), NULL));
            }
        }
        else
        {
            for (MAP_STR_STR::iterator iter = files.begin(); iter != files.end(); ++iter)
            {
                sprintf_s(info, "%s%s", project->remotePath.c_str(), iter->second.c_str());
                ok += (0 == sftp_download_file(ssh, iter->first.c_str(), info));
            }
        }

        sprintf_s(info, "下载文件(%s:%d)完成(%d/%d)", ssh->server_addr, ssh->server_port, ok, files.size());
        proc_.statusbarString(info);
    }
};

OBJECT_ENTRY_AUTO(CLSID_VSSFTP, CVSSFTPPackage)

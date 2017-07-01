#include "stdafx.h"
#include "SSHConfig.h"
#include "XTXml.h"

Config::Config()
{
}

Config::~Config()
{
}

/**
 *\fn        int openFile(const char *filename)
 *\brief     打开配置文件
 *\param[in] const char * filename 配置文件名
 *\return    int 0成功,其它失败
 */
int Config::openFile(const char *filename)
{
    if (NULL == filename)
    {
        return -1;
    }

    XT_XML::xml file;
    XT_XML::TiXmlElementExs sshs;
    XT_XML::TiXmlElementExs servers;
    XT_XML::TiXmlElementExs projects;

    if (!file.LoadFile(filename))
    {
        return -2;
    }

    serverInfo_.clear();
    projectInfo_.clear();

    servers = file.GetRoot()->SelectNodes("server");
    projects = file.GetRoot()->SelectNodes("project");

    for (int i = 0; i < servers.getCount(); i++)
    {
        ServerInfo info;
        info.addr = servers[i]->Attribute("addr");
        info.username = servers[i]->Attribute("username");
        info.password = servers[i]->Attribute("password");
        info.port = atoi(servers[i]->Attribute("port"));
        info.type = (0 == strcmp(servers[i]->Attribute("type"), "ssh")) ? 0 : 1; // SSH_TYPE_SSH:SSH_TYPE_SFTP

        sshs = servers[i]->SelectNodes("ssh");

        for (int j = 0; j < sshs.getCount(); j++)
        {
            CmdInfo cmd;
            cmd.cmd = sshs[j]->Attribute("cmd");
            cmd.sleep = atoi(sshs[j]->Attribute("sleep"));
            info.cmd.push_back(cmd);
        }

        serverInfo_[info.addr] = info;
    }

    for (int i = 0; i < projects.getCount(); i++)
    {
        ProjectInfo info;
        info.name = projects[i]->Attribute("name");
        info.server = projects[i]->Attribute("server");
        info.remotePath = projects[i]->Attribute("remote_path");

        if (info.remotePath[info.remotePath.length() - 1] != '/')
        {
            info.remotePath += '/';
        }

        projectInfo_[info.name] = info;
    }

    file.Clear();

    return ((projectInfo_.size() > 0 && serverInfo_.size() > 0) ? 0 : -4);
}

/**
 *\fn        ServerInfo* getServerInfo(const char *server)
 *\brief     得到服务器参数
 *\param[in] const char *server 服务器地址
 *\return    ProjectInfo* 服务器参数
 */
ServerInfo* Config::getServerInfo(const char *server)
{
    if (NULL == server)
    {
        return NULL;
    }

    return &(serverInfo_[server]);
}

/**
 *\fn        ProjectInfo* getProjectInfo(const char *projectname)
 *\brief     得到项目参数
 *\param[in] const char *projectname
 *\return    ProjectInfo* 项目参数
 */
ProjectInfo* Config::getProjectInfo(const char *projectname)
{
    if (NULL == projectname)
    {
        return NULL;
    }

    return &(projectInfo_[projectname]);
}

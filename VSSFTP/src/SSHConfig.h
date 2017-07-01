
#pragma once
#include <map>
#include <vector>
#include <string>

typedef struct _tagCmdInfo
{
    int sleep;
    std::string cmd;

}CmdInfo;

typedef std::vector<CmdInfo> VCT_CMD;

typedef struct _tagProjectInfo
{
    std::string name;
    std::string server;
    std::string remotePath;

}ProjectInfo;

typedef struct _tagServerInfo
{
    int port;
    int type;
    std::string addr;
    std::string username;
    std::string password;
    VCT_CMD cmd;

}ServerInfo;


typedef std::map<std::string, ServerInfo> MAP_STR_SERVER;
typedef std::map<std::string, ProjectInfo> MAP_STR_PROJECT;


class Config
{
public:
    Config();
    ~Config();

protected:
    MAP_STR_SERVER serverInfo_;     // 服务器信息
    MAP_STR_PROJECT projectInfo_;   // 项目信息

public:
    /**
     *\fn        int openFile(const char *filename)
     *\brief     打开配置文件
     *\param[in] const char * filename 配置文件名
     *\return    int 0成功,其它失败
     */
    int openFile(const char *filename);

    /**
     *\fn        MAP_INT_SERVER* getServerInfo()
     *\brief     得到服务器信息
     *\param[in] 无
     *\return    MAP_INT_SERVER* 服务器信息
     */
    MAP_STR_SERVER* getServerInfo() { return &serverInfo_; }

    /**
     *\fn        MAP_STR_PROJECT* getProjectInfo()
     *\brief     得到项目信息
     *\param[in] 无
     *\return    MAP_STR_PROJECT* 项目信息
     */
    MAP_STR_PROJECT* getProjectInfo() { return &projectInfo_; }

    /**
     *\fn        ServerInfo* getServerInfo(const char *server)
     *\brief     得到服务器参数
     *\param[in] const char *server 服务器地址
     *\return    ProjectInfo* 服务器参数
     */
    ServerInfo* getServerInfo(const char *server);

    /**
     *\fn        ProjectInfo* getProjectInfo(const char *projectname)
     *\brief     得到项目参数
     *\param[in] const char *projectname 项目名
     *\return    ProjectInfo* 项目参数
     */
    ProjectInfo* getProjectInfo(const char *projectname);
};


#pragma once
#include <map>
#include <list>
#include <string>
#include <VSLCommandTarget.h>
#import "libid:1CBA492E-7263-47BB-87FE-639000619B15" version("8.0") lcid("0") raw_interfaces_only named_guids

using namespace std;
using namespace VxDTE;
using namespace Microsoft_VisualStudio_CommandBars;


typedef list<string>            LIST_STR;
typedef map<string, string>     MAP_STR_STR;

class VSProcess
{
public:
    VSProcess();
    ~VSProcess();

protected:
    bool                        init_;
    CComPtr<Events>             events_;
    CComPtr<Windows>            windows_;
    CComPtr<Commands>           commands_;
    CComPtr<Projects>           projects_;
    CComPtr<_Solution>          solution_;
    CComPtr<Documents>          documents_;
    CComPtr<StatusBar>          statusbar_;
    CComPtr<_CommandBars>       commandbars_;
    CComPtr<SelectedItems>      selecteditems_;
    CComPtr<ItemOperations>     itemoperations_;
    CComPtr<_DocumentEvents>    documentevents_;
    CComPtr<OutputWindow>       outputwindow_;
    CComPtr<OutputWindowPanes>  outputwindowpanes_;

public:
    int init(_DTE* pDTE);

    std::string BSTR2A(BSTR bstr);

    std::string getSolutionPath();

    std::string getCurProjectName();
    std::string getCurProjectPath();

    std::string getPropertyName(Property *property);
    std::string getPropertyValue(Property *property);

    std::string getProjectItemPropertyValue(ProjectItem *projectItem, const char *name);

    void getSelectFile(MAP_STR_STR &files);

    HRESULT getCurProject(Project **project);

    HRESULT getWindow(const char *name, VxDTE::Window **window);
    HRESULT getCommand(const char *name, VxDTE::Command **command);
    HRESULT addCommand(const char *name, VxDTE::Command **command);
    HRESULT getCommandBar(const char *name, CommandBar **CommandBar);
    HRESULT addCommandBar(const char *name, CommandBar *parent, int pos, CommandBar **commandBar);

    HRESULT outputString(const char *caption, const char *text);
    HRESULT statusbarString(const char *text);

    HRESULT getSelectedItem(unsigned int id, SelectedItem **selectedItem);

    void    getProjectItemProperty(ProjectItem *projectItem);
    HRESULT getProjectItemProperty(ProjectItem *projectItem, unsigned int id, Property **property);
    HRESULT getProjectItemProperty(ProjectItem *projectItem, const char *name, Property **property);

};

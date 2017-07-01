
#include "stdafx.h"
#include "VSProcess.h"


VSProcess::VSProcess()
    :init_(false)
{
}


VSProcess::~VSProcess()
{
}

int VSProcess::init(_DTE* dte)
{
    if (NULL == dte)
    {
        return -1;
    }

    if (init_)
    {
        return 0;
    }

    init_ = true;

    dte->get_Events(&events_);
    dte->get_Windows(&windows_);
    dte->get_Commands(&commands_);
    dte->get_Solution((Solution**)&solution_);
    dte->get_Documents(&documents_);
    dte->get_StatusBar(&statusbar_);
    dte->get_SelectedItems(&selecteditems_);
    dte->get_ItemOperations(&itemoperations_);

    solution_->get_Projects(&projects_);
    events_->get_DocumentEvents(NULL, (DocumentEvents**)&documentevents_);

    CComPtr<IDispatch> disp;
    dte->get_CommandBars(&disp);
    commandbars_ = disp;

    disp = NULL;
    CComPtr<VxDTE::Window> window;
    getWindow(VxDTE::vsWindowKindOutput, &window);
    window->get_Object(&disp);
    outputwindow_ = disp;
    outputwindow_->get_OutputWindowPanes(&outputwindowpanes_);

    return 0;
}

std::string VSProcess::getSolutionPath()
{
    BSTR name;
    solution_->get_FullName(&name);

    string path = this->BSTR2A(name);
    path.erase(path.rfind("\\"), 1000);
    return path;
}

std::string VSProcess::BSTR2A(BSTR bstr)
{
    std::string str;

    int res = WideCharToMultiByte(CP_ACP, 0, bstr, -1, NULL, 0, NULL, NULL);

    if (res > 0)
    {
        char *buf = new char[res + 10];
        WideCharToMultiByte(CP_ACP, 0, bstr, -1, buf, res, NULL, NULL);
        str = buf;
        delete buf;
    }

    return str;
}

HRESULT VSProcess::getWindow(const char *name, VxDTE::Window **window)
{
    if (NULL == name || NULL == window)
    {
        return E_INVALIDARG;
    }

    return windows_->Item(CComVariant(name), window);
}

HRESULT VSProcess::getCommand(const char *name, VxDTE::Command **command)
{
    if (NULL == name || NULL == command)
    {
        return E_INVALIDARG;
    }

    return commands_->Item(CComVariant(name), 1, command);
}

HRESULT VSProcess::addCommand(const char *name, VxDTE::Command **command)
{
    if (NULL == name || NULL == command)
    {
        return E_INVALIDARG;
    }

    return commands_->AddNamedCommand(NULL,
        CComBSTR(name),
        CComBSTR("ButtonText"),
        CComBSTR("pszTooltip"),
        VARIANT_TRUE,
        1,
        NULL,
        vsCommandStatusSupported + vsCommandStatusEnabled,
        command);
}

HRESULT VSProcess::getCommandBar(const char *name, CommandBar **commandBar)
{
    if (NULL == name || NULL == commandBar)
    {
        return E_INVALIDARG;
    }

    return commandbars_->get_Item(CComVariant(name), commandBar);
}

HRESULT VSProcess::addCommandBar(const char *name, CommandBar *parent, int pos, CommandBar **commandBar)
{
    if (NULL == name || NULL == parent || NULL == commandBar)
    {
        return E_INVALIDARG;
    }

    return commands_->AddCommandBar(CComBSTR(name),
        vsCommandBarTypeMenu,
        parent,
        pos,
        (IDispatch**)commandBar);
}

HRESULT VSProcess::outputString(const char *caption, const char *text)
{
    if (NULL == caption || NULL == text)
    {
        return E_INVALIDARG;
    }

    CComPtr<OutputWindowPane> pane;
    HRESULT hr = outputwindowpanes_->Item(CComVariant(caption), &pane);

    if (NULL == pane)
    {
        hr = outputwindowpanes_->Add(CComBSTR(caption), &pane);
    }

    return pane->OutputString(CComBSTR(text));
}

HRESULT VSProcess::statusbarString(const char *text)
{
    if (NULL == text)
    {
        return E_INVALIDARG;
    }

    return statusbar_->put_Text(CComBSTR(text));
}

HRESULT VSProcess::getSelectedItem(unsigned int id, SelectedItem **selectedItem)
{
    if (NULL == selectedItem)
    {
        return E_INVALIDARG;
    }

    unsigned long count = 0;
    selecteditems_->get_Count((long*)&count);

    if (id > count)
    {
        return E_INVALIDARG;
    }

    VARIANT index;
    VariantInit(&index);
    index.vt = VT_I4;
    index.lVal = id;

    return selecteditems_->Item(index, selectedItem);
}

HRESULT VSProcess::getProjectItemProperty(ProjectItem *projectItem, const char *name, Property **property)
{
    if (NULL == projectItem || NULL == name || NULL == property)
    {
        return E_INVALIDARG;
    }

    CComPtr<Properties> properties;
    projectItem->get_Properties(&properties);

    if (NULL == properties)
    {
        return E_INVALIDARG;
    }

    return properties->Item(CComVariant(name), property);
}

HRESULT VSProcess::getProjectItemProperty(ProjectItem *projectItem, unsigned int id, Property **property)
{
    if (NULL == projectItem || NULL == property)
    {
        return E_INVALIDARG;
    }

    CComPtr<Properties> properties;
    HRESULT hr = projectItem->get_Properties(&properties);

    if (NULL == properties)
    {
        return E_INVALIDARG;
    }

    unsigned long count = 0;
    hr = properties->get_Count((long*)&count);

    if (id > count)
    {
        return E_INVALIDARG;
    }

    VARIANT index;
    VariantInit(&index);
    index.vt = VT_I4; // 指明整型数据
    index.lVal = id;  // 赋值

    return properties->Item(index, property);
}

void VSProcess::getProjectItemProperty(ProjectItem *projectItem)
{
    if (NULL == projectItem)
    {
        return;
    }

    for (int j = 1; true; j++)
    {
        CComPtr<Property> property;
        HRESULT hr = this->getProjectItemProperty(projectItem, j, &property);

        if (FAILED(hr))
        {
            break;
        }

        string name = this->getPropertyName(property);
        string value = this->getPropertyValue(property);

        this->outputString("我的", name.c_str());
        this->outputString("我的", "\n");
        this->outputString("我的", value.c_str());
        this->outputString("我的", "\n");
    }
}

std::string VSProcess::getPropertyName(Property *property)
{
    if (NULL == property)
    {
        return "";
    }

    BSTR name;
    property->get_Name(&name);

    return this->BSTR2A(name);
}

std::string VSProcess::getPropertyValue(Property *property)
{
    if (NULL == property)
    {
        return "";
    }

    VARIANT index;
    VariantInit(&index);
    property->get_Value(&index);

    return ((index.vt == VT_BSTR) ? this->BSTR2A(index.bstrVal) : "");
}

std::string VSProcess::getProjectItemPropertyValue(ProjectItem *projectItem, const char *name)
{
    if (NULL == projectItem || NULL == name)
    {
        return "";
    }

    CComPtr<Property> property;
    HRESULT hr = this->getProjectItemProperty(projectItem, name, &property);

    return  ((SUCCEEDED(hr) && property != NULL) ? this->getPropertyValue(property) : "");
}

void VSProcess::getSelectFile(MAP_STR_STR &files)
{
    std::string fullPath;     // 绝对路径
    std::string relativePath; // 相对路径

    for (int i = 1; true; i++)
    {
        CComPtr<SelectedItem> selectItem;
        HRESULT hr = this->getSelectedItem(i, &selectItem);

        if (selectItem == NULL)
        {
            break;
        }

        CComPtr<ProjectItem> projectItem;
        hr = selectItem->get_ProjectItem(&projectItem);
        fullPath = this->getProjectItemPropertyValue(projectItem, "FullPath");
        relativePath = this->getProjectItemPropertyValue(projectItem, "RelativePath");

        for (size_t j = 0; j < relativePath.length(); j++)
        {
            if (relativePath[j] == '\\')
            {
                relativePath[j] = '/';
            }
        }

        files[fullPath] = relativePath;
    }
}

HRESULT VSProcess::getCurProject(Project **project)
{
    if (NULL == project)
    {
        return E_INVALIDARG;
    }

    CComPtr<SelectedItem> selectItem;
    HRESULT hr = this->getSelectedItem(1, &selectItem);

    if (NULL == selectItem)
    {
        return hr;
    }

    CComPtr<ProjectItem> item;
    hr = selectItem->get_ProjectItem(&item);

    if (NULL == item)
    {
        return hr;
    }

    return item->get_ContainingProject(project);
}

std::string VSProcess::getCurProjectName()
{
    CComPtr<Project> project;
    this->getCurProject(&project);

    if (NULL == project)
    {
        return "";
    }

    BSTR name;
    project->get_Name(&name);
    return BSTR2A(name);
}

std::string VSProcess::getCurProjectPath()
{
    CComPtr<Project> project;
    this->getCurProject(&project);

    if (NULL == project)
    {
        return "";
    }

    BSTR name;
    std::string path;
    project->get_FullName(&name);
    path = BSTR2A(name);
    path.erase(path.rfind("\\"), 1000);
    return path;
}



#include <iostream>
#include <string.h>
#include <string>
#include <tchar.h>
#include <Windows.h>
#include <regex>

using namespace std;

void recDeleteSubkey(wstring, wstring);
void regReadValuesInKey(wstring, regex);

void printMenu()
{
    printf("\n1 - choose key;\n2 - change value of key;\n3 - read value;\n4 - delete value;\n5 - create subkey;\n6 - delete subkey;\n7 - read all values in key\n8 - read subkeys of key\n9 - recursive output\n10 - search with regular expression\n0 - exit\n");
}

string wstr2str(const wstring& ws) {
    string curLocale = setlocale(LC_ALL, NULL);
    setlocale(LC_ALL, "chs");

    const wchar_t* pws = ws.c_str();
    int wsLen = ws.length();
    int bufLen = sizeof(wchar_t) * wsLen + 1;
    char* pRes = new char[bufLen];
    memset(pRes, '\0', bufLen);
    size_t convertLen = 0;
    wcstombs_s(&convertLen, pRes, bufLen, pws, _TRUNCATE);
    string resStr = pRes;
    delete[] pRes;

    setlocale(LC_ALL, curLocale.c_str());

    return resStr;
}


wchar_t* MyStrcat(const wchar_t* str1, const wchar_t* str2)
{
    int num1 = 0;
    int num2 = 0;

    const wchar_t* temp1;
    const wchar_t* temp2;

    temp1 = str1;
    temp2 = str2;

    while (*str1 != '\0') {
        num1++;
        str1++;
    }
    while (*str2 != '\0') {
        num2++;
        str2++;
    }
    wchar_t* str;
    str = (wchar_t*)calloc(num1 + num2 + 1, sizeof(wchar_t));

    str[num1 + num2] = '\0';
    int i = 0;
    while (*temp1 != '\0') {
        str[i] = *temp1;
        temp1++;
        i++;
    }
    while (*temp2 != '\0') {
        str[i] = *temp2;
        temp2++;
        i++;
    }
    return str;
}

wstring chooseKey()
{

    string key;
    wstring wkey;

    printf("\nEnter name of key in format Prekols\\");
    printf("\\");
    printf("Your key \n");
    getline(cin, key);
    getline(cin, key);
    wkey = wstring(key.begin(), key.end());

    return wkey;
}

void setValue(wstring wsubkey)
{
    HKEY hKey;
    string nameOfValue;
    string value;
    wstring wNameOfValue;
    wstring wvalue;

    printf("\nEnter name of value\n");
    getline(cin, nameOfValue);
    getline(cin, nameOfValue);
    printf("\nEnter value\n");
    getline(cin, value);

    wNameOfValue = wstring(nameOfValue.begin(), nameOfValue.end());
    wvalue = wstring(value.begin(), value.end());

    if (RegOpenKeyEx(HKEY_CURRENT_USER, wsubkey.c_str(), 0, KEY_ALL_ACCESS, &hKey) != ERROR_SUCCESS)
        cout << "\nError opening the desired subkey (doesn't exist?).\n";
    else
    {
        if (RegSetValueEx(hKey, wNameOfValue.c_str(), NULL, REG_SZ, (LPBYTE)wvalue.c_str(), _tcslen(wvalue.c_str()) * sizeof(TCHAR)) == ERROR_SUCCESS)
            cout << "\nThe value of the key was set successfully.\n";
        else
            cout << "\nError setting the value of the key.\n";
    }
    RegCloseKey(hKey);
}

void readValue(wstring wsubkey)
{
    HKEY hKey;
    string nameOfValue;
    wstring wNameOfValue;

    printf("\nEnter name of value\n");
    getline(cin, nameOfValue);
    getline(cin, nameOfValue);

    wNameOfValue = wstring(nameOfValue.begin(), nameOfValue.end());

    LONG error = RegOpenKeyEx(HKEY_CURRENT_USER, wsubkey.c_str(), NULL, KEY_WOW64_64KEY | KEY_QUERY_VALUE, &hKey);
    if (error == ERROR_SUCCESS)
    {
        unsigned char buffer[MAX_PATH];
        DWORD bufferSize = MAX_PATH;
        LONG result = RegQueryValueEx(hKey, wNameOfValue.c_str(), NULL, NULL, buffer, &bufferSize);
        if (result == ERROR_SUCCESS)
        {
            wprintf(L"Value is %s\n\n", buffer);
        }
        else
        {
            cout << "Error: " << result << "\n\n";
        }
    }
    RegCloseKey(hKey);
}

void createSubKey()
{
    HKEY hKey;
    DWORD dwDisposition;

    string subkey;
    wstring wsubkey;

    printf("\nEnter FULL path to new subkey in format Prekols\\");
    printf("\\");
    printf("NewSubkey \n");
    getline(cin, subkey);
    getline(cin, subkey);
    wsubkey = wstring(subkey.begin(), subkey.end());

    RegCreateKeyExW(HKEY_CURRENT_USER, wsubkey.c_str(), 0, NULL, 0, 0, NULL, &hKey, &dwDisposition);
    if (dwDisposition == REG_CREATED_NEW_KEY && dwDisposition != REG_OPENED_EXISTING_KEY)
        cout << "\nCreated new subkey.\n";
    if (dwDisposition != REG_CREATED_NEW_KEY && dwDisposition == REG_OPENED_EXISTING_KEY)
        cout << "\nOpened old subkey.\n";
    cout << dwDisposition << "\n";
    RegCloseKey(hKey);
}

void deleteValue(wstring wsubkey)
{
    HKEY hKey;
    string nameOfValue;
    wstring wNameOfValue;

    printf("\nEnter name of value\n");
    getline(cin, nameOfValue);
    getline(cin, nameOfValue);

    wNameOfValue = wstring(nameOfValue.begin(), nameOfValue.end());

    if (RegOpenKeyEx(HKEY_CURRENT_USER, wsubkey.c_str(), 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
    {
        if (RegDeleteValueW(hKey, wNameOfValue.c_str()) == ERROR_SUCCESS)
            cout << "\nValue successfully removed.\n";
        else
            cout << "\nError removing the specified value (permissions?).\n";
    }
    else
        cout << "\nError opening the specified subkey path (doesn't exist?).\n";

    RegCloseKey(hKey);
}

void deleteSubkey()
{
    HKEY hKey;
    string path;
    wstring wpath;
    string subkey;
    wstring wsubkey;

    printf("\nEnter path in format Prekols\\");
    printf("\\");
    printf("Your path \n");
    getline(cin, path);
    getline(cin, path);
    printf("\nEnter subkey to delete\n");
    getline(cin, subkey);
    wpath = wstring(path.begin(), path.end());
    wsubkey = wstring(subkey.begin(), subkey.end());

    recDeleteSubkey(wpath, wsubkey);

}

void readValuesInKey(wstring wpath)
{
    HKEY hKey;
    DWORD index = 0;
    wchar_t ValueName[MAX_PATH];
    DWORD SizeValueName = MAX_PATH;
    DWORD Type;
    unsigned char Data[MAX_PATH];
    DWORD DataSize = MAX_PATH;
    unsigned long iData = 0;
    int result = 0;
    int intData = 0;

    if (RegOpenKeyEx(HKEY_CURRENT_USER, wpath.c_str(), 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
    {
        while (RegEnumValue(hKey, index, ValueName, &(SizeValueName = MAX_PATH), NULL, &Type, Data, &(DataSize = MAX_PATH)) != ERROR_NO_MORE_ITEMS)
        {
            index++;
            printf("________________________________________________________________________________\n");
            printf("%ws ||", ValueName);
            switch (Type)
            {
            case 1:
                printf("REG_SZ ");
                wprintf(L"|| %s", Data);
                break;
            case 2:
                printf("REG_EXPAND_SZ ");
                wprintf(L"|| %s", Data);
                break;
            case 3:
                printf("REG_BINARY ");
                wprintf(L"|| %d", Data);
                break;
            case 4:
                //RegEnumValue(hKey, index - 1, ValueName, &(SizeValueName = MAX_PATH), NULL, &Type, iData, &(DataSize = 4));
                printf("REG_DWORD ");
                //printf("|| %x", iData);
                wprintf(L"|| %d", Data);
                break;
            case 11:
                printf("REG_QWORD ");
                wprintf(L"|| %s", Data);
                break;
            case 7:
                printf("REG_MULTI_SZ ");
                wprintf(L"|| %s", Data);
                break;
            default:
                printf("Undefined type ");
                wprintf(L" Value %s \n", Data);
            }
            printf("\n");
        }
        printf("________________________________________________________________________________\n");
    }
    else
        cout << "\nError opening the specified subkey path (doesn't exist?).\n";
    printf("%d values in total\n", index);

    RegCloseKey(hKey);
}

void readKeysInKey(wstring wpath)
{
    HKEY hKey;
    DWORD index = 0;
    wchar_t KeyName[MAX_PATH];
    DWORD SizeKeyName = MAX_PATH;
    DWORD Type;
    wchar_t Class[MAX_PATH];
    DWORD ClassSize = MAX_PATH;
    int result = 0;
    int intData = 0;

    if (RegOpenKeyEx(HKEY_CURRENT_USER, wpath.c_str(), 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
    {
        while (RegEnumKeyExW(hKey, index, KeyName, &(SizeKeyName = MAX_PATH), NULL, Class, &(SizeKeyName = MAX_PATH), NULL) != ERROR_NO_MORE_ITEMS)
        {
            index++;
            printf("________________________________________________________________________________\n");
            printf("%ws \n", KeyName);
        }
        printf("________________________________________________________________________________\n");
    }
    else
        cout << "\nError opening the specified subkey path (doesn't exist?).\n";

    printf("%d keys in total\n", index);

    RegCloseKey(hKey);
}

void recReadValuesInKey(wstring wpath)
{
    HKEY hKey;
    DWORD index = 0;
    wchar_t ValueName[MAX_PATH];
    DWORD SizeValueName = MAX_PATH;
    DWORD Type;
    unsigned char Data[MAX_PATH];
    DWORD DataSize = MAX_PATH;
    unsigned long iData = 0;
    int result = 0;
    int intData = 0;

    printf(">\n");

    if (RegOpenKeyEx(HKEY_CURRENT_USER, wpath.c_str(), 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
    {
        while (RegEnumValue(hKey, index, ValueName, &(SizeValueName = MAX_PATH), NULL, &Type, Data, &(DataSize = MAX_PATH)) != ERROR_NO_MORE_ITEMS)
        {
            index++;
            printf("%ws ||", ValueName);
            switch (Type)
            {
            case 1:
                printf("REG_SZ ");
                wprintf(L"|| %s", Data);
                break;
            case 2:
                printf("REG_EXPAND_SZ ");
                wprintf(L"|| %s", Data);
                break;
            case 3:
                printf("REG_BINARY ");
                wprintf(L"|| %d", Data);
                break;
            case 4:
                //RegEnumValue(hKey, index - 1, ValueName, &(SizeValueName = MAX_PATH), NULL, &Type, iData, &(DataSize = 4));
                printf("REG_DWORD ");
                //printf("|| %x", iData);
                wprintf(L"|| %d", Data);
                break;
            case 11:
                printf("REG_QWORD ");
                wprintf(L"|| %s", Data);
                break;
            case 7:
                printf("REG_MULTI_SZ ");
                wprintf(L"|| %s", Data);
                break;
            default:
                printf("Undefined type ");
                wprintf(L" Value %s \n", Data);
            }
            printf("\n");
        }
        printf("\n");
    }
    else
        cout << "\nError opening the specified subkey path (doesn't exist?).\n";

    RegCloseKey(hKey);
}

void recursiveOutput(wstring wpath)
{
    HKEY hKey;
    DWORD index = 0;
    wchar_t KeyName[MAX_PATH];
    DWORD SizeKeyName = MAX_PATH;
    DWORD Type;
    wchar_t Class[MAX_PATH];
    DWORD ClassSize = MAX_PATH;
    int result = 0;
    int intData = 0;
    int flag = 1;

    if (wpath == L"")
    {
        flag = 0;
    }

    if (RegOpenKeyEx(HKEY_CURRENT_USER, wpath.c_str(), 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
    {
        recReadValuesInKey(wpath);
        while (RegEnumKeyExW(hKey, index, KeyName, &(SizeKeyName = MAX_PATH), NULL, Class, &(SizeKeyName = MAX_PATH), NULL) != ERROR_NO_MORE_ITEMS)
        {            
            index++;
            int length1 = 0, length2 = 0;         

            if (flag)
            {
                wpath = MyStrcat(wpath.c_str(), L"\\");
                wpath = MyStrcat(wpath.c_str(), L"\\");
            }           
            wpath = MyStrcat(wpath.c_str(), KeyName);

            while (KeyName[length1] != '\0')
                length1++;
            while (wpath[length2] != '\0')
                length2++;

            wprintf(L"%s\n", wpath.c_str());
            recursiveOutput(wpath);
            if (flag)
                wpath[length2 - length1 - 2] = '\0';
            else
                wpath[length2 - length1] = '\0';
            
        }
    }
    else
        cout << "\nError opening the specified subkey path (doesn't exist?).\n";

    RegCloseKey(hKey);
}

void recDeleteSubkey(wstring wpath, wstring wkey)
{
    HKEY hKey = 0;
    string path;    

    DWORD index = 0;
    wchar_t KeyName[MAX_PATH];
    DWORD SizeKeyName = MAX_PATH;
    DWORD Type;
    wchar_t Class[MAX_PATH];
    DWORD ClassSize = MAX_PATH;
    int result = 0;
    int intData = 0;
    int flag = 1;

    /*if (wpath == L"")
    {
        printf("You shouldn't delete whole registry");
        return;
    }*/

    if (RegOpenKeyEx(HKEY_CURRENT_USER, wpath.c_str(), 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
    {
        if (RegDeleteKey(hKey, wkey.c_str()) == ERROR_SUCCESS)
        {
            wprintf(L"SubKey %s\\\\%s successfully removed\n", wpath.c_str(), wkey.c_str());
            return;
        }            
        while (RegEnumKeyExW(hKey, index, KeyName, &(SizeKeyName = MAX_PATH), NULL, Class, &(SizeKeyName = MAX_PATH), NULL) != ERROR_NO_MORE_ITEMS)
        {
            index++;
            int length1 = 0, length2 = 0;

            if (flag)
            {
                wpath = MyStrcat(wpath.c_str(), L"\\");
                wpath = MyStrcat(wpath.c_str(), L"\\");
            }
            wpath = MyStrcat(wpath.c_str(), KeyName);

            while (KeyName[length1] != '\0')
                length1++;
            while (wpath[length2] != '\0')
                length2++;

            if (RegDeleteKey(hKey, KeyName) == ERROR_SUCCESS)
            {
                wprintf(L"SubKey %s successfully removed\n", wpath.c_str());
            }
            else
                index--;
            
            recDeleteSubkey(wpath, KeyName);            

            if (flag)
                wpath[length2 - length1 - 2] = '\0';
            else
                wpath[length2 - length1] = '\0';
        }             
    }

    RegCloseKey(hKey);
}

void regSearch(wstring wpath, regex regular)
{
    HKEY hKey;
    DWORD index = 0;
    wchar_t KeyName[MAX_PATH];
    DWORD SizeKeyName = MAX_PATH;
    DWORD Type;
    wchar_t Class[MAX_PATH];
    DWORD ClassSize = MAX_PATH;
    int result = 0;
    int intData = 0;
    int flag = 1;

    wstring wstr;
    string str;
    cmatch res;

    if (wpath == L"")
    {
        flag = 0;
    }

    if (RegOpenKeyEx(HKEY_CURRENT_USER, wpath.c_str(), 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
    {
        regReadValuesInKey(wpath, regular);
        while (RegEnumKeyExW(hKey, index, KeyName, &(SizeKeyName = MAX_PATH), NULL, Class, &(SizeKeyName = MAX_PATH), NULL) != ERROR_NO_MORE_ITEMS)
        {
            index++;
            int length1 = 0, length2 = 0;

            if (flag)
            {
                wpath = MyStrcat(wpath.c_str(), L"\\");
                wpath = MyStrcat(wpath.c_str(), L"\\");
            }
            wpath = MyStrcat(wpath.c_str(), KeyName);

            while (KeyName[length1] != '\0')
                length1++;
            while (wpath[length2] != '\0')
                length2++;

            wstr = KeyName;
            str = wstr2str(wstr);

            if (regex_search(str.c_str(), res, regular))
            {
                cout << str << endl;
                wprintf(L"\nPath: %s", wpath.c_str());
            }                            
            regSearch(wpath, regular);
            if (flag)
                wpath[length2 - length1 - 2] = '\0';
            else
                wpath[length2 - length1] = '\0';

        }
    }
    else
        cout << "\nError opening the specified subkey path (doesn't exist?).\n";

    RegCloseKey(hKey);
}

void regReadValuesInKey(wstring wpath, regex regular)
{
    HKEY hKey;
    DWORD index = 0;
    wchar_t ValueName[MAX_PATH];
    DWORD SizeValueName = MAX_PATH;
    DWORD Type;
    unsigned char Data[MAX_PATH];
    DWORD DataSize = MAX_PATH;
    unsigned long iData = 0;
    int result = 0;
    int intData = 0;

    wstring wstr;
    string str;
    cmatch res;

    if (RegOpenKeyEx(HKEY_CURRENT_USER, wpath.c_str(), 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
    {
        while (RegEnumValue(hKey, index, ValueName, &(SizeValueName = MAX_PATH), NULL, &Type, Data, &(DataSize = MAX_PATH)) != ERROR_NO_MORE_ITEMS)
        {
            index++;
            wstr = ValueName;
            str = wstr2str(wstr);
            if (regex_search(str.c_str(), res, regular))
            {                
                wprintf(L"Path: %s\n", wpath.c_str());
                printf("Value: %ws ||", ValueName);

                switch (Type)
                {
                case 1:
                    printf("REG_SZ ");
                    wprintf(L"|| %s", Data);
                    break;
                case 2:
                    printf("REG_EXPAND_SZ ");
                    wprintf(L"|| %s", Data);
                    break;
                case 3:
                    printf("REG_BINARY ");
                    wprintf(L"|| %d", Data);
                    break;
                case 4:
                    //RegEnumValue(hKey, index - 1, ValueName, &(SizeValueName = MAX_PATH), NULL, &Type, iData, &(DataSize = 4));
                    printf("REG_DWORD ");
                    //printf("|| %x", iData);
                    wprintf(L"|| %d", Data);
                    break;
                case 11:
                    printf("REG_QWORD ");
                    wprintf(L"|| %s", Data);
                    break;
                case 7:
                    printf("REG_MULTI_SZ ");
                    wprintf(L"|| %s", Data);
                    break;
                default:
                    printf("Undefined type ");
                    wprintf(L" Value %s \n", Data);
                }
                printf("\n");
            }
        }
        printf("\n");
    }
    else
        cout << "\nError opening the specified subkey path (doesn't exist?).\n";

    RegCloseKey(hKey);
}


int main()
{
    int choice; 
    wstring path = L"Prekols";
    string expression;
    regex regular("(.)");
    
    while (1)
    {
        printf("\nCurrent key: %ws \n", path.c_str());

        do
        {
            printf("Choose operation: ");
            printMenu();
        } while (!(cin >> choice));        

        switch (choice)
        {
        case 1:
            path = chooseKey();
            break;
        case 2:
            setValue(path);
            break;
        case 3:
            readValue(path);
            break;
        case 4:
            deleteValue(path);
            break;
        case 5:
            createSubKey();
            break;
        case 6: 
            deleteSubkey();
            break;
        case 7:
            readValuesInKey(path);
            break;
        case 8:
            readKeysInKey(path);
            break;
        case 9:
            recursiveOutput(path);
            break;
        case 10:
            cout << "Enter regular expression" << endl;
            getline(cin, expression);
            getline(cin, expression);
            regular.operator=(expression);
            regSearch(path, regular);
            break;
        case 0:
            return 0;
        default:
            continue;
        }
    }
}


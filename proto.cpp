﻿#include <iostream>
#include <string.h>
#include <string>
#include <tchar.h>
#include <Windows.h>

using namespace std;

void printMenu()
{
    printf("\n1 - choose key;\n2 - change value of key;\n3 - read value;\n4 - delete value;\n5 - create subkey;\n6 - delete subkey;\n7 - read all values in key\n8 - read subkeys of key\n0 - exit\n");
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
            wprintf(L"Value is %s\n", buffer);
        }
        else
        {
            cout << "Error: " << result << "\n";
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

    printf("\nEnter name of subkey in format Prekols\\");
    printf("\\");
    printf("Your subkey \n");
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
    printf("\nEnter name of subkey\n");
    getline(cin, subkey);
    wsubkey = wstring(subkey.begin(), subkey.end());
    wpath = wstring(path.begin(), path.end());

    if (RegOpenKeyEx(HKEY_CURRENT_USER, wpath.c_str(), 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
    {
        if (RegDeleteKey(hKey, wsubkey.c_str()) == ERROR_SUCCESS)
            cout << "\nSubKey successfully removed.\n";
        else
            cout << "\nError removing the specified key (permissions?).\n";
    }
    else
        cout << "\nError opening the specified subkey path (doesn't exist?).\n";

    RegCloseKey(hKey);
}

void readValuesInKey(wstring wpath)
{
    HKEY hKey;
    DWORD index = 1;
    wchar_t ValueName[MAX_PATH];
    DWORD SizeValueName = MAX_PATH;
    DWORD Type;
    unsigned char Data[MAX_PATH];
    DWORD DataSize = MAX_PATH;
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
                printf("REG_DWORD ");                
                intData = Data[0] - '0';
                wprintf(L"|| %s", Data);
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

    RegCloseKey(hKey);
}

int main()
{
    int choice; 
    wstring path = L"Prekols";
    
    while (1)
    {
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
        case 0:
            return 0;
        default:
            continue;
        }
    }
}

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#define IMPORTBYNAME(a,i) do{extern void*a##_;a##_=(void*)GetProcAddress(BaseAddressGenuine,#a);}while(0)
#define IMPORTBYORD(a,i) do{extern void*a##_;a##_=(void*)GetProcAddress(BaseAddressGenuine,(char*)i);}while(0)
EXTERN_C int(*__imp_printf)(const char*fmt,...);
EXTERN_C int(*__imp_wprintf)(const wchar_t*fmt,...);

#include <ntddk.h>

#define DEVICE_SEND CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_WRITE_DATA)
//#define DEVICE_REC  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_READ_DATA)
//--------------------------------------------------------------------------------
UNICODE_STRING DeviceName = RTL_CONSTANT_STRING(L"\\Device\\keMode_killer");
UNICODE_STRING SymLinkName = RTL_CONSTANT_STRING(L"\\??\\keMode_killer_link");
//--------------------------------------------------------------------------------
#define InitializeObjectAttributes( p, n, a, r, s ) {   \
    (p)->Length = sizeof( OBJECT_ATTRIBUTES );          \
    (p)->RootDirectory = r;                             \
    (p)->Attributes = a;                                \
    (p)->ObjectName = n;                                \
    (p)->SecurityDescriptor = s;                        \
    (p)->SecurityQualityOfService = NULL;               \
}              
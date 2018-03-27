#include "Header.h"
//--------------------------------------------------------------------------------
PDEVICE_OBJECT DeviceObject = NULL;
//--------------------------------------------------------------------------------
VOID Unload(PDRIVER_OBJECT DriverObject)
{
	IoDeleteSymbolicLink(&SymLinkName);
	IoDeleteDevice(DeviceObject);
	DbgPrint("Unload :(\n");
}
//--------------------------------------------------------------------------------
NTSTATUS KillProcess(int pID)
{
	NTSTATUS status;
	HANDLE hProcess = NULL;

	OBJECT_ATTRIBUTES objAttrib;
	CLIENT_ID clientId;

	clientId.UniqueProcess = (HANDLE)pID;
	clientId.UniqueThread = NULL;

	InitializeObjectAttributes(&objAttrib, NULL, OBJ_KERNEL_HANDLE, NULL, NULL);

	status = ZwOpenProcess(&hProcess, PROCESS_ALL_ACCESS, &objAttrib, &clientId);

	if (!NT_SUCCESS(status))
	{
		DbgPrint("ZwOpenProcess error!\n");
		return status;
	}

	status = ZwTerminateProcess(hProcess, 0);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("ZwTerminateProcess error!\n");
		return status;
	}

	ZwClose(hProcess);
	return status;
}
//--------------------------------------------------------------------------------
NTSTATUS DispatchIRP(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	NTSTATUS status = STATUS_SUCCESS;

	PIO_STACK_LOCATION irpsp = IoGetCurrentIrpStackLocation(Irp);
	
	switch (irpsp->MajorFunction)
	{
	case IRP_MJ_CREATE:
		DbgPrint("Create request\n");
		break;
	case IRP_MJ_CLOSE:
		DbgPrint("Close request\n");
		break;
	default:
		status = STATUS_INVALID_PARAMETER;
		break;
	}

	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = status;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}
//--------------------------------------------------------------------------------
NTSTATUS DispatchDeviceCTL(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	PIO_STACK_LOCATION irpsp = IoGetCurrentIrpStackLocation(Irp);
	NTSTATUS status = STATUS_SUCCESS;

	ULONG returnLength = 0;
	PVOID buffer = Irp->AssociatedIrp.SystemBuffer;
	ULONG inLength = irpsp->Parameters.DeviceIoControl.InputBufferLength;
	ULONG outLength = irpsp->Parameters.DeviceIoControl.OutputBufferLength;

	switch (irpsp->Parameters.DeviceIoControl.IoControlCode)
	{
	// Recieve from client app
	case DEVICE_SEND:
		DbgPrint("Process ID : %d", *((int *)buffer));
		returnLength = 4;//(wcsnlen(buffer, 511) + 1) * 2;
		KillProcess(*((int *)buffer));
		break;
	// Sending to client
	/*case DEVICE_REC:
		wcsncpy(buffer, demo, 511);
		returnLength = (wcsnlen(buffer, 511) + 1) * 2;
		break;*/

	default:
		status = STATUS_INVALID_PARAMETER;
	}

	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}
//--------------------------------------------------------------------------------
NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	NTSTATUS status = STATUS_SUCCESS;

	DriverObject->DriverUnload = Unload;
	status = IoCreateDevice(DriverObject, 0, &DeviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &DeviceObject);

	if (!NT_SUCCESS(status))
	{
		DbgPrint("Creating device error!\n");
		return status;
	}

	status = IoCreateSymbolicLink(&SymLinkName, &DeviceName);
	if (!NT_SUCCESS(status))
	{
		DbgPrint("Creating symlink error!\n");
		return status;
	}

	for (int i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)
	{
		DriverObject->MajorFunction[i] = DispatchIRP;
	}

	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchDeviceCTL;

	DbgPrint("KeM0d3 killer loaded :)\n");
	return status;
}
//--------------------------------------------------------------------------------
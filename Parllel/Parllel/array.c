#include "array.h"

//��������ʼ���洢����
void CreatArray(int*** arry, int n)
{
	(*arry) = (int**)malloc(sizeof(int*) * n);
	if ((*arry) != NULL)
	{
		for (int i = 0; i < n; i++)
		{
			(*arry)[i] = (int*)malloc(sizeof(int) * n);
		}
		for (int i = 0; i < n; i++)
		{
			for (int j = 0; j < n; j++)
			{
				(*arry)[i][j] = 0;
			}
		}
	}
	else
	{
		printf("���鴴��ʧ��\n");
		exit();
	}
}

void SetArray(int** arry, int n)
{
	for (int i = 0; i < n; i++)
	{
		printf("��%d��: ", i + 1);
		for (int j = 0; j < n; j++)
		{
			scanf("%d", &arry[i][j]);
		}
	}
}

//�������
void AddArray(int** a, int** b, int** c, int n)
{
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			c[i][j] = a[i][j] + b[i][j];
		}
	}
}

//��ӡ��������
void PrintArray(int** arry, int n)
{
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			printf("%d  ", *(*(arry + i) + j));
		}
		printf("\n");
	}
	printf("\n");
}

//�ͷ�����
void FreeArray(int*** arry, int n)
{
	for (int i = 0; i < n; i++)
	{
		free((*arry)[i]);
	}
	free((*arry));
}


//ѡ�� OpenCL ƽ̨������һ��������
cl_context CreateContext()
{
	cl_int errNum;
	cl_uint numPlatforms;
	cl_platform_id firstPlatformId;
	cl_context context = NULL;

	//ѡ����õ�ƽ̨�еĵ�һ��
	errNum = clGetPlatformIDs(1, &firstPlatformId, &numPlatforms);
	if (errNum != CL_SUCCESS || numPlatforms <= 0)
	{
		printf("Failed to find any OpenCL platforms.");
		return NULL;
	}

	//����һ��OpenCL�����Ļ���
	cl_context_properties contextProperties[] =
	{
		CL_CONTEXT_PLATFORM,
		(cl_context_properties)firstPlatformId,
		0
	};
	context = clCreateContextFromType(contextProperties, CL_DEVICE_TYPE_GPU,
		NULL, NULL, &errNum);

	return context;
}

//�����豸�������������
cl_command_queue CreateCommandQueue(cl_context context, cl_device_id* device)
{
	cl_int errNum;
	cl_device_id* devices;
	cl_command_queue commandQueue = NULL;
	size_t deviceBufferSize = -1;

	// ��ȡ�豸��������С
	errNum = clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &deviceBufferSize);

	if (deviceBufferSize <= 0)
	{
		printf("No devices available.\n");
		return NULL;
	}

	// Ϊ�豸���仺��ռ�
	devices = (cl_device_id *)malloc((deviceBufferSize / sizeof(cl_device_id))*sizeof(cl_device_id));
	errNum = clGetContextInfo(context, CL_CONTEXT_DEVICES, deviceBufferSize, devices, NULL);

	//ѡȡ�����豸�еĵ�һ��
	commandQueue = clCreateCommandQueue(context, devices[0], 0, NULL);

	*device = devices[0];
	free(devices);
	return commandQueue;
}

//�����͹���������
cl_program CreateProgram(cl_context context, cl_device_id device, const char* fileName)
{
	cl_program program;
	FILE* program_handle;
	char* program_buffer, * program_log;
	size_t program_size, log_size;
	int err;
	//��ȡ�ļ�
	program_handle = fopen(fileName, "r");
	if (program_handle == NULL) {
		perror("Couldn't find the program file");
		exit(1);
	}
	fseek(program_handle, 0, SEEK_END);
	program_size = ftell(program_handle);
	rewind(program_handle);
	program_buffer = (char*)malloc(program_size + 1);
	program_buffer[program_size] = '\0';
	fread(program_buffer, sizeof(char), program_size, program_handle);
	fclose(program_handle);

	//���ļ�Դ�봴������
	program = clCreateProgramWithSource(context, 1,
		(const char**)&program_buffer, &program_size, &err);
	if (err < 0) {
		perror("Couldn't create the program");
		exit(1);
	}
	free(program_buffer);

	/* Build program */
	err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	if (err < 0) {

		/* Find size of log and print to std output */
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
		program_log = (char*)malloc(log_size + 1);
		program_log[log_size] = '\0';
		clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size + 1, program_log, NULL);
		printf("%s\n", program_log);
		free(program_log);
		exit(1);
	}

	return program;
}

//�����͹���������
bool CreateMemObjects(cl_context context, cl_mem memObjects[3], int* a, int* b)
{
	memObjects[0] = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
		sizeof(int) * ARRAY_SIZE, a, NULL);
	memObjects[1] = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
		sizeof(int) * ARRAY_SIZE, b, NULL);
	memObjects[2] = clCreateBuffer(context, CL_MEM_READ_WRITE,
		sizeof(int) * ARRAY_SIZE, NULL, NULL);
	return true;
}

//�ͷ� OpenCL ��Դ
void CleanUp(cl_context context, cl_command_queue commandqueue, cl_program program, cl_kernel kernel, cl_mem memObjects[3])
{
	for (int i = 0; i < 3; i++)
	{
		if (memObjects[i] != 0)
			clReleaseMemObject(memObjects[i]);
	}
	if (commandqueue != 0)
		clReleaseCommandQueue(commandqueue);

	if (kernel != 0)
		clReleaseKernel(kernel);

	if (program != 0)
		clReleaseProgram(program);

	if (context != 0)
		clReleaseContext(context);
}

//��ʾ�ڴ������Ϣ
void displayBufferDetails(cl_mem memobj) {
	cl_mem_object_type objT;
	cl_mem_flags flags;
	size_t memSize;

	clGetMemObjectInfo(memobj, CL_MEM_TYPE, sizeof(cl_mem_object_type), &objT, 0);
	clGetMemObjectInfo(memobj, CL_MEM_FLAGS, sizeof(cl_mem_flags), &flags, 0);
	clGetMemObjectInfo(memobj, CL_MEM_SIZE, sizeof(size_t), &memSize, 0);

	char* str = '\0';
	switch (objT) {
	case CL_MEM_OBJECT_BUFFER: str = "Buffer or Sub-buffer"; break;
	case CL_MEM_OBJECT_IMAGE2D: str = "2D Image Object"; break;
	case CL_MEM_OBJECT_IMAGE3D: str = "3D Image Object"; break;
	}

	char flagStr[128] = { '\0' };
	if (flags & CL_MEM_READ_WRITE)     strcat(flagStr, "Read-Write|");
	if (flags & CL_MEM_WRITE_ONLY)     strcat(flagStr, "Write Only|");
	if (flags & CL_MEM_READ_ONLY)      strcat(flagStr, "Read Only|");
	if (flags & CL_MEM_COPY_HOST_PTR)  strcat(flagStr, "Copy from Host|");
	if (flags & CL_MEM_USE_HOST_PTR)   strcat(flagStr, "Use from Host|");
	if (flags & CL_MEM_ALLOC_HOST_PTR) strcat(flagStr, "Alloc from Host|");

	printf("\tOpenCL Buffer's details =>\n\t size: %lu MB,\n\t object type is: %s,\n\t flags:0x%lx (%s) \n", memSize >> 20, str, flags, flagStr);
}

//  ���ڵ������Դ�룬��CL�ļ��ж�ȡ��buffer��
void loadProgramSource(const char** files, size_t length, char** buffer, size_t* sizes) {
	/* ��ȡÿ��Դ�ļ���.cl���������ݴ洢����ʱ���ݴ洢�� */
	for (size_t i = 0; i < length; i++) {
		FILE* file = fopen(files[i], "r");
		if (file == NULL) {
			perror("Couldn't read the program file");
			exit(1);
		}
		fseek(file, 0, SEEK_END);
		sizes[i] = ftell(file);
		rewind(file); // �����ļ�ָ�룬�Ա㡰fread����ǰ���ȡ
		buffer[i] = (char*)malloc(sizes[i] + 1);
		buffer[i][sizes[i]] = '\0';
		fread(buffer[i], sizeof(char), sizes[i], file);
		fclose(file);
	}
}

//��Դ�봴������
cl_program build_program(cl_context ctx, cl_device_id dev, const char* filename) {

	cl_program program;
	FILE* program_handle;
	char* program_buffer, * program_log;
	size_t program_size, log_size;
	int err;

	/* Read program file and place content into buffer */
	program_handle = fopen(filename, "r");
	if (program_handle == NULL) {
		perror("Couldn't find the program file");
		exit(1);
	}
	fseek(program_handle, 0, SEEK_END);
	program_size = ftell(program_handle);
	rewind(program_handle);
	program_buffer = (char*)malloc(program_size + 1);
	program_buffer[program_size] = '\0';
	fread(program_buffer, sizeof(char), program_size, program_handle);
	fclose(program_handle);

	/* Create program from file */
	program = clCreateProgramWithSource(ctx, 1,
		(const char**)&program_buffer, &program_size, &err);
	if (err < 0) {
		perror("Couldn't create the program");
		exit(1);
	}
	free(program_buffer);

	/* Build program */
	err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	if (err < 0) {

		/* Find size of log and print to std output */
		clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
		program_log = (char*)malloc(log_size + 1);
		program_log[log_size] = '\0';
		clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, log_size + 1, program_log, NULL);
		printf("%s\n", program_log);
		free(program_log);
		exit(1);
	}

	return program;
}
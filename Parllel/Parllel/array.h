#pragma once
#pragma warning(disable : 4996)
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <CL/cl.h>
#include <stdbool.h>
#define ARRAY_SIZE  10

//��������ʼ���洢����
void CreatArray(int*** arry, int n);

//��������
void SetArray(int** arry, int n);

//�������
void AddArray(int** a, int** b, int** c, int n);

//��ӡ��������
void PrintArray(int** arry, int n);

//�ͷ�����
void FreeArray(int*** arry, int n);

//ѡ�� OpenCL ƽ̨������һ��������
cl_context CreateContext();

//�����豸�������������
cl_command_queue CreateCommandQueue(cl_context context, cl_device_id* device);

//�����͹���������
cl_program CreateProgram(cl_context context, cl_device_id device, const char* fileName);

//�����͹���������
bool CreateMemObjects(cl_context context, cl_mem memObjects[3], int* a, int* b);

//�ͷ� OpenCL ��Դ
void CleanUp(cl_context context, cl_command_queue commandqueue, cl_program program, cl_kernel kernel, cl_mem memObjects[3]);

//��ʾ�ڴ������Ϣ
void displayBufferDetails(cl_mem memobj);

//  ���ڵ������Դ�룬��CL�ļ��ж�ȡ��buffer��
void loadProgramSource(const char** files, size_t length, char** buffer, size_t* sizes);

//��Դ�봴������
cl_program build_program(cl_context ctx, cl_device_id dev, const char* filename);





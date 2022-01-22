#pragma once
#pragma warning(disable : 4996)
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <CL/cl.h>
#include <stdbool.h>
#define ARRAY_SIZE  10

//创建并初始化存储数组
void CreatArray(int*** arry, int n);

//输入数据
void SetArray(int** arry, int n);

//矩阵相加
void AddArray(int** a, int** b, int** c, int n);

//打印矩阵内容
void PrintArray(int** arry, int n);

//释放数组
void FreeArray(int*** arry, int n);

//选择 OpenCL 平台并创建一个上下文
cl_context CreateContext();

//创建设备并创建命令队列
cl_command_queue CreateCommandQueue(cl_context context, cl_device_id* device);

//创建和构造程序对象
cl_program CreateProgram(cl_context context, cl_device_id device, const char* fileName);

//创建和构造程序对象
bool CreateMemObjects(cl_context context, cl_mem memObjects[3], int* a, int* b);

//释放 OpenCL 资源
void CleanUp(cl_context context, cl_command_queue commandqueue, cl_program program, cl_kernel kernel, cl_mem memObjects[3]);

//显示内存分配信息
void displayBufferDetails(cl_mem memobj);

//  用于导入程序源码，从CL文件中读取到buffer中
void loadProgramSource(const char** files, size_t length, char** buffer, size_t* sizes);

//从源码创建程序
cl_program build_program(cl_context ctx, cl_device_id dev, const char* filename);





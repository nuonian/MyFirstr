#pragma warning(disable : 4996)
#include "array.h"

/*int main()
{
	//获取矩阵阶数
	int n = 0;
	printf("请输入矩阵的阶数：");
	scanf("%d", &n);
	
	//创建矩阵数字并初始化
	int** a = NULL;
	int** b = NULL;
	int** c = NULL;
	CreatArray(&a, n);
	CreatArray(&b, n);
	CreatArray(&c, n);

	//输入数据
	printf("请输入矩阵 A 的数据：\n");
	SetArray(a, n);

	printf("请输入矩阵 B 的数据：\n");
	SetArray(b, n);

	//矩阵相加
	AddArray(a, b, c, n);

	//数组输出
	printf("矩阵A:\n");
	PrintArray(a, n);
	printf("矩阵B:\n");
	PrintArray(b, n);
	printf("矩阵之和:\n");
	PrintArray(c, n);
	
	//释放存储数组
	FreeArray(&a, n);
	FreeArray(&b, n);
	FreeArray(&c, n);

	return 0;
}*/
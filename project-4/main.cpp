#include<iostream>
#include"sm3_SIMD.h"
#include<ctime>

using namespace std;

int main()
{
	string message = "201900460044";
	Byte* mss = new Byte[12];
	for (int i = 0; i < 12; i++)
	{
		mss[i] = int(message[i]);
	}
	time_t start;
	time_t end;
	time(&start);
	Word* result;
	for (int t = 0; t < 100000; t++)
	{
		result = SM3_SIMD::hash_SIMD(mss, 12);
	}
	time(&end);
	result = SM3_SIMD::hash_SIMD(mss, 12);
	cout << endl;
	cout << "����100000�ε�����ʱ��Ϊ��" << oct << end - start << endl;

}
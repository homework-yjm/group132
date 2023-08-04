#include <iostream>
#include "tree.h"
#include "sha256.h"
using std::endl;
using std::cout;

void veri(tree* treel, string test)
{
	if (treel->verify(test))//��֤��������ڵ� �����޸ı�
	{
		cout << "Merkle���ϴ�����֤�����ݵ�Ҷ�ӽ��" << endl;
	}
	else
	{
		cout << "Merkle���ϲ�������֤������" << endl;
	}
}

int main()
{
	string stringch = "";
	vector<string> v;
	for (int i = 0; i < 100000; i++)//����Ҷ�ڵ��������
	{
		v.push_back(to_string(i));
	}
	tree treel;
	treel.buildnodeListLeafes(v);
	cout << "��ʼ����Merkle��" << endl;
	treel.buildTree();
	cout << "��֤һ�����������ϵĽڵ㣺500 " << endl;
	stringch = to_string(500);
	stringch = sha2::hash256_hex_string(stringch);
	cout << "����֤�����ݵĹ�ϣֵΪ" << stringch << endl;
	veri(&treel, stringch);
	cout << endl;
	cout << "��֤һ�������������ϵĽڵ㣺100000 " << endl;
	stringch = to_string(100000);
	stringch = sha2::hash256_hex_string(stringch);
	cout << "����֤�����ݵĹ�ϣֵΪ" << stringch << endl;
	veri(&treel, stringch);
	return 0;
}
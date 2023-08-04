#include <iostream>
#include "tree.h"
#include "sha256.h"
using std::endl;
using std::cout;

void veri(tree* treel, string test)
{
	if (treel->verify(test))//验证有无这个节点 树有无改变
	{
		cout << "Merkle树上存在验证的数据的叶子结点" << endl;
	}
	else
	{
		cout << "Merkle树上不存在验证的数据" << endl;
	}
}

int main()
{
	string stringch = "";
	vector<string> v;
	for (int i = 0; i < 100000; i++)//对于叶节点进行输入
	{
		v.push_back(to_string(i));
	}
	tree treel;
	treel.buildnodeListLeafes(v);
	cout << "开始构建Merkle树" << endl;
	treel.buildTree();
	cout << "验证一个存在于树上的节点：500 " << endl;
	stringch = to_string(500);
	stringch = sha2::hash256_hex_string(stringch);
	cout << "想验证的数据的哈希值为" << stringch << endl;
	veri(&treel, stringch);
	cout << endl;
	cout << "验证一个不存在于树上的节点：100000 " << endl;
	stringch = to_string(100000);
	stringch = sha2::hash256_hex_string(stringch);
	cout << "想验证的数据的哈希值为" << stringch << endl;
	veri(&treel, stringch);
	return 0;
}
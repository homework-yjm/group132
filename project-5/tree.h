#pragma once
#include <iostream>
#include "sha256.h"
#include <sstream>
#include <string>
#include <vector>
using namespace std;

class node
{
private:
	string hash_string;
	node* parent;
	node* children[2];
public:
	node();
	node* getParent();
	void setChildren(node* children_l, node* children_r);
	node* getChildren(int index);
	void setParent(node* parent);
	string getHash();
	int checkLeft();
	node* getanother();
	void setHash(string hash_string);
	virtual ~node();
};
node::node()
{
	parent = NULL;
	children[0] = NULL;
	children[1] = NULL;
}

//���ù�ϣֵ
void node::setHash(string hash_string)
{
	this->hash_string = sha2::hash256_hex_string(hash_string);
}
//Ѱ�Ҹ��ڵ�
node* node::getParent()
{
	return parent;
}
//���ø��ڵ�
void node::setParent(node* parent)
{
	this->parent = parent;
}
//�����ӽڵ�
void node::setChildren(node* children_l, node* children_r)
{
	children[0] = children_l;
	children[1] = children_r;
}
node* node::getanother() //�����ӽڵ�õ����ӽڵ㣬�����ӽڵ�õ����ӽڵ�
{
	//�õ��ýڵ�ĸ��ڵ�
	node* parent = getParent();

	//�жϸ��ڵ�����ӽڵ�ͱ��ڵ��Ƿ���ͬ
	//��ͬ�������ӽڵ㣬��ͬ�������ӽڵ�
	return parent->getChildren(0) == this ? parent->getChildren(1) : parent->getChildren(0);
}
node* node::getChildren(int index)
{
	return index <= 1 ? children[index] : NULL;
}
string node::getHash()
{
	return hash_string;
}
int node::checkLeft()
{
	//����丸�ڵ�����ӽڵ��Ǹýڵ� ����0 �����򷵻�1
	return parent->getChildren(0) == this ? 0 : 1;
}

node::~node() {}

class tree
{
private:
	string Root;
	int Binary(vector<node*>& node_vector);
	vector<vector<node*>> nodeList; //��������һ�����ڵ��б�
public:
	tree();
	void buildTree();
	void buildnodeListLeafes(vector<string> nodeList_leafs);
	int verify(string hash);
	virtual ~tree();
};

tree::tree() {}

int tree::Binary(vector<node*>& node_vector) //ʹҶ�ӽڵ��Ϊ˫��
{
	int vectSize = node_vector.size();
	if ((vectSize % 2) != 0) //���Ԫ�ظ���Ϊ�������Ͱ������һ���ڵ�push_backһ��
	{
		node_vector.push_back(node_vector.end()[-1]);
		vectSize++;
	}
	return vectSize;
}

void tree::buildTree() //����merkle tree
{
	int layer = 0;
	do
	{
		vector<node*> new_nodes;//�洢ÿһ��ĸ��ڵ�
		Binary(nodeList.end()[-1]); //����βԪ�� ��һ���ڵ��б�
		for (int i = 0; i < nodeList.end()[-1].size(); i += 2)
		{
			node* new_parent = new node; //���ø��׽ڵ� �������һ��Ԫ�� ��һ���ڵ��б�ĵ�i��i+1��
			nodeList.end()[-1][i]->setParent(new_parent);
			nodeList.end()[-1][i + 1]->setParent(new_parent);

			//ͨ���������ӽڵ�Ĺ�ϣֵ���ø��ڵ��ϣֵ
			new_parent->setHash(nodeList.end()[-1][i]->getHash() + nodeList.end()[-1][i + 1]->getHash());
			//���ø��ڵ�������ӽڵ�ڵ�����Ϊ������
			new_parent->setChildren(nodeList.end()[-1][i], nodeList.end()[-1][i + 1]);
			//��new_parentѹ��new_nodes
			new_nodes.push_back(new_parent);
		}
		nodeList.push_back(new_nodes); //����һ�ֵĸ��ڵ�new_nodesѹ��nodeList
		cout << "�ò�Ľ���� " << nodeList.end()[-1].size() << " ��:" << endl;
		layer++;
	} while (nodeList.end()[-1].size() > 1); //����ÿһ�ֵõ���һ��ĸ��ڵ㣬ֱ���õ����ڵ� �˳�ѭ��
	Root = nodeList.end()[-1][0]->getHash(); //���ڵ�Ĺ�ϣֵ
	cout << "Merkle tree�ĸ��ڵ�Ϊ�� : " << Root << endl;
	cout << "����" << layer << "��" << endl;
}

void tree::buildnodeListLeafes(vector<string> nodeList_leafs) //����Ҷ�ӽڵ��б�
{
	vector<node*> new_nodes;

	for (auto leaf : nodeList_leafs) //��ÿһ���ַ���������Ӧ�ڵ㣬��ͨ������ַ������ù�ϣֵ
	{
		node* new_node = new node;
		new_node->setHash(leaf);
		new_nodes.push_back(new_node);
	}
	nodeList.push_back(new_nodes);
	cout << endl;
}

int tree::verify(string hash)
{
	node* el_node = NULL;
	string act_hash = hash;

	for (int i = 0; i < nodeList[0].size(); i++)
	{
		if (nodeList[0][i]->getHash() == hash)
		{
			el_node = nodeList[0][i];
		}
	}
	if (el_node == NULL)
	{
		return 0;
	}

	do  //��֤merkle tree�Ƿ�ı�� 
	{
		//���ڵ�Ĺ�ϣ�����ӽڵ�Ĺ�ϣstring+���ӽڵ�Ĺ�ϣstring
		//���el_node�Ǹ��ڵ�����ӽڵ�
		if (el_node->checkLeft() == 0)
		{
			//�����ӽڵ�ͼ������ӽڵ�Ĺ�ϣstring+���ӽڵ�Ĺ�ϣstring����֮Ҳ��ͬ����ʽ����
			act_hash = sha2::hash256_hex_string(act_hash + el_node->getanother()->getHash());
		}
		else
		{
			act_hash = sha2::hash256_hex_string(el_node->getanother()->getHash() + act_hash);
		}
		el_node = el_node->getParent();
	} while ((el_node->getParent()) != NULL); //������ڵ�

	return act_hash == Root ? 1 : 0;
}

tree::~tree() {}
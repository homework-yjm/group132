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

//设置哈希值
void node::setHash(string hash_string)
{
	this->hash_string = sha2::hash256_hex_string(hash_string);
}
//寻找父节点
node* node::getParent()
{
	return parent;
}
//设置父节点
void node::setParent(node* parent)
{
	this->parent = parent;
}
//设置子节点
void node::setChildren(node* children_l, node* children_r)
{
	children[0] = children_l;
	children[1] = children_r;
}
node* node::getanother() //是左子节点得到右子节点，是右子节点得到左子节点
{
	//得到该节点的父节点
	node* parent = getParent();

	//判断父节点的左子节点和本节点是否相同
	//相同返回右子节点，不同返回左子节点
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
	//如果其父节点的左子节点是该节点 返回0 否则则返回1
	return parent->getChildren(0) == this ? 0 : 1;
}

node::~node() {}

class tree
{
private:
	string Root;
	int Binary(vector<node*>& node_vector);
	vector<vector<node*>> nodeList; //里面存的是一个个节点列表
public:
	tree();
	void buildTree();
	void buildnodeListLeafes(vector<string> nodeList_leafs);
	int verify(string hash);
	virtual ~tree();
};

tree::tree() {}

int tree::Binary(vector<node*>& node_vector) //使叶子节点成为双数
{
	int vectSize = node_vector.size();
	if ((vectSize % 2) != 0) //如果元素个数为奇数，就把再最后一个节点push_back一次
	{
		node_vector.push_back(node_vector.end()[-1]);
		vectSize++;
	}
	return vectSize;
}

void tree::buildTree() //建造merkle tree
{
	int layer = 0;
	do
	{
		vector<node*> new_nodes;//存储每一层的父节点
		Binary(nodeList.end()[-1]); //传入尾元素 即一个节点列表
		for (int i = 0; i < nodeList.end()[-1].size(); i += 2)
		{
			node* new_parent = new node; //设置父亲节点 传入最后一个元素 即一个节点列表的第i和i+1个
			nodeList.end()[-1][i]->setParent(new_parent);
			nodeList.end()[-1][i + 1]->setParent(new_parent);

			//通过两个孩子节点的哈希值设置父节点哈希值
			new_parent->setHash(nodeList.end()[-1][i]->getHash() + nodeList.end()[-1][i + 1]->getHash());
			//将该父节点的左右子节点节点设置为这两个
			new_parent->setChildren(nodeList.end()[-1][i], nodeList.end()[-1][i + 1]);
			//将new_parent压入new_nodes
			new_nodes.push_back(new_parent);
		}
		nodeList.push_back(new_nodes); //将新一轮的父节点new_nodes压入nodeList
		cout << "该层的结点有 " << nodeList.end()[-1].size() << " 个:" << endl;
		layer++;
	} while (nodeList.end()[-1].size() > 1); //这样每一轮得到新一层的父节点，直到得到根节点 退出循环
	Root = nodeList.end()[-1][0]->getHash(); //根节点的哈希值
	cout << "Merkle tree的根节点为： : " << Root << endl;
	cout << "共有" << layer << "层" << endl;
}

void tree::buildnodeListLeafes(vector<string> nodeList_leafs) //建立叶子节点列表
{
	vector<node*> new_nodes;

	for (auto leaf : nodeList_leafs) //给每一个字符串创建对应节点，并通过这个字符串设置哈希值
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

	do  //验证merkle tree是否改变过 
	{
		//父节点的哈希是左子节点的哈希string+右子节点的哈希string
		//如果el_node是父节点的左子节点
		if (el_node->checkLeft() == 0)
		{
			//是左子节点就计算左子节点的哈希string+右子节点的哈希string，反之也以同样格式操作
			act_hash = sha2::hash256_hex_string(act_hash + el_node->getanother()->getHash());
		}
		else
		{
			act_hash = sha2::hash256_hex_string(el_node->getanother()->getHash() + act_hash);
		}
		el_node = el_node->getParent();
	} while ((el_node->getParent()) != NULL); //到达根节点

	return act_hash == Root ? 1 : 0;
}

tree::~tree() {}
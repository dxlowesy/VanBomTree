#include <cstdio>
#include <set>
#include <vector>
#include <iostream>
using namespace std;
struct Node
{
	int msize;
	int mdownsize;
	int maxval, minval;
	vector<Node*> cluster;
	Node* summary;//summary means the condition of this level, organized in veb-tree too
}*mroot;

const int NIL = -1;
int nsize;

inline int getMin(Node* u)
{
	return u->minval;
}

inline int getMax(Node* u)
{
	return u->maxval;
}

inline int Log2(int u)
{
	int cnt = 0;
	while (u > 0)
	{
		cnt++;
		u >>= 1;
	}
	return cnt - 1;
}

inline int getDivd(int u)
{
	int x = Log2(u);
	x /= 2;
	return (1 << x);
}

inline int getHigh(const int& x, const int& div)
{
	return x / div;
}

inline int getLow(const int& x, const int& div)
{
	return x % div;
}

inline int getIndex(const int& high, const int& low, const int& div)
{
	return high * div + low;
}

void creatNode(int u, Node*& newnode, int logu)
{
	newnode = new Node();
	newnode->msize = u;
	newnode->maxval = newnode->minval = NIL;
	if (u == 2)
		return;

	int upsize = 1 << ((logu + 1) / 2);
	newnode->mdownsize = 1 << (logu / 2);
	newnode->cluster.resize(upsize);
}

bool veb_Tree_Member(Node* u, int x)
{
	if (x == u->minval || x == u->maxval)
		return true;
	else if (u->msize == 2)
		return false;
	else
	{
		int divdx = u->mdownsize;
		return veb_Tree_Member(u->cluster[getHigh(x, divdx)], getLow(x, divdx));
	}
}

void buildTree(int u, Node*& unode, int logu)
{
	creatNode(u, unode, logu);
	if (u > 2)
	{
		for (int i = 0; i < unode->cluster.size(); i++)
		{
			buildTree(unode->mdownsize, unode->cluster[i], logu / 2);
		}
		int th = ((logu + 1) / 2);
		buildTree(1 << th, unode->summary, th);
	}
}

int veb_tree_successor(Node* unode, int x)
{
	if (unode->msize == 2)
	{
		if (x == 0 && unode->maxval == 1)
			return 1;
		else
			return NIL;
	}
	else if (unode->minval != NIL && x < unode->minval)
	{
		return unode->minval;
	}
	else
	{
		int divdx = unode->mdownsize;
		int highx = getHigh(x, divdx);
		int lowx = getLow(x, divdx);
		int maxlow = getMax(unode->cluster[highx]);
		if (maxlow != NIL && lowx < maxlow)
		{
			int suc_low = veb_tree_successor(unode->cluster[highx], lowx);
			return getIndex(highx, suc_low, divdx);
		}
		else
		{
			int succluster = veb_tree_successor(unode->summary, highx);
			if (succluster == NIL)
				return NIL;
			else
			{
				int offset = getMin(unode->cluster[succluster]);
				return getIndex(succluster, offset, divdx);
			}
		}

	}
}

int veb_tree_predecessor(Node* unode, int x)
{
	if (unode->msize == 2)
	{
		if (unode->minval == 0 && x == 1)
			return 0;
		else
			return NIL;
	}
	else if (unode->maxval != NIL && unode->maxval < x)
	{
		return unode->maxval;
	}
	else
	{
		int divdx = unode->mdownsize;
		int highx = getHigh(x, divdx);
		int lowx = getLow(x, divdx);
		int min_cur = getMin(unode->cluster[highx]);
		if (min_cur != NIL && min_cur < lowx)
		{
			int pre_cur = veb_tree_predecessor(unode->cluster[highx], lowx);
			return getIndex(highx, pre_cur, divdx);
		}
		else
		{
			int pre_cluster = veb_tree_predecessor(unode->summary, highx);
			if (pre_cluster != NIL)
			{
				int max_pre = getMax(unode->cluster[pre_cluster]);
				return getIndex(pre_cluster, max_pre, divdx);
			}
			else
			{
				if (unode->minval != NIL && unode->minval < x)
					return unode->minval;
				else
					return NIL;
			}
		}
	}
}

void empty_insert(Node* unode, int x)
{
	unode->minval = x;
	unode->maxval = x;
}


void veb_tree_insert(Node* unode, int x)
{
	if (unode->minval == NIL)
	{
		empty_insert(unode, x);
	}
	else
	{
		if (x < unode->minval)
		{
			//swap(x, unode->minval);
			int tmp = x;
			x = unode->minval;
			unode->minval = tmp;
		}
		if (unode->msize > 2)
		{
			int divdx = unode->mdownsize;
			int highx = getHigh(x, divdx);
			int lowx = getLow(x, divdx);
			if (getMin(unode->cluster[highx]) == NIL)
			{
				veb_tree_insert(unode->summary, highx);
				empty_insert(unode->cluster[highx], lowx);
			}
			else
			{
				veb_tree_insert(unode->cluster[highx], lowx);
			}
		}
		if (x > unode->maxval)
			unode->maxval = x;
	}
}

void veb_tree_delete(Node* unode, int x)
{
	if (unode->minval == unode->maxval)
	{
		unode->minval = NIL;
		unode->maxval = NIL;
	}
	else if (unode->msize == 2)
	{
		if (x == 0)
			unode->minval = 1;
		else
			unode->maxval = 0;
		//unode->maxval = unode->minval;
	}
	else
	{
		int divdx = unode->mdownsize;
		if (x == unode->minval)
		{
			int fir_cluster = getMin(unode->summary);
			int offset = getMin(unode->cluster[fir_cluster]);
			x = getIndex(fir_cluster, offset, divdx);
			unode->minval = x;
		}
		int highx = getHigh(x, divdx);
		int lowx = getLow(x, divdx);
		veb_tree_delete(unode->cluster[highx], lowx);
		if (getMin(unode->cluster[highx]) == NIL)
		{
			veb_tree_delete(unode->summary, highx);
			if (x == unode->maxval)
			{
				int maxcluster = getMax(unode->summary);
				if (maxcluster != NIL)
					unode->maxval = getIndex(maxcluster, unode->cluster[maxcluster]->maxval, divdx);
				else
					unode->maxval = unode->minval;
			}
		}
		else if (x == unode->maxval)
		{
			unode->maxval = getIndex(highx, unode->cluster[highx]->maxval, divdx);
		}
	}

}
void vanBomTree()
{

}

int main()
{
	srand(102);
	int n = 100033;
	set<int> numset;
	int tmp = Log2(n);
	nsize = (1 << tmp);
	if (nsize < n)
	{
		nsize <<= 1;
		tmp++;
	}
	buildTree(nsize, mroot, tmp);
	for (int i = 0; i < n; i++)
	{
		int tmp = rand() % n;

		if (numset.count(tmp) > 0)
		{
			numset.insert(tmp);
			veb_tree_insert(mroot, tmp);
		}
		if (rand() % 3 == 2)
		{
			tmp = rand() % n;
			if (veb_Tree_Member(mroot, tmp))
			{
				if (numset.count(tmp) > 0)
					numset.erase(tmp);
				else
					numset.erase(tmp);
				veb_tree_delete(mroot, tmp);
			}

		}
		if (i % 1000 == 0)
		{
			for (int j = 0; j < n; j++)
			{
				if ((numset.count(j) > 0 && veb_Tree_Member(mroot, j)) || (numset.count(j) == 0 && !veb_Tree_Member(mroot, j)))
					continue;
				cout << j << " false" << endl;
				veb_Tree_Member(mroot, j);
			}
		}
	}
}
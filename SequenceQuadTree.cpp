#include<iostream>
using namespace std;
#include<ctime>
#define MAX_NUM  1000100    //结点数组长度
#define MAX_ELE_NUM  100   //一个矩形区域中的最大点数


struct Rect
{
	double up;
	double bottom;
	double left;
	double right;
};

struct ElePoint
{
	double lng;  //x坐标
	double lat;  //y坐标
};

struct QuadTreeNode
{
	int depth;  //深度
	int is_leaf;  //叶子结点
	Rect rect;   //矩形区域
	int RU_0;  //右上子节点下标
	int LU_1;  //左上子节点下标
	int LB_2;  //左下子节点下标
	int RB_3;  //右下子节点下标
	int ele_num;  //一个矩形区域中的位置点数
	ElePoint ele_list[MAX_ELE_NUM];  //矩形区域中的位置点数组
};

QuadTreeNode QTList[MAX_NUM];   //结点数组


//初始化矩形区域
void InitRect(Rect &rect, double up, double bottom, double left, double right)
{
	rect.up = up;
	rect.bottom = bottom;
	rect.left = left;
	rect.right = right;
}

//初始化结点
void InitNode(QuadTreeNode &node, int depth, Rect &rect)
{
	node.depth = depth;
	node.rect = rect;
	node.ele_num = 0;
	node.is_leaf = 1;
	node.RU_0 = 0;
	node.LU_1 = 0;
	node.LB_2 = 0;
	node.RB_3 = 0;
}


//初始化子节点
QuadTreeNode CreatChildNode(QuadTreeNode node, double up, double bottom, double left, double right)
{
	int depth = node.depth + 1;

	Rect rect;
	QuadTreeNode childNode;

	InitRect(rect, up, bottom, left, right);
	InitNode(childNode, depth, rect);
	return childNode;
}


//结点分裂
void SplitNode(QuadTreeNode &node)
{
	double mid_horizontal = (node.rect.left + node.rect.right) / 2;
	double mid_vertical = (node.rect.up + node.rect.bottom) / 2;

	node.RU_0 = 4 * node.RU_0 + 1;
	node.LU_1 = 4 * node.LU_1 + 2;
	node.LB_2 = 4 * node.LB_2 + 3;
	node.RB_3 = 4 * node.RB_3 + 4;

	node.is_leaf = 0;

	//初始化子节点
	QTList[node.RU_0] = CreatChildNode(QTList[node.RU_0], node.rect.up, mid_vertical, mid_horizontal, node.rect.right);

	QTList[node.LU_1] = CreatChildNode(QTList[node.LU_1], node.rect.up, mid_vertical, node.rect.left, mid_horizontal);

	QTList[node.LB_2] = CreatChildNode(QTList[node.LB_2], mid_vertical, node.rect.bottom, node.rect.left, mid_horizontal);

	QTList[node.RB_3] = CreatChildNode(QTList[node.RB_3], mid_vertical, node.rect.bottom, mid_horizontal, node.rect.right);
}

//插入点
void InsertNode(QuadTreeNode &node, ElePoint ele)
{
	//是叶子结点
	if (node.is_leaf == 1) {	
		//如果矩形区域内的点过载，就分裂出4个子节点
		if (node.ele_num + 1 > MAX_ELE_NUM) {
			SplitNode(node);	
			InsertNode(node, ele);	//将过载的点插入到子节点上		
		}
		else {
			node.ele_list[node.ele_num] = ele;  //不过载就将该点插入到父节点中
			node.ele_num++;
		}
		return;
	}

	//不是叶子结点，递归寻找其中一个子结点进行插入
	double mid_horizontal = (node.rect.left + node.rect.right) / 2;
	double mid_vertical = (node.rect.up + node.rect.bottom) / 2;

	if (ele.lng > mid_horizontal){
		if (ele.lat > mid_vertical){
			InsertNode(QTList[node.RU_0], ele);
		}
		else{
			InsertNode(QTList[node.LU_1], ele);
		}
	}
	else {
		if (ele.lat > mid_vertical){
			InsertNode(QTList[node.RB_3], ele);
		}
		else{
			InsertNode(QTList[node.LB_2], ele);
		}
	}
}


//结点查询 -> 输出点所在结点中的所有点
void queryEle(QuadTreeNode node, ElePoint &ele)
{
	//是叶子结点
	if (node.is_leaf == 1)
	{
		cout << "该结点内点有" << node.ele_num << "个点，分别为：" << endl;
		for (int i = 0; i < node.ele_num; i++)
		{
			cout << "(" << node.ele_list[i].lng << "," << node.ele_list[i].lat << ") ";
		}
		cout << endl;
		return;
	}

	//不是叶子结点
	double mid_horizontal = (node.rect.left + node.rect.right) / 2;
	double mid_vertical = (node.rect.up + node.rect.bottom) / 2;

	if (ele.lng > mid_horizontal){
		if (ele.lat > mid_vertical){
			queryEle(QTList[node.RU_0], ele);
		}
		else{
			queryEle(QTList[node.LU_1], ele);

		}
	}
	else{
		if (ele.lat > mid_vertical){
			queryEle(QTList[node.RB_3], ele);

		}
		else{
			queryEle(QTList[node.LB_2], ele);
		}
	}
}


//点查询
void queryPoint(QuadTreeNode node, ElePoint &ele)
{
	//是叶子节点
	if (node.is_leaf == 1)
	{
		for (int i = 0; i < node.ele_num; i++)
		{
			if (ele.lng == node.ele_list[i].lng&&ele.lat == node.ele_list[i].lat)
			{
				cout << "(" << node.ele_list[i].lng << "," << node.ele_list[i].lat << ") 位于第" << node.depth << "层" << endl;
				return;
			}
		}
		cout << "未找到该点！" << endl;
		return;
	}

	//不是叶子结点
	double mid_vertical = (node.rect.up + node.rect.bottom) / 2;
	double mid_horizontal = (node.rect.left + node.rect.right) / 2;

	if (ele.lng > mid_horizontal){
		if (ele.lat > mid_vertical){
			queryPoint(QTList[node.RU_0], ele);
		}
		else{
			queryPoint(QTList[node.RB_3] , ele);
		}
	}
	else{
		if (ele.lat > mid_vertical){
			queryPoint(QTList[node.LU_1], ele);
		}
		else{
			queryPoint(QTList[node.LB_2] , ele);
		}
	}
}


int main()
{	
	Rect rect;
	
	InitRect(rect, 90, -90, -180, 180);

	QuadTreeNode root;
	
	InitNode(root, 1, rect);
	

	ElePoint point2;
	point2.lng = -12.123;
	point2.lat = -42.568;
	InsertNode(root, point2);

	ElePoint ele;

	srand((unsigned)time(NULL));
	for (int i = 0; i < 1000000; i++)
	{
		ele.lng = (float)(rand() % 360 - 180 + (float)(rand() % 1000) / 1000);
		ele.lat = (float)(rand() % 180 - 90 + (float)(rand() % 1000) / 1000);
		InsertNode(root, ele);
	}

	ElePoint point1;
	point1.lng = -24.123;
	point1.lat = -42.452;

	queryEle(root, point1);

	

	ElePoint point3;
	point3.lng = 50.236;
	point3.lat = 12.365;
	InsertNode(root, point3);

	queryPoint(root, point2);
	queryPoint(root, point3);


	system("pause");
	return 0;
}

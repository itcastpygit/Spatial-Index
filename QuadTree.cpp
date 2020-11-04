#include <iostream>
using namespace std;
#include <string>
#include <stdlib.h>
#include <ctime>

#define MAX_ELE_NUM 1

#define QUADRANT_RU 1
#define QUADRANT_LU 2
#define QUADRANT_LB 3
#define QUADRANT_RB 4

//矩形区域
struct Region
{
	double up;
	double bottom;
	double left;
	double right;
};


//点结构体
struct ElePoint
{
	double lng;   //x坐标
	double lat;   //y坐标
	//char desc[16];
};

//四叉树结点
struct QuadTreeNode
{
	int depth;   //结点的深度
	int is_leaf;   //是否是叶子节点
	struct Region region;   //区域范围
	struct QuadTreeNode *LU;  //左上子节点指针
	struct QuadTreeNode *LB;   //左下子节点指针
	struct QuadTreeNode *RU;   //右上子节点指针
	struct QuadTreeNode *RB;   //右下子节点指针
	int ele_num;     //矩形区域中位置点数
	ElePoint *ele_list[MAX_ELE_NUM];   //矩形区域中位置点列表
};


//初始化矩形区域
void initRegion(Region *region, double bottom, double up, double left, double right)
{
	region->bottom = bottom;
	region->up = up;
	region->left = left;
	region->right = right;
}


//初始化四叉树结点
void initNode(QuadTreeNode *node, int depth, Region region)
{
	node->depth = depth;
	node->is_leaf = 1;
	node->ele_num = 0;
	node->region = region;
}


// 创建子节点
QuadTreeNode *createChildNode(struct QuadTreeNode *node, double bottom, double up, double left, double right)
{
	int depth = node->depth + 1;   //深度+1
	QuadTreeNode *childNode = (QuadTreeNode *)malloc(sizeof(QuadTreeNode));
	Region *region = (Region *)malloc(sizeof(Region));
	//初始化子节点
	initRegion(region, bottom, up, left, right);
	initNode(childNode, depth, *region);

	return childNode;
}

/**
 * 分裂结点
 * 1.通过父结点获取子结点的深度和范围
 * 2.生成四个结点，挂载到父结点下
 *
 * @param node
 */
void splitNode(struct QuadTreeNode *node)
{
	void insertEle(QuadTreeNode *node, ElePoint ele);
	double mid_vertical = (node->region.up + node->region.bottom) / 2;  //垂直方向的中间线
	double mid_horizontal = (node->region.left + node->region.right) / 2;  //水平方向的中间线

	node->is_leaf = 0;

	//生成四个孩子结点
	node->RU = createChildNode(node, mid_vertical, node->region.up, mid_horizontal, node->region.right);
	node->LU = createChildNode(node, mid_vertical, node->region.up, node->region.left, mid_horizontal);
	node->RB = createChildNode(node, node->region.bottom, mid_vertical, mid_horizontal, node->region.right);
	node->LB = createChildNode(node, node->region.bottom, mid_vertical, node->region.left, mid_horizontal);

	for (int i = 0; i < node->ele_num; i++)
	{
		//此时插入的时候，node不是叶子节点，此时执行 insert 函数，会将元素插入到孩子节点上
		insertEle(node, *node->ele_list[i]);   // 将父节点元素 插入到子节点
		free(node->ele_list[i]);   //释放父节点元素
		node->ele_num--;     //每插入一个元素，父节点的元素数-1
	}
}


/**
 * 插入元素
 * 1.判断是否已分裂，已分裂的选择适合的子结点，插入；
 * 2.未分裂的查看是否过载，过载的分裂结点，重新插入；
 * 3.未过载的直接添加
 *
 * @param node
 * @param ele
 * todo 使用元素原地址，避免重新分配内存造成的效率浪费
 */
void insertEle(QuadTreeNode *node, ElePoint ele)
{
	//是叶子结点
	if (1 == node->is_leaf)
	{
		if (node->ele_num + 1 > MAX_ELE_NUM)
		{
			splitNode(node);

			//分裂后的 node 不是叶子节点，所以新插入的元素会插入到 node 的子节点上
			insertEle(node, ele);  //将新插入的元素插入到node的子节点上
		}
		else
		{
			ElePoint *ele_ptr = (ElePoint *)malloc(sizeof(ElePoint));
			ele_ptr->lat = ele.lat;
			ele_ptr->lng = ele.lng;

			//将新插入的点加入到父节点的位置点列表中
			node->ele_list[node->ele_num] = ele_ptr;
			node->ele_num++;
		}

		return;
	}

	//不是叶子结点
	double mid_vertical = (node->region.up + node->region.bottom) / 2;
	double mid_horizontal = (node->region.left + node->region.right) / 2;
	if (ele.lat > mid_vertical)
	{
		if (ele.lng > mid_horizontal)
		{
			insertEle(node->RU, ele);
		}
		else
		{
			insertEle(node->LU, ele);
		}
	}
	else
	{
		if (ele.lng > mid_horizontal)
		{
			insertEle(node->RB, ele);
		}
		else
		{
			insertEle(node->LB, ele);
		}
	}
}


//区域查询  输出该点所在的矩形区域的所有点
void queryEle(struct QuadTreeNode node, struct ElePoint ele)
{
	//是叶子结点
	if (node.is_leaf == 1)
	{
		cout << "附近点有" << node.ele_num << "个,分别是：" << endl;
		for (int j = 0; j < node.ele_num; j++)
		{
			cout << "(" << node.ele_list[j]->lng << "," << node.ele_list[j]->lat << ") ";
		}
		cout << endl;
		return;
	}

	//不是叶子节点
	double mid_vertical = (node.region.up + node.region.bottom) / 2;
	double mid_horizontal = (node.region.left + node.region.right) / 2;

	if (ele.lat > mid_vertical)
	{
		if (ele.lng > mid_horizontal)
		{
			queryEle(*node.RU, ele);
		}
		else
		{
			queryEle(*node.LU, ele);
		}
	}
	else
	{
		if (ele.lng > mid_horizontal)
		{
			queryEle(*node.RB, ele);
		}
		else
		{
			queryEle(*node.LB, ele);
		}
	}
}


//点查询  输出点的深度
void Query_point(QuadTreeNode *node, ElePoint ele)
{
	//是叶子结点
	if (node->is_leaf == 1)
	{
		for (int i = 0; i < node->ele_num; i++)
		{
			if (node->ele_list[i]->lng == ele.lng && node->ele_list[i]->lat == ele.lat)  //点坐标相同
			{
				cout << "(" << ele.lng << "," << ele.lat << ") 位于第" << node->depth << "层" << endl;
				return;
			}
		}
		cout << "未找到该点" << endl;
		return;
	}

	//不是叶子结点
	double mid_vertical = (node->region.up + node->region.bottom) / 2;
	double mid_horizontal = (node->region.left + node->region.right) / 2;

	if (ele.lng > mid_horizontal)
	{
		if (ele.lat > mid_vertical)
		{
			Query_point(node->RU, ele);
		}
		else
		{
			Query_point(node->RB, ele);
		}
	}
	else
	{
		if (ele.lat > mid_vertical)
		{
			Query_point(node->LU, ele);
		}
		else
		{
			Query_point(node->LB, ele);
		}
	}
}



//任意区域查询 (有待完善，运行结果不太对)
void queryArea(QuadTreeNode *node, Region *region)
{	//是叶子节点
	if (node->is_leaf == 1)
	{	
		//遍历叶子节点中的所有点
		for (int i = 0; i < node->ele_num; i++) 
		{	
			//如果叶子节点中有点在该矩形区域中，就输出该点坐标
			if (node->ele_list[i]->lng >= region->left &&
				node->ele_list[i]->lng <= region->right &&
				node->ele_list[i]->lat >= region->bottom &&
				node->ele_list[i]->lat <= region->up) 
			{
				cout << "(" << node->ele_list[i]->lng << "," << node->ele_list[i]->lat << ") ";
			}
		}
		cout << endl;
		return;
	}
	
	//不是叶子结点 ， 递归查找与矩形区域有交集的叶子结点
	double mid_vertical = (node->region.up + node->region.bottom) / 2;
	double mid_horizontal = (node->region.left + node->region.right) / 2;

	//讨论矩形区域的9种分布情况
	if (region->bottom > mid_vertical){
		if (region->left > mid_horizontal){
			//如果矩形区域的下边界大，左边界大，就在右上区域查询
			queryArea(node->RU, region);
		}
		else if (region->right < mid_horizontal){
			queryArea(node->LU, region);
		}
		else{
			//将该矩形区域分成两块，逐块递归判断其所属的子区域
			Region *region1 = (Region *)malloc(sizeof(Region));
			*region1 = { region->bottom,region->up,region->left,mid_horizontal };
			queryArea(node->LU, region1);

			Region *region2 = (Region *)malloc(sizeof(Region));
			*region2 = { region->bottom,region->up,mid_horizontal,region->right };
			queryArea(node->RU, region2);
		}
	}

	else if (region->up < mid_vertical){
		if (region->right < mid_horizontal){
			queryArea(node->LB, region);
		}
		else if (region->left > mid_horizontal){
			queryArea(node->RB, region);
		}
		else{
			Region *region1 = (Region *)malloc(sizeof(Region));
			*region1 = { region->bottom, region->up,region->left,mid_horizontal };
			queryArea(node->LB, region1);

			Region *region2 = (Region *)malloc(sizeof(Region));
			*region2 = { region->bottom,region->up,mid_horizontal,region->right };
			queryArea(node->RB, region2);
		}
	}

	else{
		if (region->right < mid_horizontal){
			Region *region1 = (Region *)malloc(sizeof(Region));
			*region1 = { mid_vertical,region->up,region->left,region->right };
			queryArea(node->LU, region1);

			Region *region2 = (Region *)malloc(sizeof(Region));
			*region2 = { region->bottom,mid_vertical,region->left,region->right };
			queryArea(node->LB, region2);

		}
		else if (region->left > mid_horizontal)	{
			Region *region1 = (Region *)malloc(sizeof(Region));
			*region1 = { mid_vertical,region->up,region->left,region->right };
			queryArea(node->RU, region1);

			Region *region2 = (Region *)malloc(sizeof(Region));
			*region2 = { region->bottom,mid_vertical,region->left,region->right };
			queryArea(node->RB, region2);
		}
		else {
			Region *region1 = (Region *)malloc(sizeof(Region));
			*region1 = { mid_vertical,region->up,region->left,mid_horizontal };
			queryArea(node->LU, region1);

			Region *region2 = (Region *)malloc(sizeof(Region));
			*region2 = { mid_vertical,region->up,mid_horizontal,region->right };
			queryArea(node->RU, region2);

			Region *region3 = (Region *)malloc(sizeof(Region));
			*region3 = { region->bottom,mid_vertical,region->left,mid_horizontal };
			queryArea(node->LB, region3);

			Region *region4 = (Region *)malloc(sizeof(Region));
			*region4 = { region->bottom, mid_vertical,mid_horizontal,region->right };
			queryArea(node->RB, region4);
		}
	}
}

int main()
{
	QuadTreeNode root;
	Region root_region;

	ElePoint ele;
	initRegion(&root_region, -90, 90, -180, 180);
	initNode(&root, 1, root_region);

	srand((unsigned)time(NULL));  //随机数种子
	for (int i = 0; i < 1000000; i++)
	{
		ele.lng = (float)(rand() % 360 - 180 + (float)(rand() % 1000) / 1000);
		ele.lat = (float)(rand() % 180 - 90 + (float)(rand() % 1000) / 10000);
		insertEle(&root, ele);
	}
	cout << endl;

	ElePoint test;
	test.lat = -24;
	test.lng = -45.4;
	queryEle(root, test);
	Query_point(&root, test);



	ElePoint point1;
	point1.lng = 54.125;
	point1.lat = 12.139;
	insertEle(&root, point1);

	ElePoint point2;
	point2.lng = -12.123;
	point2.lat = -42.568;
	insertEle(&root, point2);

	ElePoint point3;
	point3.lng = 50.236;
	point3.lat = 12.365;

	Query_point(&root, point1);
	Query_point(&root, point2);
	Query_point(&root, point3);

	ElePoint point4;
	point4.lng = 2;
	point4.lat = 2;
	insertEle(&root, point4);

	//任意范围查询
	cout << "任意范围查询" << endl;
	Region region1;
	initRegion(&region1, -10, 10, -10, 10);
	queryArea(&root, &region1);

	system("pause");
	return 0;
}

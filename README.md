# Spatial-Index
各种空间索引方法原理的理解及代码实现
1.四叉树空间索引
  四叉树索引的基本思想是将地理空间递归划分为不同层次的树结构。它将已知范围的空间划分为四个相等的子空间，如此递归下去，直至树的层次达到一定深度或者满足某种要求后停止分割。四叉树的结构比较简单，并且当空间数据分布比较均匀时，具有比较高的空间数据插入和查询效率。
  这里给出了四叉树的两种实现方法：一种是使用指针实现，该方法将所有的点位置信息都存储在叶子节点上，中间节点以及根结点不存储点信息；另一种是不使用指针，而使用顺序表实现，该方法使用完全四叉树的结构，将数据存放在叶子结点和非叶子结点上，但是有的叶子节点是空的，没有存放真实数据。
  这两种方法的具体实现参考：
  

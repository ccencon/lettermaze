# lettermaze
play letter maze on your A4 paper.

前些天在玩迷宫游戏时突然回想起初中有段时间很喜欢画字母迷宫给同学们破解，具体做法是在草稿纸上用尺子画上一个巨大的矩形，再等分为许多的小方块，并在小方块上填充字母，借助复印纸复印几份，然后让同学们按照A-Z-A-Z...的顺序从入口到出口连接字母；由于事先会故意设置好几条深度比较大的歧路，在字母交错复杂的纸上，小伙伴们需要经历好些挫折，花费好些时间才能破解连出路径，想起来趣味性也是比较强的

为了怀念一下逝去的时光，于是乎便花费了一周的摸鱼时间写了这个小玩具。你可以使用这个玩具生成随机字母迷宫及其答案路径图片，并选择打印出来在纸上进行标注破解（入口为右上角，出口为左下角），这应该特别适合小孩子，或者几百个月大的小孩子`^_^`，复杂而有趣的求解过程不仅益智，还可以使其专注和耐心

下面的篇幅将会记录编码过程中遇到的问题，如果你对代码不感兴趣，只需执行编译脚本运行程序生成图片即可。整个程序可以简单划分为两个模块：字母矩阵、图片生成

##### 字母矩阵
按照以前手动设计字母矩阵的思路，总是会先填充出一条唯一正确的路径，然后再在此路径上的某些点引出一条歧路，并对此歧路递归以上操作；但如果将这个思路转化成代码，递归的终止条件将会变得不好设置，在手动设计时，都是凭个人感觉终止，而且按照这种方式，最终矩阵也极有可能存在还没被填充的方格，虽然可以事后再遍历一次，但明显不够优雅。或许多花一点时间研究也可以解决这个问题，但由于知道其它思路，潜意识里便把这种方法排除了

字母矩阵的生成可以借鉴迷宫的生成算法，它们本质是一样的；而常见的迷宫生成算法以`普里姆`和`克鲁斯卡尔`算法最为出名，这其实就是图论中[最小生成树](https://github.com/ccencon/structdata#DirectedGraph)的两种经典算法，生成边权重最小，且不形成环的图，使图中任意两点都可以连通。

`普里姆`在迷宫算法的具体思路为：

1. 选择一个起点，将起点四周的墙加入到列表L
2. 当列表L不为空时，重复执行以下步骤：
	1. 从L中随机弹出一面墙
	2. 如果墙两边存在未打通区域A，便打通这面墙，并将A附近未打通的墙加入到L（如果墙的另外一个区域已被打通，这个操作会使迷宫形成环；如果不想形成环，可以判断另外一个区域是否被打通，如果打通便不加入L了）

`克鲁斯卡尔`在迷宫算法的具体思路为：

1. 将迷宫所有墙加入到列表L中
2. 当列表L不为空时，重复执行以下步骤：
	1. 从L中随机弹出一面墙
	2. 借助并查集判断墙两边区域是否打通，还没通便打通这边墙

因为字母矩阵与迷宫在最终表达形式上存在差异，字母矩阵需要在路径上填充字母，所以从选定起点开始向外辐射路径的`普里姆`无疑是最佳选择，`克鲁斯卡尔`因为其特性，在执行过程中无法填充字母

但在实现字母矩阵的`普里姆`算法之后，出现了两个问题，第一个是由于在路径上填充了字母，使得原本不导通的两个区域通过字母顺序实现了导通，这样到终点的路径很有可能不再唯一；另一个问题是，由于普里姆算法是以起点为中心，向外面辐射，新加入的墙对原有墙列表的占比不大，在经历几次随机之后旧墙基本都会被选中，所以只需要沿着起点往终点直线附近的方向行走，大概率就能走到终点

为了解决这两个问题，需要对已经实现的`普里姆`算法进行优化，最终采用的方案是，先随机出起点到终点的唯一路径，并将路径上所有的墙加入到墙列表L中，并对此执行`普里姆`算法的计算；这样就从一个点的向外辐射变成了一条路径的向外辐射，随机性大大升高，而且由于事先知道了起点到终点的唯一路径，只要在此路径外填充的字母不会使其融入到此路径中，此路径就不会丢失唯一性

在对`普里姆`进行优化后，又引出另外一个问题，就是矩阵地图过大时程序会偶发卡死，在经历了一系列蛋痛的调试，终于确定问题所在；在最开始随机路径的算法中，是从起点开始，随机打通一堵墙，直至到达终点，当遇到死胡同时，便回溯路径，选择新的墙；这种方法思路是正确的，但有个小问题，当路径还没走到终点时如果已经将起点到终点的所有通道堵死，且偏离了终点，新路径将会在围起来的空间内进行随机和回溯，这是指数型的时间复杂度，当围起来的空间比较大时，消耗的时间将会特别多。所以在路径随机过程中，起点和终点的连通性判定成为了新的问题；留意到warshell算法，构造一个新的邻接矩阵，如果A点和B点连通，则连通A或B的其它点继承B或A的所有连通关系；那么在每一次的路径随机过程中，就不用都执行一次DFS或BFS，只需断掉新随机的路径点与周围其它点的连通关系，再通过邻接矩阵结构判断起点和终点的连通性即可

warshell算法可以很好的解决这个新问题，但也会使整体的实现思路不够清晰；所以思考再三后还是选用了`克鲁斯卡尔`算法，在字母矩阵的实现中，`克鲁斯卡尔`有两个变种，一种是按照其原始的算法思路，生成整个迷宫后，找出唯一路径并对其填充字母，然后再对歧路填充；另一种是将`克鲁斯卡尔`循环终止条件变为起点和终点在同一个集合，生成唯一路径之后，对其执行上述`普里姆`算法；前者偏向深度，后者偏向广度，各有特点，本文实现采取的是第二种

##### 图片生成
提供了ppm图片格式生成，但考虑到win10默认的Photos程序不支持ppm，所以另外支持了24位bmp图片格式。需要注意的是BMP是小端模式存储，像素扫描顺序从下往上，从左到右，开头两个字节应赋值"MB"，小端存储为"BM"，参考了几篇文章，作者都是没完全搞清楚的，绕了整整半天才绕出来...在这里有个疑问，BMP每行的rgb需要4字节对齐，在没对齐的时候网上所有做法都是写入0补齐，那为什么直接seek会导致图片打不开？按道理说补齐4字节，最多也是seek3个字节，根本占不满一个block甚至一个扇区，这意味着读取图片数据时能读取到完整的数据，但打不开图片是为什么？

A4纸的尺寸为297mm×210mm，适应的像素一般为842×595、2105×1487、3508×2479等，这里选择了2105×1487像素，可以依据个人喜好选择其他分辨率，甚至当打印的不是A4纸时，可以选择其他规格纸张对应的分辨率

这个功能的私有模块隐藏了图片生成的具体细节，使用者只需提供一个对外函数，检查传入参数的合法性，并提供像素点的rgb函数即可，其实这个功能可以进一步分离，但毕竟这只是一个小玩具，暂时就不考虑那么多了。在这里发生过一个小插曲，为了提高rgb函数的性能，曾写死一个静态rgb数组，长度为2105×1487×3，结果执行的时候G了，提示某些内存地址无权限，也是在这里突然意识到为什么借鉴的文章是按照RGB单独返回的了，因为数组太大，2105×1487×3个字节约为为8.9MB，这已经超出了linux下函数默认栈空间8MB的大小

字母矩阵的图片可以拆分为3个区域，一个是背景，一个是矩阵的边，剩下一个是矩阵内方格字母。为了保护眼睛，背景填充为柔和的淡黄色0xFFFFC8；矩阵的边固定为5个像素，为黑色；方格像素固定为80×80，字母从字体文件读取，留空的地方填充背景色，在另外一张路径图中，路径方格内留空的颜色将以黄色0xFFFF00填充。值得一提的是字母的字体输出，这里采用的是点阵字体；一开始考虑的也是点阵字体，但网上能找到的26个字母点阵数组只有8×8像素，精度极低；后来注意到从未接触过的矢量字体，可以适配任意像素，但矢量字体的使用需要引入第三方库，操作起来也特别麻烦，加之不需要太过复杂的功能，于是乎便又转回到点阵字体；苦于无法找到高精度的字母点阵数组，在纠结好一阵后只能手动画出26个字母，基于各种考虑，决定将字母的字体文件定为80×80像素，这也是方格固定为80×80的原因；本来还打算写个小程序输出点阵数组加入到这里的代码，不过转头一想，还不如在生成过程中读取字体文件来的方便，可维护性也更高

另外，对于重复文件名的生成，这里借鉴了linux某些命令的编号备份，当遇到重复文件名时，会自动添加递增整数后缀，如生成文件名为`lm.ppm`，但目录内已存在`lm.ppm`，`lm_1.ppm`，那么新的文件名将会变为`lm_2.ppm`，可以放心生成文件而不用担心被覆盖

参考链接：

1. [用三段 140 字符以内的代码生成一张 1024×1024 的图片](http://www.matrix67.com/blog/archives/6039#more-6039)
2. [点阵字库的原理](https://www.360docs.net/doc/1117272484.html)
3. [如何在LCD上显示字符](https://cloud.tencent.com/developer/article/1885997)
4. [FreeTpye库学习笔记：将矢量字体解析为位图](https://blog.csdn.net/weixin_40026797/article/details/114781234)
5. [文件格式标准：BMP](http://blog.leanote.com/post/gaunthan/BMP)
6. [纯C++实现24位bmp格式图片的读取和修饰](https://blog.csdn.net/m0_62505136/article/details/121153508)

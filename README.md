# 配置方法

## 编程环境

-   操作系统：macOS Monterey 12.3
-   CPU&GPU：Apple Silicon M1
-   IDE：CLion 2021.3.4
-   语言：C++ 11
-   需要cmake编译项目，依赖库只有Eigen 3.4（线性代数计算库）和OpenCV（仅用于图像读取与输出）

## 环境配置


项目是在mac上配置的，但是大部分人的环境可能是在Windows或者Linux下，由于没有在Windows下配置过环境，所以不是很了解如何进行操作，但是推荐使用CLion创建项目并用cmake配置环境（由于每个人实际的情况都不同，这里没法详细介绍），总之需要进行Eigen和OpenCV的配置，并且需要用cmake管理源文件和头文件，编译项目。

## 参数配置

配置好运行环境后，在根目录`～/renderer`下找到`～renderer/Data/modelConfig.txt`文件，该文件是**模型配置文件**，格式如下：

MODEL [模型文件路径（相对路径或绝对路径），只支持ply格式]

MATERIAL [材质配置文件路径（相对路径或绝对路径）]

**注意：关键字MODEL MATERIAL均必须大写** 

**示例：**

![img](file:////Users/wenyongyan/Library/Group%20Containers/UBF8T346G9.Office/TemporaryItems/msohtmlclip/clip_image002.jpg)

**材质配置文件**为一个文本文件，可以自行创建，项目中已经预先设置了`materialBox.txt`和`materialHelmet.txt`两种，材质配置文件的格式如下：

渲染模型（PHONG或者FLAT）

环境光系数 漫反射系数 高光系数

高光衰减系数

 <!--前面的部分属于必选内容，后面的部分为可选内容-->

TEXTURE_BASE [基本纹理（漫反射纹理）路径]

TEXTURE_NORMAL [法线贴图路径] 

TEXTURE_EMISSION [自发光贴图路径]

**注意：关键字TEXTURE_BASE, TEXTURE_NORMAL, TEXTURE_EMISSION均必须大写**

**示例：**

![img](file:////Users/wenyongyan/Library/Group%20Containers/UBF8T346G9.Office/TemporaryItems/msohtmlclip/clip_image004.jpg)


其中，渲染模型只能设置为PHONG或者FLAT，PHONG使用顶点法线的平滑插值进行渲染，因此会得到比较 平滑的渲染效果，FLAT直接使用平面法线进行渲染，在多边形面数较少时可能观察到比较明显的平面。

环境光系数决定了基础的光照强度，通常设定为比较低的值（例如0.1），漫反射系数可以决定物体漫反射光的强度，高光系数可以决定物体表面直接反射光线的强度，通常设定三个系数的和约为1左右，也可以根据想要达到的效果自由设置。高光衰减系数是一个大于0的参数，通常可设定为16或者更高，衰减系数越大，物体表面在不同位置观察到的镜面反射的衰减越快， 越接近真实的镜面效果。

下面是渲染配置文件的结构规范：

相机横向分辨率 相机竖向分辨率 传感器横向尺寸(单位mm) 传感器竖向尺寸(单位mm) 相机镜头焦距(单位 mm)

相机光心世界坐标（分别为x，y，z，均为右手系，单位为m）

相机观察中心世界坐标（分别为x，y，z，均为右手系，单位为m，它和光心位置决定了相机的朝向）

相机的竖直方向（分别为x，y，z，均为右手系，没有单位，是一个方向向量，它决定相机的正上方是哪个方向，一般设为z轴，即0， 0， 1）

光源类型（设定为SUN或者POINT）

<!--如果是POINT类型，配置信息如下-->

阴影贴图分辨率（只是一个数即可，例如设为2048，表示为2048 * 2048分辨率）

光源亮度（需要自行设置，如果设置为10000，表示在离光源100m的位置，光照强度为10000 / 1002）

光源世界坐标（分别为x，y，z，均为右手系，单位为m）

<!--如果是SUN类型，配置信息如下-->

阴影贴图分辨率（只是一个数即可，例如设为2048，表示为2048 * 2048分辨率）

阴影范围（即阴影在世界空间中的覆盖范围，通常设置为场景大小，例如模型大小约为5m，设置为6左右即可）

光源亮度（自行设置，一般设置为1左右，更高或更低会过亮或过暗）

光源中心的世界坐标（分别为x，y，z，均为右手系，单位为cm）

光源观察中心世界坐标（分别为x，y，z，均为右手系，单位为cm，它和光源中心位置决定了光照的方向）

光源的竖直方向（分别为x，y，z，均为右手系，没有单位，是一个方向向量，它决定光源的正上方是哪个方向， 一般设为z轴，即0， 0， 1）

<!--最后可以设置渲染配置信息-->

SHADOW ON（表示开启阴影渲染，默认开启）

SHADOW OFF（表示关闭阴影渲染）

CULL_MODE [BACK或者FRONT]（表示使用背面剔除，默认开启，可以不设置该项即不填该行）

SAMPLE_MAX [最大采样次数]（在光线追踪时有效，决定了每个像素的采样次数，采样次数越大噪点越少，但是需要很长的渲染时间，如果设置为1024一般的模型可能需要渲染20分钟或更长时间）

以上是配置文件的结构，实际在渲染我们的观察图像序列中，不要这么手动配置复杂的配置信息，项目中提供了事先写好的配置文件供测试使用，可以在基础上尝试修改查看效果。

下面是渲染参数的示例：

![img](file:////Users/wenyongyan/Library/Group%20Containers/UBF8T346G9.Office/TemporaryItems/msohtmlclip/clip_image006.jpg)

接下来需要在程序的可执行文件后加入程序参数，即C++主函数后面的argv变量，一共加入4个变量：分别是模型配置文件地址，渲染参数配置文件地址，渲染模式，渲染器（R表示光栅渲染器，RT是光线追踪渲染器）。其中，渲染模式共有3种，由0，1，2表示。0代表循环渲染（为了测试内存泄漏和帧数时使用)，1代表单张图像渲染（推荐使用此模式，只会渲染一张图像并输出到`~Renderer/Data/Output/image.png`，2代表图像序列渲染（是用于序列/动画渲染，渲染输出一系列图像，保存在文件夹`~/Renderer/Data/Animation`中，同时会输出相机参数信息，保存在`~/Renderer/Data/anime_config.txt`中，在此不多介绍了）。

示例：

![img](file:////Users/wenyongyan/Library/Group%20Containers/UBF8T346G9.Office/TemporaryItems/msohtmlclip/clip_image008.jpg) 

**注意：文件路径使用绝对路径还是相对路径跟具体的运行环境有关，需要自行配置。**

**注意：必须按照指定的格式写配置文件和设置程序参数，否则可能发生错误。**

## 运行实例

![img](file:////Users/wenyongyan/Library/Group%20Containers/UBF8T346G9.Office/TemporaryItems/msohtmlclip/clip_image010.jpg) 
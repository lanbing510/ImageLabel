# 图像标定工具
前段时间由于工作需要对图像中目标进行像素级标定，自己写的小工具，在此分享给大家

###环境：

QT+Opencv实现

###实现功能：

1 标定任意格式的图像

2 可手动绘制轮廓进行标定

3 可利用GrabCut进行半自动标定

4 支持Hull类型及Poly类型多种轮廓

5 支持各种后退重做等快捷键功能

6 可记录标定进度及标定所用时间

软件截图如下：

<img src="https://github.com/lanbing510/Label/raw/master/screenshots/label.png" width="60%" height="50%">

左侧第一个目标为GrabCut进行标定的示例，第二个为手动标定的结果


###快捷键说明：

1 CTRL+S Save，保存标定的图像

2 CTRL+R Redo，重新进行标定

3 CTRL+U Undo，后退一步

4 手动模式下，鼠标左键进行轮廓绘制，鼠标中键也可进行后退一步的操作，绘制完成后右键进行填充完成

5 自动模式下，鼠标左键直接在目标周围拖出一个矩形-->右键-->Ctrl+右键进行轮廓绘制-->Ctrl+右键填充;为了更精确的提取轮廓，可为GrabCut提供更多的信息：Shift+左键: GC_FG，Ctrl+左键: GC_BG，Alt+左键: GC_PR_FG，LeftClick: GC_PR_BG，参数相关信息可以查阅Opencv文档



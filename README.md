# CSC3002Project

一个假的命令行操作系统

基本逻辑：
“操作系统”启动后，其实是进入了一个while循环，在每一轮，我们接收用户的一个指令，并作出需要的操作。我们模仿DOS，在每一轮循环用户都会“在”某个目录下。（其实就是输出一段路径的字符串假装我们真的在那个目录下）

任务1: 用户管理系统

	所谓用户，我们其实就是在启动“系统”后给每个用户分配各自的一个“桌面”（创建一个文件夹作为某个用户的桌面，并在之后用户登录时将“当前路径”引导至用户的文件夹），每个用户都有对所有资源和命令的使用权。（当然，如果觉得任务太轻也可以再添加不同用户的不同权限。如果想加，这一部分应该需要后续再一起讨论）

	功能实现的最低要求：

	创建用户
	登陆用户
	切换用户
	注销用户
	删除用户
	记录已经登陆的用户
	记录存在的用户
	
任务2: 文件管理模块

我们所做的任何文件操作都将被限制在某个文件夹中。


	功能实现的最低要求：
	
	创建文件/文件夹
	删除文件/文件夹
	剪切文件
	复制文件
	
任务3: 应用管理模块

	我们所谓的应用，全部都是一些具有特定功能的函数。打开应用，实则是进入了某个函数。安装应用，我们模仿linux，通过install指令来安装我们预先准备好的一些应用。所谓安装，其实就是在某个路径下创建一个.app作为结尾的文件，用这个文件的名字来作为启动应用的命令。
	
	功能实现的最低要求：
	
	安装应用
	启动应用
	卸载应用
	
	写几个非常基础的应用（最基础可以基础到输入半径返回周长）


任务4: 进程管理模块
	
	我们在启动应用的时候，首先要告诉进程管理模块说我们建立了一个新进程。
	
	功能实现的最低要求：
	
	建立进程
	杀死进程
	
任务5: 内存管理模块

	我们在启动应用后，函数总是需要建立一些变量的。我们在建立变量的时候，要向内存管理模块确认还有足够的内存。
	
	功能实现的最低要求：
	
	确认内存是否足够
	记录内存的使用
    记录内存的释放
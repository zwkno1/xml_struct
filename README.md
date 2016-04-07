# xml_struct
神奇的代码
生成xml与结构体之间的转换的代码

src目录下编译可生成一个生成代码的程序generator
编写xml描述文件xxx,用命令行生成代码文件
./generator xxx

test文件夹下有个示例：
test.xml为用户编写的结构体描述文件
test.xml.h和test.xml.cpp为用gennerator生成的代码文件
该测试程序将一段xml转换成了结构体，然后将该结构体转回xml

gui目录下有个gui程序方便编辑xml描述文件

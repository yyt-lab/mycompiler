MiniDecafParser.h: antlr生成的parser代码
+ 针对每个规则生成一个类(如果标注了注释，则会以注释为名称生成类)
+ 每个类中有构造函数，以及每个token的类型，还有accept函数
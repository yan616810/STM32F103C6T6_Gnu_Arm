# 2025/8/14
    1.core_cm3.c文件中732行的__STREXB函数和__STREXH需要修改一下；&，memory
    
# 2025/8/11
    1.再系统时钟初始化前提前设置中断向量表偏移寄存器！注释掉system_stm32f10x.c文件中SystemInit()函数末尾的设置向量表偏移；将向量表设置操作移动到startup_stm32f103.c启动文件中Reset_Handler()函数内开头位置；

# 2025/8/10
    1.将SPL库中的system_stm32f10x.c移动到start文件夹中，因为要经常修改该文件中的宏定义来改变芯片主频；
    2.将SPL库中的stm32f10x.h移动到start文件夹中，因为修改使用的f10x系列的哪种型号；

# 2025/8/9
    1.第一次尝试使用gnu arm toolchain来生成stm32芯片可运行的固件
#pragma once
#include <afx.h>


//列出寄存器
bool ListRegister();

//单步事件.注意强断下来的时候,你可能需要先~ iNumber来切换到应该有的线程.
bool SetExecStepFlag();





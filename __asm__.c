#include <stdio.h>

int main(void){

	int a, b;
	int c, d;

	a = 4;
	b = 7;
	c = d = 0;

	printf("a # %d  b # %d  c # %d  d = %d\n", a, b, c, d);

	__asm__(
/* 变量 c, d, a, b 分别对应 1, 2, 3, 4 前面加%, 寄存器前面俩%% 用与区分 */
		"movl %2, %%eax;"       
		"movl %3, %%ebx;"
/* $1 表示立即数 */
		"subl $1, %%ebx;"        
		"movl %%eax, %0;"
		"movl %%ebx, %1;"
/* output部分“=” 修饰 */
		:"=r"(c),"=r"(d)        
/* input部分 */
		:"r"(a),"r"(b)          
/* 声明要是使用的寄存器, 这样Gcc就会协调寄存器, 这些寄存器在执行这段内联汇编时候不会被影响 */
		:"%eax","%ebx"         
	);
/* output部分的变量值会改变 :) */
	printf("a # %d  b # %d  c # %d  d = %d\n", a, b, c, d);

	return 0;
}

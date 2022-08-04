/* 
 * CS:APP Data Lab 
 * 
 * <Please put your name and userid here>
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~    
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting if the shift amount
     is less than 0 or greater than 31.


EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implement floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants. You can use any arithmetic,
logical, or comparison operations on int or unsigned data.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operations (integer, logical,
     or comparison) that you are allowed to use for your implementation
     of the function.  The max operator count is checked by dlc.
     Note that assignment ('=') is not counted; you may use as many of
     these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
//1
/* 
 * bitXor - x^y using only ~ and & 
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y) {
    return ~(~x & ~y) & ~(x & y);
}

/*
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
    return 1 << 31;
}
//2
/*
 * isTmax - returns 1 if x is the maximum, two's complement number,
 *     and 0 otherwise 
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */
int isTmax(int x) {
    //假设x是最大值，那么x = 0x8ffffff...
    //并且x+1应该是最小值
    int min = x + 1;    //min不能是0
    //min+x == 0xfffff,对0xfff进行取反那么必定是0
    int is = ~(min + x);  //is == 0
    return (!!min) & (!is);
}

/*
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allOddBits(int x) {
    //所有的奇位上的数都应该是1，返回1，否则返回0
    //得到偶数位上的数字全部是1，0x55555555|x ，应该得到全部是1，如果不是全部是1，那么说明x不是奇数全为0，(得到全为1的取反即可判断)
    int n = 0x55 << 24;
    n += 0x55 << 16;
    n += 0x55 << 8;
    n += 0x55;
    return !(~(n | x));
}

/*
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
    return (~x) + 1;
}
//3
/* 
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
//要求：范围在 [0x30,0x39]之内的返回1,其余的返回0
int isAsciiDigit(int x) {
    int a, u, b, l4, c, d;
    a = x >> 6;   //a must 0
    u = x & 0xff;  //获取低字节的数据   形如：0011 ____
    b = (u >> 4) ^ 3;   //b必定是2位  11^11 b must 0
    //最后获取u的后四位,找规律其中后四位  4321 其中43，42不能同时为1
    l4 = u & 0xf;  //获取最后的四位
    //判断43位是否同时为1
    c = (l4 & 0xc) ^ 0xc;  //c must not 0
    //判断42位是否同时为1
    d = (l4 & 0xa) ^ 0xa;  //d must not 0
    return (!(a | b)) & !(!c | !d);
}

/* 
 * conditional - same as x ? y : z 
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */

//参考别人的，核心思想没有想出来:0xfff...+1==0x000...
int conditional(int x, int y, int z) {
    //如果x != 0 return y, x==0 return z
    //分析0 ^ a = a,能不能让当x != 0时是  y^0   x == 0时是 0^z
    //也就是说   x!=0时   （y）|（0）  x==0时  （0）|（z）

    //判断x是否是0，x是0返回1.
    int is0 = !x;  //对x进行非操作，如果x==0返回1，如果x！=0返回1
    is0 = is0 + (~0x00);   //如果x是0，is0==0,如果x是1,is0=0xffff...
    return (is0 & y) | ((~is0) & z);
}

/*
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
    //分析比较两个数的大小，我们可以让两个数相减，因为已知  -x = ~x+1
    //需要进行y-x的操作，如果是x-y下面一步还需要进行判断x是否等于y
    int c = y + (~x + 1);  //执行y-x>=0操作判断,也就是说c>=0时return 1
    //判断c与0的情况我们只需要看c的最高位的时候，也就是说c的最高位==0时return 1
    int d = c >> 31;   //d 最高 == 1 return 0
    // 但是对于上面的操作我们需要进行溢出的注意，所以对于x与y是异号的情况需要直接返回就好了
    // 怎么书写x>0 && y<0  和  x<0 && y>0  直接return
    //获取x,y的高位
    int hx = (x >> 31) & 1;
    int hy = (y >> 31) & 1;
    int u = hx ^ hy;    //u==1代表高位异号，否则高位同号
    //u==1高位异号返回hx，高位同号返回!d
    //最后对x取-x的时候还要注意其中负数相对于正数的范围要大于一,当x==0x80000000的时候
    //对x进行取反+1 的情况是，-x = 0x8000 0000，当y == 0x8000 0000
    //y-x == 0x00000000 也就是说，y>=x
    return (u & hx) | ((~u) & !d);
}
//4
/* 
 * logicalNeg - implement the ! operator, using all of 
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int logicalNeg(int x) {
    //***
    //如果x==0，return 1，  x!=0  return 0
    //首先需要发现在0处的规律，要求这是其余的整数是没有的。
    //我们使用负数进行比较，可以发现一个数除了0之外，最高位  x|-x，最高位都是1
    //但是0|-0 的最高位是0
    int c = ~x + 1;
    int b = ((x | c) >> 31) & 1;
    return b ^ 0x1;
}

/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */

//参考
int howManyBits(int x) {
    // 最小二进制补码表示x
    // 正数负数大致查找思路一致，先假设所有的都是正数
    // 思路：从左向右进行二分查找，先查找前16位，如果前16位没有0进行后16位的查找，如果前16位存在0
    // 那么向右移动16位，继续进行后16位的查找，注意如果前16位存在1的话需要进行标记下，代表存在1，最后进行汇总
    // 首先获取最高位的值
    int fu, bit16, bit8, bit4, bit2, bit1;
    fu = x >> 31;  //获取最高位符号位
    // 对于负数来说，进行取反操作，查找第一个1答案也是没有变化的(主要是便于后面操作)，对于正数来说不需要进行变化
    // 也就是说fu==1，return ~x  fu==0，return x
    x = ((~fu) & x) | (fu & ~x);
    //开始查找第一个1，进行二分查找,判断前16位是否有1，如果没有返回0，如果有返回1
    bit16 = !!(x >> 16);
    //如果bit16==1那么向后移动16位,bit16==0，x不用动
    x = x >> (bit16 << 4);
    //二分查找前8位，如果前8位有1返回1，否则返回0
    bit8 = !!(x >> 8);
    //如果bit8==1那么向后移动8位，bit8==0，x不用动
    x = x >> (bit8 << 3);
    //二分查找前4位，如果前4位有1返回1，否则返回0
    bit4 = !!(x >> 4);
    //如果bit4==1那么向后移动4位，bit4==0，x不用动
    x = x >> (bit4 << 2);
    //二分查找前2位，如果前2位有1返回1，否则返回0
    bit2 = !!(x >> 2);
    //如果bit2==1那么向后移动2位，bit2==0，x不用动
    x = x >> (bit2 << 1);
    //终止部分
    //二分查找前1位，如果前1位有1返回1，否则返回0(注意如果前1位是1则代表)
    bit1 = !!(x >> 1);
    //如果bit1==1那么向后移动2位，bit1==0，x不用动
    x = x >> bit1;
    return (bit16 << 4) + (bit8 << 3) + (bit4 << 2) + (bit2 << 1) + bit1 + x + 1;
}
//float
/* 
 * floatScale2 - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */

//返回float*2  32位表示1,8,23
unsigned floatScale2(unsigned uf) {
    int e = uf & 0x7f800000;  //获取阶码e
    if (e == 0) {  //当前的e阶码是0代表的是非规格的数
        //获取尾数
        int f = uf & 0x007fffff;
        f = f << 1;  //尾数部分左移一位就是2倍，即使是当前非规格移动到规格上面去,变成了24位
        uf = (uf & 0x80000000) | f;
    } else if (e != 0x7f800000) {  //当前的阶码不是全是1
        //当前的阶码不是全为1，需要让阶码+1
        uf = uf + 0x00800000; //如果正好+1超出边界全为1，返回uf也可以
    }
    return uf;   //NaN
}

/*
 * floatFloat2Int - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */

//将float转换成int类型
int floatFloat2Int(unsigned uf) {
    //当超出限制的时候NaN返回INF
    unsigned INF = 1 << 31;
    //获取阶码e和符号位s
    int e = (uf >> 23) & 0xff;
    int s = (uf >> 31) & 1;
    //如果uf全部是0，直接返回0
    //e==0代表E = 2^(1-127),对于后面的尾数，右移126位直接返回0
    if (e == 0) return 0;
    //如果e阶码全是1，返回无穷，int表示不了
    if (e == 0xff) return INF;

    //将隐藏的1添加到符号位上，也就是说当前的值是：尾数位的前面添加上了1
    uf <<= 8;        //将高8位进行删除
    uf |= 1 << 31;   //添加上1，规格化隐藏的数字
    uf >>= 8;        //返回将高8位重新归0

    //e是真正的2的次幂，也就是尾数将要移动的位数
    e -= 127;
    //当e大于32的时候，代表的意思是位数需要左移32位，就会直接超出边界，直接返回INF
    if (e >= 32) return INF;
    //如果e<0，需要向右移动，也就是说该数的范围介于0-1之间，直接返回0
    if (e < 0) return 0;
    //我们进行查看对于当前的uf实际上是已经进行过移位操作的了
    //该1应该是处于右数第一位上，但是此时却是处在24位，
    //如果e是小于22的情况，需要向右移动
    //如果e==23可以不用移动
    if (e <= 22) uf >>= 23 - e;
    else uf <<= e - 23;
    //最后我们将符号位进行添加上去
    if (s) uf = ~uf + 1;
    return uf;
}

/*
 * floatPower2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 * 
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while 
 *   Max ops: 30 
 *   Rating: 4
 */

//2^x 得到的值转换成float表达形式
unsigned floatPower2(int x) {
    //首先定义一个float中的+无穷数字  0x7f800000
    unsigned INF = 0xff << 23;

    //分析对于规格化的情况仅仅需要阶码e即可完成2^x的范围表示
    //对于规格化的范围表示，x可以在[0x01-0xfe]范围内也就是[1-254]范围内
    //实际上需要进行偏移127
    int e = 127 + x;
    //当阶码e全部是0的时候可以表示1-127=-126的x的形式
    //如果过大返回INF表达不了
    if (e >= 255) return INF;
    //也就是说e在[1-254]范围内就是规格化表示
    //如果e==0那么就是对应非规格化的阶码是全0的情况
    //对于e<0的情况我们查看对于尾数能够贡献多少位，首先尾数一共有23位，0x00400000可以表示2^-127,同理0x00000001可以表示2^-149
    if (x <= -127 && x >= -149) { //在这个范围内是阶码为0由尾数进行贡献的
        //移动到尾数的位置
        return 1 << (x + 127 + 22);  //返回尾数表示的2^x,阶码全是0
    }
    if (x < -149) {
        return 0;
    }
    //将e代表左移23位，移动到表示阶码的位置
    return e << 23;
}

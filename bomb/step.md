# 第一个

首先使用gdb进行调试查看出现错误的地方在哪里

```gdb
73          input = read_line();             /* Get input                   */
(gdb)
123456
74          phase_1(input);                  /* Run the phase               */
(gdb)

BOOM!!!
The bomb has blown up.
[Inferior 1 (process 63) exited with code 010]
```

经过了gdb进行执行我们发现输入123456之后在函数phase_1(input)的时候发生了爆炸。下面的思考是使用反编译查看phase_1函数到底执行了什么东西。

```
(gdb) disas phase_1
Dump of assembler code for function phase_1:
   0x0000000000400ee0 <+0>:     sub    $0x8,%rsp
   0x0000000000400ee4 <+4>:     mov    $0x402400,%esi
   0x0000000000400ee9 <+9>:     callq  0x401338 <strings_not_equal>
   0x0000000000400eee <+14>:    test   %eax,%eax
   0x0000000000400ef0 <+16>:    je     0x400ef7 <phase_1+23>
   0x0000000000400ef2 <+18>:    callq  0x40143a <explode_bomb>
   0x0000000000400ef7 <+23>:    add    $0x8,%rsp
   0x0000000000400efb <+27>:    retq
End of assembler dump.
```

当执行完上面的第四行的时候，调用命令查看寄存器esi内的值，发现将立即数0x402400写入了esi寄存器中，然后吊起函数strings_not_equal，这个时候我们发现根据规则，esi寄存器存放的应该是第二个参数的位置所以我们使用相关的命令查看下第一个寄存器rdi存放的是什么参数。之后我们查看了寄存器中是0x603780，并且查看了这个0x603780地址里面是什么东西我们进行了6位的查看，后发现就是我刚才输入的6个字符，之后大概的思路就明白了，第一个参数是指针参数，该指针执行我们输入的char数组，然后该数组需要和地址是0x402400的地方一样吧，可能是这样，之后我们在进入strings_not_equal函数进行分析，查看是不是这样。

```
(gdb) p /x $esi
$3 = 0x402400
(gdb) p /x $rdi
$4 = 0x603780
(gdb) x/6b 0x603780
0x603780 <input_strings>:       0x31    0x32    0x33    0x34    0x35    0x36
```

```
(gdb) disas strings_not_equal
Dump of assembler code for function strings_not_equal:
   0x0000000000401338 <+0>:     push   %r12
   0x000000000040133a <+2>:     push   %rbp
=> 0x000000000040133b <+3>:     push   %rbx
   0x000000000040133c <+4>:     mov    %rdi,%rbx
   0x000000000040133f <+7>:     mov    %rsi,%rbp
   0x0000000000401342 <+10>:    callq  0x40131b <string_length>
   0x0000000000401347 <+15>:    mov    %eax,%r12d
   0x000000000040134a <+18>:    mov    %rbp,%rdi
   0x000000000040134d <+21>:    callq  0x40131b <string_length>
   0x0000000000401352 <+26>:    mov    $0x1,%edx
   0x0000000000401357 <+31>:    cmp    %eax,%r12d
   0x000000000040135a <+34>:    jne    0x40139b <strings_not_equal+99>
   0x000000000040135c <+36>:    movzbl (%rbx),%eax
   0x000000000040135f <+39>:    test   %al,%al
   0x0000000000401361 <+41>:    je     0x401388 <strings_not_equal+80>
   0x0000000000401363 <+43>:    cmp    0x0(%rbp),%al
   0x0000000000401366 <+46>:    je     0x401372 <strings_not_equal+58>
   0x0000000000401368 <+48>:    jmp    0x40138f <strings_not_equal+87>
   0x000000000040136a <+50>:    cmp    0x0(%rbp),%al
   0x000000000040136d <+53>:    nopl   (%rax)
   0x0000000000401370 <+56>:    jne    0x401396 <strings_not_equal+94>
   0x0000000000401372 <+58>:    add    $0x1,%rbx
   0x0000000000401376 <+62>:    add    $0x1,%rbp
   0x000000000040137a <+66>:    movzbl (%rbx),%eax
   0x000000000040137d <+69>:    test   %al,%al
   0x000000000040137f <+71>:    jne    0x40136a <strings_not_equal+50>
   0x0000000000401381 <+73>:    mov    $0x0,%edx
   0x0000000000401386 <+78>:    jmp    0x40139b <strings_not_equal+99>
   0x0000000000401388 <+80>:    mov    $0x0,%edx
   0x000000000040138d <+85>:    jmp    0x40139b <strings_not_equal+99>
   0x000000000040138f <+87>:    mov    $0x1,%edx
   0x0000000000401394 <+92>:    jmp    0x40139b <strings_not_equal+99>
   0x0000000000401396 <+94>:    mov    $0x1,%edx
   0x000000000040139b <+99>:    mov    %edx,%eax
   0x000000000040139d <+101>:   pop    %rbx
   0x000000000040139e <+102>:   pop    %rbp
   0x000000000040139f <+103>:   pop    %r12
   0x00000000004013a1 <+105>:   retq
End of assembler dump.
```

进入strings_not_equal进行分析，寄存器需要使用上面的三个所以先进行保存起来，对于上一个函数传递的参数寄存器rdi,rsi的值传递到rbx,rbp寄存器中。再然后就是进入string_length ()函数中。

```
Dump of assembler code for function string_length:
=> 0x000000000040131b <+0>:     cmpb   $0x0,(%rdi)
   0x000000000040131e <+3>:     je     0x401332 <string_length+23>
   0x0000000000401320 <+5>:     mov    %rdi,%rdx
   0x0000000000401323 <+8>:     add    $0x1,%rdx
   0x0000000000401327 <+12>:    mov    %edx,%eax
   0x0000000000401329 <+14>:    sub    %edi,%eax
   0x000000000040132b <+16>:    cmpb   $0x0,(%rdx)
   0x000000000040132e <+19>:    jne    0x401323 <string_length+8>
   0x0000000000401330 <+21>:    repz retq
   0x0000000000401332 <+23>:    mov    $0x0,%eax
   0x0000000000401337 <+28>:    retq
End of assembler dump.
```

在该函数中没有发现rsi寄存器的使用，猜测该函数应该只有一个参数，rdi寄存器中

```
每一行的分析与上面的函数相对应  ,分析string_length函数
根据b可以知道对于的是一个字节，(%rdi)寄存器中的地址处于0x0立即数进行比较
je 相等/零，如果相等的话跳转到0x401332也就是11行处，将0结果存到寄存器eax中进行返回
执行不相等的地方，将参数1寄存器rdi内容放到寄存器rdx中
然后rdx代表地址值+1
将加1之后的edx，放到寄存器eax  (代表的依旧是地址)
eax寄存器内容减去edi寄存器的内容，之后变成了0x1(eax寄存器代表的是返回值)
比较0x0和rdx，也就是参数的下一个地址
jne代表不相等/非0，也就是说不相等的时候跳转到0x401323的地方
//不断地进行比较rdx代表的地址+1，
//将rdx传到eax，将+2后的eax减去之前的参数edi，是2
//直到ret，
```

之后回到调用完函数string_length的地方第9行，我们发现将返回的参数放到了寄存器r12d里面，之后我们对于第二个地址rbp也就是0x402400的地方也调用string_length

| 行数(代表执行完某一行) | %rdi(参1) | %rsi(参2) | %rax(返回) | %rbx     | %rdx | %rbp     | %r12d |
| ---------------------- | --------- | --------- | ---------- | -------- | ---- | -------- | ----- |
| 开始                   | 0x603780  | 0x402400  |            |          |      |          |       |
| ....134a指令处完       | 0x402400  | 0x402400  | 0x34       | 0x603780 |      | 0x402400 | 0x34  |
| ....1352完             | 0x402400  | 0x402400  | 0x34       | 0x603780 | 0x1  | 0x402400 | 0x34  |
| 。。135c完             | 0x402400  | 0x402400  | 0x42       | 0x603780 | 0x1  | 0x402400 | 0x34  |

```
分析strings_not_equal函数
最初是将传递的两个参数保存到另外的两个寄存器中
参数1吊起函数length，将返回的答案保存到r12d寄存器中，然后将第二个参数放到rdi(传参寄存器1中)继续吊起函数
然后将两个字符串的长度进行比较是否相等，不相等的话就直接将edx寄存器的0x1放到eax寄存器中直接返回
如果相等的话
首先将rbx内的数字取出内存的值，放到rax中，然后在比较是否rax的值是否为=0x0，如果是0x0的话，跳转到388指令，将0放到寄存器edx中
也就是返回值变成了0，到了0x0也就是比较到了最后都是一样的情况
如果当前rax的值不是0x0代表没有到达最后，需要取出rbp寄存器的值于rax寄存器的值进行比较(注意我所说的比较都是char比较，并不是64位寄存器比较)，如果相等的话跳到1372，寄存器rbx，rbp都+1，然后继续比较。
```

## Border relations with Canada have never been better.







# 第二个输入

先查看read_six_numbers

```
000000000040145c <read_six_numbers>:
  40145c:	48 83 ec 18          	sub    $0x18,%rsp
  401460:	48 89 f2             	mov    %rsi,%rdx
  401463:	48 8d 4e 04          	lea    0x4(%rsi),%rcx
  401467:	48 8d 46 14          	lea    0x14(%rsi),%rax
  40146b:	48 89 44 24 08       	mov    %rax,0x8(%rsp)
  401470:	48 8d 46 10          	lea    0x10(%rsi),%rax
  401474:	48 89 04 24          	mov    %rax,(%rsp)
  401478:	4c 8d 4e 0c          	lea    0xc(%rsi),%r9
  40147c:	4c 8d 46 08          	lea    0x8(%rsi),%r8
  401480:	be c3 25 40 00       	mov    $0x4025c3,%esi
  401485:	b8 00 00 00 00       	mov    $0x0,%eax
  40148a:	e8 61 f7 ff ff       	call   400bf0 <__isoc99_sscanf@plt>
  40148f:	83 f8 05             	cmp    $0x5,%eax
  401492:	7f 05                	jg     401499 <read_six_numbers+0x3d>
  401494:	e8 a1 ff ff ff       	call   40143a <explode_bomb>
  401499:	48 83 c4 18          	add    $0x18,%rsp
  40149d:	c3                   	ret    
```

首先分析read_six_numbers发现里面含有scanf接着发现scanf的返回值应该与0x5进行比较大小，jg>，eax>5的时候会跳过炸弹，我们查看调用scanf函数之前的操作，发现寄存器%r9都使用了，说明可能是传参是6个，根据scanf输入形式，我们需要输出6个数字，中间使用空格隔开，scanf的传参并不是很明白，参数第三个才是我们输入的地址的值，按理说应该是第二个。注意我们输入数字的地址存放的地址是：

```
read_six_numbers主要的操作时将输入的字符存放到0x7ffffffee1a0（rsp寄存器的值），地址位置处
```

```
(gdb) p *(int *) 0x7ffffffee1a0
$12 = 1
(gdb) p *(int *) 0x7ffffffee1a4
$13 = 2
(gdb) p *(int *) 0x7ffffffee1a8
$14 = 3
(gdb) p *(int *) 0x7ffffffee1ac
$15 = 4
(gdb) p *(int *) 0x7ffffffee1b0
$16 = 5
(gdb) p *(int *) 0x7ffffffee1b4
$17 = 6
```

回到函数phase_2我们发现

```
0000000000400efc <phase_2>:
  400efc:  55                     push   %rbp
  400efd:  53                     push   %rbx
  400efe:  48 83 ec 28            sub    $0x28,%rsp
  400f02:  48 89 e6               mov    %rsp,%rsi
  400f05:  e8 52 05 00 00         call   40145c <read_six_numbers>
  400f0a:  83 3c 24 01            cmpl   $0x1,(%rsp)
  400f0e:  74 20                  je     400f30 <phase_2+0x34>
  400f10:  e8 25 05 00 00         call   40143a <explode_bomb>
  400f15:  eb 19                  jmp    400f30 <phase_2+0x34>
  400f17:  8b 43 fc               mov    -0x4(%rbx),%eax
  400f1a:  01 c0                  add    %eax,%eax
  400f1c:  39 03                  cmp    %eax,(%rbx)
  400f1e:  74 05                  je     400f25 <phase_2+0x29>
  400f20:  e8 15 05 00 00         call   40143a <explode_bomb>
  400f25:  48 83 c3 04            add    $0x4,%rbx
  400f29:  48 39 eb               cmp    %rbp,%rbx
  400f2c:  75 e9                  jne    400f17 <phase_2+0x1b>
  400f2e:  eb 0c                  jmp    400f3c <phase_2+0x40>
  400f30:  48 8d 5c 24 04         lea    0x4(%rsp),%rbx
  400f35:  48 8d 6c 24 18         lea    0x18(%rsp),%rbp
  400f3a:  eb db                  jmp    400f17 <phase_2+0x1b>
  400f3c:  48 83 c4 28            add    $0x28,%rsp
  400f40:  5b                     pop    %rbx
  400f41:  5d                     pop    %rbp
  400f42:  c3                     ret    
```

```
rsp            0x7ffffffee1a0
(gdb) p *(int *) $rsp
$29 = 1
```

此时的rsp寄存器指向的地址的内存是1，回到第7行

```
7  将0x1和寄存器rsp中指向的内存之进行比较
je  等于0x1那么就会跳过爆炸
跳到20行
rsp寄存器的值+0x4放到rbx中
rsp寄存器的值+0x18放到rbp中
无条件跳到第11行
将rbx-0x4值的内存地址放到eax寄存器中，rax       0x1
然后eax+eax放到eax  =   0x2
比较eax和寄存器值rbx指向的内存值是否相等，相等的话跳过炸弹 (也就是说下一地址+4的值应该是上一个的2倍)。
```

## 1 2 4 8 16 32



# 第三个输入

```
0000000000400f43 <phase_3>:
  400f43:  48 83 ec 18            sub    $0x18,%rsp
  400f47:  48 8d 4c 24 0c         lea    0xc(%rsp),%rcx
  400f4c:  48 8d 54 24 08         lea    0x8(%rsp),%rdx
  400f51:  be cf 25 40 00         mov    $0x4025cf,%esi
  400f56:  b8 00 00 00 00         mov    $0x0,%eax
  400f5b:  e8 90 fc ff ff         call   400bf0 <__isoc99_sscanf@plt>  
  400f60:  83 f8 01               cmp    $0x1,%eax
  400f63:  7f 05                  jg     400f6a <phase_3+0x27>
  400f65:  e8 d0 04 00 00         call   40143a <explode_bomb>
  400f6a:  83 7c 24 08 07         cmpl   $0x7,0x8(%rsp)
  400f6f:  77 3c                  ja     400fad <phase_3+0x6a>
  400f71:  8b 44 24 08            mov    0x8(%rsp),%eax
  400f75:  ff 24 c5 70 24 40 00   jmp    *0x402470(,%rax,8)
  400f7c:  b8 cf 00 00 00         mov    $0xcf,%eax
  400f81:  eb 3b                  jmp    400fbe <phase_3+0x7b>
  400f83:  b8 c3 02 00 00         mov    $0x2c3,%eax
  400f88:  eb 34                  jmp    400fbe <phase_3+0x7b>
  400f8a:  b8 00 01 00 00         mov    $0x100,%eax
  400f8f:  eb 2d                  jmp    400fbe <phase_3+0x7b>
  400f91:  b8 85 01 00 00         mov    $0x185,%eax
  400f96:  eb 26                  jmp    400fbe <phase_3+0x7b>
  400f98:  b8 ce 00 00 00         mov    $0xce,%eax
  400f9d:  eb 1f                  jmp    400fbe <phase_3+0x7b>
  400f9f:  b8 aa 02 00 00         mov    $0x2aa,%eax
  400fa4:  eb 18                  jmp    400fbe <phase_3+0x7b>
  400fa6:  b8 47 01 00 00         mov    $0x147,%eax
  400fab:  eb 11                  jmp    400fbe <phase_3+0x7b>
  400fad:  e8 88 04 00 00         call   40143a <explode_bomb>
  400fb2:  b8 00 00 00 00         mov    $0x0,%eax
  400fb7:  eb 05                  jmp    400fbe <phase_3+0x7b>
  400fb9:  b8 37 01 00 00         mov    $0x137,%eax
  400fbe:  3b 44 24 0c            cmp    0xc(%rsp),%eax
  400fc2:  74 05                  je     400fc9 <phase_3+0x86>
  400fc4:  e8 71 04 00 00         call   40143a <explode_bomb>
  400fc9:  48 83 c4 18            add    $0x18,%rsp
  400fcd:  c3                     ret
```



对于scanf函数我们可以进行尝试，对于第8行我们可以发现返回值scanf必须大于1，对于不同的输入我们尝试多个输入后发现scanf的返回值只有2，可以发现我们输入的数字是2个。

在执行完毕scanf之后我们查看一下我们输入的参数放在了哪里。

```
1 311            

(gdb) p $rsp
$13 = (void *) 0x7fffffffe060

//写入的数据放到rsp+8与rsp+c的地址处
(gdb) x/2w 0x7fffffffe068
0x7fffffffe068: 0x00000001      0x00000137

x/16g 0x402470
```

400f6a指令我们需要0x8(%rsp) 也就是我们输入的第一个数据与7进行无符号数据比较，第一个数必须小于等于7，否则爆炸。

```
jmp    *0x402470(,%rax,8)      
跳转到内存地址是:（rax*8+0x402470的位置处）的值
先查看下0x402470附近都有什么值

(gdb) x/7g 0x402470
0x402470:       0x0000000000400f7c      0x0000000000400fb9
0x402480:       0x0000000000400f83      0x0000000000400f8a
0x402490:       0x0000000000400f91      0x0000000000400f98
0x4024a0:       0x0000000000400f9f

400f7c:	b8 cf 00 00 00       	mov    $0xcf,%eax                跳转0、将0xcf放到eax寄存器中(0,0xcf)
400fb9:	b8 37 01 00 00       	mov    $0x137,%eax               跳转1、将0x137放到eax寄存器中(1,0x137)
400f83:	b8 c3 02 00 00       	mov    $0x2c3,%eax               跳转2、将0x2c3放到寄存器中(2,0x2c3)
400f8a:	b8 00 01 00 00       	mov    $0x100,%eax               跳转3、将0x100放到寄存器中(3,0x100)
400f91:	b8 85 01 00 00       	mov    $0x185,%eax               跳转4、将0x185放到寄存器中(4,0x185)
400f98:	b8 ce 00 00 00       	mov    $0xce,%eax                跳转5、将0xce放到寄存器中(5,0xce)
400f9f:	b8 aa 02 00 00       	mov    $0x2aa,%eax               跳转6、将0x2aa放到寄存器中(6,0x2aa)
400fa6:	b8 47 01 00 00       	mov    $0x147,%eax               跳转7、将0x147放到寄存器中(7,0x147)
共有7个答案
最后输入的第二个数需要和eax进行比较，按照后面的对应
```

## 答案 :

0 207

1 311

2 707

3 256

4 389

5 206

6 682

7 327



# 第四个输入

```
000000000040100c <phase_4>:
  40100c:  48 83 ec 18            sub    $0x18,%rsp
  401010:  48 8d 4c 24 0c         lea    0xc(%rsp),%rcx
  401015:  48 8d 54 24 08         lea    0x8(%rsp),%rdx
  40101a:  be cf 25 40 00         mov    $0x4025cf,%esi
  40101f:  b8 00 00 00 00         mov    $0x0,%eax
  401024:  e8 c7 fb ff ff         call   400bf0 <__isoc99_sscanf@plt>
  401029:  83 f8 02               cmp    $0x2,%eax
  40102c:  75 07                  jne    401035 <phase_4+0x29>
  40102e:  83 7c 24 08 0e         cmpl   $0xe,0x8(%rsp)
  401033:  76 05                  jbe    40103a <phase_4+0x2e>
  401035:  e8 00 04 00 00         call   40143a <explode_bomb>
  40103a:  ba 0e 00 00 00         mov    $0xe,%edx
  40103f:  be 00 00 00 00         mov    $0x0,%esi
  401044:  8b 7c 24 08            mov    0x8(%rsp),%edi
  401048:  e8 81 ff ff ff         call   400fce <func4>
  40104d:  85 c0                  test   %eax,%eax
  40104f:  75 07                  jne    401058 <phase_4+0x4c>
  401051:  83 7c 24 0c 00         cmpl   $0x0,0xc(%rsp)
  401056:  74 05                  je     40105d <phase_4+0x51>
  401058:  e8 dd 03 00 00         call   40143a <explode_bomb>
  40105d:  48 83 c4 18            add    $0x18,%rsp
  401061:  c3                     ret    
```

```
scanf的返回值eax与0x2进行比较
eax!=2会爆炸，输入的数据需要时两个数
输入的第1个数据和0xe进行比较
无符号比较，jbe：<=    也就是说输入的第一个数必须小于等于0xe
将0xe放入edx中
将0x0放入esi中
将输入的第一个数放到edi寄存器中
吊起func4函数传入该函数的值是(输入的第一个数，0，e)
函数的返回值!=0会爆炸
比较输入的第二个数和0的关系
== 的话才不会爆炸
//就是说输入的第一个数必须小于0xe，第二个数必须是0
//第一个数作为参数传入fun4函数中，必须让该函数返回值是0
```

```
0000000000400fce <func4>:
  400fce:  48 83 ec 08            sub    $0x8,%rsp
  400fd2:  89 d0                  mov    %edx,%eax
  400fd4:  29 f0                  sub    %esi,%eax
  400fd6:  89 c1                  mov    %eax,%ecx
  400fd8:  c1 e9 1f               shr    $0x1f,%ecx
  400fdb:  01 c8                  add    %ecx,%eax
  400fdd:  d1 f8                  sar    %eax
  400fdf:  8d 0c 30               lea    (%rax,%rsi,1),%ecx
  400fe2:  39 f9                  cmp    %edi,%ecx
  400fe4:  7e 0c                  jle    400ff2 <func4+0x24>
  400fe6:  8d 51 ff               lea    -0x1(%rcx),%edx
  400fe9:  e8 e0 ff ff ff         call   400fce <func4>
  400fee:  01 c0                  add    %eax,%eax
  400ff0:  eb 15                  jmp    401007 <func4+0x39>
  400ff2:  b8 00 00 00 00         mov    $0x0,%eax
  400ff7:  39 f9                  cmp    %edi,%ecx
  400ff9:  7d 0c                  jge    401007 <func4+0x39>
  400ffb:  8d 71 01               lea    0x1(%rcx),%esi
  400ffe:  e8 cb ff ff ff         call   400fce <func4>
  401003:  8d 44 00 01            lea    0x1(%rax,%rax,1),%eax
  401007:  48 83 c4 08            add    $0x8,%rsp
  40100b:  c3                     ret   
```



```c
//一步一步的写汇编代码和c语言的关系，然后在化简汇编代码
//其中rdi  a,   rsi  b,  rdx   c,  t1   eax,  t2   ecx
int func4(int a,int b,int c){
    int t1 = c-b;
    int t2 = t1 >> 31;
    t1 = (t1 + t2) >> 1;
    t2 = t1 + b;
    if(t2 > a){
        c = t2 - 1;
        return 2* func4(a,b,c);
    } else{
        t1 = 0;
        if(t2 < a){
            b = t2 + 1;
            return 2* func4(a,b,c)+1;
        }
        return t1;
    }
}
```

查找在a的范围内函数func4函数返回是0的情况，[0,0xe]范围内

## 答案：

0 0    

1 0

3 0

7 0







# 第五个

```
0000000000401062 <phase_5>:
  401062:  53                     push   %rbx
  401063:  48 83 ec 20            sub    $0x20,%rsp
  401067:  48 89 fb               mov    %rdi,%rbx
  40106a:  64 48 8b 04 25 28 00   mov    %fs:0x28,%rax
  401071:  00 00 
  401073:  48 89 44 24 18         mov    %rax,0x18(%rsp)
  401078:  31 c0                  xor    %eax,%eax
  40107a:  e8 9c 02 00 00         call   40131b <string_length>
  40107f:  83 f8 06               cmp    $0x6,%eax
  401082:  74 4e                  je     4010d2 <phase_5+0x70>
  401084:  e8 b1 03 00 00         call   40143a <explode_bomb>
  401089:  eb 47                  jmp    4010d2 <phase_5+0x70>
  40108b:  0f b6 0c 03            movzbl (%rbx,%rax,1),%ecx
  40108f:  88 0c 24               mov    %cl,(%rsp)
  401092:  48 8b 14 24            mov    (%rsp),%rdx
  401096:  83 e2 0f               and    $0xf,%edx
  401099:  0f b6 92 b0 24 40 00   movzbl 0x4024b0(%rdx),%edx
  4010a0:  88 54 04 10            mov    %dl,0x10(%rsp,%rax,1)
  4010a4:  48 83 c0 01            add    $0x1,%rax
  4010a8:  48 83 f8 06            cmp    $0x6,%rax
  4010ac:  75 dd                  jne    40108b <phase_5+0x29>
  4010ae:  c6 44 24 16 00         movb   $0x0,0x16(%rsp)
  4010b3:  be 5e 24 40 00         mov    $0x40245e,%esi
  4010b8:  48 8d 7c 24 10         lea    0x10(%rsp),%rdi
  4010bd:  e8 76 02 00 00         call   401338 <strings_not_equal>
  4010c2:  85 c0                  test   %eax,%eax
  4010c4:  74 13                  je     4010d9 <phase_5+0x77>
  4010c6:  e8 6f 03 00 00         call   40143a <explode_bomb>
  4010cb:  0f 1f 44 00 00         nopl   0x0(%rax,%rax,1)
  4010d0:  eb 07                  jmp    4010d9 <phase_5+0x77>
  4010d2:  b8 00 00 00 00         mov    $0x0,%eax
  4010d7:  eb b2                  jmp    40108b <phase_5+0x29>
  4010d9:  48 8b 44 24 18         mov    0x18(%rsp),%rax
  4010de:  64 48 33 04 25 28 00   xor    %fs:0x28,%rax
  4010e5:  00 00 
  4010e7:  74 05                  je     4010ee <phase_5+0x8c>
  4010e9:  e8 42 fa ff ff         call   400b30 <__stack_chk_fail@plt>
  4010ee:  48 83 c4 20            add    $0x20,%rsp
  4010f2:  5b                     pop    %rbx
  4010f3:  c3                     ret    
```



```
第五行主要是为了缓冲区溢出保护生成一个随机的金丝雀数值
```

之后我们查看将金丝雀放到rsp寄存器下面的0x18的内存中位置处。

我们可以查看上面的开始和结束的情况，最初随机化一个数(金丝雀)放到rax寄存器中，最后使用xor进行je检测是否为0，则跳过fail函数，否则说明金丝雀被破坏，进行fail函数。

```
14行开始解释
rbx(输入字符串指针)+rax=0，也就是我们输入的第一个值放到ecx寄存器中
ecx的低8位放到rsp内存位置处(输入的字符ascii码放到rsp位置处)
将rsp内存位置处64位数据放到rdx寄存器中(取出的是64位放到rdx中)
1111&edx数据，获取的是我们输入的字符的ascii的后四位 (重要)
将地址0x4024b0和我们输入的字符串的后四位相加后取地址放到edx寄存器中
%edx后8位放到rsp+rax*1+10的位置处
rax+=0x1         便于下一次循环dl放置的位置
rax和0x6比较
```





查看以下我们输入的值:123456

```
(gdb) x/6b 0x6038c0
0x6038c0 <input_strings+320>:   0x31    0x32    0x33    0x34    0x35    0x36

获取上面的后四位的值
1 2 3 4 5 6

查看下位置是0x4024b0位置的值
(gdb) x/6b 0x4024b0
0x4024b0 <array.3449>:  0x6d    0x61    0x64    0x75    0x69    0x65

0x4024b0加上上面的1 2 3 4 5 6的偏移量取地址存放起来。
查看rsp寄存器
(gdb) p $rsp
$25 = (void *) 0x7ffffffee1b0
查看由我们决定的rsp寄存器后0x10位
(gdb) x/6b 0x7ffffffee1c0
0x7ffffffee1c0: 0x61    0x64    0x75    0x69    0x65    0x72

因为我输入的是1 2 3 4 5 6
所以rsp寄存器后0x10位的应该是地址值是0x4024b0的地址偏移内存值的情况
最后我们可以查看0x4024b0内存位置的附近的值与应该对比0x40245e的地址值进行比较，算出地址偏差

0x4024b0 <array.3449>:  0x6d    0x61    0x64    0x75    0x69    0x65    0x72    0x73
0x4024b8 <array.3449+8>:        0x6e    0x66    0x6f    0x74    0x76    0x62    0x79    0x6c

(gdb) x/6b 0x40245e
0x40245e:       0x66    0x6c    0x79    0x65    0x72    0x73
```

后四位代表的数字

0x66应该偏移9            I

0x6c应该偏移F            O

0x79应该便宜E           N

0x65应该便宜5           E

0x72应该便宜6           F

0x73应该便宜7           G



## 答案`IONEFG`



# 第6个

以1 2 3 4 5 6为例：（介绍的不好，先看下面的正确的答案贼好）

```
最初输入的数据放到0x603910地方
(gdb) x/12bx 0x603910
0x603910 <input_strings+400>:   0x31    0x20    0x32    0x20    0x33    0x20    0x34    0x20
0x603918 <input_strings+408>:   0x35    0x20    0x36    0x00
```

其中read_six_numbers函数的目的是，将我们输入的数字放到rsp寄存器指向的地址的位置处：

```
(gdb) p $rsp
$1 = (void *) 0x7fffffffdff0
(gdb) x/6wx 0x7fffffffdff0
0x7fffffffdff0: 0x00000001      0x00000002      0x00000003      0x00000004
0x7fffffffe000: 0x00000005      0x00000006
```



然后会进行每两两元素需要互不相等的判断操作

判断完毕之后会让7减去输入的数据

```
(gdb) x/6wx 0x7fffffffdff0
0x7fffffffdff0: 0x00000006      0x00000005      0x00000004      0x00000003
0x7fffffffe000: 0x00000002      0x00000001
```



对于这个地址进行查看信息0x6032d0将该地址放到edx中，然后不断的更新rdx寄存器的值，并将rdx寄存器的值传递到内存rsp+0x20+rsi*2的地址处。查看情况。

注意更新的内存的值的情况和我们输入的次数有关，比如说第一个输入的是1，查看eax寄存器的情况判断循环地址的次数，查看地址，该地址和输入的字符有关。

```
(gdb) x/6gx 0x7fffffffdff0+0x20
0x7fffffffe010: 0x0000000000603320      0x0000000000603310
0x7fffffffe020: 0x0000000000603300      0x00000000006032f0
0x7fffffffe030: 0x00000000006032e0      0x00000000006032d0
```





后面的循环有在内存中存储了，我们重新修改了0x6032e0处的地址值。

```
(gdb) x/10gx 0x6032e0+0x8
0x6032e8 <node2+8>:     0x00000000006032d0      0x000000030000039c
0x6032f8 <node3+8>:     0x00000000006032e0      0x00000004000002b3
0x603308 <node4+8>:     0x00000000006032f0      0x00000005000001dd
0x603318 <node5+8>:     0x0000000000603300      0x00000006000001bb
0x603328 <node6+8>:     0x0000000000603310      0x0000000000000000
查看左边的16位，第一个地址并没有存储
```



```
4011df: 48 8b 43 08            mov    0x8(%rbx),%rax     
4011e3:    8b 00                  mov    (%rax),%eax
有这个过程我们可以看出发生了有指针指向了指针参数
```





```
(gdb) x/24wx 0x6032d0
0x6032d0 <node1>:       0x0000014c      0x00000001      0x006032e0      0x00000000
0x6032e0 <node2>:       0x000000a8      0x00000002      0x006032f0      0x00000000
0x6032f0 <node3>:       0x0000039c      0x00000003      0x00603300      0x00000000
0x603300 <node4>:       0x000002b3      0x00000004      0x00603310      0x00000000
0x603310 <node5>:       0x000001dd      0x00000005      0x00603320      0x00000000
0x603320 <node6>:       0x000001bb      0x00000006      0x00000000      0x00000000
```



## 4 3 2 1 6 5

先查看最初存放的位置

```
(gdb) p /x $rdi
$23 = 0x603910
(gdb) x/12bx $rdi
0x603910 <input_strings+400>:   0x34    0x20    0x33    0x20    0x32    0x20    0x31    0x20
0x603918 <input_strings+408>:   0x36    0x20    0x35    0x00
```



查看read函数之后的位置

```
(gdb) p $rsp
$24 = (void *) 0x7fffffffe000
(gdb) x/6wxx $rsp
0x7fffffffe000: 0x00000004      0x00000003      0x00000002      0x00000001
0x7fffffffe010: 0x00000006      0x00000005
```



经过减去7之后的情况

```
(gdb) x/6wxx $rsp
0x7fffffffe000: 0x00000003      0x00000004      0x00000005      0x00000006
0x7fffffffe010: 0x00000001      0x00000002
```



```
0x6032d0地址附近的值
最初是0x6032d0+0x8在取地址
1、
(gdb) x/1wxx 0x6032d8
0x6032d8 <node1+8>:     0x006032e0
(gdb) p /x $rdx
$28 = 0x6032e0
然后是0x6032e0+0x8取地址
2、
(gdb) x/1wxx 0x6032e8
0x6032e8 <node2+8>:     0x006032f0
(gdb) p /x $rdx
$31 = 0x6032f0      
```

```
继续下去3、
(gdb) x/1wxx 0x6032f8
0x6032f8 <node3+8>:     0x00603300

(gdb) x/1wxx 0x603308
0x603308 <node4+8>:     0x00603310

(gdb) x/1wxx 0x603318
0x603318 <node5+8>:     0x00603320
```

下面是输入1，2，3，4，5，6分别对应的地址

rsp+0x20位置之后

```
1           2               3              4              5              6
0x6032d0    0x006032e0     0x006032f0     0x00603300     0x00603310     0x00603320
```

第一次循环之后

将获得的rdx放到0x20(%rsp,%rsi,2)    rsp+rsi*2+0x20位置处

```
(gdb) x/1gx $rsp+0x20
0x7fffffffe020: 0x00000000006032f0
```

本次循环之后

查看该地址与减7之后的情况一致

```
(gdb) x/6gx $rsp+0x20
                 3                       4
0x7fffffffe020: 0x00000000006032f0      0x0000000000603300
                 5                       6
0x7fffffffe030: 0x0000000000603310      0x0000000000603320
                 1                       2
0x7fffffffe040: 0x00000000006032d0      0x00000000006032e0
```



下一个循环之后并没有进行改变值(跳过)

```
(gdb) x/6gx $rsp+0x20
0x7fffffffe020: 0x00000000006032f0      0x0000000000603300
0x7fffffffe030: 0x0000000000603310      0x0000000000603320
0x7fffffffe040: 0x00000000006032d0      0x00000000006032e0
```

之后就是按照上面的地址进行比较应该让其降序排列

```
查看0x6032d0范围的数据值 
```

```
(gdb) x/24d 0x6032d0
0x6032d0 <node1>:       332     1       6304480 0
0x6032e0 <node2>:       168     2       0       0
0x6032f0 <node3>:       924     3       6304512 0
0x603300 <node4>:       691     4       6304528 0
0x603310 <node5>:       477     5       6304544 0
0x603320 <node6>:       443     6       6304464 0
```

因为是32位比较只需要看前面的第一列

3 4 5 6 1 2按照从大到小应该是，但是需要与7 进行相减为 4 3 2 1 6 5

# 完工！！！

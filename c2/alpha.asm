[bits 32]

[global _alpha]

[section .text]

_alpha:
    ; 初始化代码
    push ebp ; ebp 压栈 
    mov ebp,esp ; 保存 esp 到 ebp 
    mov edi,0x7e0f81f ; dx=00000111111000001111100000011111
    add esp,8 ; esp 指向参数 c1 
    pop eax ; 弹出 c1 到 ax 
    pop ebx ; 弹出 c2 到 bx 
    ; 处理颜色
    mov cx,ax ; cx=r1..b1
    mov dx,ax ; dx=r2..b2
    sal eax,16 ; eax=r1g1b1......
    sal ebx,16 ; ebx=r2g2b2......
    mov ax,cx ; eax=r1g1b1r1g1b1
    mov bx,dx ; ebx=r2g2b2r2g2b2
    and eax,edi ; eax=..g1..r1..b1 
    pop esi ; 弹出 alpha
    mul esi ; eax*=alpha
    neg esi ; -alpha
    and ebx,edi ; ebx=..g2..r2..b2 
    add esi,0x20 ; 32-alpha
    xchg eax,ebx ; 交换 eax,ebx
    mul esi ; c2*=(32-alpha)
    add eax,ebx ; c1*alpha+c2*(32-alpha)
    mov esp,ebp 
    sar eax,5 ;color=(c1*alpha+c2*(32-alpha))/32
    ;还原成 RGB 形式
    pop ebp 
    and eax,edi ; color=..g..r..b
    mov cx,ax ;
    sar eax,16 ; 
    or ax,cx ; color=rgb (eax)
    ret
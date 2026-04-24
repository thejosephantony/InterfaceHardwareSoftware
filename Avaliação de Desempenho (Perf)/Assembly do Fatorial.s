0000000000001241 <fatorial_r>:
    1241:       f3 0f 1e fa             endbr64
    1245:       55                      push   rbp
    1246:       48 89 e5                mov    rbp,rsp
    1249:       53                      push   rbx
    124a:       48 83 ec 18             sub    rsp,0x18
    124e:       ff 15 94 2d 00 00       call   QWORD PTR [rip+0x2d94]        # 3fe8 <mcount@GLIBC_2.2.5>
    1254:       89 7d ec                mov    DWORD PTR [rbp-0x14],edi
    1257:       83 7d ec 01             cmp    DWORD PTR [rbp-0x14],0x1
    125b:       77 07                   ja     1264 <fatorial_r+0x23>
    125d:       b8 01 00 00 00          mov    eax,0x1
    1262:       eb 14                   jmp    1278 <fatorial_r+0x37>
    1264:       8b 5d ec                mov    ebx,DWORD PTR [rbp-0x14]
    1267:       8b 45 ec                mov    eax,DWORD PTR [rbp-0x14]
    126a:       83 e8 01                sub    eax,0x1
    126d:       89 c7                   mov    edi,eax
    126f:       e8 cd ff ff ff          call   1241 <fatorial_r>
    1274:       48 0f af c3             imul   rax,rbx
    1278:       48 8b 5d f8             mov    rbx,QWORD PTR [rbp-0x8]
    127c:       c9                      leave
    127d:       c3                      ret
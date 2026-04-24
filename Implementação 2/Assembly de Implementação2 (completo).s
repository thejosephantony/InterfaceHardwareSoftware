
implementação2.elf:     file format elf64-x86-64


Disassembly of section .interp:

0000000000000318 <.interp>:
 318:   2f                      (bad)
 319:   6c                      ins    BYTE PTR es:[rdi],dx
 31a:   69 62 36 34 2f 6c 64    imul   esp,DWORD PTR [rdx+0x36],0x646c2f34
 321:   2d 6c 69 6e 75          sub    eax,0x756e696c
 326:   78 2d                   js     355 <__abi_tag-0x37>
 328:   78 38                   js     362 <__abi_tag-0x2a>
 32a:   36 2d 36 34 2e 73       ss sub eax,0x732e3436
 330:   6f                      outs   dx,DWORD PTR ds:[rsi]
 331:   2e 32 00                cs xor al,BYTE PTR [rax]

Disassembly of section .note.gnu.property:

0000000000000338 <.note.gnu.property>:
 338:   04 00                   add    al,0x0
 33a:   00 00                   add    BYTE PTR [rax],al
 33c:   20 00                   and    BYTE PTR [rax],al
 33e:   00 00                   add    BYTE PTR [rax],al
 340:   05 00 00 00 47          add    eax,0x47000000
 345:   4e 55                   rex.WRX push rbp
 347:   00 02                   add    BYTE PTR [rdx],al
 349:   00 00                   add    BYTE PTR [rax],al
 34b:   c0 04 00 00             rol    BYTE PTR [rax+rax*1],0x0
 34f:   00 03                   add    BYTE PTR [rbx],al
 351:   00 00                   add    BYTE PTR [rax],al
 353:   00 00                   add    BYTE PTR [rax],al
 355:   00 00                   add    BYTE PTR [rax],al
 357:   00 02                   add    BYTE PTR [rdx],al
 359:   80 00 c0                add    BYTE PTR [rax],0xc0
 35c:   04 00                   add    al,0x0
 35e:   00 00                   add    BYTE PTR [rax],al
 360:   01 00                   add    DWORD PTR [rax],eax
 362:   00 00                   add    BYTE PTR [rax],al
 364:   00 00                   add    BYTE PTR [rax],al
 366:   00 00                   add    BYTE PTR [rax],al

Disassembly of section .note.gnu.build-id:

0000000000000368 <.note.gnu.build-id>:
 368:   04 00                   add    al,0x0
 36a:   00 00                   add    BYTE PTR [rax],al
 36c:   14 00                   adc    al,0x0
 36e:   00 00                   add    BYTE PTR [rax],al
 370:   03 00                   add    eax,DWORD PTR [rax]
 372:   00 00                   add    BYTE PTR [rax],al
 374:   47                      rex.RXB
 375:   4e 55                   rex.WRX push rbp
 377:   00 8a d7 22 05 ee       add    BYTE PTR [rdx-0x11fadd29],cl
 37d:   4e f1                   rex.WRX int1
 37f:   81 ab 99 19 7a a3 12    sub    DWORD PTR [rbx-0x5c85e667],0xb5ad4912
 386:   49 ad b5
 389:   4a eb 1c                rex.WX jmp 3a8 <__abi_tag+0x1c>

Disassembly of section .note.ABI-tag:

000000000000038c <__abi_tag>:
 38c:   04 00                   add    al,0x0
 38e:   00 00                   add    BYTE PTR [rax],al
 390:   10 00                   adc    BYTE PTR [rax],al
 392:   00 00                   add    BYTE PTR [rax],al
 394:   01 00                   add    DWORD PTR [rax],eax
 396:   00 00                   add    BYTE PTR [rax],al
 398:   47                      rex.RXB
 399:   4e 55                   rex.WRX push rbp
 39b:   00 00                   add    BYTE PTR [rax],al
 39d:   00 00                   add    BYTE PTR [rax],al
 39f:   00 03                   add    BYTE PTR [rbx],al
 3a1:   00 00                   add    BYTE PTR [rax],al
 3a3:   00 02                   add    BYTE PTR [rdx],al
 3a5:   00 00                   add    BYTE PTR [rax],al
 3a7:   00 00                   add    BYTE PTR [rax],al
 3a9:   00 00                   add    BYTE PTR [rax],al
 3ab:   00                      .byte 0

Disassembly of section .gnu.hash:

00000000000003b0 <.gnu.hash>:
 3b0:   02 00                   add    al,BYTE PTR [rax]
 3b2:   00 00                   add    BYTE PTR [rax],al
 3b4:   05 00 00 00 01          add    eax,0x1000000
 3b9:   00 00                   add    BYTE PTR [rax],al
 3bb:   00 06                   add    BYTE PTR [rsi],al
 3bd:   00 00                   add    BYTE PTR [rax],al
 3bf:   00 00                   add    BYTE PTR [rax],al
 3c1:   00 81 00 00 00 00       add    BYTE PTR [rcx+0x0],al
 3c7:   00 05 00 00 00 00       add    BYTE PTR [rip+0x0],al        # 3cd <__abi_tag+0x41>
 3cd:   00 00                   add    BYTE PTR [rax],al
 3cf:   00 d1                   add    cl,dl
 3d1:   65 ce                   gs (bad)
 3d3:   6d                      ins    DWORD PTR es:[rdi],dx

Disassembly of section .dynsym:

00000000000003d8 <.dynsym>:
 3d8:   00 00                   add    BYTE PTR [rax],al
 3da:   00 00                   add    BYTE PTR [rax],al
 3dc:   00 00                   add    BYTE PTR [rax],al
 3de:   00 00                   add    BYTE PTR [rax],al
 3e0:   00 00                   add    BYTE PTR [rax],al
 3e2:   00 00                   add    BYTE PTR [rax],al
 3e4:   00 00                   add    BYTE PTR [rax],al
 3e6:   00 00                   add    BYTE PTR [rax],al
 3e8:   00 00                   add    BYTE PTR [rax],al
 3ea:   00 00                   add    BYTE PTR [rax],al
 3ec:   00 00                   add    BYTE PTR [rax],al
 3ee:   00 00                   add    BYTE PTR [rax],al
 3f0:   01 00                   add    DWORD PTR [rax],eax
 3f2:   00 00                   add    BYTE PTR [rax],al
 3f4:   12 00                   adc    al,BYTE PTR [rax]
 3f6:   00 00                   add    BYTE PTR [rax],al
 3f8:   00 00                   add    BYTE PTR [rax],al
 3fa:   00 00                   add    BYTE PTR [rax],al
 3fc:   00 00                   add    BYTE PTR [rax],al
 3fe:   00 00                   add    BYTE PTR [rax],al
 400:   00 00                   add    BYTE PTR [rax],al
 402:   00 00                   add    BYTE PTR [rax],al
 404:   00 00                   add    BYTE PTR [rax],al
 406:   00 00                   add    BYTE PTR [rax],al
 408:   43 00 00                rex.XB add BYTE PTR [r8],al
 40b:   00 20                   add    BYTE PTR [rax],ah
 40d:   00 00                   add    BYTE PTR [rax],al
 40f:   00 00                   add    BYTE PTR [rax],al
 411:   00 00                   add    BYTE PTR [rax],al
 413:   00 00                   add    BYTE PTR [rax],al
 415:   00 00                   add    BYTE PTR [rax],al
 417:   00 00                   add    BYTE PTR [rax],al
 419:   00 00                   add    BYTE PTR [rax],al
 41b:   00 00                   add    BYTE PTR [rax],al
 41d:   00 00                   add    BYTE PTR [rax],al
 41f:   00 5f 00                add    BYTE PTR [rdi+0x0],bl
 422:   00 00                   add    BYTE PTR [rax],al
 424:   20 00                   and    BYTE PTR [rax],al
 426:   00 00                   add    BYTE PTR [rax],al
 428:   00 00                   add    BYTE PTR [rax],al
 42a:   00 00                   add    BYTE PTR [rax],al
 42c:   00 00                   add    BYTE PTR [rax],al
 42e:   00 00                   add    BYTE PTR [rax],al
 430:   00 00                   add    BYTE PTR [rax],al
 432:   00 00                   add    BYTE PTR [rax],al
 434:   00 00                   add    BYTE PTR [rax],al
 436:   00 00                   add    BYTE PTR [rax],al
 438:   6e                      outs   dx,BYTE PTR ds:[rsi]
 439:   00 00                   add    BYTE PTR [rax],al
 43b:   00 20                   add    BYTE PTR [rax],ah
 43d:   00 00                   add    BYTE PTR [rax],al
 43f:   00 00                   add    BYTE PTR [rax],al
 441:   00 00                   add    BYTE PTR [rax],al
 443:   00 00                   add    BYTE PTR [rax],al
 445:   00 00                   add    BYTE PTR [rax],al
 447:   00 00                   add    BYTE PTR [rax],al
 449:   00 00                   add    BYTE PTR [rax],al
 44b:   00 00                   add    BYTE PTR [rax],al
 44d:   00 00                   add    BYTE PTR [rax],al
 44f:   00 13                   add    BYTE PTR [rbx],dl
 451:   00 00                   add    BYTE PTR [rax],al
 453:   00 22                   add    BYTE PTR [rdx],ah
 455:   00 00                   add    BYTE PTR [rax],al
 457:   00 00                   add    BYTE PTR [rax],al
 459:   00 00                   add    BYTE PTR [rax],al
 45b:   00 00                   add    BYTE PTR [rax],al
 45d:   00 00                   add    BYTE PTR [rax],al
 45f:   00 00                   add    BYTE PTR [rax],al
 461:   00 00                   add    BYTE PTR [rax],al
 463:   00 00                   add    BYTE PTR [rax],al
 465:   00 00                   add    BYTE PTR [rax],al
 467:   00                      .byte 0

Disassembly of section .dynstr:

0000000000000468 <.dynstr>:
 468:   00 5f 5f                add    BYTE PTR [rdi+0x5f],bl
 46b:   6c                      ins    BYTE PTR es:[rdi],dx
 46c:   69 62 63 5f 73 74 61    imul   esp,DWORD PTR [rdx+0x63],0x6174735f
 473:   72 74                   jb     4e9 <__abi_tag+0x15d>
 475:   5f                      pop    rdi
 476:   6d                      ins    DWORD PTR es:[rdi],dx
 477:   61                      (bad)
 478:   69 6e 00 5f 5f 63 78    imul   ebp,DWORD PTR [rsi+0x0],0x78635f5f
 47f:   61                      (bad)
 480:   5f                      pop    rdi
 481:   66 69 6e 61 6c 69       imul   bp,WORD PTR [rsi+0x61],0x696c
 487:   7a 65                   jp     4ee <__abi_tag+0x162>
 489:   00 6c 69 62             add    BYTE PTR [rcx+rbp*2+0x62],ch
 48d:   63 2e                   movsxd ebp,DWORD PTR [rsi]
 48f:   73 6f                   jae    500 <__abi_tag+0x174>
 491:   2e 36 00 47 4c          cs ss add BYTE PTR [rdi+0x4c],al
 496:   49                      rex.WB
 497:   42                      rex.X
 498:   43 5f                   rex.XB pop r15
 49a:   32 2e                   xor    ch,BYTE PTR [rsi]
 49c:   32 2e                   xor    ch,BYTE PTR [rsi]
 49e:   35 00 47 4c 49          xor    eax,0x494c4700
 4a3:   42                      rex.X
 4a4:   43 5f                   rex.XB pop r15
 4a6:   32 2e                   xor    ch,BYTE PTR [rsi]
 4a8:   33 34 00                xor    esi,DWORD PTR [rax+rax*1]
 4ab:   5f                      pop    rdi
 4ac:   49 54                   rex.WB push r12
 4ae:   4d 5f                   rex.WRB pop r15
 4b0:   64 65 72 65             fs gs jb 519 <__abi_tag+0x18d>
 4b4:   67 69 73 74 65 72 54    imul   esi,DWORD PTR [ebx+0x74],0x4d547265
 4bb:   4d
 4bc:   43 6c                   rex.XB ins BYTE PTR es:[rdi],dx
 4be:   6f                      outs   dx,DWORD PTR ds:[rsi]
 4bf:   6e                      outs   dx,BYTE PTR ds:[rsi]
 4c0:   65 54                   gs push rsp
 4c2:   61                      (bad)
 4c3:   62 6c 65                (bad)
 4c6:   00 5f 5f                add    BYTE PTR [rdi+0x5f],bl
 4c9:   67 6d                   ins    DWORD PTR es:[edi],dx
 4cb:   6f                      outs   dx,DWORD PTR ds:[rsi]
 4cc:   6e                      outs   dx,BYTE PTR ds:[rsi]
 4cd:   5f                      pop    rdi
 4ce:   73 74                   jae    544 <__abi_tag+0x1b8>
 4d0:   61                      (bad)
 4d1:   72 74                   jb     547 <__abi_tag+0x1bb>
 4d3:   5f                      pop    rdi
 4d4:   5f                      pop    rdi
 4d5:   00 5f 49                add    BYTE PTR [rdi+0x49],bl
 4d8:   54                      push   rsp
 4d9:   4d 5f                   rex.WRB pop r15
 4db:   72 65                   jb     542 <__abi_tag+0x1b6>
 4dd:   67 69 73 74 65 72 54    imul   esi,DWORD PTR [ebx+0x74],0x4d547265
 4e4:   4d
 4e5:   43 6c                   rex.XB ins BYTE PTR es:[rdi],dx
 4e7:   6f                      outs   dx,DWORD PTR ds:[rsi]
 4e8:   6e                      outs   dx,BYTE PTR ds:[rsi]
 4e9:   65 54                   gs push rsp
 4eb:   61                      (bad)
 4ec:   62                      .byte 0x62
 4ed:   6c                      ins    BYTE PTR es:[rdi],dx
 4ee:   65                      gs
 4ef:   00                      .byte 0

Disassembly of section .gnu.version:

00000000000004f0 <.gnu.version>:
 4f0:   00 00                   add    BYTE PTR [rax],al
 4f2:   02 00                   add    al,BYTE PTR [rax]
 4f4:   01 00                   add    DWORD PTR [rax],eax
 4f6:   01 00                   add    DWORD PTR [rax],eax
 4f8:   01 00                   add    DWORD PTR [rax],eax
 4fa:   03 00                   add    eax,DWORD PTR [rax]

Disassembly of section .gnu.version_r:

0000000000000500 <.gnu.version_r>:
 500:   01 00                   add    DWORD PTR [rax],eax
 502:   02 00                   add    al,BYTE PTR [rax]
 504:   22 00                   and    al,BYTE PTR [rax]
 506:   00 00                   add    BYTE PTR [rax],al
 508:   10 00                   adc    BYTE PTR [rax],al
 50a:   00 00                   add    BYTE PTR [rax],al
 50c:   00 00                   add    BYTE PTR [rax],al
 50e:   00 00                   add    BYTE PTR [rax],al
 510:   75 1a                   jne    52c <__abi_tag+0x1a0>
 512:   69 09 00 00 03 00       imul   ecx,DWORD PTR [rcx],0x30000
 518:   2c 00                   sub    al,0x0
 51a:   00 00                   add    BYTE PTR [rax],al
 51c:   10 00                   adc    BYTE PTR [rax],al
 51e:   00 00                   add    BYTE PTR [rax],al
 520:   b4 91                   mov    ah,0x91
 522:   96                      xchg   esi,eax
 523:   06                      (bad)
 524:   00 00                   add    BYTE PTR [rax],al
 526:   02 00                   add    al,BYTE PTR [rax]
 528:   38 00                   cmp    BYTE PTR [rax],al
 52a:   00 00                   add    BYTE PTR [rax],al
 52c:   00 00                   add    BYTE PTR [rax],al
 52e:   00 00                   add    BYTE PTR [rax],al

Disassembly of section .rela.dyn:

0000000000000530 <.rela.dyn>:
 530:   f0 3d 00 00 00 00       lock cmp eax,0x0
 536:   00 00                   add    BYTE PTR [rax],al
 538:   08 00                   or     BYTE PTR [rax],al
 53a:   00 00                   add    BYTE PTR [rax],al
 53c:   00 00                   add    BYTE PTR [rax],al
 53e:   00 00                   add    BYTE PTR [rax],al
 540:   20 11                   and    BYTE PTR [rcx],dl
 542:   00 00                   add    BYTE PTR [rax],al
 544:   00 00                   add    BYTE PTR [rax],al
 546:   00 00                   add    BYTE PTR [rax],al
 548:   f8                      clc
 549:   3d 00 00 00 00          cmp    eax,0x0
 54e:   00 00                   add    BYTE PTR [rax],al
 550:   08 00                   or     BYTE PTR [rax],al
 552:   00 00                   add    BYTE PTR [rax],al
 554:   00 00                   add    BYTE PTR [rax],al
 556:   00 00                   add    BYTE PTR [rax],al
 558:   e0 10                   loopne 56a <__abi_tag+0x1de>
 55a:   00 00                   add    BYTE PTR [rax],al
 55c:   00 00                   add    BYTE PTR [rax],al
 55e:   00 00                   add    BYTE PTR [rax],al
 560:   08 40 00                or     BYTE PTR [rax+0x0],al
 563:   00 00                   add    BYTE PTR [rax],al
 565:   00 00                   add    BYTE PTR [rax],al
 567:   00 08                   add    BYTE PTR [rax],cl
 569:   00 00                   add    BYTE PTR [rax],al
 56b:   00 00                   add    BYTE PTR [rax],al
 56d:   00 00                   add    BYTE PTR [rax],al
 56f:   00 08                   add    BYTE PTR [rax],cl
 571:   40 00 00                rex add BYTE PTR [rax],al
 574:   00 00                   add    BYTE PTR [rax],al
 576:   00 00                   add    BYTE PTR [rax],al
 578:   d8 3f                   fdivr  DWORD PTR [rdi]
 57a:   00 00                   add    BYTE PTR [rax],al
 57c:   00 00                   add    BYTE PTR [rax],al
 57e:   00 00                   add    BYTE PTR [rax],al
 580:   06                      (bad)
 581:   00 00                   add    BYTE PTR [rax],al
 583:   00 01                   add    BYTE PTR [rcx],al
 585:   00 00                   add    BYTE PTR [rax],al
 587:   00 00                   add    BYTE PTR [rax],al
 589:   00 00                   add    BYTE PTR [rax],al
 58b:   00 00                   add    BYTE PTR [rax],al
 58d:   00 00                   add    BYTE PTR [rax],al
 58f:   00 e0                   add    al,ah
 591:   3f                      (bad)
 592:   00 00                   add    BYTE PTR [rax],al
 594:   00 00                   add    BYTE PTR [rax],al
 596:   00 00                   add    BYTE PTR [rax],al
 598:   06                      (bad)
 599:   00 00                   add    BYTE PTR [rax],al
 59b:   00 02                   add    BYTE PTR [rdx],al
 59d:   00 00                   add    BYTE PTR [rax],al
 59f:   00 00                   add    BYTE PTR [rax],al
 5a1:   00 00                   add    BYTE PTR [rax],al
 5a3:   00 00                   add    BYTE PTR [rax],al
 5a5:   00 00                   add    BYTE PTR [rax],al
 5a7:   00 e8                   add    al,ch
 5a9:   3f                      (bad)
 5aa:   00 00                   add    BYTE PTR [rax],al
 5ac:   00 00                   add    BYTE PTR [rax],al
 5ae:   00 00                   add    BYTE PTR [rax],al
 5b0:   06                      (bad)
 5b1:   00 00                   add    BYTE PTR [rax],al
 5b3:   00 03                   add    BYTE PTR [rbx],al
 5b5:   00 00                   add    BYTE PTR [rax],al
 5b7:   00 00                   add    BYTE PTR [rax],al
 5b9:   00 00                   add    BYTE PTR [rax],al
 5bb:   00 00                   add    BYTE PTR [rax],al
 5bd:   00 00                   add    BYTE PTR [rax],al
 5bf:   00 f0                   add    al,dh
 5c1:   3f                      (bad)
 5c2:   00 00                   add    BYTE PTR [rax],al
 5c4:   00 00                   add    BYTE PTR [rax],al
 5c6:   00 00                   add    BYTE PTR [rax],al
 5c8:   06                      (bad)
 5c9:   00 00                   add    BYTE PTR [rax],al
 5cb:   00 04 00                add    BYTE PTR [rax+rax*1],al
 5ce:   00 00                   add    BYTE PTR [rax],al
 5d0:   00 00                   add    BYTE PTR [rax],al
 5d2:   00 00                   add    BYTE PTR [rax],al
 5d4:   00 00                   add    BYTE PTR [rax],al
 5d6:   00 00                   add    BYTE PTR [rax],al
 5d8:   f8                      clc
 5d9:   3f                      (bad)
 5da:   00 00                   add    BYTE PTR [rax],al
 5dc:   00 00                   add    BYTE PTR [rax],al
 5de:   00 00                   add    BYTE PTR [rax],al
 5e0:   06                      (bad)
 5e1:   00 00                   add    BYTE PTR [rax],al
 5e3:   00 05 00 00 00 00       add    BYTE PTR [rip+0x0],al        # 5e9 <__abi_tag+0x25d>
 5e9:   00 00                   add    BYTE PTR [rax],al
 5eb:   00 00                   add    BYTE PTR [rax],al
 5ed:   00 00                   add    BYTE PTR [rax],al
 5ef:   00                      .byte 0

Disassembly of section .init:

0000000000001000 <_init>:
    1000:       f3 0f 1e fa             endbr64
    1004:       48 83 ec 08             sub    rsp,0x8
    1008:       48 8b 05 d9 2f 00 00    mov    rax,QWORD PTR [rip+0x2fd9]        # 3fe8 <__gmon_start__@Base>
    100f:       48 85 c0                test   rax,rax
    1012:       74 02                   je     1016 <_init+0x16>
    1014:       ff d0                   call   rax
    1016:       48 83 c4 08             add    rsp,0x8
    101a:       c3                      ret

Disassembly of section .plt:

0000000000001020 <.plt>:
    1020:       ff 35 a2 2f 00 00       push   QWORD PTR [rip+0x2fa2]        # 3fc8 <_GLOBAL_OFFSET_TABLE_+0x8>
    1026:       ff 25 a4 2f 00 00       jmp    QWORD PTR [rip+0x2fa4]        # 3fd0 <_GLOBAL_OFFSET_TABLE_+0x10>
    102c:       0f 1f 40 00             nop    DWORD PTR [rax+0x0]

Disassembly of section .plt.got:

0000000000001030 <__cxa_finalize@plt>:
    1030:       f3 0f 1e fa             endbr64
    1034:       ff 25 be 2f 00 00       jmp    QWORD PTR [rip+0x2fbe]        # 3ff8 <__cxa_finalize@GLIBC_2.2.5>
    103a:       66 0f 1f 44 00 00       nop    WORD PTR [rax+rax*1+0x0]

Disassembly of section .text:

0000000000001040 <_start>:
    1040:       f3 0f 1e fa             endbr64
    1044:       31 ed                   xor    ebp,ebp
    1046:       49 89 d1                mov    r9,rdx
    1049:       5e                      pop    rsi
    104a:       48 89 e2                mov    rdx,rsp
    104d:       48 83 e4 f0             and    rsp,0xfffffffffffffff0
    1051:       50                      push   rax
    1052:       54                      push   rsp
    1053:       45 31 c0                xor    r8d,r8d
    1056:       31 c9                   xor    ecx,ecx
    1058:       48 8d 3d ca 00 00 00    lea    rdi,[rip+0xca]        # 1129 <main>
    105f:       ff 15 73 2f 00 00       call   QWORD PTR [rip+0x2f73]        # 3fd8 <__libc_start_main@GLIBC_2.34>
    1065:       f4                      hlt
    1066:       66 2e 0f 1f 84 00 00    cs nop WORD PTR [rax+rax*1+0x0]
    106d:       00 00 00

0000000000001070 <deregister_tm_clones>:
    1070:       48 8d 3d a1 2f 00 00    lea    rdi,[rip+0x2fa1]        # 4018 <__TMC_END__>
    1077:       48 8d 05 9a 2f 00 00    lea    rax,[rip+0x2f9a]        # 4018 <__TMC_END__>
    107e:       48 39 f8                cmp    rax,rdi
    1081:       74 15                   je     1098 <deregister_tm_clones+0x28>
    1083:       48 8b 05 56 2f 00 00    mov    rax,QWORD PTR [rip+0x2f56]        # 3fe0 <_ITM_deregisterTMCloneTable@Base>
    108a:       48 85 c0                test   rax,rax
    108d:       74 09                   je     1098 <deregister_tm_clones+0x28>
    108f:       ff e0                   jmp    rax
    1091:       0f 1f 80 00 00 00 00    nop    DWORD PTR [rax+0x0]
    1098:       c3                      ret
    1099:       0f 1f 80 00 00 00 00    nop    DWORD PTR [rax+0x0]

00000000000010a0 <register_tm_clones>:
    10a0:       48 8d 3d 71 2f 00 00    lea    rdi,[rip+0x2f71]        # 4018 <__TMC_END__>
    10a7:       48 8d 35 6a 2f 00 00    lea    rsi,[rip+0x2f6a]        # 4018 <__TMC_END__>
    10ae:       48 29 fe                sub    rsi,rdi
    10b1:       48 89 f0                mov    rax,rsi
    10b4:       48 c1 ee 3f             shr    rsi,0x3f
    10b8:       48 c1 f8 03             sar    rax,0x3
    10bc:       48 01 c6                add    rsi,rax
    10bf:       48 d1 fe                sar    rsi,1
    10c2:       74 14                   je     10d8 <register_tm_clones+0x38>
    10c4:       48 8b 05 25 2f 00 00    mov    rax,QWORD PTR [rip+0x2f25]        # 3ff0 <_ITM_registerTMCloneTable@Base>
    10cb:       48 85 c0                test   rax,rax
    10ce:       74 08                   je     10d8 <register_tm_clones+0x38>
    10d0:       ff e0                   jmp    rax
    10d2:       66 0f 1f 44 00 00       nop    WORD PTR [rax+rax*1+0x0]
    10d8:       c3                      ret
    10d9:       0f 1f 80 00 00 00 00    nop    DWORD PTR [rax+0x0]

00000000000010e0 <__do_global_dtors_aux>:
    10e0:       f3 0f 1e fa             endbr64
    10e4:       80 3d 2d 2f 00 00 00    cmp    BYTE PTR [rip+0x2f2d],0x0        # 4018 <__TMC_END__>
    10eb:       75 2b                   jne    1118 <__do_global_dtors_aux+0x38>
    10ed:       55                      push   rbp
    10ee:       48 83 3d 02 2f 00 00    cmp    QWORD PTR [rip+0x2f02],0x0        # 3ff8 <__cxa_finalize@GLIBC_2.2.5>
    10f5:       00
    10f6:       48 89 e5                mov    rbp,rsp
    10f9:       74 0c                   je     1107 <__do_global_dtors_aux+0x27>
    10fb:       48 8b 3d 06 2f 00 00    mov    rdi,QWORD PTR [rip+0x2f06]        # 4008 <__dso_handle>
    1102:       e8 29 ff ff ff          call   1030 <__cxa_finalize@plt>
    1107:       e8 64 ff ff ff          call   1070 <deregister_tm_clones>
    110c:       c6 05 05 2f 00 00 01    mov    BYTE PTR [rip+0x2f05],0x1        # 4018 <__TMC_END__>
    1113:       5d                      pop    rbp
    1114:       c3                      ret
    1115:       0f 1f 00                nop    DWORD PTR [rax]
    1118:       c3                      ret
    1119:       0f 1f 80 00 00 00 00    nop    DWORD PTR [rax+0x0]

0000000000001120 <frame_dummy>:
    1120:       f3 0f 1e fa             endbr64
    1124:       e9 77 ff ff ff          jmp    10a0 <register_tm_clones>

0000000000001129 <main>:
    1129:       f3 0f 1e fa             endbr64
    112d:       55                      push   rbp
    112e:       48 89 e5                mov    rbp,rsp
    1131:       48 8b 05 d8 2e 00 00    mov    rax,QWORD PTR [rip+0x2ed8]        # 4010 <a>
    1138:       48 c1 e8 02             shr    rax,0x2
    113c:       89 05 da 2e 00 00       mov    DWORD PTR [rip+0x2eda],eax        # 401c <b>
    1142:       8b 05 d4 2e 00 00       mov    eax,DWORD PTR [rip+0x2ed4]        # 401c <b>
    1148:       83 e8 01                sub    eax,0x1
    114b:       66 89 45 fe             mov    WORD PTR [rbp-0x2],ax
    114f:       0f b7 45 fe             movzx  eax,WORD PTR [rbp-0x2]
    1153:       5d                      pop    rbp
    1154:       c3                      ret

Disassembly of section .fini:

0000000000001158 <_fini>:
    1158:       f3 0f 1e fa             endbr64
    115c:       48 83 ec 08             sub    rsp,0x8
    1160:       48 83 c4 08             add    rsp,0x8
    1164:       c3                      ret

Disassembly of section .rodata:

0000000000002000 <_IO_stdin_used>:
    2000:       01 00                   add    DWORD PTR [rax],eax
    2002:       02 00                   add    al,BYTE PTR [rax]

Disassembly of section .eh_frame_hdr:

0000000000002004 <__GNU_EH_FRAME_HDR>:
    2004:       01 1b                   add    DWORD PTR [rbx],ebx
    2006:       03 3b                   add    edi,DWORD PTR [rbx]
    2008:       28 00                   sub    BYTE PTR [rax],al
    200a:       00 00                   add    BYTE PTR [rax],al
    200c:       04 00                   add    al,0x0
    200e:       00 00                   add    BYTE PTR [rax],al
    2010:       1c f0                   sbb    al,0xf0
    2012:       ff                      (bad)
    2013:       ff 5c 00 00             call   FWORD PTR [rax+rax*1+0x0]
    2017:       00 2c f0                add    BYTE PTR [rax+rsi*8],ch
    201a:       ff                      (bad)
    201b:       ff 84 00 00 00 3c f0    inc    DWORD PTR [rax+rax*1-0xfc40000]
    2022:       ff                      (bad)
    2023:       ff 44 00 00             inc    DWORD PTR [rax+rax*1+0x0]
    2027:       00 25 f1 ff ff 9c       add    BYTE PTR [rip+0xffffffff9cfffff1],ah        # ffffffff9d00201e <_end+0xffffffff9cffdffe>
    202d:       00 00                   add    BYTE PTR [rax],al
    202f:       00                      .byte 0

Disassembly of section .eh_frame:

0000000000002030 <__FRAME_END__-0x90>:
    2030:       14 00                   adc    al,0x0
    2032:       00 00                   add    BYTE PTR [rax],al
    2034:       00 00                   add    BYTE PTR [rax],al
    2036:       00 00                   add    BYTE PTR [rax],al
    2038:       01 7a 52                add    DWORD PTR [rdx+0x52],edi
    203b:       00 01                   add    BYTE PTR [rcx],al
    203d:       78 10                   js     204f <__GNU_EH_FRAME_HDR+0x4b>
    203f:       01 1b                   add    DWORD PTR [rbx],ebx
    2041:       0c 07                   or     al,0x7
    2043:       08 90 01 00 00 14       or     BYTE PTR [rax+0x14000001],dl
    2049:       00 00                   add    BYTE PTR [rax],al
    204b:       00 1c 00                add    BYTE PTR [rax+rax*1],bl
    204e:       00 00                   add    BYTE PTR [rax],al
    2050:       f0 ef                   lock out dx,eax
    2052:       ff                      (bad)
    2053:       ff 26                   jmp    QWORD PTR [rsi]
    2055:       00 00                   add    BYTE PTR [rax],al
    2057:       00 00                   add    BYTE PTR [rax],al
    2059:       44 07                   rex.R (bad)
    205b:       10 00                   adc    BYTE PTR [rax],al
    205d:       00 00                   add    BYTE PTR [rax],al
    205f:       00 24 00                add    BYTE PTR [rax+rax*1],ah
    2062:       00 00                   add    BYTE PTR [rax],al
    2064:       34 00                   xor    al,0x0
    2066:       00 00                   add    BYTE PTR [rax],al
    2068:       b8 ef ff ff 10          mov    eax,0x10ffffef
    206d:       00 00                   add    BYTE PTR [rax],al
    206f:       00 00                   add    BYTE PTR [rax],al
    2071:       0e                      (bad)
    2072:       10 46 0e                adc    BYTE PTR [rsi+0xe],al
    2075:       18 4a 0f                sbb    BYTE PTR [rdx+0xf],cl
    2078:       0b 77 08                or     esi,DWORD PTR [rdi+0x8]
    207b:       80 00 3f                add    BYTE PTR [rax],0x3f
    207e:       1a 39                   sbb    bh,BYTE PTR [rcx]
    2080:       2a 33                   sub    dh,BYTE PTR [rbx]
    2082:       24 22                   and    al,0x22
    2084:       00 00                   add    BYTE PTR [rax],al
    2086:       00 00                   add    BYTE PTR [rax],al
    2088:       14 00                   adc    al,0x0
    208a:       00 00                   add    BYTE PTR [rax],al
    208c:       5c                      pop    rsp
    208d:       00 00                   add    BYTE PTR [rax],al
    208f:       00 a0 ef ff ff 10       add    BYTE PTR [rax+0x10ffffef],ah
    2095:       00 00                   add    BYTE PTR [rax],al
    2097:       00 00                   add    BYTE PTR [rax],al
    2099:       00 00                   add    BYTE PTR [rax],al
    209b:       00 00                   add    BYTE PTR [rax],al
    209d:       00 00                   add    BYTE PTR [rax],al
    209f:       00 1c 00                add    BYTE PTR [rax+rax*1],bl
    20a2:       00 00                   add    BYTE PTR [rax],al
    20a4:       74 00                   je     20a6 <__GNU_EH_FRAME_HDR+0xa2>
    20a6:       00 00                   add    BYTE PTR [rax],al
    20a8:       81 f0 ff ff 2c 00       xor    eax,0x2cffff
    20ae:       00 00                   add    BYTE PTR [rax],al
    20b0:       00 45 0e                add    BYTE PTR [rbp+0xe],al
    20b3:       10 86 02 43 0d 06       adc    BYTE PTR [rsi+0x60d4302],al
    20b9:       63 0c 07                movsxd ecx,DWORD PTR [rdi+rax*1]
    20bc:       08 00                   or     BYTE PTR [rax],al
    20be:       00 00                   add    BYTE PTR [rax],al

00000000000020c0 <__FRAME_END__>:
    20c0:       00 00                   add    BYTE PTR [rax],al
    20c2:       00 00                   add    BYTE PTR [rax],al

Disassembly of section .init_array:

0000000000003df0 <__frame_dummy_init_array_entry>:
    3df0:       20 11                   and    BYTE PTR [rcx],dl
    3df2:       00 00                   add    BYTE PTR [rax],al
    3df4:       00 00                   add    BYTE PTR [rax],al
    3df6:       00 00                   add    BYTE PTR [rax],al

Disassembly of section .fini_array:

0000000000003df8 <__do_global_dtors_aux_fini_array_entry>:
    3df8:       e0 10                   loopne 3e0a <_DYNAMIC+0xa>
    3dfa:       00 00                   add    BYTE PTR [rax],al
    3dfc:       00 00                   add    BYTE PTR [rax],al
    3dfe:       00 00                   add    BYTE PTR [rax],al

Disassembly of section .dynamic:

0000000000003e00 <_DYNAMIC>:
    3e00:       01 00                   add    DWORD PTR [rax],eax
    3e02:       00 00                   add    BYTE PTR [rax],al
    3e04:       00 00                   add    BYTE PTR [rax],al
    3e06:       00 00                   add    BYTE PTR [rax],al
    3e08:       22 00                   and    al,BYTE PTR [rax]
    3e0a:       00 00                   add    BYTE PTR [rax],al
    3e0c:       00 00                   add    BYTE PTR [rax],al
    3e0e:       00 00                   add    BYTE PTR [rax],al
    3e10:       0c 00                   or     al,0x0
    3e12:       00 00                   add    BYTE PTR [rax],al
    3e14:       00 00                   add    BYTE PTR [rax],al
    3e16:       00 00                   add    BYTE PTR [rax],al
    3e18:       00 10                   add    BYTE PTR [rax],dl
    3e1a:       00 00                   add    BYTE PTR [rax],al
    3e1c:       00 00                   add    BYTE PTR [rax],al
    3e1e:       00 00                   add    BYTE PTR [rax],al
    3e20:       0d 00 00 00 00          or     eax,0x0
    3e25:       00 00                   add    BYTE PTR [rax],al
    3e27:       00 58 11                add    BYTE PTR [rax+0x11],bl
    3e2a:       00 00                   add    BYTE PTR [rax],al
    3e2c:       00 00                   add    BYTE PTR [rax],al
    3e2e:       00 00                   add    BYTE PTR [rax],al
    3e30:       19 00                   sbb    DWORD PTR [rax],eax
    3e32:       00 00                   add    BYTE PTR [rax],al
    3e34:       00 00                   add    BYTE PTR [rax],al
    3e36:       00 00                   add    BYTE PTR [rax],al
    3e38:       f0 3d 00 00 00 00       lock cmp eax,0x0
    3e3e:       00 00                   add    BYTE PTR [rax],al
    3e40:       1b 00                   sbb    eax,DWORD PTR [rax]
    3e42:       00 00                   add    BYTE PTR [rax],al
    3e44:       00 00                   add    BYTE PTR [rax],al
    3e46:       00 00                   add    BYTE PTR [rax],al
    3e48:       08 00                   or     BYTE PTR [rax],al
    3e4a:       00 00                   add    BYTE PTR [rax],al
    3e4c:       00 00                   add    BYTE PTR [rax],al
    3e4e:       00 00                   add    BYTE PTR [rax],al
    3e50:       1a 00                   sbb    al,BYTE PTR [rax]
    3e52:       00 00                   add    BYTE PTR [rax],al
    3e54:       00 00                   add    BYTE PTR [rax],al
    3e56:       00 00                   add    BYTE PTR [rax],al
    3e58:       f8                      clc
    3e59:       3d 00 00 00 00          cmp    eax,0x0
    3e5e:       00 00                   add    BYTE PTR [rax],al
    3e60:       1c 00                   sbb    al,0x0
    3e62:       00 00                   add    BYTE PTR [rax],al
    3e64:       00 00                   add    BYTE PTR [rax],al
    3e66:       00 00                   add    BYTE PTR [rax],al
    3e68:       08 00                   or     BYTE PTR [rax],al
    3e6a:       00 00                   add    BYTE PTR [rax],al
    3e6c:       00 00                   add    BYTE PTR [rax],al
    3e6e:       00 00                   add    BYTE PTR [rax],al
    3e70:       f5                      cmc
    3e71:       fe                      (bad)
    3e72:       ff 6f 00                jmp    FWORD PTR [rdi+0x0]
    3e75:       00 00                   add    BYTE PTR [rax],al
    3e77:       00 b0 03 00 00 00       add    BYTE PTR [rax+0x3],dh
    3e7d:       00 00                   add    BYTE PTR [rax],al
    3e7f:       00 05 00 00 00 00       add    BYTE PTR [rip+0x0],al        # 3e85 <_DYNAMIC+0x85>
    3e85:       00 00                   add    BYTE PTR [rax],al
    3e87:       00 68 04                add    BYTE PTR [rax+0x4],ch
    3e8a:       00 00                   add    BYTE PTR [rax],al
    3e8c:       00 00                   add    BYTE PTR [rax],al
    3e8e:       00 00                   add    BYTE PTR [rax],al
    3e90:       06                      (bad)
    3e91:       00 00                   add    BYTE PTR [rax],al
    3e93:       00 00                   add    BYTE PTR [rax],al
    3e95:       00 00                   add    BYTE PTR [rax],al
    3e97:       00 d8                   add    al,bl
    3e99:       03 00                   add    eax,DWORD PTR [rax]
    3e9b:       00 00                   add    BYTE PTR [rax],al
    3e9d:       00 00                   add    BYTE PTR [rax],al
    3e9f:       00 0a                   add    BYTE PTR [rdx],cl
    3ea1:       00 00                   add    BYTE PTR [rax],al
    3ea3:       00 00                   add    BYTE PTR [rax],al
    3ea5:       00 00                   add    BYTE PTR [rax],al
    3ea7:       00 88 00 00 00 00       add    BYTE PTR [rax+0x0],cl
    3ead:       00 00                   add    BYTE PTR [rax],al
    3eaf:       00 0b                   add    BYTE PTR [rbx],cl
    3eb1:       00 00                   add    BYTE PTR [rax],al
    3eb3:       00 00                   add    BYTE PTR [rax],al
    3eb5:       00 00                   add    BYTE PTR [rax],al
    3eb7:       00 18                   add    BYTE PTR [rax],bl
    3eb9:       00 00                   add    BYTE PTR [rax],al
    3ebb:       00 00                   add    BYTE PTR [rax],al
    3ebd:       00 00                   add    BYTE PTR [rax],al
    3ebf:       00 15 00 00 00 00       add    BYTE PTR [rip+0x0],dl        # 3ec5 <_DYNAMIC+0xc5>
    3ec5:       00 00                   add    BYTE PTR [rax],al
    3ec7:       00 00                   add    BYTE PTR [rax],al
    3ec9:       00 00                   add    BYTE PTR [rax],al
    3ecb:       00 00                   add    BYTE PTR [rax],al
    3ecd:       00 00                   add    BYTE PTR [rax],al
    3ecf:       00 03                   add    BYTE PTR [rbx],al
    3ed1:       00 00                   add    BYTE PTR [rax],al
    3ed3:       00 00                   add    BYTE PTR [rax],al
    3ed5:       00 00                   add    BYTE PTR [rax],al
    3ed7:       00 c0                   add    al,al
    3ed9:       3f                      (bad)
    3eda:       00 00                   add    BYTE PTR [rax],al
    3edc:       00 00                   add    BYTE PTR [rax],al
    3ede:       00 00                   add    BYTE PTR [rax],al
    3ee0:       07                      (bad)
    3ee1:       00 00                   add    BYTE PTR [rax],al
    3ee3:       00 00                   add    BYTE PTR [rax],al
    3ee5:       00 00                   add    BYTE PTR [rax],al
    3ee7:       00 30                   add    BYTE PTR [rax],dh
    3ee9:       05 00 00 00 00          add    eax,0x0
    3eee:       00 00                   add    BYTE PTR [rax],al
    3ef0:       08 00                   or     BYTE PTR [rax],al
    3ef2:       00 00                   add    BYTE PTR [rax],al
    3ef4:       00 00                   add    BYTE PTR [rax],al
    3ef6:       00 00                   add    BYTE PTR [rax],al
    3ef8:       c0 00 00                rol    BYTE PTR [rax],0x0
    3efb:       00 00                   add    BYTE PTR [rax],al
    3efd:       00 00                   add    BYTE PTR [rax],al
    3eff:       00 09                   add    BYTE PTR [rcx],cl
    3f01:       00 00                   add    BYTE PTR [rax],al
    3f03:       00 00                   add    BYTE PTR [rax],al
    3f05:       00 00                   add    BYTE PTR [rax],al
    3f07:       00 18                   add    BYTE PTR [rax],bl
    3f09:       00 00                   add    BYTE PTR [rax],al
    3f0b:       00 00                   add    BYTE PTR [rax],al
    3f0d:       00 00                   add    BYTE PTR [rax],al
    3f0f:       00 1e                   add    BYTE PTR [rsi],bl
    3f11:       00 00                   add    BYTE PTR [rax],al
    3f13:       00 00                   add    BYTE PTR [rax],al
    3f15:       00 00                   add    BYTE PTR [rax],al
    3f17:       00 08                   add    BYTE PTR [rax],cl
    3f19:       00 00                   add    BYTE PTR [rax],al
    3f1b:       00 00                   add    BYTE PTR [rax],al
    3f1d:       00 00                   add    BYTE PTR [rax],al
    3f1f:       00 fb                   add    bl,bh
    3f21:       ff                      (bad)
    3f22:       ff 6f 00                jmp    FWORD PTR [rdi+0x0]
    3f25:       00 00                   add    BYTE PTR [rax],al
    3f27:       00 01                   add    BYTE PTR [rcx],al
    3f29:       00 00                   add    BYTE PTR [rax],al
    3f2b:       08 00                   or     BYTE PTR [rax],al
    3f2d:       00 00                   add    BYTE PTR [rax],al
    3f2f:       00 fe                   add    dh,bh
    3f31:       ff                      (bad)
    3f32:       ff 6f 00                jmp    FWORD PTR [rdi+0x0]
    3f35:       00 00                   add    BYTE PTR [rax],al
    3f37:       00 00                   add    BYTE PTR [rax],al
    3f39:       05 00 00 00 00          add    eax,0x0
    3f3e:       00 00                   add    BYTE PTR [rax],al
    3f40:       ff                      (bad)
    3f41:       ff                      (bad)
    3f42:       ff 6f 00                jmp    FWORD PTR [rdi+0x0]
    3f45:       00 00                   add    BYTE PTR [rax],al
    3f47:       00 01                   add    BYTE PTR [rcx],al
    3f49:       00 00                   add    BYTE PTR [rax],al
    3f4b:       00 00                   add    BYTE PTR [rax],al
    3f4d:       00 00                   add    BYTE PTR [rax],al
    3f4f:       00 f0                   add    al,dh
    3f51:       ff                      (bad)
    3f52:       ff 6f 00                jmp    FWORD PTR [rdi+0x0]
    3f55:       00 00                   add    BYTE PTR [rax],al
    3f57:       00 f0                   add    al,dh
    3f59:       04 00                   add    al,0x0
    3f5b:       00 00                   add    BYTE PTR [rax],al
    3f5d:       00 00                   add    BYTE PTR [rax],al
    3f5f:       00 f9                   add    cl,bh
    3f61:       ff                      (bad)
    3f62:       ff 6f 00                jmp    FWORD PTR [rdi+0x0]
    3f65:       00 00                   add    BYTE PTR [rax],al
    3f67:       00 03                   add    BYTE PTR [rbx],al
    3f69:       00 00                   add    BYTE PTR [rax],al
    3f6b:       00 00                   add    BYTE PTR [rax],al
    3f6d:       00 00                   add    BYTE PTR [rax],al
    3f6f:       00 00                   add    BYTE PTR [rax],al
    3f71:       00 00                   add    BYTE PTR [rax],al
    3f73:       00 00                   add    BYTE PTR [rax],al
    3f75:       00 00                   add    BYTE PTR [rax],al
    3f77:       00 00                   add    BYTE PTR [rax],al
    3f79:       00 00                   add    BYTE PTR [rax],al
    3f7b:       00 00                   add    BYTE PTR [rax],al
    3f7d:       00 00                   add    BYTE PTR [rax],al
    3f7f:       00 00                   add    BYTE PTR [rax],al
    3f81:       00 00                   add    BYTE PTR [rax],al
    3f83:       00 00                   add    BYTE PTR [rax],al
    3f85:       00 00                   add    BYTE PTR [rax],al
    3f87:       00 00                   add    BYTE PTR [rax],al
    3f89:       00 00                   add    BYTE PTR [rax],al
    3f8b:       00 00                   add    BYTE PTR [rax],al
    3f8d:       00 00                   add    BYTE PTR [rax],al
    3f8f:       00 00                   add    BYTE PTR [rax],al
    3f91:       00 00                   add    BYTE PTR [rax],al
    3f93:       00 00                   add    BYTE PTR [rax],al
    3f95:       00 00                   add    BYTE PTR [rax],al
    3f97:       00 00                   add    BYTE PTR [rax],al
    3f99:       00 00                   add    BYTE PTR [rax],al
    3f9b:       00 00                   add    BYTE PTR [rax],al
    3f9d:       00 00                   add    BYTE PTR [rax],al
    3f9f:       00 00                   add    BYTE PTR [rax],al
    3fa1:       00 00                   add    BYTE PTR [rax],al
    3fa3:       00 00                   add    BYTE PTR [rax],al
    3fa5:       00 00                   add    BYTE PTR [rax],al
    3fa7:       00 00                   add    BYTE PTR [rax],al
    3fa9:       00 00                   add    BYTE PTR [rax],al
    3fab:       00 00                   add    BYTE PTR [rax],al
    3fad:       00 00                   add    BYTE PTR [rax],al
    3faf:       00 00                   add    BYTE PTR [rax],al
    3fb1:       00 00                   add    BYTE PTR [rax],al
    3fb3:       00 00                   add    BYTE PTR [rax],al
    3fb5:       00 00                   add    BYTE PTR [rax],al
    3fb7:       00 00                   add    BYTE PTR [rax],al
    3fb9:       00 00                   add    BYTE PTR [rax],al
    3fbb:       00 00                   add    BYTE PTR [rax],al
    3fbd:       00 00                   add    BYTE PTR [rax],al
    3fbf:       00                      .byte 0

Disassembly of section .got:

0000000000003fc0 <_GLOBAL_OFFSET_TABLE_>:
    3fc0:       00 3e                   add    BYTE PTR [rsi],bh
    3fc2:       00 00                   add    BYTE PTR [rax],al
    3fc4:       00 00                   add    BYTE PTR [rax],al
    3fc6:       00 00                   add    BYTE PTR [rax],al
    3fc8:       00 00                   add    BYTE PTR [rax],al
    3fca:       00 00                   add    BYTE PTR [rax],al
    3fcc:       00 00                   add    BYTE PTR [rax],al
    3fce:       00 00                   add    BYTE PTR [rax],al
    3fd0:       00 00                   add    BYTE PTR [rax],al
    3fd2:       00 00                   add    BYTE PTR [rax],al
    3fd4:       00 00                   add    BYTE PTR [rax],al
    3fd6:       00 00                   add    BYTE PTR [rax],al
    3fd8:       00 00                   add    BYTE PTR [rax],al
    3fda:       00 00                   add    BYTE PTR [rax],al
    3fdc:       00 00                   add    BYTE PTR [rax],al
    3fde:       00 00                   add    BYTE PTR [rax],al
    3fe0:       00 00                   add    BYTE PTR [rax],al
    3fe2:       00 00                   add    BYTE PTR [rax],al
    3fe4:       00 00                   add    BYTE PTR [rax],al
    3fe6:       00 00                   add    BYTE PTR [rax],al
    3fe8:       00 00                   add    BYTE PTR [rax],al
    3fea:       00 00                   add    BYTE PTR [rax],al
    3fec:       00 00                   add    BYTE PTR [rax],al
    3fee:       00 00                   add    BYTE PTR [rax],al
    3ff0:       00 00                   add    BYTE PTR [rax],al
    3ff2:       00 00                   add    BYTE PTR [rax],al
    3ff4:       00 00                   add    BYTE PTR [rax],al
    3ff6:       00 00                   add    BYTE PTR [rax],al
    3ff8:       00 00                   add    BYTE PTR [rax],al
    3ffa:       00 00                   add    BYTE PTR [rax],al
    3ffc:       00 00                   add    BYTE PTR [rax],al
    3ffe:       00 00                   add    BYTE PTR [rax],al

Disassembly of section .data:

0000000000004000 <__data_start>:
    4000:       00 00                   add    BYTE PTR [rax],al
    4002:       00 00                   add    BYTE PTR [rax],al
    4004:       00 00                   add    BYTE PTR [rax],al
    4006:       00 00                   add    BYTE PTR [rax],al

0000000000004008 <__dso_handle>:
    4008:       08 40 00                or     BYTE PTR [rax+0x0],al
    400b:       00 00                   add    BYTE PTR [rax],al
    400d:       00 00                   add    BYTE PTR [rax],al
    400f:       00                      add    BYTE PTR [rax+0x0],al

0000000000004010 <a>:
    4010:       40 00 00                rex add BYTE PTR [rax],al
    4013:       00 00                   add    BYTE PTR [rax],al
    4015:       00 00                   add    BYTE PTR [rax],al
    4017:       00                      .byte 0

Disassembly of section .comment:

0000000000000000 <.comment>:
   0:   47                      rex.RXB
   1:   43                      rex.XB
   2:   43 3a 20                rex.XB cmp spl,BYTE PTR [r8]
   5:   28 55 62                sub    BYTE PTR [rbp+0x62],dl
   8:   75 6e                   jne    78 <__abi_tag-0x314>
   a:   74 75                   je     81 <__abi_tag-0x30b>
   c:   20 31                   and    BYTE PTR [rcx],dh
   e:   33 2e                   xor    ebp,DWORD PTR [rsi]
  10:   33 2e                   xor    ebp,DWORD PTR [rsi]
  12:   30 2d 36 75 62 75       xor    BYTE PTR [rip+0x75627536],ch        # 7562754e <_end+0x7562352e>
  18:   6e                      outs   dx,BYTE PTR ds:[rsi]
  19:   74 75                   je     90 <__abi_tag-0x2fc>
  1b:   32 7e 32                xor    bh,BYTE PTR [rsi+0x32]
  1e:   34 2e                   xor    al,0x2e
  20:   30 34 2e                xor    BYTE PTR [rsi+rbp*1],dh
  23:   31 29                   xor    DWORD PTR [rcx],ebp
  25:   20 31                   and    BYTE PTR [rcx],dh
  27:   33 2e                   xor    ebp,DWORD PTR [rsi]
  29:   33 2e                   xor    ebp,DWORD PTR [rsi]
  2b:   30 00                   xor    BYTE PTR [rax],al

Disassembly of section .debug_aranges:

0000000000000000 <.debug_aranges>:
   0:   2c 00                   sub    al,0x0
   2:   00 00                   add    BYTE PTR [rax],al
   4:   02 00                   add    al,BYTE PTR [rax]
   6:   00 00                   add    BYTE PTR [rax],al
   8:   00 00                   add    BYTE PTR [rax],al
   a:   08 00                   or     BYTE PTR [rax],al
   c:   00 00                   add    BYTE PTR [rax],al
   e:   00 00                   add    BYTE PTR [rax],al
  10:   29 11                   sub    DWORD PTR [rcx],edx
  12:   00 00                   add    BYTE PTR [rax],al
  14:   00 00                   add    BYTE PTR [rax],al
  16:   00 00                   add    BYTE PTR [rax],al
  18:   2c 00                   sub    al,0x0
  1a:   00 00                   add    BYTE PTR [rax],al
  1c:   00 00                   add    BYTE PTR [rax],al
  1e:   00 00                   add    BYTE PTR [rax],al
  20:   00 00                   add    BYTE PTR [rax],al
  22:   00 00                   add    BYTE PTR [rax],al
  24:   00 00                   add    BYTE PTR [rax],al
  26:   00 00                   add    BYTE PTR [rax],al
  28:   00 00                   add    BYTE PTR [rax],al
  2a:   00 00                   add    BYTE PTR [rax],al
  2c:   00 00                   add    BYTE PTR [rax],al
  2e:   00 00                   add    BYTE PTR [rax],al

Disassembly of section .debug_info:

0000000000000000 <.debug_info>:
   0:   02 01                   add    al,BYTE PTR [rcx]
   2:   00 00                   add    BYTE PTR [rax],al
   4:   05 00 01 08 00          add    eax,0x80100
   9:   00 00                   add    BYTE PTR [rax],al
   b:   00 04 62                add    BYTE PTR [rdx+riz*2],al
   e:   00 00                   add    BYTE PTR [rax],al
  10:   00 1d 0d 00 00 00       add    BYTE PTR [rip+0xd],bl        # 23 <__abi_tag-0x369>
  16:   00 00                   add    BYTE PTR [rax],al
  18:   00 00                   add    BYTE PTR [rax],al
  1a:   29 11                   sub    DWORD PTR [rcx],edx
  1c:   00 00                   add    BYTE PTR [rax],al
  1e:   00 00                   add    BYTE PTR [rax],al
  20:   00 00                   add    BYTE PTR [rax],al
  22:   2c 00                   sub    al,0x0
  24:   00 00                   add    BYTE PTR [rax],al
  26:   00 00                   add    BYTE PTR [rax],al
  28:   00 00                   add    BYTE PTR [rax],al
  2a:   00 00                   add    BYTE PTR [rax],al
  2c:   00 00                   add    BYTE PTR [rax],al
  2e:   01 01                   add    DWORD PTR [rcx],eax
  30:   08 3b                   or     BYTE PTR [rbx],bh
  32:   00 00                   add    BYTE PTR [rax],al
  34:   00 01                   add    BYTE PTR [rcx],al
  36:   02 07                   add    al,BYTE PTR [rdi]
  38:   00 00                   add    BYTE PTR [rax],al
  3a:   00 00                   add    BYTE PTR [rax],al
  3c:   01 04 07                add    DWORD PTR [rdi+rax*1],eax
  3f:   23 00                   and    eax,DWORD PTR [rax]
  41:   00 00                   add    BYTE PTR [rax],al
  43:   01 08                   add    DWORD PTR [rax],ecx
  45:   07                      (bad)
  46:   1e                      (bad)
  47:   00 00                   add    BYTE PTR [rax],al
  49:   00 01                   add    BYTE PTR [rcx],al
  4b:   01 06                   add    DWORD PTR [rsi],eax
  4d:   3d 00 00 00 01          cmp    eax,0x1000000
  52:   02 05 f0 00 00 00       add    al,BYTE PTR [rip+0xf0]        # 148 <__abi_tag-0x244>
  58:   02 30                   add    dh,BYTE PTR [rax]
  5a:   00 00                   add    BYTE PTR [rax],al
  5c:   00 02                   add    BYTE PTR [rdx],al
  5e:   28 1c 35 00 00 00 05    sub    BYTE PTR [rsi*1+0x5000000],bl
  65:   04 05                   add    al,0x5
  67:   69 6e 74 00 02 13 00    imul   ebp,DWORD PTR [rsi+0x74],0x130200
  6e:   00 00                   add    BYTE PTR [rax],al
  70:   02 2a                   add    ch,BYTE PTR [rdx]
  72:   16                      (bad)
  73:   3c 00                   cmp    al,0x0
  75:   00 00                   add    BYTE PTR [rax],al
  77:   01 08                   add    DWORD PTR [rax],ecx
  79:   05 4e 00 00 00          add    eax,0x4e
  7e:   02 57 00                add    dl,BYTE PTR [rdi+0x0]
  81:   00 00                   add    BYTE PTR [rax],al
  83:   02 2d 1b 43 00 00       add    ch,BYTE PTR [rip+0x431b]        # 43a4 <_end+0x384>
  89:   00 01                   add    BYTE PTR [rcx],al
  8b:   01 06                   add    DWORD PTR [rsi],eax
  8d:   44 00 00                add    BYTE PTR [rax],r8b
  90:   00 02                   add    BYTE PTR [rdx],al
  92:   32 00                   xor    al,BYTE PTR [rax]
  94:   00 00                   add    BYTE PTR [rax],al
  96:   03 19                   add    ebx,DWORD PTR [rcx]
  98:   14 58                   adc    al,0x58
  9a:   00 00                   add    BYTE PTR [rax],al
  9c:   00 02                   add    BYTE PTR [rdx],al
  9e:   15 00 00 00 03          adc    eax,0x3000000
  a3:   1a 14 6b                sbb    dl,BYTE PTR [rbx+rbp*2]
  a6:   00 00                   add    BYTE PTR [rax],al
  a8:   00 02                   add    BYTE PTR [rdx],al
  aa:   59                      pop    rcx
  ab:   00 00                   add    BYTE PTR [rax],al
  ad:   00 03                   add    BYTE PTR [rbx],al
  af:   1b 14 7e                sbb    edx,DWORD PTR [rsi+rdi*2]
  b2:   00 00                   add    BYTE PTR [rax],al
  b4:   00 03                   add    BYTE PTR [rbx],al
  b6:   61                      (bad)
  b7:   00 03                   add    BYTE PTR [rbx],al
  b9:   a9 00 00 00 09          test   eax,0x9000000
  be:   03 10                   add    edx,DWORD PTR [rax]
  c0:   40 00 00                rex add BYTE PTR [rax],al
  c3:   00 00                   add    BYTE PTR [rax],al
  c5:   00 00                   add    BYTE PTR [rax],al
  c7:   03 62 00                add    esp,DWORD PTR [rdx+0x0]
  ca:   04 9d                   add    al,0x9d
  cc:   00 00                   add    BYTE PTR [rax],al
  ce:   00 09                   add    BYTE PTR [rcx],cl
  d0:   03 1c 40                add    ebx,DWORD PTR [rax+rax*2]
  d3:   00 00                   add    BYTE PTR [rax],al
  d5:   00 00                   add    BYTE PTR [rax],al
  d7:   00 00                   add    BYTE PTR [rax],al
  d9:   06                      (bad)
  da:   49 00 00                rex.WB add BYTE PTR [r8],al
  dd:   00 01                   add    BYTE PTR [rcx],al
  df:   06                      (bad)
  e0:   05 64 00 00 00          add    eax,0x64
  e5:   29 11                   sub    DWORD PTR [rcx],edx
  e7:   00 00                   add    BYTE PTR [rax],al
  e9:   00 00                   add    BYTE PTR [rax],al
  eb:   00 00                   add    BYTE PTR [rax],al
  ed:   2c 00                   sub    al,0x0
  ef:   00 00                   add    BYTE PTR [rax],al
  f1:   00 00                   add    BYTE PTR [rax],al
  f3:   00 00                   add    BYTE PTR [rax],al
  f5:   01 9c 07 63 00 01 0b    add    DWORD PTR [rdi+rax*1+0xb010063],ebx
  fc:   0e                      (bad)
  fd:   91                      xchg   ecx,eax
  fe:   00 00                   add    BYTE PTR [rax],al
 100:   00 02                   add    BYTE PTR [rdx],al
 102:   91                      xchg   ecx,eax
 103:   6e                      outs   dx,BYTE PTR ds:[rsi]
 104:   00 00                   add    BYTE PTR [rax],al

Disassembly of section .debug_abbrev:

0000000000000000 <.debug_abbrev>:
   0:   01 24 00                add    DWORD PTR [rax+rax*1],esp
   3:   0b 0b                   or     ecx,DWORD PTR [rbx]
   5:   3e 0b 03                ds or  eax,DWORD PTR [rbx]
   8:   0e                      (bad)
   9:   00 00                   add    BYTE PTR [rax],al
   b:   02 16                   add    dl,BYTE PTR [rsi]
   d:   00 03                   add    BYTE PTR [rbx],al
   f:   0e                      (bad)
  10:   3a 0b                   cmp    cl,BYTE PTR [rbx]
  12:   3b 0b                   cmp    ecx,DWORD PTR [rbx]
  14:   39 0b                   cmp    DWORD PTR [rbx],ecx
  16:   49 13 00                adc    rax,QWORD PTR [r8]
  19:   00 03                   add    BYTE PTR [rbx],al
  1b:   34 00                   xor    al,0x0
  1d:   03 08                   add    ecx,DWORD PTR [rax]
  1f:   3a 21                   cmp    ah,BYTE PTR [rcx]
  21:   01 3b                   add    DWORD PTR [rbx],edi
  23:   0b 39                   or     edi,DWORD PTR [rcx]
  25:   21 0a                   and    DWORD PTR [rdx],ecx
  27:   49 13 3f                adc    rdi,QWORD PTR [r15]
  2a:   19 02                   sbb    DWORD PTR [rdx],eax
  2c:   18 00                   sbb    BYTE PTR [rax],al
  2e:   00 04 11                add    BYTE PTR [rcx+rdx*1],al
  31:   01 25 0e 13 0b 03       add    DWORD PTR [rip+0x30b130e],esp        # 30b1345 <_end+0x30ad325>
  37:   1f                      (bad)
  38:   1b 1f                   sbb    ebx,DWORD PTR [rdi]
  3a:   11 01                   adc    DWORD PTR [rcx],eax
  3c:   12 07                   adc    al,BYTE PTR [rdi]
  3e:   10 17                   adc    BYTE PTR [rdi],dl
  40:   00 00                   add    BYTE PTR [rax],al
  42:   05 24 00 0b 0b          add    eax,0xb0b0024
  47:   3e 0b 03                ds or  eax,DWORD PTR [rbx]
  4a:   08 00                   or     BYTE PTR [rax],al
  4c:   00 06                   add    BYTE PTR [rsi],al
  4e:   2e 01 3f                cs add DWORD PTR [rdi],edi
  51:   19 03                   sbb    DWORD PTR [rbx],eax
  53:   0e                      (bad)
  54:   3a 0b                   cmp    cl,BYTE PTR [rbx]
  56:   3b 0b                   cmp    ecx,DWORD PTR [rbx]
  58:   39 0b                   cmp    DWORD PTR [rbx],ecx
  5a:   49 13 11                adc    rdx,QWORD PTR [r9]
  5d:   01 12                   add    DWORD PTR [rdx],edx
  5f:   07                      (bad)
  60:   40 18 7a 19             sbb    BYTE PTR [rdx+0x19],dil
  64:   00 00                   add    BYTE PTR [rax],al
  66:   07                      (bad)
  67:   34 00                   xor    al,0x0
  69:   03 08                   add    ecx,DWORD PTR [rax]
  6b:   3a 0b                   cmp    cl,BYTE PTR [rbx]
  6d:   3b 0b                   cmp    ecx,DWORD PTR [rbx]
  6f:   39 0b                   cmp    DWORD PTR [rbx],ecx
  71:   49 13 02                adc    rax,QWORD PTR [r10]
  74:   18 00                   sbb    BYTE PTR [rax],al
  76:   00 00                   add    BYTE PTR [rax],al

Disassembly of section .debug_line:

0000000000000000 <.debug_line>:
   0:   63 00                   movsxd eax,DWORD PTR [rax]
   2:   00 00                   add    BYTE PTR [rax],al
   4:   05 00 08 00 38          add    eax,0x38000800
   9:   00 00                   add    BYTE PTR [rax],al
   b:   00 01                   add    BYTE PTR [rcx],al
   d:   01 01                   add    DWORD PTR [rcx],eax
   f:   fb                      sti
  10:   0e                      (bad)
  11:   0d 00 01 01 01          or     eax,0x1010100
  16:   01 00                   add    DWORD PTR [rax],eax
  18:   00 00                   add    BYTE PTR [rax],al
  1a:   01 00                   add    DWORD PTR [rax],eax
  1c:   00 01                   add    BYTE PTR [rcx],al
  1e:   01 01                   add    DWORD PTR [rcx],eax
  20:   1f                      (bad)
  21:   02 00                   add    al,BYTE PTR [rax]
  23:   00 00                   add    BYTE PTR [rax],al
  25:   00 20                   add    BYTE PTR [rax],ah
  27:   00 00                   add    BYTE PTR [rax],al
  29:   00 02                   add    BYTE PTR [rdx],al
  2b:   01 1f                   add    DWORD PTR [rdi],ebx
  2d:   02 0f                   add    cl,BYTE PTR [rdi]
  2f:   04 0d                   add    al,0xd
  31:   00 00                   add    BYTE PTR [rax],al
  33:   00 00                   add    BYTE PTR [rax],al
  35:   0d 00 00 00 00          or     eax,0x0
  3a:   43 00 00                rex.XB add BYTE PTR [r8],al
  3d:   00 01                   add    BYTE PTR [rcx],al
  3f:   4b 00 00                rex.WXB add BYTE PTR [r8],al
  42:   00 01                   add    BYTE PTR [rcx],al
  44:   05 0b 00 09 02          add    eax,0x209000b
  49:   29 11                   sub    DWORD PTR [rcx],edx
  4b:   00 00                   add    BYTE PTR [rax],al
  4d:   00 00                   add    BYTE PTR [rax],al
  4f:   00 00                   add    BYTE PTR [rax],al
  51:   17                      (bad)
  52:   84 05 07 ac 05 14       test   BYTE PTR [rip+0x1405ac07],al        # 1405ac5f <_end+0x14056c3f>
  58:   69 05 0e 66 05 0c 76    imul   eax,DWORD PTR [rip+0xc05660e],0x4b010576        # c056670 <_end+0xc052650>
  5f:   05 01 4b
  62:   02 02                   add    al,BYTE PTR [rdx]
  64:   00 01                   add    BYTE PTR [rcx],al
  66:   01                      .byte 0x1

Disassembly of section .debug_str:

0000000000000000 <.debug_str>:
   0:   73 68                   jae    6a <__abi_tag-0x322>
   2:   6f                      outs   dx,DWORD PTR ds:[rsi]
   3:   72 74                   jb     79 <__abi_tag-0x313>
   5:   20 75 6e                and    BYTE PTR [rbp+0x6e],dh
   8:   73 69                   jae    73 <__abi_tag-0x319>
   a:   67 6e                   outs   dx,BYTE PTR ds:[esi]
   c:   65 64 20 69 6e          gs and BYTE PTR fs:[rcx+0x6e],ch
  11:   74 00                   je     13 <__abi_tag-0x379>
  13:   5f                      pop    rdi
  14:   5f                      pop    rdi
  15:   75 69                   jne    80 <__abi_tag-0x30c>
  17:   6e                      outs   dx,BYTE PTR ds:[rsi]
  18:   74 33                   je     4d <__abi_tag-0x33f>
  1a:   32 5f 74                xor    bl,BYTE PTR [rdi+0x74]
  1d:   00 6c 6f 6e             add    BYTE PTR [rdi+rbp*2+0x6e],ch
  21:   67 20 75 6e             and    BYTE PTR [ebp+0x6e],dh
  25:   73 69                   jae    90 <__abi_tag-0x2fc>
  27:   67 6e                   outs   dx,BYTE PTR ds:[esi]
  29:   65 64 20 69 6e          gs and BYTE PTR fs:[rcx+0x6e],ch
  2e:   74 00                   je     30 <__abi_tag-0x35c>
  30:   5f                      pop    rdi
  31:   5f                      pop    rdi
  32:   75 69                   jne    9d <__abi_tag-0x2ef>
  34:   6e                      outs   dx,BYTE PTR ds:[rsi]
  35:   74 31                   je     68 <__abi_tag-0x324>
  37:   36 5f                   ss pop rdi
  39:   74 00                   je     3b <__abi_tag-0x351>
  3b:   75 6e                   jne    ab <__abi_tag-0x2e1>
  3d:   73 69                   jae    a8 <__abi_tag-0x2e4>
  3f:   67 6e                   outs   dx,BYTE PTR ds:[esi]
  41:   65 64 20 63 68          gs and BYTE PTR fs:[rbx+0x68],ah
  46:   61                      (bad)
  47:   72 00                   jb     49 <__abi_tag-0x343>
  49:   6d                      ins    DWORD PTR es:[rdi],dx
  4a:   61                      (bad)
  4b:   69 6e 00 6c 6f 6e 67    imul   ebp,DWORD PTR [rsi+0x0],0x676e6f6c
  52:   20 69 6e                and    BYTE PTR [rcx+0x6e],ch
  55:   74 00                   je     57 <__abi_tag-0x335>
  57:   5f                      pop    rdi
  58:   5f                      pop    rdi
  59:   75 69                   jne    c4 <__abi_tag-0x2c8>
  5b:   6e                      outs   dx,BYTE PTR ds:[rsi]
  5c:   74 36                   je     94 <__abi_tag-0x2f8>
  5e:   34 5f                   xor    al,0x5f
  60:   74 00                   je     62 <__abi_tag-0x32a>
  62:   47                      rex.RXB
  63:   4e 55                   rex.WRX push rbp
  65:   20 43 31                and    BYTE PTR [rbx+0x31],al
  68:   37                      (bad)
  69:   20 31                   and    BYTE PTR [rcx],dh
  6b:   33 2e                   xor    ebp,DWORD PTR [rsi]
  6d:   33 2e                   xor    ebp,DWORD PTR [rsi]
  6f:   30 20                   xor    BYTE PTR [rax],ah
  71:   2d 6d 74 75 6e          sub    eax,0x6e75746d
  76:   65 3d 67 65 6e 65       gs cmp eax,0x656e6567
  7c:   72 69                   jb     e7 <__abi_tag-0x2a5>
  7e:   63 20                   movsxd esp,DWORD PTR [rax]
  80:   2d 6d 61 72 63          sub    eax,0x6372616d
  85:   68 3d 78 38 36          push   0x3638783d
  8a:   2d 36 34 20 2d          sub    eax,0x2d203436
  8f:   67 20 2d 66 61 73 79    and    BYTE PTR [eip+0x79736166],ch        # 797361fc <_end+0x797321dc>
  96:   6e                      outs   dx,BYTE PTR ds:[rsi]
  97:   63 68 72                movsxd ebp,DWORD PTR [rax+0x72]
  9a:   6f                      outs   dx,DWORD PTR ds:[rsi]
  9b:   6e                      outs   dx,BYTE PTR ds:[rsi]
  9c:   6f                      outs   dx,DWORD PTR ds:[rsi]
  9d:   75 73                   jne    112 <__abi_tag-0x27a>
  9f:   2d 75 6e 77 69          sub    eax,0x69776e75
  a4:   6e                      outs   dx,BYTE PTR ds:[rsi]
  a5:   64 2d 74 61 62 6c       fs sub eax,0x6c626174
  ab:   65 73 20                gs jae ce <__abi_tag-0x2be>
  ae:   2d 66 73 74 61          sub    eax,0x61747366
  b3:   63 6b 2d                movsxd ebp,DWORD PTR [rbx+0x2d]
  b6:   70 72                   jo     12a <__abi_tag-0x262>
  b8:   6f                      outs   dx,DWORD PTR ds:[rsi]
  b9:   74 65                   je     120 <__abi_tag-0x26c>
  bb:   63 74 6f 72             movsxd esi,DWORD PTR [rdi+rbp*2+0x72]
  bf:   2d 73 74 72 6f          sub    eax,0x6f727473
  c4:   6e                      outs   dx,BYTE PTR ds:[rsi]
  c5:   67 20 2d 66 73 74 61    and    BYTE PTR [eip+0x61747366],ch        # 61747432 <_end+0x61743412>
  cc:   63 6b 2d                movsxd ebp,DWORD PTR [rbx+0x2d]
  cf:   63 6c 61 73             movsxd ebp,DWORD PTR [rcx+riz*2+0x73]
  d3:   68 2d 70 72 6f          push   0x6f72702d
  d8:   74 65                   je     13f <__abi_tag-0x24d>
  da:   63 74 69 6f             movsxd esi,DWORD PTR [rcx+rbp*2+0x6f]
  de:   6e                      outs   dx,BYTE PTR ds:[rsi]
  df:   20 2d 66 63 66 2d       and    BYTE PTR [rip+0x2d666366],ch        # 2d66644b <_end+0x2d66242b>
  e5:   70 72                   jo     159 <__abi_tag-0x233>
  e7:   6f                      outs   dx,DWORD PTR ds:[rsi]
  e8:   74 65                   je     14f <__abi_tag-0x23d>
  ea:   63 74 69 6f             movsxd esi,DWORD PTR [rcx+rbp*2+0x6f]
  ee:   6e                      outs   dx,BYTE PTR ds:[rsi]
  ef:   00 73 68                add    BYTE PTR [rbx+0x68],dh
  f2:   6f                      outs   dx,DWORD PTR ds:[rsi]
  f3:   72 74                   jb     169 <__abi_tag-0x223>
  f5:   20 69 6e                and    BYTE PTR [rcx+0x6e],ch
  f8:   74 00                   je     fa <__abi_tag-0x292>

Disassembly of section .debug_line_str:

0000000000000000 <.debug_line_str>:
   0:   2f                      (bad)
   1:   68 6f 6d 65 2f          push   0x2f656d6f
   6:   6a 6f                   push   0x6f
   8:   73 65                   jae    6f <__abi_tag-0x31d>
   a:   70 68                   jo     74 <__abi_tag-0x318>
   c:   00 69 6d                add    BYTE PTR [rcx+0x6d],ch
   f:   70 6c                   jo     7d <__abi_tag-0x30f>
  11:   65 6d                   gs ins DWORD PTR es:[rdi],dx
  13:   65 6e                   outs   dx,BYTE PTR gs:[rsi]
  15:   74 61                   je     78 <__abi_tag-0x314>
  17:   c3                      ret
  18:   a7                      cmps   DWORD PTR ds:[rsi],DWORD PTR es:[rdi]
  19:   c3                      ret
  1a:   a3 6f 32 2e 63 00 2f    movabs ds:0x73752f00632e326f,eax
  21:   75 73
  23:   72 2f                   jb     54 <__abi_tag-0x338>
  25:   69 6e 63 6c 75 64 65    imul   ebp,DWORD PTR [rsi+0x63],0x6564756c
  2c:   2f                      (bad)
  2d:   78 38                   js     67 <__abi_tag-0x325>
  2f:   36 5f                   ss pop rdi
  31:   36 34 2d                ss xor al,0x2d
  34:   6c                      ins    BYTE PTR es:[rdi],dx
  35:   69 6e 75 78 2d 67 6e    imul   ebp,DWORD PTR [rsi+0x75],0x6e672d78
  3c:   75 2f                   jne    6d <__abi_tag-0x31f>
  3e:   62 69 74 73 00          (bad)
  43:   74 79                   je     be <__abi_tag-0x2ce>
  45:   70 65                   jo     ac <__abi_tag-0x2e0>
  47:   73 2e                   jae    77 <__abi_tag-0x315>
  49:   68 00 73 74 64          push   0x64747300
  4e:   69 6e 74 2d 75 69 6e    imul   ebp,DWORD PTR [rsi+0x74],0x6e69752d
  55:   74 6e                   je     c5 <__abi_tag-0x2c7>
  57:   2e                      cs
  58:   68                      .byte 0x68
  59:   00                      .byte 0
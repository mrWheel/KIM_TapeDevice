Testing RAM isn't just a question of storing a value and then 
checking it. It's a important to test for interference between 
locations. Such tests often involve writing to one location and 
then checking all other locations to see they haven't been 
disturbed; this can be time consuming.

This program checks memory thoroughly and runs exceptionally fast.  
It is adapted from an algorithm by Knaizuk and Hartmann published 
in 'IFEE. Transactions on Computers', April 1977.

The program first puts value FF in every location under test.
Then it puts 00 in every third location, after which it tests
all locations for correctness.  The test is repeated twice more 
with the positions of the 00's changed each time.  Finally, the 
whole thing is repeated with the FF and 00 values interchanged.

To run:  Set the addresses of the first and last memory pages
you wish to test into locations 0000 and 0001 respectively.
Start the program at address 0002; it will halt with a memory
address on the display.  If no faults were round, the address
will be one location past the last address tested.  If a fault
is found, its address will be displayed.

Example:  To test 0100 to 02FF (pages 01 and 02) in KIM:
Set 0000 to 01, 0001 to 02, start program at 0002.  If memory
is good, see 0300 (=02FF + 1).  Now if you try testing
0100 to 2000 (0000=0l,000l=20) the program will halt at
the first bad location - this will be 0400 if you haven't
added memory.

0001   0000                     .org $0000
0002   0000             
0003   0000             #define POINTL   ($FA)
0004   0000             #define POINTH   ($FB)
0005   0000             #define FLAG     ($70)
0006   0000             #define FLIP     ($71)
0007   0000             #define MOD      ($72)
0008   0000             #define START    ($1C4F)
0009   0000             
0010   0000 00          BEGIN:  .byte   $00             ; starting page for test
0011   0001 00          END:    .byte   $00             ; ending page for test
0012   0002             
0013   0002 A9 00       STRT:   LDA     #0              ; zero pointers
0014   0004 A8                  TAY                     ; for low-order
0015   0005 85 FA               STA     POINTL          ; addresses;
0016   0007 85 70       BIGLP:  STA     FLAG            ; =00 first pass, =FF second pass
0017   0009 A2 02               LDX     #2
0018   000B 86 72               STX     MOD             ; set 3 tests each pass
0019   000D A5 00       PASS:   LDA     BEGIN           ; set pointer to..
0020   000F 85 FB               STA     POINTH          ; ..start of test area
0021   0011 A6 01               LDX     END
0022   0013 A5 70               LDA     FLAG
0023   0015 49 FF               EOR     #$FF            ; reverse FLAG
0024   0017 85 71               STA     FLIP            ; ..=FF first pass, =00 second pass
0025   0019 91 FA       CLEAR:  STA     (POINTL),Y      ; write above FLIP value..
0026   001B C8                  INY                     ; ..into all locations
0027   001C D0 FB               BNE     CLEAR
0028   001E E6 FB               INC     POINTH
0029   0020 E4 FB               CPX     POINTH
0030   0022 B0 F5               BCS     CLEAR
0031   0024             ; FLIP value in all locations - now change 1 in 3
0032   0024 A6 72               LDX     MOD
0033   0026 A5 00               LDA     BEGIN           ; set pointer..
0034   0028 85 FB               STA     POINTH          ; ..back to start
0035   002A A5 70       FILL:   LDA     FLAG            ; change value
0036   002C CA          TOP:    DEX
0037   002D 10 04               BPL     SKIP            ; skip 2 out of 3
0038   002F A2 02               LDX     #2              ; restore 3-counter
0039   0031 91 FA               STA     (POINTL),Y      ; change 1 out of 3
0040   0033 C8          SKIP:   INY
0041   0034 D0 F6               BNE     TOP
0042   0036 E6 FB               INC     POINTH          ; new page
0043   0038 A5 01               LDA     END             ; have we passed..
0044   003A C5 FB               CMP     POINTH          ; ..end of test area?
0045   003C B0 EC               BCS     FILL            ; nope, keep going
0046   003E             ; memory set up - now we test it
0047   003E A5 00               LDA     BEGIN           ; set pointer..
0048   0040 85 FB               STA     POINTH          ; ..back to start
0049   0042 A6 72               LDX     MOD             ; set up 3-counter
0050   0044 A5 71       POP:    LDA     FLIP            ; test for FLIP value..
0051   0046 CA                  DEX                     ; ..2 out of 3 times..
0052   0047 10 04               BPL     SLIP            ; - or -
0053   0049 A2 02               LDX     #2              ; 1 out of 3..
0054   004B A5 70               LDA     FLAG            ; test for FLAG value;
0055   004D D1 FA       SLIP:   CMP     (POINTL),Y      ; here's the test...
0056   004F D0 15               BNE     OUT             ; branch if failed
0057   0051 C8                  INY
0058   0052 D0 F0               BNE     POP
0059   0054 E6 FB               INC     POINTH
0060   0056 A5 01               LDA     END
0061   0058 C5 FB               CMP     POINTH
0062   005A B0 E8               BCS     POP
0063   005C             ; above test OK - change & repeat
0064   005C C6 72               DEC     MOD             ; change 1/3 position
0065   005E 10 AD               BPL     PASS            ; ..& do next third
0066   0060 A5 70               LDA     FLAG            ; invert..
0067   0062 49 FF               EOR     #$FF            ; ..flag for pass two
0068   0064 30 A1               BMI     BIGLP
0069   0066 84 FA       OUT:    STY     POINTL          ; put low order adds to display
0070   0068 6C 4F 1C            JMP     START           ; ..and exit to KIM
0071   006B             .END
# tasm: Number of errors = 0



;1800000000A900A885FA8570A2028672A50085FBA601A57049FF850B27
;1800187191FAC8D0FBE6FBE4FBB0F5A672A50085FBA570CA1004A20FF6
;1800300291FAC8D0F6E6FBA501C5FBB0ECA50085FBA672A571CA100F73
;18004804A202A570D1FAD015C8D0F0E6FBA501C5FBB0E8C67210AD0F29
;0B0060A57049FF30A184FA6C4F1C05EE
;00

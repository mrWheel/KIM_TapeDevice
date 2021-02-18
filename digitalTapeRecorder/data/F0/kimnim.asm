;
;-- KIM Nim by Jim Butterfield
;

IQ        .EQU $00
PILE      .EQU $01
MOVE      .EQU $02
TEMP      .EQU $03
VALUE     .EQU $04
FLASHR    .EQU $0A
WINDOW    .EQU $0B
CUE       .EQU $11
WAIT      .EQU $12

;TMR0001  .EQU $1704            ; KIM-1
TMR0001   .EQU $1744            ; microKIM
SAD       .EQU $1740
PADD      .EQU $1741
SBD       .EQU $1742
KEYIN     .EQU $1F40
GETKEY    .EQU $1F6A
TABLE     .EQU $1FE7

        .ORG    $0200

START:  JSR     KEYIN           ; directional regs
        JSR     GETKEY
        CMP     #$13            ; GO key?
        BNE     NOGO            ; nope, skip
        LDA     TMR0001           ; get random nbr
        LDX     #2              ; split into 3
SPLIT:  TAY                     ; save A
        AND     #7              ; extract 3 bits
        BEQ     ZINCH           ; unless zero..
        CLC                     ; ..add two
        ADC     #2
ZINCH:  STA     VALUE,X         ; store pile val
        TYA                     ; bring back rand
        LSR     A
        LSR     A
        LSR     A
        DEX
        BPL     SPLIT
STALL:  JSR     KEYIN           ; wait for..
        BNE     STALL           ; ..key release
        LDA     TMR0001         ; new random nbr
        LDX     #2              ; split 3 ways
SPLAT:  TAY                     ; again
        AND     #7              ; 3 bits
        STA     VALUE+3,X
        TYA
        LSR     A
        LSR     A
        LSR     A
        DEX
        BPL     SPLAT
        STA     PILE            ; pile zero
        STA     MOVE            ; it's your move
        LDX     #6              ; for each pile..
DRESS:  LDA     VALUE-1,X       ; ..change to
        JSR     SEG             ; ..segments
        DEX
        BNE     DRESS
NOGO:   LDX     MOVE            ; whose move?
        BNE     NOKEY           ; computer's, skip
        CMP     #$10            ; hex digit keyed?
        BCS     NOKEY           ; no, skip
        CMP     #0              ; zero key?
        BEQ     NOKEY           ; yes, skip
        CMP     #$0A            ; alphabetic?
        BCC     NUM             ; no, numeric
        SEC                     ; change A-F
        SBC     #9              ; ..to 1-6
        LDX     PILE            ; pile already..
        BNE     NOKEY           ; ..selected?
        TAX
        LDA     FLASHR,X
        BEQ     NOKEY           ; nothing in pile?
        STX     PILE            ; OK, mark pile
        STA     FLASHR          ; store flash code
        BCS     NOKEY           ; unconditional
NUM:    LDX     PILE
        BEQ     NOKEY           ; no pile selected
        STA     TEMP            ; save number
        LDA     VALUE-1,X       ; pile value
        CMP     TEMP            ; pile big enough?
        BCC     NOKEY           ; nope
        SBC     TEMP            ; yes, take out
        JSR     SEG             ; compute segments
        INC     MOVE            ; computer's move
        JSR     SURVEY          ; end of game?
        BNE     NOKEY           ; no, keep going
        JSR     MESSAG          ; yes, show messg
        STA     WINDOW          ; ''I LOSE''
        LSR     IQ              ; get smart!

; all routines join here - display

NOKEY:  LDX     PILE
        LDA     FLASHR          ; flash pile
        EOR     FLASHR,X
        STA     FLASHR,X
        LDA     #$7F
        STA     PADD
LIGHT:  LDY     #$13
        LDX     #5
LITE:   LDA     WINDOW,X
        STA     SAD
        STY     SBD
LITEX:  INC     CUE
        BNE     LITEX
        DEY
        DEY
        DEX
        BPL     LITE
        INC     WAIT
        BNE     LIGHT
        LDA     #$F8
        STA     WAIT
        LDX     MOVE            ; whose move?
        BEQ     EXIT            ; not computer's
        DEX                     ; first step?
        BNE     TRY             ; no, skip stratgy
        LDA     #0
        LDX     #5              ; merge all piles..
MERGE:  EOR     VALUE,X         ; ..by EOR-ing them
        DEX
        BPL     MERGE
        STA     FLASHR          ; save EOR product
        LDX     #6              ; re-examine piles
LOOP:   LDA     VALUE-1,X
        EOR     FLASHR
        CMP     VALUE-1,X
        BCC     FOUND
        DEX
        BNE     LOOP
        BEQ     MOV
FOUND:  LDY     IQ              ; IQ high enuff?
        CPY     TMR0001         ; ..randomly..
        BCS     MOV             ; no, move dumb
        STA     TEMP            ; amount
        STX     PILE            ; pile number
MOV:    LDX     PILE
        LDA     FLASHR,X        ; flash mask
        STA     FLASHR          ; Flash...
TRY:    INC     MOVE            ; but don't make
        LDA     MOVE            ; ..the move till..
        CMP     #$10            ; ..time has passed
        BCC     EXIT
        LDX     PILE            ; time to move!
        LDA     TEMP
        JSR     SEG             ; make move
        JSR     SURVEY          ; end of game?
        BNE     KEEP            ; nope, keep goin
        JSR     MESSAG          ; 'U LOSE'
        SEC                     ; dummy up..
        ROL     IQ              ; ..the computer
KEEP:   LDA     #0
        STA     MOVE            ; it's your move
        STA     PILE            ; un-flash
EXIT:   CLD
        JMP     START
MESSAG: LDA     #0
        STA     MOVE            ; end of play
        STA     PILE            ; no flashing
        LDX     #6              ; move 7 digits
MLOOP:  LDA     DATA,X          ; pick em up..
        STA     FLASHR,X        ; ..put em down
        DEX
        BPL     MLOOP
        RTS
SURVEY: LDA     #0
        STA     FLASHR          ; un-flash
        LDX     #6              ; for all piles..
REVUE:  CMP     VALUE-1,X
        BCS     SMALL
        LDA     VALUE-1,X
        STA     TEMP
        STX     PILE
SMALL:  DEX
        BNE     REVUE
        DEC     TEMP
        TAY                     ; test A
        RTS
SEG:    STA     VALUE-1,X       ; store value
        BEQ     NIL             ; blank digit
        TAY
        LDA     TABLE,Y
NIL:    STA     FLASHR,X        ; segments to wndw
        LDA     #0
        RTS

        .BYTE   $FF
DATA:   .BYTE   $06, $BE, $00, $B8, $BF, $ED, $F9

.END

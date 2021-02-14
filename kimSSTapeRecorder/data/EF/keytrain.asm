;
;-- KeyTrain by Jim Butterfield
;

INH       .EQU  $F9
POINTL    .EQU  $FA
POINTH    .EQU  $FB
TEMP      .EQU  $FF
;TMR0001  .EQU  $1704          ; KIM-1
TMR0001   .EQU  $1744          ; microKIM
SCANDS    .EQU  $1F1F
KEYIN     .EQU  $1F40
GETKEY    .EQU  $1F6A

        .ORG    $0000

START:  JSR     KEYIN
        BNE     START                   ; key still depressed - blank
        LDA     TMR0001                   ; random value
        LSR     A                       ; wipe high order bits
        LSR     A
        LSR     A
        LSR     A
        STA     TEMP                    ; save the digit
        ASL     A                       ; move back left
        ASL     A
        ASL     A
        ASL     A
        ORA     TEMP                    ; repeat the digit
        STA     INH                     ; put..
        STA     POINTL                  ;   ..into..
        STA     POINTH                  ;        ..display
LIGHT:  JSR     SCANDS                  ; light display
        JSR     GETKEY                  ; test keys
        CMP     TEMP                    ; right key?
        BEQ     START                   ; yes, blank & rpeat
        BNE     LIGHT

        .END

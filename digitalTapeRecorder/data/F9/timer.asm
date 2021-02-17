;
;-- Timer by Joel Swank
;

INH     .EQU   $F9
POINTL  .EQU   $FA
POINTH  .EQU   $FB

SCANDS  .EQU   $1F1F
GETKEY  .EQU   $1F6A
ROM     .EQU   $1C00

TMR0064 .EQU    $1746
TMR1024 .EQU    $1747

        .ORG    $0200

BEGN:   LDA     #$00
        STA     INH             ; ZERO DISPLAY
        STA     POINTL
        STA     POINTH
HOLD:   JSR     SCANDS          ; LIGHT DISPLAY
        JSR     GETKEY
        CMP     #$04            ; KEY 4?
        BNE     CONT
        JMP     $1C64           ; RETURN TO KIM
CONT:   CMP     #$02            ; KEY 2?
        BEQ     BEGN            ; BACK TO ZERO
        CMP     #$01            ; KEY 1?
        BNE     HOLD
        LDA     #$9C
        STA     TMR0064         ; SET TIMER
DISP:   JSR     SCANDS          ; DISPLAY VALUE
CLCK:   LDA     TMR1024         ; CHECK TIMER
        BEQ     CLCK
        STA     ROM             ; DELAY 4 MICROSEC.
        LDA     #$9C            ; SET TIMER
        STA     TMR0064
        CLC
        SED                     ; SET FLAGS
        LDA     INH
        ADC     #$01            ; INC. 100THS
        STA     INH
        LDA     POINTL
        ADC     #$00            ; INC. SECONDS
        STA     POINTL
        CMP     #$60            ; STOP AT 60
        BNE     CKEY
        LDA     #$00
        STA     POINTL          ; ZERO SECONDS
        LDA     POINTH
        CLC
        ADC     #$01            ; INC. MINUTES
        STA     POINTH
CKEY:   CLD
        JSR     GETKEY          ; READ KEYBOARD
        CMP     #$00            ; KEY 0?
        BNE     DISP
        BEQ     HOLD            ; STOP

.END

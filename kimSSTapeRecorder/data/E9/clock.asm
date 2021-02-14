;
;-- Clock by Charles Parsons
;

; PAGE ZERO LOCATIONS

NOTE        .EQU $0070         ; Sets frequency of note
QSEC        .EQU $0080         ; 1/4 second counter
SECS        .EQU $0081         ; second counter
MIN         .EQU $0082         ; minute counter
HR          .EQU $0083         ; hour counter
DAY         .EQU $0084         ; day counter for AM-PM
INH         .EQU $00F9
POINTL      .EQU $00FA
POINTH      .EQU $00FB

PBD         .EQU $1702
PBDD        .EQU $1703
TMR0001     .EQU $1704
TMR1024     .EQU $1707
TMR1024PB7  .EQU $170F          ; Write to Timer /1024 PB7 enabled
SAVE1       .EQU $1C05
SCANDS      .EQU $1F1F
GETKEY      .EQU $1F6A

; ESCAPE TO KIM IF 1 ON KIM IS PRESSED

;     This is a subroutine which will return to the KIM monitor routine
; without stopping the real time clock.  It is done by pressing 1 on the
; KIM keyboard.

       .ORG     $0300

KIM:    JSR     GETKEY          ; go back to KIM if
        CMP     #$01            ; KIM keyboard is one
        BNE     ENDR
        JSR     SCANDS          ; delay to make sure
        JSR     GETKEY
        CMP     #$01
        BNE     ENDR
        JMP     SAVE1
ENDR:   RTS

; TWO TONE SOUND TO INDICATE HOURS

;     This is a subroutine which when added to the clock display
; routine will use the real time clock data to produce one sound
; per hour on the hour,  The output is a speaker circuit as shown
; on Pg. 57  of the KIM-1 Manual.  It is hooked to PB0 rather than
; PA0.  The specific notes can be changed by altering 0330 and 033C.

;      .RES     11
       .BLOCK   11
       .ORG     $0320

BEEP:   LDA     MIN             ; on the hour?
        BNE     END             ; if not return
        LDA     SECS            ;  execute until SEC .EQU HR
        SEC
        SBC     HR
        BPL     END
AGAIN:  LDA     QSEC            ; first 1/4 second?
        BNE     ONE
        LDA     #$1E            ; set high note
        STA     NOTE
        BNE     GO              ; sound note for 1/4 second
ONE:    LDA     #$01            ; second 1/4 second?
        CMP     QSEC
        BNE     END
        LDA     #$28            ; set low note
        STA     NOTE
GO:     LDA     #$01            ; set I/O ports
        STA     PBDD
        INC     PBD             ; toggle speaker
        LDA     NOTE
        TAX                     ; set delay
DECR:   DEX
        BPL     DECR
END:    BMI     AGAIN           ; keep sounding
        RTS

; INTERRUPT ROUTINE

;     This routine uses the NMI to update a clock in zero page
; locations.  Since the crystal may be slightly off one MHz a
; fine adjustment is located at 0366.  NMI pointers must be set
; to the start of this program.

;      .RES     16
       .BLOCK   16
       .ORG     $0360

       PHA                      ; save A
       TXA
       PHA                      ; save X
       TYA
       PHA                      ; save Y
       LDA      #$83            ; fine adjust timing
       STA      TMR0001
TM:    BIT      TMR1024           ; test timer
       BPL      TM              ; loop until time out
       INC      QSEC            ; count 1/4 seconds
       LDA      #$04            ; do four times before
       CMP      QSEC            ; updating seconds
       BNE      RTN
       LDA      #$00            ; reset 1/4 second counter
       STA      QSEC
       CLC
       SED                      ; advance clock in decimal
       LDA      SECS
       ADC      #$01            ; advance seconds
       STA      SECS
       CMP      #$60            ; until 60 seconds
       BNE      RTN
       LDA      #$00            ; then start again
       STA      SECS
       LDA      MIN
       CLC
       ADC      #$01            ; and advance minutes
       STA      MIN
       CMP      #$60            ; until 60 minutes
       BNE      RTN
       LDA      #$00            ; then start again
       STA      MIN
       LDA      HR              ; and advance hours
       CLC
       ADC      #$01
       STA      HR
       CMP      #$12            ; until 12 hours
       BNE      TH
       INC      DAY             ; advance 1/2 day
TH:    CMP      #$13            ; if 13 hours
       BNE      RTN             ; start again with one
       LDA      #$01
       STA      HR
RTN:   CLD                      ; go back to hex mode
       LDA      #$F4            ; start timer with interrupt
       STA      TMR1024PB7           ; in 249,856 microseconds
       PLA
       TAY                      ; restore Y
       PLA
       TAX                      ; restore X
       PLA                      ; restore A
       RTI                      ; return from interrupt

; DISPLAY CLOCK ON KIM-1 READOUT

;      .RES     5
       .BLOCK   5
       .ORG     $03C0

        LDA      #$00           ; reset 1/4 second counter
        STA      QSEC
        LDA      #$F4           ; start timer with interrupt
        STA      TMR1024PB7
DSP:    LDA      SECS           ; start here if clock is running
        STA      INH            ; display clock on KIM
        LDA      MIN
        STA      POINTL
        LDA      HR
        STA      POINTH
        JSR      SCANDS
        JSR      KIM            ; escape to KIM
        JSR      BEEP           ; sound on the hour
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        JMP     DSP

        .END

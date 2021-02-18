;WRITTEN BY STEVE WOZNIAK & ALLEN BAUM
;AND PUBLISHED IN DOCTOR DOBBS JOURNAL
;SEPT 1976
;

;                 *=$0
PCL    =$0
PCH    =$1
COUNT  =$2
FORMAT =$3
LENGTH =$4
LMNEM  =$5
RMNEM  =$6
YSAVE  =$7
          ;
          ;KIM I/O TO FOLLOW
PRTBYT =$1E3B
OUTCH  =$1EA0
CRLF   =$1E2F
CLEAR  =$1C64
OUTSP  =$1E9E
          ;
          *=$2000
START  JSR DSMBL
           JSR OUTSP
           JSR OUTSP
           JSR OUTSP
           JMP CLEAR
DSMBL  LDA #13          ;COUNT FOR 13 INSTR. DSMBLY
           STA COUNT
DSMBL2 JSR INSTDS     ;DISASSEMBLE AND DISPLAY INSTR.
           JSR PCADJ
           STA PCL        ;UPDATE PCL,H TO NEXT INSTR.
           STY PCH
           DEC COUNT      ;DONE FIRST 19 INSTR?.
           BNE DSMBL2     ;YES, LOOP. ELSE DSMBL 20TH.
INSTDS JSR PRPC       ;PRINT PCL,H
           LDA (PCL,X)    ;GET OPCODE
           TAY
           LSR ; A           ;EVEN/ODD TEST
           BCC IEVEN
           LSR ; A           ;TEST BIT 1.
           BCS ERR        ;XXXXXX11 INSTR. INVALID.
           CMP #$22
           BEQ ERR        ;10001001 INSTR. INVALID.
           AND #$7        ;MASK 3 BITS FOR ADDRESS MODE &
           ORA #$80       ;ADD INDEXING OFFSET.
IEVEN  LSR ; A           ;LSB INTO CARRY FOR
           TAX            ;LEFT/RIGHT TEST BELOW.
           LDA MODE,X     ;INDEX INTO ADDRESS MODE TABLE.
           BCS RTMODE     ;IF CARRY SET USE LSD FOR
           LSR ; A           ;PRINT FORMAT INDEX
           LSR ; A 
           LSR ; A           ;IF CARRY CLEAR USE MSD.
           LSR ; A 
RTMODE AND #$F        ;MASK FOR 4-BIT INDEX.
           BNE GETFMT     ;$0 FOR INVALID OP CODES.
ERR    LDY #$80       ;SUBSTITUTE $80 FOR INVALID OP,
           LDA #$0        ;SET PRINT FORMAT INDEX TO 0
GETFMT TAX
           LDA MODE2,X    ;INDEX INTO PRINT FORMAT TABLE.
           STA FORMAT     ;SAVE FOR ADDRESS FIELD FORMAT.
           AND #$3        ;MASK 2-BIT LENGTH. 0=1-BYTE
           STA LENGTH     ;1=2-BYTE, 2=3-BYTE,
           TYA            ;OP CODE.
           AND #$8F       ;MASK IT FOR 1XXX1010 TEST.
           TAX            ;SAVE IT.
           TYA            ;OP CODE TO 'A' AGAIN.
           LDY #$3
           CPX #$8A
           BEQ MNNDX3
MNNDX1 LSR ; A 
           BCC MNNDX3     ;FORM INDEX INTO MNEMONIC TABLE.
           LSR ; A 
MNNDX2 LSR ; A           ;1XXX1010 -> 00101XXX
           ORA #$20       ;XXXYYY01 -> 00111XXX
           DEY            ;XXXYYY10 -> 00110XXX
           BNE MNNDX2     ;XXXYY100 -> 00100XXX
           INY            ;XXXXX000 -> 000XXXXX
MNNDX3 DEY
           BNE MNNDX1
           PHA            ;SAVE MNEMONIC TABLE INDEX.
PROP   LDA (PCL),Y
           JSR PRBYT
           LDX #$1
PROPBL JSR PRBL2
           CPY LENGTH     ;PRINT INSTR (1 TO 3 BYTES)
           INY            ;IN A 12 CHARACTYER FIELD.
           BCC PROP
           LDX #$3        ;CHAR COUNT FOR MNEMONIC PRINT.
           CPY #$4
           BCC PROPBL
           PLA            ;RECOVER MNEMONIC INDEX.
           TAY
           LDA MNEML,Y
           STA LMNEM      ;FETCH 3-CHAR MNEMONIC
           LDA MNEMR,Y    ;(PACKED IN TWO BYTES)
           STA RMNEM
PRMN1   LDA #$0
           LDY #$5
PRMN2   ASL RMNEM
           ROL LMNEM      ;SHIFT 5 BITS OF CHAR INTO 'A'.
           ROL ; A          ;(CLEAR CARRY)
           DEY
           BNE PRMN2
           ADC #$3F       ;ADD '?' OFFSET.
           JSR OUTC       ;OUTPUT A CHAR OR MNEMONIC
           DEX
           BNE PRMN1
           JSR PRBLNK     ;OUTPUT 3 BLANKS.
           LDX #$6        ;COUNT FOR 6 PRINT FORMAT BITS.
PRADR1  CPX #$3
           BNE PRADR3     ;IF X=3 THEN PRINT ADDRESS VAL.
           LDY LENGTH
           BEQ PRADR3     ;NO PRINT IF LENGTH=0.
PRADR2  LDA FORMAT
           CMP #$E8       ;HANDLE REL ADDRESSING MODE
           LDA (PCL),Y    ;SPECIAL (PRINT TARGET ADDR)
           BCS RELADR     ;(NOT DISPLACEMENT)
           JSR PRBYT      ;OUTPUT 1- OR 2- BYTE ADDRESS.
           DEY            ;MORE SIGNIFICANT BYTE FIRST.
           BNE PRADR2
PRADR3 ASL FORMAT      ;TEST NEXT PRINT FORMAT BIT.
         BCC PRADR4      ;IF 0, DONT PRINT
         LDA CHAR1-1,X   ;CORRESPONDING CHAR.
         JSR OUTC        ;OUTPUT 1 OR 2 CHARS.
         LDA CHAR2-1,X   ;(IF CHAR FROM CHAR2 IS 0,
         BEQ PRADR4      ;DON'T FRINT IT)
         JSR OUTC
PRADR4 DEX
         BNE PRADR1
         RTS             ;RETURN IF DONE 6 FORMAT BITS.
RELADR JSR PCADJ3      ;PCL,H + DISPL + 1 TO 'A','Y'.
         TAX
         INX
         BNE PRNTYX      ;  +1 TO 'X','Y'.
         INY
PRNTYX TYA
PRNTAX JSR PRBYT       ;PRINT TARGET ADDRESS OF BRANCH
PRNTX  TXA             ;AND RETURN
         JMP PRBYT
PRPC   JSR CRLF        ;OUTPUT CARRIAGE RETURN.
         LDA PCH
         LDX PCL
         JSR PRNTAX      ;OUTPUT PCL,H
         LDA #'-'
         JSR OUTC        ;"OUTPUT '_'
PRBLNK LDX #$3         ;BLANK COUNT
PRBL2  LDA #' '
PRBL3  JSR OUTC        ;OUTPUT A BLANK
         DEX
         BNE PRBL2       ;LOOP UNTIL COUNT =0
         RTS
PCADJ  LDA LENGTH      ;0=1-BYTE, 1=2-BYTE, 2=3-BYTE.
PCADJ2 SEC
PCADJ3 LDY PCH
         TAX             ;TEST DISPL SIGN (FOR REL
         BPL PCADJ4      ;BRANCH). EXTEND NEG
         DEY             ;BY DECREMENTING PCH.
PCADJ4 ADC PCL
         BCC RTS1        ;PCL+LENGTH (OR DISPL) +1 TO 'A'.
         INY             ;CARRY INTO 'Y' (PCH)
RTS1   RTS
OUTC   STY YSAVE
         JSR OUTCH
         LDY YSAVE
         RTS
PRBYT  STY YSAVE
         JSR PRTBYT
         LDY YSAVE
         RTS
  ;
  ;
  ;THE TABLES FOLLOW---
  ;
MODE   .BYTE $40,$2,$45,$3,$D0,$8,$40,$9,$30
  ;XXXXXXZ0 INSTRUCTIONS.
         .BYTE $22,$45,$33,$D0,$8,$40,$9,$40,$2,$45
  ;Z=0, LEFT HALF BYTE
  ;Z=1, RIGHT HALF BYTE
         .BYTE $33,$D0,$8,$40,$9,$40,$2,$45,$B3,$D0
         .BYTE $8,$40,$9,$0,$22,$44,$33,$D0,$8C,$44
         .BYTE $0,$11,$22,$44,$33,$D0,$8C,$44,$9A,$10
         .BYTE $22,$44,$33,$D0,$8,$40,$9,$10,$22,$44
         .BYTE $33,$D0,$8,$40,$9,$62
  ;YYXXXZ01 INSTRUCTIONS
         .BYTE $13,$78,$A9

MODE2  .BYTE $0        ;ERR
         .BYTE $21       ;IMM
         .BYTE $01       ;Z-PAG
         .BYTE $02       ;ABS
         .BYTE $0        ;IMPL
         .BYTE $80       ;ACC
         .BYTE $59       ;(Z-PAG,X)
         .BYTE $4D       ;(Z-PAG),Y
         .BYTE $11       ;Z-PAG,X
         .BYTE $12       ;ABS,X
         .BYTE $6        ;ABS,Y
         .BYTE $4A       ;(ABS)
         .BYTE $5        ;Z-PAG,Y
         .BYTE $1D       ;REL
CHAR1  .BYTE $2C,$29,$2C,$23,$28,$41
CHAR2  .BYTE $59,$0,$58,$00,$0,$0

  ;XXXXX000 INSTRUCTIONS
MNEML  .BYTE $1C,$8A,$1C,$23,$5D,$8B,$1B,$A1,$9D

         .BYTE $8A,$1D,$23,$9D,$8B,$1D,$A1,$0,$29,$19
         .BYTE $AE,$69,$A8,$19,$23,$24,$53,$1B,$23
         .BYTE $24,$53,$19,$A1,$0

   ;XXXYY100 INSTRUCTIONS.
          .BYTE $1A,$5B,$5B,$A5,$69,$24,$24

   ;1XXX1010 INSTRUCTIONS
          .BYTE $AE,$AE,$A8,$AD,$29,$0,$7C,$0

   ;XXXYYY10 INSTRUCTIONS
          .BYTE $15,$9C,$6D,$9C,$A5,$69,$29,$53

   ;XXXYY01 INSTRUCTIONS.
          .BYTE $84,$13,$34,$11,$A5,$69,$23,$A0

   ;XXXXX000 INSTRUCTIONS.
MNEMR  .BYTE $D8,$62,$5A,$48,$26,$62,$94,$88
          .BYTE $54,$44,$C8,$54,$68,$44,$E8,$94,$0,$B4
          .BYTE $8,$84,$74,$B4,$28,$6E,$74,$F4,$CC,$4A
          .BYTE $72,$F2,$A4,$8A

   ;XXXYY100 INSTRUCTIONS.
          .BYTE $0,$AA,$A2,$A2,$74,$74,$74,$72

   ;1XXX1010 INSTRUCTIONS.
          .BYTE $44,$68,$B2,$32,$B2,$0,$22,$0

   ;XXXYYY10 INSTRUCTIONS.
          .BYTE $1A,$1A,$26,$26,$72,$72,$88,$C8

   ;XXXYYY01 INSTRUCTIONS
          .BYTE $C4,$CA,$26,$48,$44,$44,$A2,$C8

FINISH .END

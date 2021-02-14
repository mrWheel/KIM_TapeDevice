;***********************************************************
; This memory test was originally based on Jim Butterfield's
; memory test program in the First Book of Kim, but has
; grown a bit.
;
; This now tests every memory location using a rolling 9-bit
; pattern.  Ie the pattern repeats every 9 bytes, so this
; will detect most shorted address line problems.  I use
; this to test memory boards, so it will run forever unless
; an error is detected.  At the end of each pass, a '.' is
; printed.
;
; This does output to the TTY port, so if you're only using
; the default KIM display, the output functions will need to
; be tweaked.  Not hard to do, but I didn't need it.
;
; Written February 2006 by Bob Applegate, but it uses some
; bits of code from Jim Butterfield, and Ross Archer
; (http://www.6502.org/source/io/primm.htm).
;
; bob@applegate.org
; www.k2ut.org - look for my KIM page
;
; Rev 1.0 - 02/02/2006
;
; .module "KIM-1 MEMORY TEST"
;
;***********************************************************
; These are KIM-specific symbols...
;
#define   start ($1c4f)
#define   prtbyt  ($1e3b)
#define   outch ($1ea0)
#define   pointl  ($00fa)
#define   pointh  ($00fb)
;
; ASCII things
;
;;#define   CR  .equ    $0d
#define   CR      $0d
;;#define   LF  .equ    $0a
#define   LF      $0a
;
; Test patterns
;
;;#define   patt_0  .equ $01
#define   patt_0   $01
;;#define   patt_9  .equ $ba
#define   patt_9   $ba
;
; Zero page storage
;
          .ORG $0030
;
pattern .ds 1
backup  .ds 1
dpl .ds 1
dph .ds 1
tempy .ds 1
;
;***********************************************************
; This is the start of the code.  This is a fairly large
; chunk of code, and since I'm usually testing memory in the
; 0400 - 13FF region, I put this into page 2.
;
; Just like Jim Butterfield's original code, the pages to
; start and end are stored in the first two bytes of the
; program.  Execution starts at main.
;
  .ORG  $0200
;
begin   .db $03     ;first page to test
end     .db $13     ;last page to test
;
; set up the initial pattern...
;
main    jsr puts      ;print greeting message
        .db CR, LF
        .db "Bob's memory test"
        .db CR, LF, 0
;
; Load the initial pattern.  pattern contains the next
; value to write, while backup contains the value expected
; next when we're reading back.
;
        lda #patt_0
        sta pattern
        sta backup
        ldx end
;
; Load the starting page into point and Y.  Y is the offset.
;         
bigloop lda  #0
        tay
        sta  pointl
        lda  begin
        sta  pointh
;
; Fill memory from the start page until we reach the end page.
; Write the current pattern byte, then cycle to the next pattern
; in the sequence.
;
        lda pattern
fill2   sta (pointl),y
;
        cmp  #patt_9   ;ninth pattern?
        bne  fill3
;
; Time to reload the pattern with 1 again.
;
        lda #patt_0
        jmp fill4   ;new pattern is in A
;
; Rotate the pattern left one bit.  If we have Carry, then load
; with the ninth pattern.
;
fill3   asl a
        bcc fill4
;
        lda #patt_9
;
; The new pattern is in A.  Now increment Y and see if we've
; wrapped to the next page.
;
fill4   iny
        bne fill2
;
; Y wrapped, see if we're done yet or not.
;
        inc pointh
        cpx pointh
        bcs fill2
;
;---------------------------------------------------------
; Okay, memory is filled, so now we have to test.  We begin
; with a copy of the test pattern stored in backup.
;
        sta pattern   ;next value to use
;
; Load the starting page into point and Y.  Y is the offset.
;
        lda #0
        tay
        sta pointl
        lda begin
        sta pointh
;
; Test to see if the location pointed to by (pointl),Y is the
; same as the expected test pattern in A.
;
        lda backup
test2   cmp (pointl),y
        bne fail
;
        cmp #patt_9   ;ninth pattern?
        bne test3
;
; Time to reload the pattern with 1 again.
;
        lda #patt_0
        jmp test4   ;new pattern is in A
;
; Rotate the pattern left one bit.  If we have Carry, then
; load with the ninth pattern.
;
test3   asl a
        bcc test4
;
        lda #patt_9   ;9th pattern
;
; The new pattern is in A.  Now increment Y and see if we've
; wrapped to the next page.
;
test4   sta backup
        iny
        bne test2
;
; Y wrapped, see if we're done yet or not.
;
        inc pointh
        cpx pointh
        bcs test2
;
;----------------------------------------------------
; One pass has passed successfully!
;
        lda #'.'
        jsr putch
        jmp bigloop
;
;----------------------------------------------------
; Hmmm.  Test failed.  Report the error and stop the
; test.  The operator can figure out what happened.
;
fail    jsr puts
        .db CR, LF
        .db "Test failed at address $", 0
        lda pointh
        jsr prtbyt
        lda pointl
        jsr prtbyt
        jsr puts
        .db ", wrote $", 0
        lda pattern
        jsr prtbyt
        jsr puts
        .db " but read $", 0
        lda backup
        jsr prtbyt
        jsr puts
        .db CR, LF, 0
        jmp start   ;go back to KIM.
;
;***********************************************************
; Write the char in A to the TTY.  Preserves X and Y.  The
; KIM outch routine saves X, but not Y, so this wrapper
; does it.
;
putch   sty tempy
        jsr outch
        ldy tempy
        rts
;
;***********************************************************
; From Ross Archer.  When called, the string following the
; jsr puts will be displayed.  The string ends with a 0.
; Clobbers Y and A.
;
puts    pla     ;Get the low part of "return" address
        sta dpl     
        pla
        sta dph   ;Get the high part of "return" address
psinb   ldy #1
        lda (dpl),y   ;Get the next string character
        inc dpl   ;update the pointer
        bne psicho    ;if not, we're pointing to next character
        inc dph   ;account for page crossing
psicho  ora #0    ;Set flags according to contents of A
        beq psix1   ;don't print the final NULL 
        jsr putch
        jmp psinb   ;back around
psix1   inc dpl
        bne psix2
        inc dph   ;account for page crossing
psix2   jmp (dpl)   ;return to byte following final NULL
        ;

.END

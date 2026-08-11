/ Copyright 1974 Bell Telephone Laboratories Inc
/ low core

br4 = 200
br5 = 240
br6 = 300
br7 = 340

. = 0^.
	br	1f
	4

/ trap vectors
	trap; br7+0.		/ bus error
	trap; br7+1.		/ illegal instruction
	trap; br7+2.		/ bpt-trace trap
	trap; br7+3.		/ iot trap
	trap; br7+4.		/ power fail
	trap; br7+5.		/ emulator trap
	trap; br7+6.		/ system entry

. = 40^.
.globl	start
1:	jmp	start


. = 60^.
	klin; br4
	klou; br4

. = 70^.
	pcin; br4
	pcou; br4

. = 100^.
	kwlp; br6
	kwlp; br6

. = 214^.
	tcio; br6

. = 220^.
	rkio; br5

. = 224^.
	tmio; br5

. = 240^.
	trap; br7+7.		/ programmed interrupt
	trap; br7+8.		/ floating point
	trap; br7+9.		/ segmentation violation

/ floating vectors
. = 300^.
	dcin; br5+0.
	dcou; br5+0.
	dcin; br5+1.
	dcou; br5+1.
	dcin; br5+2.
	dcou; br5+2.
	dcin; br5+3.
	dcou; br5+3.
	dcin; br5+4.
	dcou; br5+4.
	dcin; br5+5.
	dcou; br5+5.
	dcin; br5+6.
	dcou; br5+6.
	dcin; br5+7.
	dcou; br5+7.
	dcin; br5+8.
	dcou; br5+8.
	dcin; br5+9.
	dcou; br5+9.
	dcin; br5+10.
	dcou; br5+10.
	dcin; br5+11.
	dcou; br5+11.
	dcin; br5+12.
	dcou; br5+12.
	dcin; br5+13.
	dcou; br5+13.
	dcin; br5+14.
	dcou; br5+14.
	dcin; br5+15.
	dcou; br5+15.
	klin; br4+1+0.
	klou; br4+1+0.
	klin; br4+1+1.
	klou; br4+1+1.
	klin; br4+1+2.
	klou; br4+1+2.
	klin; br4+1+3.
	klou; br4+1+3.
	klin; br4+1+4.
	klou; br4+1+4.
	klin; br4+1+5.
	klou; br4+1+5.
	klin; br4+1+6.
	klou; br4+1+6.
	klin; br4+1+7.
	klou; br4+1+7.
	klin; br4+1+8.
	klou; br4+1+8.
	klin; br4+1+9.
	klou; br4+1+9.
	klin; br4+1+10.
	klou; br4+1+10.
	klin; br4+1+11.
	klou; br4+1+11.
	klin; br4+1+12.
	klou; br4+1+12.
	klin; br4+1+13.
	klou; br4+1+13.
	klin; br4+1+14.
	klou; br4+1+14.
	klin; br4+1+15.
	klou; br4+1+15.

//////////////////////////////////////////////////////
/		interface code to C
//////////////////////////////////////////////////////

.globl	call, trap

.globl	_klrint
klin:	jsr	r0,call; _klrint
.globl	_klxint
klou:	jsr	r0,call; _klxint

.globl	_pcrint
pcin:	jsr	r0,call; _pcrint
.globl	_pcpint
pcou:	jsr	r0,call; _pcpint

.globl	_clock
kwlp:	jsr	r0,call; _clock

.globl	_tcintr
tcio:	jsr	r0,call; _tcintr

.globl	_rkintr
rkio:	jsr	r0,call; _rkintr

.globl	_tmintr
tmio:	jsr	r0,call; _tmintr

.globl	_dcrint
dcin:	jsr	r0,call; _dcrint
.globl	_dcxint
dcou:	jsr	r0,call; _dcxint


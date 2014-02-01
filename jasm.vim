" Vim syntax file
" Language:	JakVM Assembler
" based off of jasm.vim

if version < 600
  syntax clear
elseif exists("b:current_syntax")
  finish
endif

let s:cpo_save = &cpo
set cpo&vim

setlocal iskeyword=a-z,A-Z,48-57,.,_
setlocal isident=a-z,A-Z,48-57,.,_
syn case ignore

syn keyword jasmRegister	ax bx cx sp
syn keyword jasmAddressSizes 	byte word
"syn keyword jasmDataDirectives
syn keyword jasmInstr 	add and
syn keyword jasmInstr	cal car cli cmp cmu
syn keyword jasmInstr	dec dei div
syn keyword jasmInstr	eni
syn keyword jasmInstr	hlt
syn keyword jasmInstr	inc int ior irf
syn keyword jasmInstr	jmp jiz jnz jlt jge jof jnf jmr jzr jnr jlr jgr jor jur
syn keyword jasmInstr	ldi ljp lod lri
syn keyword jasmInstr	mov mul mvi
syn keyword jasmInstr	neg nop not
syn keyword jasmInstr	pop pus
syn keyword jasmInstr	rcl rcr ret rol ror rst
syn keyword jasmInstr	shl shr sri sti sto sub swp
syn keyword jasmInstr	xor
"syn keyword jasmOperator

syn match	jasmNumericOperator	"[+-/*]"
syn match	jasmLogicalOperator	"[=|&~<>]\|<=\|>=\|<>"
" numbers
syn match	jasmBinaryNumber	"\<[01]\+b\>"
syn match	jasmHexNumber		"\<\d\x*h\>"
syn match	jasmHexNumber		"\<\(0x\|$\)\x*\>"
"syn match	jasmFPUNumber		"\<\d\+\(\.\d*\)\=\(e[-+]\=\d*\)\=\>"
syn match	jasmOctalNumber		"\<\(0\o\+o\=\|\o\+o\)\>"
syn match	jasmDecimalNumber	"\<\(0\|[1-9]\d*\)\>"
syn region	jasmComment		start=";" end="$"
syn region	jasmString		start="\"" end="\"\|$"
syn region	jasmString		start="'" end="'\|$"
"syn match	jasmSymbol		"[()|\[\]:]"
"syn match	jasmSpecial		"[#?%$,]"
syn match   jasmSymbol        "[,]"
syn match	jasmLabel		":[^; \t]\+:"
syn match   jasmConstant      "[\$][a-zA-Z0-9_-]*\>"
syn match   jasmDataAddress   "[(][^()]*[)]"
syn match   jasmSection   "^\s*\.[a-zA-Z]*"
syn match   jasmPreprocessor "^\s*#.*$"

hi def link	jasmAddressSizes	type
hi def link	jasmNumericOperator	jasmOperator
hi def link	jasmLogicalOperator	jasmOperator

hi def link	jasmBinaryNumber	jasmNumber
hi def link	jasmHexNumber		jasmNumber
hi def link	jasmFPUNumber		jasmNumber
hi def link	jasmOctalNumber		jasmNumber
hi def link	jasmDecimalNumber	jasmNumber

hi def link	jasmSymbols		jasmRegister
hi def link	jasmPreprocess		jasmDirective

"  link to standard syn groups so the 'colorschemes' work:
hi def link	jasmOperator operator
hi def link	jasmComment  comment
hi def link	jasmDirective	preproc
hi def link	jasmRegister  type
hi def link	jasmNumber   constant
hi def link	jasmSymbol special
hi def link	jasmConstant special
hi def link	jasmDataAddress special
hi def link	jasmSection special
hi def link	jasmString  String
hi def link	jasmSpecial	special
hi def link	jasmInstr keyword
hi def link	jasmLabel label
hi def link	jasmPrefix preproc
hi def link jasmIdent ident
hi def link jasmPreprocessor preproc
let b:current_syntax = "jasm"

let &cpo = s:cpo_save
unlet s:cpo_save

" vim: ts=8 sw=8 :

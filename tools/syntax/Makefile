HOMEDIR = ~

$(HOMEDIR)/.vim/syntax/jasm.vim: jasm.vim
	mkdir -p "$(HOMEDIR)/.vim/syntax/"
	tar cv jasm.vim | tar xv -C "$(HOMEDIR)/.vim/syntax/"

.PHONY: uninstall
uninstall:
	rm -f "$(HOMEDIR)/.vim/syntax/jasm.vim"

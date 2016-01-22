.PHONY: lint-check
.PHONY: prepare-doc gen-doc publish-doc

REPLACE_REVISION=./.replace_revision.sh
define REPLACE_REVISION_CMD
#!/bin/sh

str=$$3
if [ -e "$$2" ]; then
rev_in=`grep "$$3" $$2 | awk '{print $$3}' | tr -d '"'`
else
rev_in=""
fi
rev=`git rev-parse HEAD`;

if [ -n "$$4" ] || [ "$$rev" != "$$rev_in" ]; then
  mkdir -p `dirname $$2`
  sed -E "s/#define[[:space:]]+$$3[[:space:]].*/#define $$3 \"$$rev\"/" $$1 > $$2
fi
endef
export REPLACE_REVISION_CMD

$(REPLACE_REVISION) :
	@echo "$$REPLACE_REVISION_CMD" > $@
	@chmod a+x $@

gen-doc: prepare-doc
	rm -rf html/*
	doxygen doc/Doxyfile > /dev/null

prepare-doc:
	mkdir -p html
	if [ ! -d html/.git ]; then \
      git clone git@github.com:wantee/$(PROJECT).git html; \
    fi
	cd html && \
    git checkout gh-pages

publish-doc: gen-doc
	cd html && \
	git add * && \
	git commit -a -m"generate doc on $(shell date)" && \
	git push origin gh-pages

lint-check : *.h *.cpp *.c
	splint +posixlib +D__gnuc_va_list=int -fileextensions $^ 

clean-static:
	rm -rf $(REPLACE_REVISION)
	rm -f tags cscope.*

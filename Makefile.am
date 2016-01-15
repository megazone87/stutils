SUBDIRS = src

ACLOCAL_AMFLAGS = -I m4
AM_CFLAGS = -I$(top_srcdir)/src
AM_CFLAGS += -m64 -pipe

include aminclude_static.am
@VALGRIND_CHECK_RULES_LOCAL@

dist_doc_DATA = README.md
EXTRA_DIST = LICENSE doc/html

TESTS =

publish-doc: doxygen-run
	if [ ! -d .downloads/stutils-doc ]; then \
      git clone -b gh-pages git@github.com:wantee/stutils.git .downloads/stutils-doc; \
    fi
	rm -rf .downloads/stutils-doc/* && \
	cp -r doc/html/* .downloads/stutils-doc/ && \
	cd .downloads/stutils-doc/ && \
	git add * && \
	git commit -a -m"generate doc on $(shell date)" && \
	git push origin gh-pages

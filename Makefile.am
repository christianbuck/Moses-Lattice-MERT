## Process this file with automake to produce Makefile.in
## Created by Anjuta

SUBDIRS = src 

latticemertdocdir = ${prefix}/doc/latticemert
latticemertdoc_DATA = \
	README\
	COPYING\
	AUTHORS\
	ChangeLog\
	INSTALL\
	NEWS


EXTRA_DIST = $(latticemertdoc_DATA)


# Remove doc directory on uninstall
uninstall-local:
	-rm -r $(latticemertdocdir)

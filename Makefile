include include.mk

DOC_DIR = ./doc/
DOC_BUILD_DIR = ./doc/build

MAINPAGE = ./doc/mainpage.dox
LAYOUTFILE = .doc/layout_pdf.xml
DOXYFILE = ./doc/doxyfile
WORKING_DOXYFILE ?= temp_doxyfile
DOXYGEN_WARNING ?= doxygen.warning

include ./doc/mk_LibraryDoc.mk

all: lib_doc

DOC_DIR ?= ./
DOC_BUILD_DIR ?= ./build

MAINPAGE ?= mainpage.dox
LAYOUTFILE ?= layout_pdf.xml
DOXYFILE ?= doxyfile
WORKING_DOXYFILE ?= temp_doxyfile
DOXYGEN_WARNING ?= doxygen.warning

ECHO ?= echo
DOXYGEN_ECHO ?= echo
RM ?= rm
RM_DIR ?= rm -rf
COPY ?= cp

#change to your own doxygen path
DOXYGEN ?= C:/Program Files/doxygen/bin/doxygen.exe

#change to your own dot path
DOT_PATH ?= C:\sofit\shei\graphviz\bin\dot.exe

FILENAME = library

lib_doc: clean $(DOC_DIR)/$(FILENAME).pdf
	
$(DOC_DIR)/$(FILENAME).pdf:  
	$(COPY) $(DOXYFILE) $(WORKING_DOXYFILE)
	$(DOXYGEN_ECHO) >> $(WORKING_DOXYFILE)
	$(DOXYGEN_ECHO) INPUT=$(SRC_DIR) >> $(WORKING_DOXYFILE)
	$(DOXYGEN_ECHO) PROJECT_NAME="Arduino library documentation" >> $(WORKING_DOXYFILE)
	$(DOXYGEN_ECHO) PROJECT_BRIEF="" >> $(WORKING_DOXYFILE)
	$(DOXYGEN_ECHO) OUTPUT_DIRECTORY = $(DOC_BUILD_DIR) >> $(WORKING_DOXYFILE)
	$(DOXYGEN_ECHO) GENERATE_LATEX = YES >> $(WORKING_DOXYFILE)
	$(DOXYGEN_ECHO) GENERATE_HTML = NO >> $(WORKING_DOXYFILE)
	$(DOXYGEN_ECHO) LATEX_OUTPUT = latex >> $(WORKING_DOXYFILE)
	$(DOXYGEN_ECHO) SHOW_USED_FILES = NO >> $(WORKING_DOXYFILE)
	$(DOXYGEN_ECHO) SHOW_INCLUDE_FILES = NO >> $(WORKING_DOXYFILE)
	$(DOXYGEN_ECHO) SOURCE_BROWSER = NO >> $(WORKING_DOXYFILE)
	$(DOXYGEN_ECHO)  >> $(WORKING_DOXYFILE)
	$(DOXYGEN_ECHO) SHOW_FILES = NO >> $(WORKING_DOXYFILE)
	$(DOXYGEN_ECHO) DOT_PATH = "$(DOT_PATH)" >> $(WORKING_DOXYFILE)
	$(RM_DIR) $(DOC_BUILD_DIR)/latex/*
	$(DOXYGEN) $(WORKING_DOXYFILE)
	
	cd $(DOC_BUILD_DIR)/latex && pdflatex -quiet -interaction=batchmode refman
	cd $(DOC_BUILD_DIR)/latex && makeindex -q refman.idx
	cd $(DOC_BUILD_DIR)/latex && pdflatex -quiet -interaction=batchmode refman
	latex_count=8 ; \
	while egrep -s 'Rerun (LaTeX|to get cross-references right)' $(DOC_BUILD_DIR)/latex/refman.log && [ $$latex_count -gt 0 ] ;\
	    do \
	      echo "Rerunning latex...." ;\
	      cd $(DOC_BUILD_DIR)/latex && pdflatex -quiet -interaction=batchmode refman ;\
	      latex_count=`expr $$latex_count - 1` ;\
	    done
	cd $(DOC_BUILD_DIR)/latex && makeindex -q refman.idx
	cd $(DOC_BUILD_DIR)/latex && pdflatex -quiet -interaction=batchmode refman
	$(COPY) $(DOC_BUILD_DIR)/latex/refman.pdf $@
	$(RM) $(WORKING_DOXYFILE)

clean:
	@$(ECHO) Removing build directory
	@$(RM_DIR) $(DOC_BUILD_DIR)
	@$(ECHO) Removing temporary files
	@$(RM) $(DOXYGEN_WARNING)
	@$(RM) $(WORKING_DOXYFILE)
	@$(RM) $(DOC_DIR)/$(FILENAME).pdf

.PHONY: lib_doc clean
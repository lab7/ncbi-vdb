# ===========================================================================
#
#                            PUBLIC DOMAIN NOTICE
#               National Center for Biotechnology Information
#
#  This software/database is a "United States Government Work" under the
#  terms of the United States Copyright Act.  It was written as part of
#  the author's official duties as a United States Government employee and
#  thus cannot be copyrighted.  This software/database is freely available
#  to the public for use. The National Library of Medicine and the U.S.
#  Government have not placed any restriction on its use or reproduction.
#
#  Although all reasonable efforts have been taken to ensure the accuracy
#  and reliability of the software and data, the NLM and the U.S.
#  Government do not and cannot warrant the performance or results that
#  may be obtained by using this software or data. The NLM and the U.S.
#  Government disclaim all warranties, express or implied, including
#  warranties of performance, merchantability or fitness for any particular
#  purpose.
#
#  Please cite the author in any work or product based on this material.
#
# ===========================================================================

default: runtests

TOP ?= $(abspath ../..)

MODULE = test/vfs

TEST_TOOLS = \
    test-vfsmanager \
	path-test \
	test-path \


include $(TOP)/build/Makefile.env

$(TEST_TOOLS): makedirs
	@ $(MAKE_CMD) $(TEST_BINDIR)/$@

.PHONY: $(TEST_TOOLS)

clean: stdclean

#-------------------------------------------------------------------------------
# C test on VPath (aka test-vfs on Windows)
#

TEST_SRC = \
	path-test

TEST_OBJ = \
	$(addsuffix .$(OBJX),$(TEST_SRC))

TEST_LIB = \
	-skapp \
	-sncbi-vdb \

$(TEST_BINDIR)/path-test: $(TEST_OBJ)
	$(LD) --exe -o $@ $^ $(TEST_LIB)

valgrind_path_c: std
	valgrind --ncbi --show-reachable=no --suppressions=$(SRCDIR)/valgrind_suppressions.txt $(TEST_BINDIR)/path-test    
    
#----------------------------------------------------------------
# C++ test on VPath
#

PATHTEST_SRC = \
	pathtest 

PATHTEST_OBJ = \
	$(addsuffix .$(OBJX),$(PATHTEST_SRC))

PATHTEST_LIB = \
	-skapp \
	-sktst \
	-sncbi-vdb \

$(TEST_BINDIR)/test-path: $(PATHTEST_OBJ)
	$(LP) --exe -o $@ $^ $(PATHTEST_LIB)

valgrind_path_c++: std
	valgrind --ncbi --show-reachable=no --suppressions=$(SRCDIR)/valgrind_suppressions.txt $(TEST_BINDIR)/test-path    

#----------------------------------------------------------------
# C++ test on VFSManager
#

MANAGERTEST_SRC = \
	managertest 

MANAGERTEST_OBJ = \
	$(addsuffix .$(OBJX),$(MANAGERTEST_SRC))

MANAGERTEST_LIB = \
	-skapp \
	-sktst \
	-sncbi-vdb \

$(TEST_BINDIR)/test-vfsmanager: $(MANAGERTEST_OBJ)
	$(LP) --exe -o $@ $^ $(MANAGERTEST_LIB)

valgrind_path_manager: std
	valgrind --ncbi --show-reachable=no --suppressions=$(SRCDIR)/valgrind_suppressions.txt $(TEST_BINDIR)/test-vfsmanager    
    
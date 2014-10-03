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


default: std

TOP ?= $(abspath ../..)
MODULE = libs/ncbi-vdb

EXT_LIBS = \
	libncbi-vdb \
	libncbi-wvdb

ALL_LIBS = \
	$(EXT_LIBS)

include $(TOP)/build/Makefile.env

#-------------------------------------------------------------------------------
# outer targets
#
all std: makedirs
	@ $(MAKE_CMD) $(TARGDIR)/std

$(EXT_LIBS): makedirs
	@ $(MAKE_CMD) $(LIBDIR)/$@

.PHONY: all std $(ALL_LIBS)

#-------------------------------------------------------------------------------
# std
#
$(TARGDIR)/std: \
	$(addprefix $(LIBDIR)/,$(EXT_LIBS))

.PHONY: $(TARGDIR)/all $(TARGDIR)/std

#-------------------------------------------------------------------------------
# clean
#
clean: stdclean

.PHONY: clean

#-------------------------------------------------------------------------------
# virtual db library
# made in both static and shared versions
$(LIBDIR)/libncbi-vdb: $(addprefix $(LIBDIR)/libncbi-vdb.,$(LIBX) $(SHLX))

ifeq (linux,$(OS))
  LIBKFF = kff magic
else
  LIBKFF =
endif

ifeq (64,$(BITS))
  LIBVDB_BLAST = vdb-blast
else
  LIBVDB_BLAST =
endif

ifeq (win,$(OS))
LIBKQ =
LIBKXML =
else
LIBKQ = kq
LIBKXML = kxfs
endif

ifeq (1,$(HAVE_XML2))
LIBKXML = kxml
endif

## ?? why is this ordering used?
VDB_LIB_CMN = \
	align-access \
	ncbi-bam \
	vfs \
	$(LIBKXML) \
	kns \
	kfg \
	krypto \
	ksrch \
	kfs \
	klib \
	kproc \
	$(LIBKQ) \
	kfc \
	bz2 \
	z

VDB_LIB_RD = \
	align-reader \
	ascp \
	$(EXT_SCHEMA_MODULES) \
	vdb \
	kdb \
	sradb \
	srareader \
	$(LIBVDB_BLAST) \
	$(VDB_LIB_CMN) \
    ngs \
	ngs-jni \
	ngs-py

VDB_OBJ = \
	$(addprefix $(ILIBDIR)/lib,$(addsuffix .a,$(VDB_LIB_RD)))

VDB_LIB = \
	$(addprefix -s,$(VDB_LIB_RD))

$(LIBDIR)/libncbi-vdb.$(SHLX): $(VDB_OBJ)
	$(LD) --dlib --vers $(SRCDIR) -o $@ $(VDB_LIB)

$(LIBDIR)/libncbi-vdb.$(LIBX): $(VDB_OBJ)
	$(LD) --slib --vers $(SRCDIR) -o $@ $(VDB_LIB)

#-------------------------------------------------------------------------------
# update version
#
$(LIBDIR)/libncbi-wvdb: $(addprefix $(LIBDIR)/libncbi-wvdb.,$(LIBX) $(SHLX))

VDB_LIB_RDWR = \
	align-writer \
	waxf \
	wkdb \
	wsradb \
	wsraxf \
	wvdb \
	wvxf \
	wwgsxf \
	$(VDB_LIB_CMN)

WVDB_OBJ = \
	$(addprefix $(ILIBDIR)/lib,$(addsuffix .a,$(VDB_LIB_RDWR)))

WVDB_LIB = \
	$(addprefix -s,$(VDB_LIB_RDWR))

$(LIBDIR)/libncbi-wvdb.$(SHLX): $(WVDB_OBJ)
	$(LD) --dlib --vers $(SRCDIR) -o $@ $(WVDB_LIB) -lxml2

$(LIBDIR)/libncbi-wvdb.$(LIBX): $(WVDB_OBJ)
	$(LD) --slib --vers $(SRCDIR) -o $@ $(WVDB_LIB)
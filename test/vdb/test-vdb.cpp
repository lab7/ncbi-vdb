// ===========================================================================
//
//                            PUBLIC DOMAIN NOTICE
//               National Center for Biotechnology Information
//
//  This software/database is a "United States Government Work" under the
//  terms of the United States Copyright Act.  It was written as part of
//  the author's official duties as a United States Government employee and
//  thus cannot be copyrighted.  This software/database is freely available
//  to the public for use. The National Library of Medicine and the U.S.
//  Government have not placed any restriction on its use or reproduction.
//
//  Although all reasonable efforts have been taken to ensure the accuracy
//  and reliability of the software and data, the NLM and the U.S.
//  Government do not and cannot warrant the performance or results that
//  may be obtained by using this software or data. The NLM and the U.S.
//  Government disclaim all warranties, express or implied, including
//  warranties of performance, merchantability or fitness for any particular
//  purpose.
//
//  Please cite the author in any work or product based on this material.
//
// ===========================================================================

#include <vdb/manager.h> // VDBManager
#include <vdb/database.h> 
#include <vdb/table.h> 
#include <vdb/cursor.h> 
#include <vdb/blob.h> 
#include <vdb/vdb-priv.h>
#include <sra/sraschema.h> // VDBManagerMakeSRASchema
#include <vdb/schema.h> /* VSchemaRelease */

extern "C" {
    #include <../libs/vdb/blob-priv.h>
    #include <../libs/vdb/page-map.h>
}

#include <ktst/unit_test.hpp> // TEST_CASE
#include <kfg/config.h> 

#include <sysalloc.h>
#include <cstdlib>
#include <stdexcept>

using namespace std;

TEST_SUITE( VdbTestSuite )

extern "C" rc_t VDBManagerRunPeriodicTasks(const VDBManager*);

TEST_CASE( VdbMgr ) {
    const VDBManager *mgr = NULL;
    rc_t rc = VDBManagerMakeRead(NULL, NULL);
    if (rc == 0)
        FAIL( "FAIL: VDBManagerMakeRead(NULL) succeed" );

    rc = VDBManagerMakeRead(&mgr, NULL);
    if (rc != 0)
        FAIL( "failed to make VDB manager" );

    rc = VDBManagerAddRef(mgr);
    if (rc != 0)
        FAIL( "failed to AddRef VDB manager" );

    rc = VDBManagerRelease(mgr);
    if (rc != 0)
        FAIL( "failed to release VDB manager" );

    rc = VDBManagerVersion(mgr, NULL);
    if (rc == 0)
        FAIL( "FAIL: VDBManagerVersion(mgr, NULL) succeed" );

    uint32_t version = 0;
    rc = VDBManagerVersion(NULL, &version);
    if (rc == 0)
        FAIL( "FAIL: VDBManagerVersion(NULL) succeed" );

    rc = VDBManagerVersion(mgr, &version);
    if (rc == 0)
        CHECK_GE( version, (uint32_t)0x2000000 );
    else
        FAIL( "failed to get VDB manager Version" );

    rc = VDBManagerRunPeriodicTasks(NULL);
    if (rc == 0)
        FAIL( "FAIL: VDBManagerRunPeriodicTasks(NULL) succeed" );

    rc = VDBManagerRunPeriodicTasks(mgr);
    if (rc != 0)
        FAIL( "failed VDBManagerRunPeriodicTasks" );

    // In the following, NULL and "" are interpreted as "."

    // VDBManagerAddSchemaIncludePath
    REQUIRE_RC(VDBManagerAddSchemaIncludePath(mgr, NULL));
    REQUIRE_RC(VDBManagerAddSchemaIncludePath(mgr, ""));
#ifndef WINDOWS    
    REQUIRE_RC(VDBManagerAddSchemaIncludePath(mgr, "/"));
    REQUIRE_RC(VDBManagerAddSchemaIncludePath(mgr, "/usr"));
#else    
    REQUIRE_RC_FAIL(VDBManagerAddSchemaIncludePath(mgr, "/"));
    REQUIRE_RC_FAIL(VDBManagerAddSchemaIncludePath(mgr, "/usr"));
#endif

    // VDBManagerAddLoadLibraryPath
    REQUIRE_RC(VDBManagerAddLoadLibraryPath(mgr, NULL));
    REQUIRE_RC(VDBManagerAddLoadLibraryPath(mgr, ""));
#ifndef WINDOWS    
    REQUIRE_RC(VDBManagerAddLoadLibraryPath(mgr, "/"));
    REQUIRE_RC(VDBManagerAddLoadLibraryPath(mgr, "/usr"));
#else
    REQUIRE_RC_FAIL(VDBManagerAddLoadLibraryPath(mgr, "/"));
    REQUIRE_RC_FAIL(VDBManagerAddLoadLibraryPath(mgr, "/usr"));
#endif
    //

    struct Test { int i; };
    Test t;
    t.i = 12345;
    rc = VDBManagerGetUserData(mgr, NULL);
    if (rc == 0)
        FAIL( "FAIL: VDBManagerGetUserData(mgr, NULL) succeed" );
    void *data = NULL;
    rc = VDBManagerGetUserData(NULL, &data);
    if (rc == 0)
        FAIL( "FAIL: VDBManagerGetUserData(NULL) succeed" );
    rc = VDBManagerGetUserData(mgr, &data);
    if (rc != 0)
        FAIL( "failed VDBManagerGetUserData" );
    if (data != NULL)
        FAIL( "VDBManagerGetUserData != NULL" );

    rc = VDBManagerSetUserData(NULL, &t, free);
    if (rc == 0)
        FAIL( "FAIL: VDBManagerSetUserData(NULL) succeed" );
    rc = VDBManagerSetUserData(mgr, &t, NULL);
    if (rc != 0)
        FAIL( "failed VDBManagerSetUserData" );
    rc = VDBManagerGetUserData(mgr, &data);
    if (rc != 0)
        FAIL( "failed VDBManagerGetUserData after Set" );
    if (data == NULL)
        FAIL( "VDBManagerGetUserData == NULL after Get" );
    else {
        Test *r = static_cast<Test*>(data);
        CHECK_EQUAL( r->i, 12345 );
    }

    Test *pt = static_cast<Test*>(malloc(sizeof *pt));
    rc = VDBManagerSetUserData(mgr, pt, free);
    if (rc != 0)
        FAIL( "failed VDBManagerSetUserData(free)" );

    //

    rc = VDBManagerRelease(mgr);
    if (rc != 0)
        FAIL( "failed to release VDB manager" );
    mgr = NULL;

}
#if 0
//#if _ARCH_BITS != 32
TEST_CASE(SimultaneousCursors)
{   // SRA-1669 WGS ALWZ01 cannot open multiple cursors with SEQUENCE.CONTIG_NAME column simultaneously (Win32)

    for ( int i = 0; i < 1; ++i ) {
        const VDBManager* mgr = 0;
        REQUIRE_RC(VDBManagerMakeRead(&mgr, 0));
        const VDatabase* db = 0;
        REQUIRE_RC(VDBManagerOpenDBRead(mgr, &db, 0, "ALWZ01"));
        const VTable* table = 0;
        REQUIRE_RC(VDatabaseOpenTableRead(db, &table, "SEQUENCE"));
        const size_t CURSOR_CNT = 10; 
        const VCursor* cursors[CURSOR_CNT] = {};
        for ( size_t i = 0; i < CURSOR_CNT; ++i ) {
            const VCursor* cursor = 0;
            REQUIRE_RC(VTableCreateCursorRead(table, &cursor));
            cursors[i] = cursor;
            REQUIRE_RC(VCursorPermitPostOpenAdd(cursor));
            REQUIRE_RC(VCursorOpen(cursor));
            uint32_t col_index;
            REQUIRE_RC(VCursorAddColumn(cursor, &col_index, "CONTIG_NAME"));
        }
        for ( size_t i = 0; i < CURSOR_CNT; ++i ) {
            REQUIRE_RC(VCursorRelease(cursors[i]));
        }
        REQUIRE_RC(VTableRelease(table));
        REQUIRE_RC(VDatabaseRelease(db));
        REQUIRE_RC(VDBManagerRelease(mgr));
    }
}
#endif

class VdbFixture
{
public:
    VdbFixture()
    : mgr(0), curs(0), col_idx(~0)
    {
        if ( VDBManagerMakeRead(&mgr, NULL) != 0 )
            throw logic_error ( "VdbFixture: VDBManagerMakeRead failed" );
    }
    
    ~VdbFixture()
    {
        if ( mgr && VDBManagerRelease ( mgr ) != 0 )
            throw logic_error ( "~VdbFixture: VDBManagerRelease failed" );
        if ( curs && VCursorRelease ( curs ) != 0 )
            throw logic_error ( "~VdbFixture: VCursorRelease failed" );
    }
    
    rc_t Setup( const char * acc, const char* p_colName )
    {
        const VDatabase *db = NULL;
        rc_t rc = VDBManagerOpenDBRead ( mgr, &db, NULL, acc );
        rc_t rc2;

        const VTable *tbl = NULL;
        if (rc == 0) 
        {
            rc = VDatabaseOpenTableRead ( db, &tbl, "SEQUENCE" );
        }
        else 
        {
            rc = VDBManagerOpenTableRead ( mgr, &tbl, NULL, acc );
            if (rc != 0) 
            {
                VSchema *schema = NULL;
                rc = VDBManagerMakeSRASchema(mgr, &schema);
                if ( rc != 0 )
                {
                    return rc;
                }
                    
                rc = VDBManagerOpenTableRead ( mgr, &tbl, schema, acc );
                
                rc2 = VSchemaRelease ( schema );
                if ( rc == 0 )
                    rc = rc2;
            }
        }

        if ( rc == 0 )
        {
            rc = VTableCreateCursorRead(tbl, &curs);
            if ( rc == 0 )
            {
                col_idx = ~0;
                rc = VCursorAddColumn ( curs, &col_idx, p_colName );
                if ( rc == 0 )
                {
                    rc = VCursorOpen(curs);
                }
            }
        }
        
        rc2 = VTableRelease ( tbl );
        if ( rc == 0 )
            rc = rc2;
        
        if ( db != NULL )
        {
            rc2 = VDatabaseRelease ( db );
            if ( rc == 0 )
               rc = rc2;
        }
            
        return rc;
    }
    
    const VDBManager * mgr;
    const VCursor * curs;
    uint32_t col_idx;
};

FIXTURE_TEST_CASE(TestCursorIsStatic_SingleRowRun1, VdbFixture) 
{
    REQUIRE_RC ( Setup ( "SRR002749", "READ_LEN" ) );
    bool is_static = false;
    REQUIRE_RC ( VCursorIsStaticColumn ( curs, col_idx, &is_static) );
    REQUIRE ( is_static );
}
FIXTURE_TEST_CASE(TestCursorIsStatic_VariableREAD_LEN, VdbFixture) 
{
    REQUIRE_RC ( Setup ( "SRR050566", "READ_LEN" ) );
    bool is_static = true;
    REQUIRE_RC ( VCursorIsStaticColumn ( curs, col_idx, &is_static) );
    REQUIRE ( ! is_static );
}
#if 0
FIXTURE_TEST_CASE(TestCursorIsStatic_SingleRowRun2, VdbFixture) 
{
    REQUIRE_RC ( Setup ( "SRR053325", "READ_LEN" ) );
    bool is_static = false;
    REQUIRE_RC ( VCursorIsStaticColumn ( curs, col_idx, &is_static) );
    REQUIRE ( is_static );
}
#endif
FIXTURE_TEST_CASE(TestCursorIsStatic_FixedREAD_LEN_MultipleRows, VdbFixture) 
{
    REQUIRE_RC ( Setup ( "SRR125365", "READ_LEN" ) );
    bool is_static = false;
    REQUIRE_RC ( VCursorIsStaticColumn ( curs, col_idx, &is_static) );
    REQUIRE ( is_static );
}
FIXTURE_TEST_CASE(TestCursorIsStatic_DB_FixedREAD_LEN_MultipleRows, VdbFixture) 
{
    REQUIRE_RC ( Setup ( "SRR600096", "READ_LEN" ) );
    bool is_static = false;
    REQUIRE_RC ( VCursorIsStaticColumn ( curs, col_idx, &is_static) );
    REQUIRE ( is_static );
}
FIXTURE_TEST_CASE(TestCursorIsStatic_DB_VariableREAD_LEN_MultipleRows, VdbFixture) 
{
    REQUIRE_RC ( Setup ( "SRR619505", "READ_LEN" ) );
    bool is_static = true;
    REQUIRE_RC ( VCursorIsStaticColumn ( curs, col_idx, &is_static) );
    REQUIRE ( ! is_static );
}

/////////////////// VBlob

FIXTURE_TEST_CASE(VCursor_GetBlob_SRA, VdbFixture) 
{   // multiple fragments per row (some are technical), multiple rows per blob
    REQUIRE_RC ( Setup ( "SRR000123", "READ" ) );
    REQUIRE_RC ( VCursorOpen (curs ) );
    
    {
        REQUIRE_RC ( VCursorSetRowId (curs, 1 ) );
        REQUIRE_RC ( VCursorOpenRow (curs ) );
        
        struct VBlob const *blob;
        REQUIRE_RC ( VCursorGetBlob ( curs, &blob, col_idx ) );
        
        int64_t first;
        uint64_t count;
        REQUIRE_RC ( VBlobIdRange ( blob, &first, &count ) );
        REQUIRE_EQ ( (int64_t)1, first );    
        REQUIRE_EQ ( (uint64_t)4, count );
        
        REQUIRE_EQ ( (size_t)858, BlobBufferBytes ( blob ) );
        
        //TODO: use row map to convert an offset into a rowId
        REQUIRE_NOT_NULL ( blob->pm ); 
        REQUIRE ( ! blob->pm->random_access );
        REQUIRE_EQ ( PageMap::eBlobPageMapOptimizedNone, blob->pm->optimized );
        
        REQUIRE_EQ ( (pm_size_t)4, blob->pm->leng_recs ); // not expanded ?
        REQUIRE_NOT_NULL ( blob->pm->length );   // array of lengths, size blob->pm->leng_recs
        REQUIRE_EQ ( (elem_count_t)157, blob->pm->length[0] ); 
        REQUIRE_EQ ( (elem_count_t)280, blob->pm->length[1] ); 
        REQUIRE_EQ ( (elem_count_t)168, blob->pm->length[2] ); 
        REQUIRE_EQ ( (elem_count_t)253, blob->pm->length[3] ); 
        
        REQUIRE_NOT_NULL ( blob->pm->leng_run ); // array of run lengths, size blob->pm->leng_recs
        REQUIRE_EQ ( (row_count_t)1, blob->pm->leng_run[0] ); 
        REQUIRE_EQ ( (row_count_t)1, blob->pm->leng_run[1] ); 
        REQUIRE_EQ ( (row_count_t)1, blob->pm->leng_run[2] ); 
        REQUIRE_EQ ( (row_count_t)1, blob->pm->leng_run[3] ); 
        //NOTE: very unlikely to happen for READ column, but if leng_run > 1, the corresponding fragments should all appear in the search results

        REQUIRE_EQ ( (pm_size_t)4, blob->pm->data_recs ); 
        
        REQUIRE_EQ ( (row_count_t)4,  blob->pm->row_count );
        REQUIRE_EQ ( (row_count_t)0,  blob->pm->pre_exp_row_count );
        
        REQUIRE_RC ( VCursorCloseRow (curs ) );
        REQUIRE_RC ( VBlobRelease ( (struct VBlob *) blob ) );
    }

    {
        REQUIRE_RC ( VCursorSetRowId (curs, 5 ) );
        REQUIRE_RC ( VCursorOpenRow (curs ) );

        struct VBlob const *blob;

        int64_t first;
        uint64_t count;
        REQUIRE_RC ( VCursorGetBlob ( curs, &blob, col_idx ) );
        REQUIRE_RC ( VBlobIdRange ( blob, &first, &count ) );
        REQUIRE_EQ ( (int64_t)5, first );    
        REQUIRE_EQ ( (uint64_t)4, count );
        
        REQUIRE_EQ ( (size_t)1221, BlobBufferBytes ( blob ) );
        
        //TODO: use row map to convert an offset into a rowId

        REQUIRE_RC ( VCursorCloseRow (curs ) );
        REQUIRE_RC ( VBlobRelease ( (struct VBlob *) blob ) );
    }        
}

FIXTURE_TEST_CASE(VCursor_GetBlob_WGS, VdbFixture) 
{   // single fragment per row, multiple rows per blob
    REQUIRE_RC ( Setup ( "ALWZ01", "READ" ) );
    REQUIRE_RC ( VCursorOpen (curs ) );
    
    {
        REQUIRE_RC ( VCursorSetRowId (curs, 1 ) );
        REQUIRE_RC ( VCursorOpenRow (curs ) );
        
        struct VBlob const *blob;
        REQUIRE_RC ( VCursorGetBlob ( curs, &blob, col_idx ) );
        
        int64_t first;
        uint64_t count;
        REQUIRE_RC ( VBlobIdRange ( blob, &first, &count ) );
        REQUIRE_EQ ( (int64_t)1, first );    
        REQUIRE_EQ ( (uint64_t)4, count );
        
        REQUIRE_EQ ( (size_t)948, BlobBufferBytes ( blob ) );
        
        //TODO: use row map to convert an offset into a rowId
        REQUIRE_NOT_NULL ( blob->pm ); 
        REQUIRE ( ! blob->pm->random_access );
        REQUIRE_EQ ( PageMap::eBlobPageMapOptimizedNone, blob->pm->optimized );
        
        REQUIRE_EQ ( (pm_size_t)4, blob->pm->leng_recs ); // not expanded ?
        REQUIRE_NOT_NULL ( blob->pm->length );   // array of lengths, size blob->pm->leng_recs
        REQUIRE_EQ ( (elem_count_t)227, blob->pm->length[0] ); 
        REQUIRE_EQ ( (elem_count_t)245, blob->pm->length[1] ); 
        REQUIRE_EQ ( (elem_count_t)211, blob->pm->length[2] ); 
        REQUIRE_EQ ( (elem_count_t)265, blob->pm->length[3] ); 
        
        REQUIRE_NOT_NULL ( blob->pm->leng_run ); // array of run lengths, size blob->pm->leng_recs
        REQUIRE_EQ ( (row_count_t)1, blob->pm->leng_run[0] ); 
        REQUIRE_EQ ( (row_count_t)1, blob->pm->leng_run[1] ); 
        REQUIRE_EQ ( (row_count_t)1, blob->pm->leng_run[2] ); 
        REQUIRE_EQ ( (row_count_t)1, blob->pm->leng_run[3] ); 
        //NOTE: very unlikely to happen for READ column, but if leng_run > 1, the corresponding fragments should all appear in the search results

        REQUIRE_EQ ( (pm_size_t)4, blob->pm->data_recs ); 
        
        REQUIRE_EQ ( (row_count_t)4,  blob->pm->row_count );
        REQUIRE_EQ ( (row_count_t)0,  blob->pm->pre_exp_row_count );
        
        REQUIRE_RC ( VCursorCloseRow (curs ) );
        REQUIRE_RC ( VBlobRelease ( (struct VBlob *) blob ) );
    }

    {
        REQUIRE_RC ( VCursorSetRowId (curs, 5 ) );
        REQUIRE_RC ( VCursorOpenRow (curs ) );

        struct VBlob const *blob;

        int64_t first;
        uint64_t count;
        REQUIRE_RC ( VCursorGetBlob ( curs, &blob, col_idx ) );
        REQUIRE_RC ( VBlobIdRange ( blob, &first, &count ) );
        REQUIRE_EQ ( (int64_t)5, first );    
        REQUIRE_EQ ( (uint64_t)4, count );
        
        REQUIRE_EQ ( (size_t)1184, BlobBufferBytes ( blob ) );
        
        //TODO: use row map to convert an offset into a rowId

        REQUIRE_RC ( VCursorCloseRow (curs ) );
        REQUIRE_RC ( VBlobRelease ( (struct VBlob *) blob ) );
    }        
}

#if VDB_2858
FIXTURE_TEST_CASE(VCursor_GetBlob_BlobIdRange_WGS, VdbFixture) 
{   // single fragment per row, multiple rows per blob
    REQUIRE_RC ( Setup ( "ALAI01", "READ" ) );
    REQUIRE_RC ( VCursorOpen (curs ) );
    
    int64_t first;
    uint64_t count;
    
    REQUIRE_RC ( VCursorIdRange (curs, 0, &first, &count ) );
cout << first << ", " << count << endl;             

    int64_t rowId = 1;
    while (true) 
    {
        REQUIRE_RC ( VCursorSetRowId (curs, rowId ) );
        REQUIRE_RC ( VCursorOpenRow (curs ) );
        
        struct VBlob const *blob;
        if ( VCursorGetBlob ( curs, &blob, col_idx ) != 0 )
        {
            break;
        }
        
        REQUIRE_RC ( VBlobIdRange ( blob, &first, &count ) );
cout << first << ", " << count << endl;             
        REQUIRE_EQ ( rowId, first );
        
        REQUIRE_RC ( VCursorCloseRow (curs ) );
        REQUIRE_RC ( VBlobRelease ( (struct VBlob *) blob ) );
        
        rowId += count;
    }
}
#endif

FIXTURE_TEST_CASE(PageMapIterator_WGS, VdbFixture) 
{   // single fragment per row, multiple rows per blob
    REQUIRE_RC ( Setup ( "ALWZ01", "READ" ) );
    REQUIRE_RC ( VCursorOpen (curs ) );
    
    {
        REQUIRE_RC ( VCursorSetRowId (curs, 1 ) );
        REQUIRE_RC ( VCursorOpenRow (curs ) );
        
        struct VBlob const *blob;
        REQUIRE_RC ( VCursorGetBlob ( curs, &blob, col_idx ) );
        
        PageMapIterator pmIt;
        REQUIRE_RC ( PageMapNewIterator ( (const PageMap*)blob->pm, &pmIt, 0, 4 ) ); 
        REQUIRE_EQ ( (elem_count_t)227, PageMapIteratorDataLength ( &pmIt ) );     
        REQUIRE_EQ ( (elem_count_t)0, PageMapIteratorDataOffset ( &pmIt ) );   
        REQUIRE_EQ ( (row_count_t)1, PageMapIteratorRepeatCount ( &pmIt ) );   
         
        REQUIRE ( PageMapIteratorNext ( &pmIt ) );
        REQUIRE_EQ ( (elem_count_t)245, PageMapIteratorDataLength ( &pmIt ) );     
        REQUIRE_EQ ( (elem_count_t)227, PageMapIteratorDataOffset ( &pmIt ) );   
        REQUIRE_EQ ( (row_count_t)1, PageMapIteratorRepeatCount ( &pmIt ) );   
        
        REQUIRE ( PageMapIteratorNext ( &pmIt ) );
        REQUIRE_EQ ( (elem_count_t)211, PageMapIteratorDataLength ( &pmIt ) );     
        REQUIRE_EQ ( (elem_count_t)227+245, PageMapIteratorDataOffset ( &pmIt ) );   
        REQUIRE_EQ ( (row_count_t)1, PageMapIteratorRepeatCount ( &pmIt ) );   
        
        REQUIRE ( PageMapIteratorNext ( &pmIt ) );
        REQUIRE_EQ ( (elem_count_t)265, PageMapIteratorDataLength ( &pmIt ) );     
        REQUIRE_EQ ( (elem_count_t)227+245+211, PageMapIteratorDataOffset ( &pmIt ) );   
        REQUIRE_EQ ( (row_count_t)1, PageMapIteratorRepeatCount ( &pmIt ) );   
        
        REQUIRE ( ! PageMapIteratorNext ( &pmIt ) );
        
        REQUIRE_RC ( VCursorCloseRow (curs ) );
        REQUIRE_RC ( VBlobRelease ( (struct VBlob *) blob ) );
    }
}


//////////////////////////////////////////// Main
extern "C"
{

#include <kapp/args.h>

ver_t CC KAppVersion ( void )
{
    return 0x1000000;
}
rc_t CC UsageSummary (const char * progname)
{
    return 0;
}

rc_t CC Usage ( const Args * args )
{
    return 0;
}

const char UsageDefaultName[] = "test-vdb";

rc_t CC KMain ( int argc, char *argv [] )
{
    KConfigDisableUserSettings();
    rc_t rc=VdbTestSuite(argc, argv);
    return rc;
}

}

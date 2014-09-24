/*===========================================================================
*
*                            PUBLIC DOMAIN NOTICE
*               National Center for Biotechnology Information
*
*  This software/database is a "United States Government Work" under the
*  terms of the United States Copyright Act.  It was written as part of
*  the author's official duties as a United States Government employee and
*  thus cannot be copyrighted.  This software/database is freely available
*  to the public for use. The National Library of Medicine and the U.S.
*  Government have not placed any restriction on its use or reproduction.
*
*  Although all reasonable efforts have been taken to ensure the accuracy
*  and reliability of the software and data, the NLM and the U.S.
*  Government do not and cannot warrant the performance or results that
*  may be obtained by using this software or data. The NLM and the U.S.
*  Government disclaim all warranties, express or implied, including
*  warranties of performance, merchantability or fitness for any particular
*  purpose.
*
*  Please cite the author in any work or product based on this material.
*
* ===========================================================================
*
*/

#ifndef _h_simple_VDBSpot_
#define _h_simple_VDBSpot_

#ifndef _h_simple_extern_
#include <simple/extern.h>
#endif

#ifndef _h_klib_defs_
#include <klib/defs.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif


/*--------------------------------------------------------------------------
 * forwards
 */
struct VCursor;
struct SRA_Spot;
struct SRA_Sequence;
struct SRAException;


/*--------------------------------------------------------------------------
 * VDBSpot
 *  an entire spot
 */

/* MakeIterator
 *  makes a spot iterator
 */
struct SRA_Spot *VDBSpotMakeIterator ( struct SRAException *x, struct VCursor const *curs );

/* MakeRangeIterator
 *  makes a spot iterator over a specific range
 */
struct SRA_Spot *VDBSpotMakeRangeIterator ( struct SRAException *x,
    struct VCursor const *curs, uint64_t startIdx, uint64_t spotCount );


/*--------------------------------------------------------------------------
 * VDBSpotRead
 *  individual reads from spots
 */

/* MakeIterator
 *  makes a spot-read iterator
 */
struct SRA_Sequence *VDBSpotReadMakeIterator ( struct SRAException *x,
    struct VCursor const *curs, bool biological_only );

/* MakeRangeIterator
 *  makes a spot-read iterator over a specific range
 */
struct SRA_Sequence *VDBSpotReadMakeRangeIterator ( struct SRAException *x,
    struct VCursor const *curs, uint64_t startReadIdx, uint64_t readCount, bool biological_only );

/* MakeSpotIterator
 *  makes a spot-read iterator over the reads of a single spot
 */
struct SRA_Sequence *VDBSpotReadMakeSpotIterator ( struct SRAException *x,
    struct VCursor const *curs, int64_t row_id, bool biological_only );


#ifdef __cplusplus
}
#endif

#endif /* _h_simple_VDBSpot_ */

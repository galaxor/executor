/* Copyright 1986 - 2000 by Abacus Research and
 * Development, Inc.  All rights reserved.
 */

/* Forward declarations in ResourceMgr.h (DO NOT DELETE THIS LINE) */

#include "rsys/common.h"

#include "ResourceMgr.h"
#include "QuickDraw.h"
#include "MemoryMgr.h"

#include "rsys/resource.h"
#include "rsys/file.h"
#include "rsys/mman.h"
#include "rsys/flags.h"
#include "rsys/version.h"
#include "rsys/appearance.h"

using namespace Executor;

/* extracts an int32 from a string of the form [^(]*([0-9]*).*
 * () == 0, any invalid string == -1
 */

PRIVATE int32
extract_vers_num(Handle h)
{
    uint32 retval, old_retval;
    const char *p, *ep;
    bool seen_left, seen_right, done;

    retval = old_retval = 0;
    seen_left = false;
    seen_right = false;
    done = false;
    for(p = (char *)STARH(h), ep = p + GetHandleSize(h);
        !done && p < ep; ++p)
    {
        switch(*p)
        {
            case '(':
                if(seen_left)
                    done = true;
                else
                    seen_left = true;
                break;
            case ')':
                seen_right = true;
                done = true;
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                if(seen_left)
                    retval = retval * 10 + *p - '0';
                if(retval < old_retval)
                    done = true;
                else
                    old_retval = retval;
                break;
            default:
                if(seen_left)
                    done = true;
                break;
        }
    }
    if(!seen_right)
        retval = -1;
    return retval;
}

P0(PUBLIC pascal trap, INTEGER, InitResources)
{
    /* Initialize globals */
    TheZoneGuard guard(SysZone);

    Handle versh;
    int32 versnum;

    ROMlib_setreserr(noErr);
    str255assign(SysResName, SYSMACNAME);
    SysMap = CW(OpenRFPerm((StringPtr)SYSMACNAME, Cx(BootDrive),
                           fsCurPerm));

    if(SysMap == CWC(-1))
    {
        fprintf(stderr, "OpenRFPerm (\"%.*s\", 0x%x, fsCurPerm) failed\n",
                SYSMACNAME[0], SYSMACNAME + 1, (uint16)Cx(BootDrive));

        if(afpd_conventions_p)
            fprintf(stderr,
                    "Try omitting \"-afpd\" from the command line\n");
        else if(netatalk_conventions_p)
            fprintf(stderr,
                    "Try omitting \"-netatalk\" from the command line\n");

        exit(1);
    }

    SysMapHndl = TopMapHndl;
    ROMlib_invalar();
    SetResLoad(true);
    versh = GetResource(TICK("vers"), 1);
    versnum = extract_vers_num(versh);
    if(versnum < MINIMUM_SYSTEM_FILE_NEEDED)
        system_file_version_skew_p = true;

    ROMlib_set_appearance();

    return CW(SysMap);
}

P0(PUBLIC pascal trap, void, RsrcZoneInit) /* no op */
{
}

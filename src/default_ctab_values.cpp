/* Copyright 1994 by Abacus Research and
 * Development, Inc.  All rights reserved.
 */

#include "rsys/common.h"
#include "rsys/cquick.h"
#include "QuickDraw.h"

using namespace Executor;

#define COLORWC(cwc) CWC((unsigned short)cwc)

ColorSpec Executor::ctab_1bpp_values[] = {
    { COLORWC(0), { COLORWC(65535), COLORWC(65535), COLORWC(65535) } },
    { COLORWC(0), { COLORWC(0), COLORWC(0), COLORWC(0) } },
};

ColorSpec Executor::ctab_2bpp_values[] = {
    { COLORWC(0), { COLORWC(65535), COLORWC(65535), COLORWC(65535) } },
    { COLORWC(0), { COLORWC(44204), COLORWC(44204), COLORWC(44204) } },
    { COLORWC(0), { COLORWC(21845), COLORWC(21845), COLORWC(21845) } },
    { COLORWC(0), { COLORWC(0), COLORWC(0), COLORWC(0) } },
};

ColorSpec Executor::ctab_4bpp_values[] = {
    { COLORWC(0), { COLORWC(65535), COLORWC(65535), COLORWC(65535) } },
    { COLORWC(0), { COLORWC(64512), COLORWC(62333), COLORWC(1327) } },
    { COLORWC(0), { COLORWC(65535), COLORWC(25738), COLORWC(652) } },
    { COLORWC(0), { COLORWC(56683), COLORWC(2242), COLORWC(1698) } },
    { COLORWC(0), { COLORWC(62167), COLORWC(2134), COLORWC(34028) } },
    { COLORWC(0), { COLORWC(18147), COLORWC(0), COLORWC(42302) } },
    { COLORWC(0), { COLORWC(0), COLORWC(0), COLORWC(54272) } },
    { COLORWC(0), { COLORWC(577), COLORWC(43860), COLORWC(60159) } },
    { COLORWC(0), { COLORWC(7969), COLORWC(46995), COLORWC(5169) } },
    { COLORWC(0), { COLORWC(0), COLORWC(25775), COLORWC(4528) } },
    { COLORWC(0), { COLORWC(22016), COLORWC(11421), COLORWC(1316) } },
    { COLORWC(0), { COLORWC(37079), COLORWC(29024), COLORWC(14900) } },
    { COLORWC(0), { COLORWC(49152), COLORWC(49152), COLORWC(49152) } },
    { COLORWC(0), { COLORWC(32768), COLORWC(32768), COLORWC(32768) } },
    { COLORWC(0), { COLORWC(16384), COLORWC(16384), COLORWC(16384) } },
    { COLORWC(0), { COLORWC(0), COLORWC(0), COLORWC(0) } },
};

ColorSpec Executor::ctab_8bpp_values[] = {
    { COLORWC(0), { COLORWC(65535), COLORWC(65535), COLORWC(65535) } },
    { COLORWC(0), { COLORWC(65535), COLORWC(65535), COLORWC(52428) } },
    { COLORWC(0), { COLORWC(65535), COLORWC(65535), COLORWC(39321) } },
    { COLORWC(0), { COLORWC(65535), COLORWC(65535), COLORWC(26214) } },
    { COLORWC(0), { COLORWC(65535), COLORWC(65535), COLORWC(13107) } },
    { COLORWC(0), { COLORWC(65535), COLORWC(65535), COLORWC(0) } },
    { COLORWC(0), { COLORWC(65535), COLORWC(52428), COLORWC(65535) } },
    { COLORWC(0), { COLORWC(65535), COLORWC(52428), COLORWC(52428) } },
    { COLORWC(0), { COLORWC(65535), COLORWC(52428), COLORWC(39321) } },
    { COLORWC(0), { COLORWC(65535), COLORWC(52428), COLORWC(26214) } },
    { COLORWC(0), { COLORWC(65535), COLORWC(52428), COLORWC(13107) } },
    { COLORWC(0), { COLORWC(65535), COLORWC(52428), COLORWC(0) } },
    { COLORWC(0), { COLORWC(65535), COLORWC(39321), COLORWC(65535) } },
    { COLORWC(0), { COLORWC(65535), COLORWC(39321), COLORWC(52428) } },
    { COLORWC(0), { COLORWC(65535), COLORWC(39321), COLORWC(39321) } },
    { COLORWC(0), { COLORWC(65535), COLORWC(39321), COLORWC(26214) } },
    { COLORWC(0), { COLORWC(65535), COLORWC(39321), COLORWC(13107) } },
    { COLORWC(0), { COLORWC(65535), COLORWC(39321), COLORWC(0) } },
    { COLORWC(0), { COLORWC(65535), COLORWC(26214), COLORWC(65535) } },
    { COLORWC(0), { COLORWC(65535), COLORWC(26214), COLORWC(52428) } },
    { COLORWC(0), { COLORWC(65535), COLORWC(26214), COLORWC(39321) } },
    { COLORWC(0), { COLORWC(65535), COLORWC(26214), COLORWC(26214) } },
    { COLORWC(0), { COLORWC(65535), COLORWC(26214), COLORWC(13107) } },
    { COLORWC(0), { COLORWC(65535), COLORWC(26214), COLORWC(0) } },
    { COLORWC(0), { COLORWC(65535), COLORWC(13107), COLORWC(65535) } },
    { COLORWC(0), { COLORWC(65535), COLORWC(13107), COLORWC(52428) } },
    { COLORWC(0), { COLORWC(65535), COLORWC(13107), COLORWC(39321) } },
    { COLORWC(0), { COLORWC(65535), COLORWC(13107), COLORWC(26214) } },
    { COLORWC(0), { COLORWC(65535), COLORWC(13107), COLORWC(13107) } },
    { COLORWC(0), { COLORWC(65535), COLORWC(13107), COLORWC(0) } },
    { COLORWC(0), { COLORWC(65535), COLORWC(0), COLORWC(65535) } },
    { COLORWC(0), { COLORWC(65535), COLORWC(0), COLORWC(52428) } },
    { COLORWC(0), { COLORWC(65535), COLORWC(0), COLORWC(39321) } },
    { COLORWC(0), { COLORWC(65535), COLORWC(0), COLORWC(26214) } },
    { COLORWC(0), { COLORWC(65535), COLORWC(0), COLORWC(13107) } },
    { COLORWC(0), { COLORWC(65535), COLORWC(0), COLORWC(0) } },
    { COLORWC(0), { COLORWC(52428), COLORWC(65535), COLORWC(65535) } },
    { COLORWC(0), { COLORWC(52428), COLORWC(65535), COLORWC(52428) } },
    { COLORWC(0), { COLORWC(52428), COLORWC(65535), COLORWC(39321) } },
    { COLORWC(0), { COLORWC(52428), COLORWC(65535), COLORWC(26214) } },
    { COLORWC(0), { COLORWC(52428), COLORWC(65535), COLORWC(13107) } },
    { COLORWC(0), { COLORWC(52428), COLORWC(65535), COLORWC(0) } },
    { COLORWC(0), { COLORWC(52428), COLORWC(52428), COLORWC(65535) } },
    { COLORWC(0), { COLORWC(52428), COLORWC(52428), COLORWC(52428) } },
    { COLORWC(0), { COLORWC(52428), COLORWC(52428), COLORWC(39321) } },
    { COLORWC(0), { COLORWC(52428), COLORWC(52428), COLORWC(26214) } },
    { COLORWC(0), { COLORWC(52428), COLORWC(52428), COLORWC(13107) } },
    { COLORWC(0), { COLORWC(52428), COLORWC(52428), COLORWC(0) } },
    { COLORWC(0), { COLORWC(52428), COLORWC(39321), COLORWC(65535) } },
    { COLORWC(0), { COLORWC(52428), COLORWC(39321), COLORWC(52428) } },
    { COLORWC(0), { COLORWC(52428), COLORWC(39321), COLORWC(39321) } },
    { COLORWC(0), { COLORWC(52428), COLORWC(39321), COLORWC(26214) } },
    { COLORWC(0), { COLORWC(52428), COLORWC(39321), COLORWC(13107) } },
    { COLORWC(0), { COLORWC(52428), COLORWC(39321), COLORWC(0) } },
    { COLORWC(0), { COLORWC(52428), COLORWC(26214), COLORWC(65535) } },
    { COLORWC(0), { COLORWC(52428), COLORWC(26214), COLORWC(52428) } },
    { COLORWC(0), { COLORWC(52428), COLORWC(26214), COLORWC(39321) } },
    { COLORWC(0), { COLORWC(52428), COLORWC(26214), COLORWC(26214) } },
    { COLORWC(0), { COLORWC(52428), COLORWC(26214), COLORWC(13107) } },
    { COLORWC(0), { COLORWC(52428), COLORWC(26214), COLORWC(0) } },
    { COLORWC(0), { COLORWC(52428), COLORWC(13107), COLORWC(65535) } },
    { COLORWC(0), { COLORWC(52428), COLORWC(13107), COLORWC(52428) } },
    { COLORWC(0), { COLORWC(52428), COLORWC(13107), COLORWC(39321) } },
    { COLORWC(0), { COLORWC(52428), COLORWC(13107), COLORWC(26214) } },
    { COLORWC(0), { COLORWC(52428), COLORWC(13107), COLORWC(13107) } },
    { COLORWC(0), { COLORWC(52428), COLORWC(13107), COLORWC(0) } },
    { COLORWC(0), { COLORWC(52428), COLORWC(0), COLORWC(65535) } },
    { COLORWC(0), { COLORWC(52428), COLORWC(0), COLORWC(52428) } },
    { COLORWC(0), { COLORWC(52428), COLORWC(0), COLORWC(39321) } },
    { COLORWC(0), { COLORWC(52428), COLORWC(0), COLORWC(26214) } },
    { COLORWC(0), { COLORWC(52428), COLORWC(0), COLORWC(13107) } },
    { COLORWC(0), { COLORWC(52428), COLORWC(0), COLORWC(0) } },
    { COLORWC(0), { COLORWC(39321), COLORWC(65535), COLORWC(65535) } },
    { COLORWC(0), { COLORWC(39321), COLORWC(65535), COLORWC(52428) } },
    { COLORWC(0), { COLORWC(39321), COLORWC(65535), COLORWC(39321) } },
    { COLORWC(0), { COLORWC(39321), COLORWC(65535), COLORWC(26214) } },
    { COLORWC(0), { COLORWC(39321), COLORWC(65535), COLORWC(13107) } },
    { COLORWC(0), { COLORWC(39321), COLORWC(65535), COLORWC(0) } },
    { COLORWC(0), { COLORWC(39321), COLORWC(52428), COLORWC(65535) } },
    { COLORWC(0), { COLORWC(39321), COLORWC(52428), COLORWC(52428) } },
    { COLORWC(0), { COLORWC(39321), COLORWC(52428), COLORWC(39321) } },
    { COLORWC(0), { COLORWC(39321), COLORWC(52428), COLORWC(26214) } },
    { COLORWC(0), { COLORWC(39321), COLORWC(52428), COLORWC(13107) } },
    { COLORWC(0), { COLORWC(39321), COLORWC(52428), COLORWC(0) } },
    { COLORWC(0), { COLORWC(39321), COLORWC(39321), COLORWC(65535) } },
    { COLORWC(0), { COLORWC(39321), COLORWC(39321), COLORWC(52428) } },
    { COLORWC(0), { COLORWC(39321), COLORWC(39321), COLORWC(39321) } },
    { COLORWC(0), { COLORWC(39321), COLORWC(39321), COLORWC(26214) } },
    { COLORWC(0), { COLORWC(39321), COLORWC(39321), COLORWC(13107) } },
    { COLORWC(0), { COLORWC(39321), COLORWC(39321), COLORWC(0) } },
    { COLORWC(0), { COLORWC(39321), COLORWC(26214), COLORWC(65535) } },
    { COLORWC(0), { COLORWC(39321), COLORWC(26214), COLORWC(52428) } },
    { COLORWC(0), { COLORWC(39321), COLORWC(26214), COLORWC(39321) } },
    { COLORWC(0), { COLORWC(39321), COLORWC(26214), COLORWC(26214) } },
    { COLORWC(0), { COLORWC(39321), COLORWC(26214), COLORWC(13107) } },
    { COLORWC(0), { COLORWC(39321), COLORWC(26214), COLORWC(0) } },
    { COLORWC(0), { COLORWC(39321), COLORWC(13107), COLORWC(65535) } },
    { COLORWC(0), { COLORWC(39321), COLORWC(13107), COLORWC(52428) } },
    { COLORWC(0), { COLORWC(39321), COLORWC(13107), COLORWC(39321) } },
    { COLORWC(0), { COLORWC(39321), COLORWC(13107), COLORWC(26214) } },
    { COLORWC(0), { COLORWC(39321), COLORWC(13107), COLORWC(13107) } },
    { COLORWC(0), { COLORWC(39321), COLORWC(13107), COLORWC(0) } },
    { COLORWC(0), { COLORWC(39321), COLORWC(0), COLORWC(65535) } },
    { COLORWC(0), { COLORWC(39321), COLORWC(0), COLORWC(52428) } },
    { COLORWC(0), { COLORWC(39321), COLORWC(0), COLORWC(39321) } },
    { COLORWC(0), { COLORWC(39321), COLORWC(0), COLORWC(26214) } },
    { COLORWC(0), { COLORWC(39321), COLORWC(0), COLORWC(13107) } },
    { COLORWC(0), { COLORWC(39321), COLORWC(0), COLORWC(0) } },
    { COLORWC(0), { COLORWC(26214), COLORWC(65535), COLORWC(65535) } },
    { COLORWC(0), { COLORWC(26214), COLORWC(65535), COLORWC(52428) } },
    { COLORWC(0), { COLORWC(26214), COLORWC(65535), COLORWC(39321) } },
    { COLORWC(0), { COLORWC(26214), COLORWC(65535), COLORWC(26214) } },
    { COLORWC(0), { COLORWC(26214), COLORWC(65535), COLORWC(13107) } },
    { COLORWC(0), { COLORWC(26214), COLORWC(65535), COLORWC(0) } },
    { COLORWC(0), { COLORWC(26214), COLORWC(52428), COLORWC(65535) } },
    { COLORWC(0), { COLORWC(26214), COLORWC(52428), COLORWC(52428) } },
    { COLORWC(0), { COLORWC(26214), COLORWC(52428), COLORWC(39321) } },
    { COLORWC(0), { COLORWC(26214), COLORWC(52428), COLORWC(26214) } },
    { COLORWC(0), { COLORWC(26214), COLORWC(52428), COLORWC(13107) } },
    { COLORWC(0), { COLORWC(26214), COLORWC(52428), COLORWC(0) } },
    { COLORWC(0), { COLORWC(26214), COLORWC(39321), COLORWC(65535) } },
    { COLORWC(0), { COLORWC(26214), COLORWC(39321), COLORWC(52428) } },
    { COLORWC(0), { COLORWC(26214), COLORWC(39321), COLORWC(39321) } },
    { COLORWC(0), { COLORWC(26214), COLORWC(39321), COLORWC(26214) } },
    { COLORWC(0), { COLORWC(26214), COLORWC(39321), COLORWC(13107) } },
    { COLORWC(0), { COLORWC(26214), COLORWC(39321), COLORWC(0) } },
    { COLORWC(0), { COLORWC(26214), COLORWC(26214), COLORWC(65535) } },
    { COLORWC(0), { COLORWC(26214), COLORWC(26214), COLORWC(52428) } },
    { COLORWC(0), { COLORWC(26214), COLORWC(26214), COLORWC(39321) } },
    { COLORWC(0), { COLORWC(26214), COLORWC(26214), COLORWC(26214) } },
    { COLORWC(0), { COLORWC(26214), COLORWC(26214), COLORWC(13107) } },
    { COLORWC(0), { COLORWC(26214), COLORWC(26214), COLORWC(0) } },
    { COLORWC(0), { COLORWC(26214), COLORWC(13107), COLORWC(65535) } },
    { COLORWC(0), { COLORWC(26214), COLORWC(13107), COLORWC(52428) } },
    { COLORWC(0), { COLORWC(26214), COLORWC(13107), COLORWC(39321) } },
    { COLORWC(0), { COLORWC(26214), COLORWC(13107), COLORWC(26214) } },
    { COLORWC(0), { COLORWC(26214), COLORWC(13107), COLORWC(13107) } },
    { COLORWC(0), { COLORWC(26214), COLORWC(13107), COLORWC(0) } },
    { COLORWC(0), { COLORWC(26214), COLORWC(0), COLORWC(65535) } },
    { COLORWC(0), { COLORWC(26214), COLORWC(0), COLORWC(52428) } },
    { COLORWC(0), { COLORWC(26214), COLORWC(0), COLORWC(39321) } },
    { COLORWC(0), { COLORWC(26214), COLORWC(0), COLORWC(26214) } },
    { COLORWC(0), { COLORWC(26214), COLORWC(0), COLORWC(13107) } },
    { COLORWC(0), { COLORWC(26214), COLORWC(0), COLORWC(0) } },
    { COLORWC(0), { COLORWC(13107), COLORWC(65535), COLORWC(65535) } },
    { COLORWC(0), { COLORWC(13107), COLORWC(65535), COLORWC(52428) } },
    { COLORWC(0), { COLORWC(13107), COLORWC(65535), COLORWC(39321) } },
    { COLORWC(0), { COLORWC(13107), COLORWC(65535), COLORWC(26214) } },
    { COLORWC(0), { COLORWC(13107), COLORWC(65535), COLORWC(13107) } },
    { COLORWC(0), { COLORWC(13107), COLORWC(65535), COLORWC(0) } },
    { COLORWC(0), { COLORWC(13107), COLORWC(52428), COLORWC(65535) } },
    { COLORWC(0), { COLORWC(13107), COLORWC(52428), COLORWC(52428) } },
    { COLORWC(0), { COLORWC(13107), COLORWC(52428), COLORWC(39321) } },
    { COLORWC(0), { COLORWC(13107), COLORWC(52428), COLORWC(26214) } },
    { COLORWC(0), { COLORWC(13107), COLORWC(52428), COLORWC(13107) } },
    { COLORWC(0), { COLORWC(13107), COLORWC(52428), COLORWC(0) } },
    { COLORWC(0), { COLORWC(13107), COLORWC(39321), COLORWC(65535) } },
    { COLORWC(0), { COLORWC(13107), COLORWC(39321), COLORWC(52428) } },
    { COLORWC(0), { COLORWC(13107), COLORWC(39321), COLORWC(39321) } },
    { COLORWC(0), { COLORWC(13107), COLORWC(39321), COLORWC(26214) } },
    { COLORWC(0), { COLORWC(13107), COLORWC(39321), COLORWC(13107) } },
    { COLORWC(0), { COLORWC(13107), COLORWC(39321), COLORWC(0) } },
    { COLORWC(0), { COLORWC(13107), COLORWC(26214), COLORWC(65535) } },
    { COLORWC(0), { COLORWC(13107), COLORWC(26214), COLORWC(52428) } },
    { COLORWC(0), { COLORWC(13107), COLORWC(26214), COLORWC(39321) } },
    { COLORWC(0), { COLORWC(13107), COLORWC(26214), COLORWC(26214) } },
    { COLORWC(0), { COLORWC(13107), COLORWC(26214), COLORWC(13107) } },
    { COLORWC(0), { COLORWC(13107), COLORWC(26214), COLORWC(0) } },
    { COLORWC(0), { COLORWC(13107), COLORWC(13107), COLORWC(65535) } },
    { COLORWC(0), { COLORWC(13107), COLORWC(13107), COLORWC(52428) } },
    { COLORWC(0), { COLORWC(13107), COLORWC(13107), COLORWC(39321) } },
    { COLORWC(0), { COLORWC(13107), COLORWC(13107), COLORWC(26214) } },
    { COLORWC(0), { COLORWC(13107), COLORWC(13107), COLORWC(13107) } },
    { COLORWC(0), { COLORWC(13107), COLORWC(13107), COLORWC(0) } },
    { COLORWC(0), { COLORWC(13107), COLORWC(0), COLORWC(65535) } },
    { COLORWC(0), { COLORWC(13107), COLORWC(0), COLORWC(52428) } },
    { COLORWC(0), { COLORWC(13107), COLORWC(0), COLORWC(39321) } },
    { COLORWC(0), { COLORWC(13107), COLORWC(0), COLORWC(26214) } },
    { COLORWC(0), { COLORWC(13107), COLORWC(0), COLORWC(13107) } },
    { COLORWC(0), { COLORWC(13107), COLORWC(0), COLORWC(0) } },
    { COLORWC(0), { COLORWC(0), COLORWC(65535), COLORWC(65535) } },
    { COLORWC(0), { COLORWC(0), COLORWC(65535), COLORWC(52428) } },
    { COLORWC(0), { COLORWC(0), COLORWC(65535), COLORWC(39321) } },
    { COLORWC(0), { COLORWC(0), COLORWC(65535), COLORWC(26214) } },
    { COLORWC(0), { COLORWC(0), COLORWC(65535), COLORWC(13107) } },
    { COLORWC(0), { COLORWC(0), COLORWC(65535), COLORWC(0) } },
    { COLORWC(0), { COLORWC(0), COLORWC(52428), COLORWC(65535) } },
    { COLORWC(0), { COLORWC(0), COLORWC(52428), COLORWC(52428) } },
    { COLORWC(0), { COLORWC(0), COLORWC(52428), COLORWC(39321) } },
    { COLORWC(0), { COLORWC(0), COLORWC(52428), COLORWC(26214) } },
    { COLORWC(0), { COLORWC(0), COLORWC(52428), COLORWC(13107) } },
    { COLORWC(0), { COLORWC(0), COLORWC(52428), COLORWC(0) } },
    { COLORWC(0), { COLORWC(0), COLORWC(39321), COLORWC(65535) } },
    { COLORWC(0), { COLORWC(0), COLORWC(39321), COLORWC(52428) } },
    { COLORWC(0), { COLORWC(0), COLORWC(39321), COLORWC(39321) } },
    { COLORWC(0), { COLORWC(0), COLORWC(39321), COLORWC(26214) } },
    { COLORWC(0), { COLORWC(0), COLORWC(39321), COLORWC(13107) } },
    { COLORWC(0), { COLORWC(0), COLORWC(39321), COLORWC(0) } },
    { COLORWC(0), { COLORWC(0), COLORWC(26214), COLORWC(65535) } },
    { COLORWC(0), { COLORWC(0), COLORWC(26214), COLORWC(52428) } },
    { COLORWC(0), { COLORWC(0), COLORWC(26214), COLORWC(39321) } },
    { COLORWC(0), { COLORWC(0), COLORWC(26214), COLORWC(26214) } },
    { COLORWC(0), { COLORWC(0), COLORWC(26214), COLORWC(13107) } },
    { COLORWC(0), { COLORWC(0), COLORWC(26214), COLORWC(0) } },
    { COLORWC(0), { COLORWC(0), COLORWC(13107), COLORWC(65535) } },
    { COLORWC(0), { COLORWC(0), COLORWC(13107), COLORWC(52428) } },
    { COLORWC(0), { COLORWC(0), COLORWC(13107), COLORWC(39321) } },
    { COLORWC(0), { COLORWC(0), COLORWC(13107), COLORWC(26214) } },
    { COLORWC(0), { COLORWC(0), COLORWC(13107), COLORWC(13107) } },
    { COLORWC(0), { COLORWC(0), COLORWC(13107), COLORWC(0) } },
    { COLORWC(0), { COLORWC(0), COLORWC(0), COLORWC(65535) } },
    { COLORWC(0), { COLORWC(0), COLORWC(0), COLORWC(52428) } },
    { COLORWC(0), { COLORWC(0), COLORWC(0), COLORWC(39321) } },
    { COLORWC(0), { COLORWC(0), COLORWC(0), COLORWC(26214) } },
    { COLORWC(0), { COLORWC(0), COLORWC(0), COLORWC(13107) } },
    { COLORWC(0), { COLORWC(61166), COLORWC(0), COLORWC(0) } },
    { COLORWC(0), { COLORWC(56797), COLORWC(0), COLORWC(0) } },
    { COLORWC(0), { COLORWC(48059), COLORWC(0), COLORWC(0) } },
    { COLORWC(0), { COLORWC(43690), COLORWC(0), COLORWC(0) } },
    { COLORWC(0), { COLORWC(34952), COLORWC(0), COLORWC(0) } },
    { COLORWC(0), { COLORWC(30583), COLORWC(0), COLORWC(0) } },
    { COLORWC(0), { COLORWC(21845), COLORWC(0), COLORWC(0) } },
    { COLORWC(0), { COLORWC(17476), COLORWC(0), COLORWC(0) } },
    { COLORWC(0), { COLORWC(8738), COLORWC(0), COLORWC(0) } },
    { COLORWC(0), { COLORWC(4369), COLORWC(0), COLORWC(0) } },
    { COLORWC(0), { COLORWC(0), COLORWC(61166), COLORWC(0) } },
    { COLORWC(0), { COLORWC(0), COLORWC(56797), COLORWC(0) } },
    { COLORWC(0), { COLORWC(0), COLORWC(48059), COLORWC(0) } },
    { COLORWC(0), { COLORWC(0), COLORWC(43690), COLORWC(0) } },
    { COLORWC(0), { COLORWC(0), COLORWC(34952), COLORWC(0) } },
    { COLORWC(0), { COLORWC(0), COLORWC(30583), COLORWC(0) } },
    { COLORWC(0), { COLORWC(0), COLORWC(21845), COLORWC(0) } },
    { COLORWC(0), { COLORWC(0), COLORWC(17476), COLORWC(0) } },
    { COLORWC(0), { COLORWC(0), COLORWC(8738), COLORWC(0) } },
    { COLORWC(0), { COLORWC(0), COLORWC(4369), COLORWC(0) } },
    { COLORWC(0), { COLORWC(0), COLORWC(0), COLORWC(61166) } },
    { COLORWC(0), { COLORWC(0), COLORWC(0), COLORWC(56797) } },
    { COLORWC(0), { COLORWC(0), COLORWC(0), COLORWC(48059) } },
    { COLORWC(0), { COLORWC(0), COLORWC(0), COLORWC(43690) } },
    { COLORWC(0), { COLORWC(0), COLORWC(0), COLORWC(34952) } },
    { COLORWC(0), { COLORWC(0), COLORWC(0), COLORWC(30583) } },
    { COLORWC(0), { COLORWC(0), COLORWC(0), COLORWC(21845) } },
    { COLORWC(0), { COLORWC(0), COLORWC(0), COLORWC(17476) } },
    { COLORWC(0), { COLORWC(0), COLORWC(0), COLORWC(8738) } },
    { COLORWC(0), { COLORWC(0), COLORWC(0), COLORWC(4369) } },
    { COLORWC(0), { COLORWC(61166), COLORWC(61166), COLORWC(61166) } },
    { COLORWC(0), { COLORWC(56797), COLORWC(56797), COLORWC(56797) } },
    { COLORWC(0), { COLORWC(48059), COLORWC(48059), COLORWC(48059) } },
    { COLORWC(0), { COLORWC(43690), COLORWC(43690), COLORWC(43690) } },
    { COLORWC(0), { COLORWC(34952), COLORWC(34952), COLORWC(34952) } },
    { COLORWC(0), { COLORWC(30583), COLORWC(30583), COLORWC(30583) } },
    { COLORWC(0), { COLORWC(21845), COLORWC(21845), COLORWC(21845) } },
    { COLORWC(0), { COLORWC(17476), COLORWC(17476), COLORWC(17476) } },
    { COLORWC(0), { COLORWC(8738), COLORWC(8738), COLORWC(8738) } },
    { COLORWC(0), { COLORWC(4369), COLORWC(4369), COLORWC(4369) } },
    { COLORWC(0), { COLORWC(0), COLORWC(0), COLORWC(0) } },
};

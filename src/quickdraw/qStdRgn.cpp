/* Copyright 1986 - 1996 by Abacus Research and
 * Development, Inc.  All rights reserved.
 */

/* Forward declarations in QuickDraw.h (DO NOT DELETE THIS LINE) */

#include <base/common.h>
#include <QuickDraw.h>
#include <CQuickDraw.h>
#include <MemoryMgr.h>
#include <ToolboxUtil.h>

#include <quickdraw/quick.h>
#include <quickdraw/cquick.h>
#include <quickdraw/picture.h>
#include <prefs/prefs.h>
#include <mman/mman.h>
#include <commandline/flags.h>
#include <vdriver/vdriver.h>
#include <mman/tempalloc.h>

#include <quickdraw/xdblt.h>
#include <quickdraw/srcblt.h>
#include <vdriver/dirtyrect.h>

using namespace Executor;

void Executor::ROMlib_blt_rgn_update_dirty_rect(RgnHandle rh,
                                                int16_t mode, bool tile_src_p, int bpp,
                                                const PixMap *src_pm, PixMap *dst_pm,
                                                const Rect *src_rect, const Rect *dst_rect,
                                                uint32_t fg_color, uint32_t bk_color)
{
    bool screen_dst_p;
    Rect bbox;
    TEMP_ALLOC_DECL(temp_alloc_space);

    if(mode == mask)
        mode = patCopy;
    else if(mode >= hilite
            || ((mode == patXor
                 || mode == srcXor)
                && !(LM(HiliteMode) & 128)))
        mode = hilite;

    if(mode == grayishTextOr)
    {
        if(bpp == 1)
        {
            PixMap *new_src_pm = (PixMap *)alloca(sizeof *new_src_pm);
            int baseaddr_size, height, row_bytes;
            uint8_t *new_bits, *s, *d;
            int y;

            /* Make a copy of the source bitmap, so we can dither it. */
            *new_src_pm = *src_pm;
            height = RECT_HEIGHT(&new_src_pm->bounds);
            row_bytes = BITMAP_ROWBYTES(new_src_pm);
            baseaddr_size = row_bytes * height;
            TEMP_ALLOC_ALLOCATE(new_bits, temp_alloc_space, baseaddr_size);
            new_src_pm->baseAddr = (Ptr)new_bits;

            s = (uint8_t *)src_pm->baseAddr;
            d = new_bits;
            for(y = 0; y < height; y++)
            {
                int x, mask;
                mask = (y & 1) ? 0xAA : 0x55;
                for(x = row_bytes - 1; x >= 0; x--)
                    d[x] = s[x] & mask;
                s += row_bytes;
                d += row_bytes;
            }

            src_pm = new_src_pm;
        }
        else
        {
            RGBColor fg_rgb, bk_rgb;
            const rgb_spec_t *dst_rgb_spec;

            dst_rgb_spec = pixmap_rgb_spec(dst_pm);
            ROMlib_fg_bk(nullptr, nullptr, &fg_rgb, &bk_rgb, dst_rgb_spec,
                         active_screen_addr_p(dst_pm), bpp <= 8);

            AVERAGE_COLOR(&fg_rgb, &bk_rgb, 0x8000, &fg_rgb);

            if(dst_rgb_spec)
                fg_color = ((*dst_rgb_spec->rgbcolor_to_pixel)(dst_rgb_spec, &fg_rgb, true));
            else
                fg_color = Color2Index(&fg_rgb);
        }

        mode = srcOr;
    }
    else if((IMV_XFER_MODE_P(mode)
             || mode == hilite)
            && bpp > 1)
    {
        PixMap *new_src_pm;
        int row_bytes;
        void *new_bits;

        Rect convert_src_rect;
        int top, left;
        int bbox_width, bbox_height;

        new_src_pm = (PixMap *)alloca(sizeof *new_src_pm);

        bbox = RGN_BBOX(rh);
        bbox_width = RECT_WIDTH(&bbox);
        bbox_height = RECT_HEIGHT(&bbox);

        row_bytes = ((bbox_width * bpp + 31) / 32) * 4;
        new_src_pm->rowBytes = row_bytes;
        TEMP_ALLOC_ALLOCATE(new_bits, temp_alloc_space,
                            row_bytes * bbox_height);
        new_src_pm->baseAddr = (Ptr)new_bits;

        pixmap_set_pixel_fields(new_src_pm, bpp);

        new_src_pm->pmTable = ROMlib_dont_depthconv_ctab;

        top = (src_rect->top
               + (bbox.top - dst_rect->top));
        left = (src_rect->left
                + (bbox.left - dst_rect->left));

        convert_src_rect.top = top;
        convert_src_rect.left = left;
        convert_src_rect.bottom = top + bbox_height;
        convert_src_rect.right = left + bbox_width;

        if(mode == transparent
           || mode == hilite)
        {
            convert_transparent(src_pm, dst_pm, new_src_pm,
                                &convert_src_rect, &bbox,
                                mode,
                                tile_src_p,
                                /* #warning "have mat figure out what the tiling offsets should be" */
                                0, 0);
        }
        else
        {
            CTabHandle ctab;
            ITabHandle itab;

            ctab = PIXMAP_TABLE(GD_PMAP(LM(TheGDevice)));
            itab = GD_ITABLE(LM(TheGDevice));

            convert_pixmap_with_IMV_mode(src_pm, dst_pm, new_src_pm,
                                         ctab, ctab, itab,
                                         &convert_src_rect, &bbox,
                                         mode,
                                         &CPORT_OP_COLOR(theCPort),
                                         tile_src_p,
                                         /* #warning "have mat figure out what the tiling offsets should be" */
                                         0, 0);
        }

        src_pm = new_src_pm;
        dst_rect = src_rect = &bbox;
        mode = srcCopy;
        pixmap_black_white(src_pm, &fg_color, &bk_color);
        tile_src_p = false;
    }

    /* #warning "don't ignore dither mode at some point in the future" */

    mode &= 0x3F;
    if(mode >= 0x20 && mode <= 0x2F)
    {
        static const INTEGER funny_mode_lookup[16] = {
            srcCopy, srcBic, srcXor, srcOr, srcOr, srcBic, srcXor, srcOr,
            patCopy, patBic, patXor, patOr, patOr, patBic, patXor, patOr,
        };
        mode = funny_mode_lookup[mode - 0x20];
    }
    else if(mode == hilite)
        mode = patXor;

    if(tile_src_p)
        mode |= 0x8;
    else
        mode &= 0x37;

    screen_dst_p = active_screen_addr_p(dst_pm);

    srcblt_rgn(rh, mode, ROMlib_log2[bpp],
               (blt_bitmap_t *)src_pm,
               (blt_bitmap_t *)dst_pm,
               (GUEST<Point> *)src_rect, (GUEST<Point> *)dst_rect,
               fg_color, bk_color);

    /* if drawing to the screen, update the dirty rect (or copy the
     image straight to screen) */
    if(screen_dst_p)
    {
        const Rect *r = &RGN_BBOX(rh);
        int dst_top = dst_pm->bounds.top;
        int dst_left = dst_pm->bounds.left;

        dirty_rect_accrue(r->top - dst_top, r->left - dst_left,
                          r->bottom - dst_top, r->right - dst_left);
    }

    TEMP_ALLOC_FREE(temp_alloc_space);
}

const int Executor::ROMlib_log2[] = {
    -1,
    0,
    1,
    -1, 2,
    -1, -1, -1, 3,
    -1, -1, -1, -1, -1, -1, -1, 4,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 5,
};

const uint32_t Executor::ROMlib_pixel_tile_scale[6] = {
    0xFFFFFFFF,
    0x55555555,
    0x11111111,
    0x01010101,
    0x00010001,
    0x00000001,
};

const uint32_t Executor::ROMlib_pixel_size_mask[6] = {
    0x00000001,
    0x00000003,
    0x0000000F,
    0x000000FF,
    0x0000FFFF,
    0xFFFFFFFF,
};

#define BLT_PAT_SIMPLE(rh, mode, pixpat_accessor, pattern_accessor)                 \
    do                                                                              \
    {                                                                               \
        GrafPtr the_port = qdGlobals().thePort;                                                 \
        if(CGrafPort_p(the_port))                                                   \
            blt_pixpat_to_pixmap_simple_mode(rh, mode,                              \
                                             pixpat_accessor((CGrafPtr)the_port),   \
                                             CPORT_PIXMAP((CGrafPtr)the_port));     \
        else                                                                        \
            blt_pattern_to_bitmap_simple_mode(rh, mode, pattern_accessor(the_port), \
                                              &the_port->portBits);                 \
    } while(0)

#define BLT_PAT_FANCY(rh, mode, pixpat_accessor, pattern_accessor)            \
    do                                                                        \
    {                                                                         \
        GrafPtr the_port = qdGlobals().thePort;                               \
        if(CGrafPort_p(the_port))                                             \
        {                                                                     \
            PixMapHandle cport_pmap = CPORT_PIXMAP((CGrafPtr)the_port);       \
            HLockGuard guard(cport_pmap);                                     \
            blt_fancy_pat_mode_to_pixmap(rh, mode,                            \
                                         pixpat_accessor((CGrafPtr)           \
                                                             the_port),       \
                                         nullptr, *cport_pmap);               \
        }                                                                     \
        else if(active_screen_addr_p(&the_port->portBits))                    \
        {                                                                     \
            PixMap copy_of_screen;                                            \
            copy_of_screen = **GD_PMAP(LM(TheGDevice));                       \
            copy_of_screen.bounds = the_port->portBits.bounds;                \
            blt_fancy_pat_mode_to_pixmap(rh, mode, nullptr,                   \
                                         &pattern_accessor(the_port),         \
                                         &copy_of_screen);                    \
        }                                                                     \
        else                                                                  \
        {                                                                     \
            /* We shouldn't get here; we don't allow fancy blits to old-style \
             * grafports not on the screen.                                   \
             */                                                               \
            gui_abort();                                                      \
        }                                                                     \
    } while(0)

static void
blt_pattern_to_bitmap_simple_mode(RgnHandle rh, INTEGER mode,
                                  Pattern src, BitMap *dst)
{
    PixMapHandle main_gd_pmap;
    GDHandle main_gd;
    uint32_t bk_pixel, fg_pixel;
    int bpp, dst_top, dst_left;
    bool screen_dst_p;
    PixMap dst_pixmap;
    GrafPtr the_port;

    main_gd = LM(MainDevice);
    main_gd_pmap = GD_PMAP(main_gd);
    screen_dst_p = active_screen_addr_p(dst);
    bpp = PIXMAP_PIXEL_SIZE(main_gd_pmap);

    the_port = qdGlobals().thePort;

    /* ### is this if necessary? */
    if(screen_dst_p)
    {
        dst_pixmap = **main_gd_pmap;
        ROMlib_fg_bk(&fg_pixel, &bk_pixel, nullptr, nullptr,
                     pixmap_rgb_spec(*main_gd_pmap),
                     true, false);
    }
    else
    {
        dst_pixmap.pixelType = Indirect;
        dst_pixmap.pixelSize = 1;
        dst_pixmap.baseAddr = dst->baseAddr;
        dst_pixmap.rowBytes = dst->rowBytes | PIXMAP_DEFAULT_ROW_BYTES;
        dst_pixmap.pmTable = ROMlib_bw_ctab;

        ROMlib_fg_bk(&fg_pixel, &bk_pixel, nullptr, nullptr, nullptr, false, false);
    }

    dst_pixmap.bounds = dst->bounds;
    dst_top = dst_pixmap.bounds.top;
    dst_left = dst_pixmap.bounds.left;

    /* Actually do the blt. */
    xdblt_pattern(rh, mode, -dst_left, -dst_top, src,
                  &dst_pixmap, fg_pixel, bk_pixel);

    /* Update the real screen as appropriate. */
    if(screen_dst_p)
    {
        const Rect *r = &RGN_BBOX(rh);
        dirty_rect_accrue(r->top - dst_top, r->left - dst_left,
                          r->bottom - dst_top, r->right - dst_left);
    }
}

static void
blt_pixpat_to_pixmap_simple_mode(RgnHandle rh, INTEGER mode,
                                 PixPatHandle srch, PixMapHandle dsth)
{
    bool screen_dst_p;
    int dst_top, dst_left;

    {
        HLockGuard guard1(srch), guard2(dsth);
        PixPat *src = *srch;
        PixMap *dst = *dsth;
        GrafPtr the_port = qdGlobals().thePort;

        screen_dst_p = active_screen_addr_p(dst);

        dst_top = dst->bounds.top;
        dst_left = dst->bounds.left;

        if(src->patType == pixpat_old_style_pattern)
        {
            const rgb_spec_t *dst_rgb_spec;
            uint32_t fg_color;
            uint32_t bk_color;

            dst_rgb_spec = pixmap_rgb_spec(dst);
            canonical_from_bogo_color(PORT_FG_COLOR(the_port), dst_rgb_spec,
                                      &fg_color, nullptr);
            canonical_from_bogo_color(PORT_BK_COLOR(the_port), dst_rgb_spec,
                                      &bk_color, nullptr);

            xdblt_pattern(rh, mode, -dst_left, -dst_top,
                          src->pat1Data, dst, fg_color,
                          bk_color);
        }
        else
        {
            bool xdata_valid_p;
            bool handle_size_wrong_p;
            xdata_handle_t xh;

            xh = (xdata_handle_t)src->patXData;
            if(!xh)
            {
                warning_unexpected("xdata handle """);
                xh = (xdata_handle_t)NewHandle(sizeof(xdata_t));
                (*xh)->raw_pat_bits_mem = nullptr;
                src->patXData = (Handle)xh;
                xdata_valid_p = false;
                handle_size_wrong_p = false;
            }
            else
            {
                handle_size_wrong_p = (GetHandleSize((Handle)xh)
                                       != sizeof(xdata_t));
                xdata_valid_p = (!handle_size_wrong_p
                                 && ((*xh)->magic_cookie
                                     == XDATA_MAGIC_COOKIE));
            }

            if(src->patXValid == -1
               || !xdata_valid_p)
            {
                if(xdata_valid_p)
                {
                    Ptr raw = (*xh)->raw_pat_bits_mem;
                    if(raw)
                    {
                        DisposePtr(raw);
                        (*xh)->raw_pat_bits_mem = nullptr;
                    }
                }
                else if(handle_size_wrong_p)
                {
                    SetHandleSize((Handle)xh, sizeof(xdata_t));
                    warning_unexpected("invalid xdata size; appl juked xdata maybe");
                }

                xdata_for_pixpat_with_space(src, dst, xh);

                src->patXValid = 0;
            }
            else
            {
                update_xdata_if_needed(xh, src, dst);
            }

            HLockGuard guard(xh);
            xdata_t *x = *xh;
            (*x->blt_func)(rh, mode, -dst_left,
                           -dst_top, x, dst);
        }
    }

    /* Update the real screen as appropriate. */
    if(screen_dst_p)
    {
        const Rect *r = &RGN_BBOX(rh);
        dirty_rect_accrue(r->top - dst_top, r->left - dst_left,
                          r->bottom - dst_top, r->right - dst_left);
    }
}

static void
blt_fancy_pat_mode_to_pixmap(RgnHandle rh, int mode,
                             PixPatHandle pixpat_handle,
                             Pattern *pattern, /* valid iff !pixpat_handle */
                             PixMap *pixmap)
{
    PixMap converted_pm;
    PixMap pattern_pm;
    Rect bbox;
    int row_bytes;
    void *new_bits;
    xdata_handle_t xh;
    xdata_t *x;
    bool apply_fg_bk_p;
    int bpp, log2_bpp;
    TEMP_ALLOC_DECL(temp_alloc_space);

    /* Set up xdata for the thing being blitted. */
    bpp = pixmap->pixelSize;
    log2_bpp = ROMlib_log2[bpp];

    if(!pixpat_handle)
    {
        xh = xdata_for_pattern(*pattern, pixmap);
        apply_fg_bk_p = true;
    }
    else
    {
        PixPat *pixpat = *pixpat_handle;
        if(pixpat->patType == pixpat_type_orig)
        {
            xh = xdata_for_pattern(pixpat->pat1Data, pixmap);
            apply_fg_bk_p = true;
        }
        else /* newer-style pixpat */
        {
            xh = xdata_for_pixpat(pixpat, pixmap);
            apply_fg_bk_p = false;
        }
    }

    /* Set up the pattern bitmap. */
    HLock((Handle)xh);
    x = *xh;
    pattern_pm.bounds.top = 0;
    pattern_pm.bounds.left = 0;
    pattern_pm.bounds.bottom = x->height_minus_1 + 1;
    pattern_pm.bounds.right = (x->row_bytes << (5 - x->log2_bpp)) >> 2;
    pattern_pm.rowBytes = x->row_bytes;
    if(x->pat_bits)
        pattern_pm.baseAddr = (Ptr)x->pat_bits;
    else
        pattern_pm.baseAddr = (Ptr)&x->pat_value;

    pixmap_set_pixel_fields(&pattern_pm, 1 << x->log2_bpp);
    pattern_pm.pmTable = ROMlib_dont_depthconv_ctab;

    /* When dealing with an old-style pattern, we need to apply fg/bk colors. */
    if(apply_fg_bk_p)
    {
        uint32_t *p, *end, fg_pixel, bk_pixel, tiled_fg_pixel, tiled_bk_pixel;

        ROMlib_fg_bk(&fg_pixel, &bk_pixel, nullptr, nullptr,
                     pixmap_rgb_spec(pixmap),
                     active_screen_addr_p(pixmap), false);

        /* Tile the pixel values out to 32 bpp. */
        tiled_fg_pixel = ((fg_pixel & ROMlib_pixel_size_mask[log2_bpp])
                          * ROMlib_pixel_tile_scale[log2_bpp]);
        tiled_bk_pixel = ((bk_pixel & ROMlib_pixel_size_mask[log2_bpp])
                          * ROMlib_pixel_tile_scale[log2_bpp]);

        /* Note that we don't care if we clobber the bits of this temp xdata. */
        p = (uint32_t *)pattern_pm.baseAddr;
        end = (uint32_t *)((char *)p + x->byte_size);

        if(bpp >= 16)
        {
            for(; p != end; p++)
            {
                uint32_t v = ~*p;
                /* #warning "not sure how this interacts w/RGB" */
                *p = (v & tiled_fg_pixel) | ((~v) & tiled_bk_pixel);
            }
        }
        else
        {
            for(; p != end; p++)
            {
                uint32_t v = *p;
                /* #warning "not sure how this interacts w/RGB" */
                *p = (v & tiled_fg_pixel) | ((~v) & tiled_bk_pixel);
            }
        }
    }

    /* Set up the converted bitmap. */
    bbox = RGN_BBOX(rh);
    converted_pm.bounds = bbox;
    row_bytes = (((RECT_WIDTH(&bbox) << log2_bpp) + 31U) / 32) * 4;
    converted_pm.rowBytes = row_bytes;
    TEMP_ALLOC_ALLOCATE(new_bits, temp_alloc_space,
                        row_bytes * RECT_HEIGHT(&bbox));
    converted_pm.baseAddr = (Ptr)new_bits;

    pixmap_set_pixel_fields(&converted_pm, 1 << log2_bpp);
    converted_pm.pmTable = ROMlib_dont_depthconv_ctab;

    if(mode == transparent || mode == hilite)
    {
        convert_transparent(&pattern_pm, pixmap, &converted_pm,
                            &bbox, &bbox,
                            mode,
                            true,
                            /* #warning "have mat figure out what the tiling offsets should be" */
                            0, 0);
    }
    else
    {
        RGBColor op_color;
        CTabHandle ctab;
        ITabHandle itab;

        ctab = PIXMAP_TABLE(GD_PMAP(LM(TheGDevice)));
        itab = GD_ITABLE(LM(TheGDevice));

        if(CGrafPort_p(qdGlobals().thePort))
            op_color = CPORT_OP_COLOR(theCPort);
        else /* I have no idea what I'm supposed to do in this case */
            op_color = ROMlib_black_rgb_color;

        convert_pixmap_with_IMV_mode(&pattern_pm, pixmap, &converted_pm,
                                     ctab, ctab, itab,
                                     &bbox, &bbox,
                                     mode, &op_color, true,
                                     /* #warning "have mat figure out what the tiling offsets should be" */
                                     0, 0);
    }

    {
        uint32_t fg, bk;

        pixmap_black_white(pixmap, &fg, &bk);
        srcblt_rgn(rh, srcCopy, log2_bpp,
                   (blt_bitmap_t *)&converted_pm, (blt_bitmap_t *)pixmap,
                   (GUEST<Point> *)&bbox, (GUEST<Point> *)&bbox,
                   fg, bk);
    }

    if(active_screen_addr_p(pixmap))
    {
        const Rect *r = &RGN_BBOX(rh);
        int dst_top = pixmap->bounds.top;
        int dst_left = pixmap->bounds.left;

        dirty_rect_accrue(r->top - dst_top, r->left - dst_left,
                          r->bottom - dst_top, r->right - dst_left);
    }

    HUnlock((Handle)xh);
    xdata_free(xh);

    TEMP_ALLOC_FREE(temp_alloc_space);
}

/* Returns the actual bpp to be used for the destination. */
static inline int
theport_bpp(void)
{
    GrafPtr the_port = qdGlobals().thePort;
    int bpp;

    if(CGrafPort_p(the_port))
        bpp = PIXMAP_PIXEL_SIZE(CPORT_PIXMAP((CGrafPtr)the_port));
    else if(active_screen_addr_p(&the_port->portBits))
        bpp = PIXMAP_PIXEL_SIZE(GD_PMAP(LM(MainDevice)));
    else
        bpp = 1;

    return bpp;
}

void
Executor::ROMlib_blt_pn(RgnHandle rh, INTEGER mode)
{
    int bpp = theport_bpp();

    mode &= 0x3F;
    if(mode < 8)
        mode |= 8;
    if(mode >= hilite || (mode == patXor && !(LM(HiliteMode) & 128)))
        mode = hilite;

    if(bpp == 1 || (!IMV_XFER_MODE_P(mode) && mode != hilite))
    {
        /* I have no idea where this table came from.  It's just been
       * passed down from the old black and white blitter.
       */
        static const uint8_t imvxfer_pat_mode_lookup[8] = { patCopy, patBic, patXor, patOr, patOr, patBic, patXor, patOr };

        if(mode >= 0x20 && mode <= 0x2F)
            mode = imvxfer_pat_mode_lookup[mode & 7];
        else if(mode == hilite)
            mode = patXor;
        BLT_PAT_SIMPLE(rh, mode, CPORT_PEN_PIXPAT, PORT_PEN_PAT);
    }
    else
    {
        BLT_PAT_FANCY(rh, mode, CPORT_PEN_PIXPAT, PORT_PEN_PAT);
    }
}

void Executor::C_StdRgn(GrafVerb verb, RgnHandle rgn)
{
    RgnHandle rh;
    Rect r;

    if(!rgn || EmptyRgn(rgn))
        return;

    if((*rgn)->rgnSize & 0x8000)
    {
        warning_unexpected("negative rgnSize = 0x%x\n", toHost((*rgn)->rgnSize));
        return;
    }

    if(qdGlobals().thePort->picSave)
    {
        ROMlib_drawingverbpicupdate(verb);
        PICOP(OP_frameRgn + (int)verb);
        HLockGuard guard(rgn);
        RgnPtr rp = *rgn;
        PICWRITE(rp, rp->rgnSize);
    }

    /* intersect the region to be drawn with the
     port bounds and port rect */
    rh = NewRgn();
    r = PORT_BOUNDS(qdGlobals().thePort);

    RectRgn(rh, &r);
    SectRgn(rgn, rh, rh);

    if(verb == frame)
    {
        RgnHandle rsave;
        GUEST<Point> pen_size;

        /* remove the current region from rgnSave */
        /* #warning "How does XOR remove it?  e.g. two framerects in a row." */
        pen_size = PORT_PEN_SIZE(qdGlobals().thePort);
        rsave = (RgnHandle)PORT_REGION_SAVE(qdGlobals().thePort);
        if(rsave)
            XorRgn(rgn, rsave, rsave);

        /* construct the frame */
        /* #warning "We inset the region AFTER we've clipped it to the port bounds???" */
        InsetRgn(rh, pen_size.h, pen_size.v);
        XorRgn(rgn, rh, rh);

        /* now `paint' the frame */
        verb = paint;
    }

    /* can this check be moved up? */
    /* if verb == frame, we juke region_save above, so this check
   * probably can't be moved up.  on the other hand, are we supposed
   * to juke region_save if the pen isn't visible? -Mat */
    if(PORT_PEN_VIS(qdGlobals().thePort) < 0)
    {
        DisposeRgn(rh);
        return;
    }

    SectRgn(rh, PORT_VIS_REGION(qdGlobals().thePort), rh);
    SectRgn(rh, PORT_CLIP_REGION(qdGlobals().thePort), rh);

    if(GWorld_p(qdGlobals().thePort))
        LockPixels(CPORT_PIXMAP(qdGlobals().thePort));

    switch(verb)
    {
        case paint:
            ROMlib_blt_pn(rh, PORT_PEN_MODE(qdGlobals().thePort));
            break;
        case erase:
            BLT_PAT_SIMPLE(rh, patCopy, CPORT_BK_PIXPAT, PORT_BK_PAT);
            break;
        case invert:
        {
            int bpp = theport_bpp();
            /* FIXME: is it ok to thrash the fill pattern here? */
            ROMlib_fill_pat(qdGlobals().black);
            if(bpp == 1 || (LM(HiliteMode) & 128))
                BLT_PAT_SIMPLE(rh, patXor, CPORT_FILL_PIXPAT, PORT_FILL_PAT);
            else
                BLT_PAT_FANCY(rh, hilite, CPORT_FILL_PIXPAT, PORT_FILL_PAT);
        }
        break;
        case fill:
            BLT_PAT_SIMPLE(rh, patCopy, CPORT_FILL_PIXPAT, PORT_FILL_PAT);
            break;
        default:
            gui_fatal("unknown GrafVerb");
            break;
    }

    if(GWorld_p(qdGlobals().thePort))
        UnlockPixels(CPORT_PIXMAP(qdGlobals().thePort));

    SET_HILITE_BIT();
    DisposeRgn(rh);
}

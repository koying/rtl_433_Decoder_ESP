#!/usr/bin/env python3
from pathlib import Path
import argparse
import shutil

# leveraging the difficult work done by rtl_433_ESP
# this script automates copy from a populated rtl_433 directory... it doesn't do any error checking
# always review the results
copy_exact="""include/c_util.h include/abuf.h include/bitbuffer.h include/compat_time.h 
include/decoder.h include/decoder_util.h include/fatal.h include/list.h include/logger.h 
include/optparse.h include/output_log.h include/pulse_detect.h include/pulse_slicer.h 
include/r_api.h include/r_device.h include/r_util.h include/rfraw.h include/util.h
include/data.h include/bit_util.h
src/abuf.c src/bitbuffer.c src/compat_time.c src/data.c src/decoder_util.c src/list.c
src/logger.c src/output_log.c src/pulse_data.c src/r_util.c src/util.c
src/devices/*.c
""".split()

# manually review for updates
#src/r_api.c
#include/pulse_data.h
#include/r_private.h
#include/rtl_433.h


#include/data.h

def do_copy_exact(rtl433dir,outdir):
    for filepattern in copy_exact:
        for srcfilename in rtl433dir.glob(filepattern):
            outfilename=outdir / srcfilename.relative_to(rtl433dir).as_posix().replace("src/","src/rtl_433/")
            shutil.copy(srcfilename,outfilename)

def main():
    rtl433dir=Path(__file__).resolve().parent.parent
    parser = argparse.ArgumentParser()
    parser.add_argument("rtl433dir",help="rtl_433 directory")
    parser.add_argument("outdir", nargs='?',default=rtl433dir.as_posix(),help='output directory')
    args = parser.parse_args()
    rtl433dir=Path(args.rtl433dir)
    outdir=Path(args.outdir)

    # pulse_detect.c: fix bitbuffer init to be portable
    srcfile=rtl433dir / "src/pulse_slicer.c"
    outfile=outdir / "src/rtl_433/pulse_slicer.c"
    outfile.write_text(srcfile.read_text()
                       .replace("bitbuffer_t bits = {0};","bitbuffer_clear(&bits);")
                       .replace('#include <limits.h>',"#include <limits.h>\nbitbuffer_t bits;"))

    do_copy_exact(rtl433dir,outdir)

if __name__ == "__main__":
    main()


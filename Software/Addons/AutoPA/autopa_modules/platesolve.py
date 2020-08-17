#!/usr/bin/env python3

import subprocess
import re

def solve (filename, RA, DEC, radius, pixel_resolution):
    config_file="/home/astroberry/.local/share/kstars/astrometry/astrometry.cfg"
    cmd = f"/usr/bin/solve-field --config {config_file} -O --no-plots --no-verify --resort --downsample 2 -3 {RA} -4 {DEC} -5 {radius} {filename}"
    if pixel_resolution != 0:
        cmd = f"{cmd} {filename} --scale-low={pixel_resolution*0.75} --scale-high={pixel_resolution*1.25} --scale-units=\"arcsecperpix\""
    cmd = f"{cmd} {filename}"
    print(f"{cmd}")
    output = str(subprocess.check_output(cmd, shell=True))
    result = re.search(r"Field center: \(RA H:M:S, Dec D:M:S\) = \((?P<RA_H>\d+):(?P<RA_M>\d+):(?P<RA_S>\d+\.\d+), (?P<DEC_D>[+-]?\d+):(?P<DEC_M>\d+):(?P<DEC_S>\d+\.\d+)\)", output)
    if result is None:
        return "Failed"
    else:
        RA_H = result.group("RA_H")
        RA_M = result.group("RA_M")
        RA_S = result.group("RA_S")
        DEC_D = result.group("DEC_D")
        DEC_M = result.group("DEC_M")
        DEC_S = result.group("DEC_S")
        return (f"{RA_H}h{RA_M}m{RA_S}s", f"{DEC_D}d{DEC_M}m{DEC_S}s")
#!/usr/bin/env python3
"""Generate a 256-entry Q15 sin lookup table for FOC motor control.

Usage: python gen_sin_table.py > sin_table.h
"""

import math

NUM_ENTRIES = 256
Q15_MAX = 32767
Q15_MIN = -32768

entries = []
for i in range(NUM_ENTRIES):
    angle = 2.0 * math.pi * i / NUM_ENTRIES
    val = math.sin(angle)
    q15 = int(round(val * Q15_MAX))
    if q15 > Q15_MAX:
        q15 = Q15_MAX
    elif q15 < Q15_MIN:
        q15 = Q15_MIN
    entries.append(q15)

print("""/* Auto-generated sin lookup table
 * Entries: 256
 * Format: Q15 (32767 = 1.0)
 * Index:  angle_rad * (256 / (2*PI))  idx = angle_rad * 40.743665
 */
""")

print(f"const int16 SIN_TABLE_Q15[{NUM_ENTRIES}] = {{")

entries_per_line = 8
for i in range(0, len(entries), entries_per_line):
    line_entries = entries[i:i + entries_per_line]
    line_str = ", ".join(f"{v:6d}" for v in line_entries)
    if i + entries_per_line < len(entries):
        print(f"    {line_str},")
    else:
        print(f"    {line_str}")
print("};")

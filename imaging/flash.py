import sys
import os
import subprocess

prepend = """@1900
%02X 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 
"""

if __name__ == "__main__":
    ti_txt_out = ""
    badge_id = int(sys.argv[1])
    prepend = prepend % badge_id
    ti_txt_out = prepend
    for line in open("qc13.txt"):
        ti_txt_out += line
    with open('tmp.txt', 'w') as tmp_out:
        tmp_out.write(ti_txt_out)
    #os.system("msp430flasher -w tmp.txt")
    subprocess.call(['msp430flasher', '-w', 'tmp.txt'], stdout=subprocess.PIPE)
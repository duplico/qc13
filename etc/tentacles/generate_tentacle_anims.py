import sys, os, string

import webcolors

color_corrections = {
    "default" : (255,255,255),
    "off" : (0,0,0),
}

eye_frames = dict()
eye_frames_uint32 = dict()

def main():    
    print '#include "../../led_display.h"'
    
    for anim in os.listdir("."):
        if not anim[-3:] == "txt": continue
        camos = []
        camo_lengths = []
        inks = []
        ink_lengths = []
        super_inks = []
        super_ink_lengths = []
        
        anim_name = anim[:-4]
        
        print
        print "///////////////// %s:" % anim_name.upper()
        # Append to "ALL" or whatever
        with open(anim) as f:
            local_colors = dict()
            
            lines = [line.strip() for line in f if line.strip() and (line[0] != "#" or "CAMO" in line or "SINGLE" in line or "DOUBLE" in line)]
            line_no = 0
            
            # Consume all of the colors in this
            while lines[line_no][0] in string.digits:
                color_num, color_name = lines[line_no].split()
                local_colors[int(color_num)] = color_corrections.get(
                    color_name,
                    webcolors.name_to_rgb(color_name) if color_name not in color_corrections else None
                )
                line_no += 1
            
            # Consume the animation type:
            # TODO:
            local_type = lines[line_no]
            line_no += 1
            
            camo_line = ""
            ink_line = ""
            doubleink_line = ""
            
            assert "CAMO" in lines[line_no].upper()
            line_no += 1
            
            while lines[line_no][0] in string.digits:
                # Consume the animation. Ignore white space
                camo_line += lines[line_no]
                line_no += 1            
            
            camo = map(lambda s: int(s.strip()), camo_line.split(','))
            camo_frames = [camo[i:i+10] for i in xrange(0, len(camo), 10)]
            
            assert "SINGLE" in lines[line_no].upper()
            line_no += 1
            
            while lines[line_no][0] in string.digits:
                # Consume the animation. Ignore white space
                ink_line += lines[line_no]
                line_no += 1            
            
            ink = map(lambda s: int(s.strip()), ink_line.split(','))
            ink_frames = [ink[i:i+10] for i in xrange(0, len(ink), 10)]
            
            assert "DOUBLE" in lines[line_no].upper()
            line_no += 1
            
            while line_no < len(lines):
                # Consume the animation. Ignore white space
                doubleink_line += lines[line_no]
                line_no += 1            
            
            doubleink = map(lambda s: int(s.strip()), doubleink_line.split(','))
            doubleink_frames = [doubleink[i:i+10] for i in xrange(0, len(doubleink), 10)]
            
            # Great. Now we've ingested the entire file.
            # Time to start generating frames.
            
            for l,lname in ((camo_frames, 'camo'), (ink_frames, 'ink'), (doubleink_frames, 'doubleink')):
                print "// frames for %s" % lname
                print "const rgbcolor_t %s_%s_frames[][8] = {" % (anim_name, lname)
                
                frames = []
                metadata = []
                for f in l:
                    metadata += f[8:]
                    fr = map(lambda a: local_colors[a], f[:8])
                    print "    {%s}," % ', '.join(map(lambda rgb: "{0x%x, 0x%x, 0x%x}" % rgb, fr))
                print "};"
                
                print "// the animation:"
                print "const tentacle_animation_t %s_%s = {%s_%s_frames, %d};" % (anim_name, lname, anim_name, lname, len(l))
                print
                
            
    
if __name__ == "__main__":
    main()
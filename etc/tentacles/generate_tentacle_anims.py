import sys, os, string

import webcolors

color_corrections = {
    "default" : (255,255,255),
    "off" : (0,0,0),
}

eye_frames = dict()
eye_frames_uint32 = dict()

c_lines = []
h_lines = [
    "#ifndef LEGANIMS_H_",
    "#define LEGANIMS_H_",
    "",
    "#define LEG_CAMO_INDEX 0",
    "#define LEG_INK_INDEX 0",
    "#define LEG_DOUBLEINK_INDEX 1",
    "",
]

def main():    
    c_lines.append('#include "../../led_display.h"')
    c_lines.append('#include "leg_anims.h"')
    
    all_animations = []
    
    for anim in os.listdir("."):
        if not anim[-3:] == "txt": continue
        camos = []
        camo_lengths = []
        inks = []
        ink_lengths = []
        super_inks = []
        super_ink_lengths = []
        
        anim_name = anim[:-4]
        all_animations.append(anim_name)
        
        c_lines.append("")
        c_lines.append("///////////////// %s:" % anim_name.upper())
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
            
            local_animation_names = []
            
            for l,lname in ((camo_frames, 'camo'), (ink_frames, 'ink'), (doubleink_frames, 'doubleink')):
                c_lines.append("// frames for %s" % lname)
                c_lines.append("const rgbcolor_t %s_%s_frames[][8] = {" % (anim_name, lname))
                h_lines.append("// frames for %s" % lname)
                h_lines.append("extern const rgbcolor_t %s_%s_frames[][8];" % (anim_name, lname))
                
                local_animation_names += ["%s_%s" % (anim_name, lname)]
                
                frames = []
                metadata1 = []
                metadata2 = []
                for f in l:
                    metadata1 += [str(f[8])]
                    metadata2 += [str(f[9])]
                    fr = map(lambda a: local_colors[a], f[:8])
                    c_lines.append("    {%s}," % ', '.join(map(lambda rgb: "{0x%x, 0x%x, 0x%x}" % rgb, fr)))
                c_lines.append("};")
                c_lines.append("uint16_t %s_%s_durations[] = {%s};" % (anim_name, lname, ', '.join(metadata1)))
                c_lines.append("uint16_t %s_%s_metadata2[] = {%s};" % (anim_name, lname, ', '.join(metadata2)))
                
                h_lines.append("extern uint16_t %s_%s_durations[];" % (anim_name, lname))
                h_lines.append("extern uint16_t %s_%s_metadata2[];" % (anim_name, lname))
                
                c_lines.append("// the animation:")
                c_lines.append("const tentacle_animation_t %s_%s = {%s_%s_frames, %s_%s_durations, %s_%s_metadata2, %d};" % (anim_name, lname, anim_name, lname, anim_name, lname, anim_name, lname, len(l)))
                
                h_lines.append("extern const tentacle_animation_t %s_%s;" % (anim_name, lname))
            c_lines.append("")
            c_lines.append("const tentacle_animation_t *%s_anim_set[3] = {%s};" % (anim_name, ', '.join(map(lambda a: "&%s" % a, local_animation_names))))
    c_lines.append("")
    h_lines.append("#define LEG_ANIM_COUNT %d" % len(all_animations))
    for i in range(len(all_animations)):
        h_lines.append("#define LEG_ANIM_%s %d" % (all_animations[i].upper(), i))
    c_lines.append("const tentacle_animation_t **legs_all_anim_sets[] = {%s};" % ', '.join(map(lambda a: "%s_anim_set" % a, all_animations)))
    h_lines.append("extern const tentacle_animation_t **legs_all_anim_sets[];")
    
    h_lines.append("#endif // _H_")
    
    print '\n'.join(c_lines)
    
    with open("leg_anims.c", 'w') as f:
        f.writelines(map(lambda a: a+"\n", c_lines))
    
    with open("leg_anims.h", 'w') as f:
        f.writelines(map(lambda a: a+"\n", h_lines))
            
    
if __name__ == "__main__":
    main()
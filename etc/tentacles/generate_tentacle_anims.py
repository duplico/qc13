import sys, os, string

import webcolors

COLOR_SCALE_FACTOR = 5.0

color_corrections = {
    "default" : (40*3.0/COLOR_SCALE_FACTOR,240*3.0/COLOR_SCALE_FACTOR,178*3.0/COLOR_SCALE_FACTOR),
    "white" : (255,128,255),
    "off" : (0,0,0),
    "pink" : (240, 20, 60),
    "bipink" : (0xff, 0, 0xb0),
    "bimiddle" : (0x53, 0x1f, 0x66),
    "biblue" : (0x50, 0, 0xff),
    "b1" : (0xc5,0x20,0x04),
    "b2" : (0xd5, 0x63, 0),
    "b3" : (0xfe, 0xdd, 0x63),
    "b4" : (0xbe, 0xa6, 0x38),
    "dim" : (0, 0, 15),
}

INK_LEN = 6000

global_color_correct = (1,.5,1)

eye_frames = dict()
eye_frames_uint32 = dict()

c_lines = []
h_lines = [
    "#ifndef LEGANIMS_H_",
    "#define LEGANIMS_H_",
    "",
    '#include "../../led_display.h"',
    "",
    "#define LEG_CAMO_INDEX 0",
    "#define LEG_INK_INDEX 1",
    "#define LEG_DOUBLEINK_INDEX 2",
    "#define LEG_ANIM_TYPE_NONE 201",
    "#define LEG_ANIM_NONE 211",
    "",
]

def main():    
    c_lines.append('#include "../../led_display.h"')
    c_lines.append('#include "leg_anims.h"')
    
    all_animations = []
    meta_animations = []
    all_types = []
    all_spray_colors = []
    meta_spray_colors = []
    
    for anim in os.listdir("."):
        if not anim[-3:] == "txt": continue
        print anim
        camos = []
        camo_lengths = []
        inks = []
        ink_lengths = []
        super_inks = []
        super_ink_lengths = []
        color_lookups = dict()
        
        
        anim_name = anim[:-4]
        if anim_name.upper().startswith('META'):
            meta_animations.append(anim_name)
        else:
            all_animations.append(anim_name)
        
        c_lines.append("")
        c_lines.append("///////////////// %s:" % anim_name.upper())
        # Append to "ALL" or whatever
        with open(anim) as f:
            local_colors = dict()
            
            lines = [line.strip() for line in f if line.strip() and (line[0] != "#" or "CAMO" in line.upper() or "SINGLE" in line.upper() or "INK" in line.upper() or "DOUBLE" in line.upper())]
            line_no = 0
            
            # Consume all of the colors in this
            while lines[line_no][0] in string.digits:
                color_num, color_name = lines[line_no].split()
                color_lookups[color_name] = int(color_num)
                local_colors[int(color_num)] = color_corrections.get(
                    color_name,
                    webcolors.name_to_rgb(color_name) if color_name not in color_corrections else None
                )
                
                local_colors[int(color_num)] = tuple(map(lambda a: COLOR_SCALE_FACTOR*a, local_colors[int(color_num)]))
                if color_name not in color_corrections:
                    local_colors[int(color_num)] = tuple(map(lambda a: int(a[0]*a[1]), zip(local_colors[int(color_num)], global_color_correct)))
                line_no += 1
            
            camo_line = ""
            ink_line = ""
            doubleink_line = ""
            wiggles = []
            anim_types = []
            
            assert "SPRAY" in lines[line_no].upper()
            if anim_name.upper().startswith('META'):
                meta_spray_colors.append(local_colors[color_lookups[lines[line_no].split()[1]]])
            else:
                all_spray_colors.append(local_colors[color_lookups[lines[line_no].split()[1]]])
            line_no += 1
            
            assert "CAMO" in lines[line_no].upper()
            line_no += 1
            
            if "WIGGLE" in lines[line_no].upper():
                wiggles.append(1)
                line_no += 1
            else:
                wiggles.append(0)
                
            local_type = lines[line_no]
            assert local_type[0] not in string.digits
            if local_type not in all_types: all_types.append(local_type)
            anim_types.append(local_type)
            line_no += 1
            
            while lines[line_no][0] in string.digits:
                # Consume the animation. Ignore white space
                camo_line += lines[line_no]
                line_no += 1
            
            if camo_line[-1] == ',':
                camo_line = camo_line[:-1]
            camo = map(lambda s: int(s.strip()), camo_line.split(','))
            camo_frames = [camo[i:i+10] for i in xrange(0, len(camo), 10)]
            
            assert "SINGLE" in lines[line_no].upper() or "INK" in lines[line_no].upper()
            line_no += 1
            
            if "WIGGLE" in lines[line_no].upper():
                wiggles.append(1)
                line_no += 1
            else:
                wiggles.append(0)
                
            local_type = lines[line_no]
            assert local_type[0] not in string.digits
            if local_type not in all_types: all_types.append(local_type)
            anim_types.append(local_type)
            line_no += 1
            
            while lines[line_no][0] in string.digits:
                # Consume the animation. Ignore white space
                ink_line += lines[line_no]
                line_no += 1            
            
            ink = map(lambda s: int(s.strip()), ink_line.split(','))
            ink_frames = [ink[i:i+10] for i in xrange(0, len(ink), 10)]
            
            assert "DOUBLE" in lines[line_no].upper()
            line_no += 1
            
            if "WIGGLE" in lines[line_no].upper():
                wiggles.append(1)
                line_no += 1
            else:
                wiggles.append(0)
                
            local_type = lines[line_no]
            assert local_type[0] not in string.digits
            if local_type not in all_types: all_types.append(local_type)
            anim_types.append(local_type)
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
            
            for l, lname, atype, wiggle in ((camo_frames, 'camo', anim_types[0], wiggles[0]), (ink_frames, 'ink', anim_types[1], wiggles[1]), (doubleink_frames, 'doubleink', anim_types[2], wiggles[2])):
                if anim_name.startswith("zflag") and lname == 'ink': break
                c_lines.append("// frames for %s" % lname)
                c_lines.append("const rgbcolor_t %s_%s_frames[][8] = {" % (anim_name, lname))
                h_lines.append("// frames for %s" % lname)
                h_lines.append("extern const rgbcolor_t %s_%s_frames[][8];" % (anim_name, lname))
                
                if anim_name.startswith("zflag"):
                    local_animation_names += ["%s_%s" % (anim_name, lname)]*3
                else:
                    local_animation_names += ["%s_%s" % (anim_name, lname)]
                
                frames = []
                metadata1 = []
                metadata2 = []
                total_duration = 0
                for f in l:
                    metadata1 += [str(f[8])]
                    metadata2 += [str(f[9])]
                    total_duration += int(f[8]) + int(f[9])
                    try:
                        fr = map(lambda a: local_colors[a], f[:8])
                    except Exception as e:
                        print 'Error on frame: ', f
                        exit(1)
                    fr = fr[::-1]
                    c_lines.append("    {%s}," % ', '.join(map(lambda rgb: "{0x%x, 0x%x, 0x%x}" % rgb, fr)))
                
                if total_duration > INK_LEN:
                    ink_loops = 0
                else:
                    ink_loops = INK_LEN / total_duration
                
                c_lines.append("};")
                c_lines.append("uint16_t %s_%s_durations[] = {%s};" % (anim_name, lname, ', '.join(metadata1)))
                c_lines.append("uint16_t %s_%s_fade_durs[] = {%s};" % (anim_name, lname, ', '.join(metadata2)))
                
                h_lines.append("extern uint16_t %s_%s_durations[];" % (anim_name, lname))
                h_lines.append("extern uint16_t %s_%s_fade_durs[];" % (anim_name, lname))
                
                c_lines.append("// the animation:")
                c_lines.append("const tentacle_animation_t %s_%s = {%s_%s_frames, %s_%s_durations, %s_%s_fade_durs, %d, ANIM_TYPE_%s, %d, %d};" % (anim_name, lname, anim_name, lname, anim_name, lname, anim_name, lname, len(l), atype.upper(), wiggle, ink_loops))
                
                h_lines.append("extern const tentacle_animation_t %s_%s;" % (anim_name, lname))
            c_lines.append("")
            h_lines.append("extern const tentacle_animation_t *%s_anim_set[3];" % anim_name)
            c_lines.append("const tentacle_animation_t *%s_anim_set[3] = {%s};" % (anim_name, ', '.join(map(lambda a: "&%s" % a, local_animation_names))))
    c_lines.append("")
    h_lines.append("#define LEG_ANIM_COUNT %d" % len(all_animations))
    h_lines.append("#define LEG_ANIM_COUNT_INCL_META %d" % len(all_animations+meta_animations))
    for i in range(len(all_animations+meta_animations)):
        h_lines.append("#define LEG_ANIM_%s %d" % ((all_animations+meta_animations)[i].upper(), i))
    for i in range(len(all_types)):
        h_lines.append("#define ANIM_TYPE_%s %d" % (all_types[i].upper(), i))
    h_lines.append("#define LEG_ANIM_TYPE_COUNT %d" % len(all_types))
    c_lines.append("const tentacle_animation_t **legs_all_anim_sets[] = {%s};" % ', '.join(map(lambda a: "%s_anim_set" % a, all_animations+meta_animations)))
    h_lines.append("extern const tentacle_animation_t **legs_all_anim_sets[];")
    
    h_lines.append("#endif // _H_")
    
    all_spray_colors += meta_spray_colors
    h_lines.append("extern const rgbcolor_t sprays[];")
    c_lines.append("const rgbcolor_t sprays[%d] = {%s};" % (len(all_animations+meta_animations),', '.join(map(lambda rgb: "{0x%x, 0x%x, 0x%x}" % rgb, all_spray_colors))))
        
    with open("leg_anims.c", 'w') as f:
        f.writelines(map(lambda a: a+"\n", c_lines))
    
    with open("leg_anims.h", 'w') as f:
        f.writelines(map(lambda a: a+"\n", h_lines))
            
    
if __name__ == "__main__":
    main()
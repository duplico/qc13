import sys, os

c_lines = [
    '#include "eye_anims.h"',
    ''
]
h_lines = [
    "#ifndef EYEANIMS_H_",
    "#define EYEANIMS_H_",
    "",
    '#include "../../led_display.h"',
    "",
]

eye_trans = {
    1:  (2,0),
    2:  (1,0),
    3:  (1,1),
    4:  (1,2),
    5:  (1,3),
    6:  (1,4),
    7:  (2,5),
    8:  (1,5),
    9:  (1,6),
    10: (1,7),
    11: (1,8),
    12: (1,9),
    13: (2,10),
    14: (1,10),
    15: (1,11),
    16: (1,12),
    17: (1,13),
    18: (1,14),
    19: (2,11,12,13,14,1),
    20: (2,1,2,3,4,6),
    21: (2,6,7,8,9,11)
}

eye_frames = dict()
eye_frames_uint32 = dict()

def main():
    # First deal with states:
    for state_file in os.listdir("states"):
        if not state_file[-3:] == "txt": continue
        with open(os.path.join("states", state_file)) as f:
            banks = {
                1 : [0]*15,
                2 : [0]*15,
            }
            name = str(state_file.split(".")[0])
            parsed = False
            i = 0
            while not parsed:
                i+=1
                line = f.readline()
                if not line.startswith('#'):
                    parsed = True
            leds = line.split(",")
            leds = map(lambda a: int(a.strip()) if a else 0, leds)
            if leds == [0]: leds = []
            
            for led in leds:
                for channel in eye_trans[led][1:]:
                    banks[eye_trans[led][0]][channel] = 1
            eye_frames[name] = banks
    
    for frame, values in eye_frames.items():
        frame_uint32 = ""
        for v in values[2]:
            frame_uint32 += (str(v))
        frame_uint32 += "0"
        for v in values[1]:
            frame_uint32 += (str(v))
        frame_uint32 += "0"
        eye_frames_uint32[frame] = frame_uint32
        
    c_lines.append('#include "../../led_display.h"')

    all_animations = []
    meta_animations = []
    
    for anim in os.listdir("."):
        if not anim[-3:] == "txt": continue
        #left_anims = []
        #right_anims = []
        anims = []
        lengths = []
        c_lines.append("")
        anim_name = anim[:-4]
        c_lines.append("// Frames for %s" % anim_name)
        
        if anim_name.upper().startswith('META'):
            meta_animations.append(anim_name)
        else:
            all_animations.append(anim_name)
        
        with open(anim) as f:
            i=0
            for line in f:
                i+=1
                if not line.strip(): continue
                line_elements = map(lambda a: a.strip(), line.split(","))
                try:
                    anims.append("0b%s%s" % (eye_frames_uint32[line_elements[0]], eye_frames_uint32[line_elements[1]]))
                except Exception as e:
                    print "Broken on %s:%d" % (anim,i)
                    exit(1)
                #left_anims.append(eye_frames_uint32[line_elements[0]])
                #right_anims.append(eye_frames_uint32[line_elements[1]])
                lengths.append(line_elements[2])
        #print "uint32_t %s_l_frames[] = {%s};" % (anim[:-4], ", ".join(left_anims))
        #print "uint32_t %s_r_frames[] = {%s};" % (anim[:-4], ", ".join(right_anims))
        c_lines.append("uint64_t %s_frames[] = {%s};" % (anim[:-4], ", ".join(anims)))
        h_lines.append("extern uint64_t %s_frames[];" % (anim[:-4]))
        c_lines.append("uint16_t %s_lengths[] = {%s};" % (anim[:-4], ", ".join(lengths)))
        h_lines.append("extern uint16_t %s_lengths[];" % (anim[:-4]))
        c_lines.append("")
        c_lines.append("// Animation struct for %s" % anim[:-4])
        c_lines.append("face_animation_t %s = {%s_frames, %s_lengths, %s};" % (anim[:-4],anim[:-4],anim[:-4],len(lengths)))
        h_lines.append("extern face_animation_t %s;" % (anim[:-4]))
    
    c_lines.append("")
    c_lines.append("// All animations here:")
    #print "#define FACE_ANIM_NONE 0"
    h_lines.append("#define FACE_ANIM_COUNT %d" % len(all_animations))
    all_animations = all_animations + meta_animations
    h_lines.append("#define FACE_ANIM_COUNT_INCL_META %d" % len(all_animations))
    for i in range(len(all_animations)):
        h_lines.append("#define FACE_ANIM_%s %d" % ((all_animations)[i].upper(), i))
    h_lines.append("")
    all_animations_ptrs = map(lambda a: "&" + a, all_animations)
    c_lines.append("face_animation_t *face_all_animations[FACE_ANIM_COUNT_INCL_META] = {%s};" % ", ".join(all_animations_ptrs))
    h_lines.append("extern face_animation_t *face_all_animations[FACE_ANIM_COUNT_INCL_META];")
    
    h_lines.append("#endif // _H_")
    with open("eye_anims.c", 'w') as f:
        f.writelines(map(lambda a: a+"\n", c_lines))
    
    with open("eye_anims.h", 'w') as f:
        f.writelines(map(lambda a: a+"\n", h_lines))
    
if __name__ == "__main__":
    main()
import sys, os

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
            leds = f.readline().split(",")
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
        
    print '#include "../../led_display.h"'

    all_animations = []
    
    for anim in os.listdir("."):
        if not anim[-3:] == "txt": continue
        #left_anims = []
        #right_anims = []
        anims = []
        lengths = []
        print
        print "// Frames for %s" % anim[:-4]
        all_animations.append(anim[:-4])
        with open(anim) as f:
            for line in f:
                if not line: continue
                line_elements = map(lambda a: a.strip(), line.split(","))
                anims.append("0b%s%s" % (eye_frames_uint32[line_elements[0]], eye_frames_uint32[line_elements[1]]))
                #left_anims.append(eye_frames_uint32[line_elements[0]])
                #right_anims.append(eye_frames_uint32[line_elements[1]])
                lengths.append(line_elements[2])
        #print "uint32_t %s_l_frames[] = {%s};" % (anim[:-4], ", ".join(left_anims))
        #print "uint32_t %s_r_frames[] = {%s};" % (anim[:-4], ", ".join(right_anims))
        print "uint64_t %s_frames[] = {%s};" % (anim[:-4], ", ".join(anims))
        print "uint16_t %s_lengths[] = {%s};" % (anim[:-4], ", ".join(lengths))
        print
        print "// Animation struct for %s" % anim[:-4]
        print "face_animation_t %s = {%s_frames, %s_lengths, %s};" % (anim[:-4],anim[:-4],anim[:-4],len(lengths))
    
    print
    print "// All animations here:"
    #print "#define FACE_ANIM_NONE 0"
    print "#define FACE_ANIM_COUNT %d" % len(all_animations)
    for i in range(len(all_animations)):
        print "#define FACE_ANIM_%s %d" % (all_animations[i].upper(), i)
    print
    all_animations_ptrs = map(lambda a: "&" + a, all_animations)
    print "face_animation_t *face_all_animations[FACE_ANIM_COUNT] = {%s};" % ", ".join(all_animations_ptrs)
    
if __name__ == "__main__":
    main()
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
    19: (2,13),
    20: (2,2),
    21: (2,8)
}

eye_frames = dict()
eye_frames_uint32 = dict()

def main():
    # First deal with states:
    for state_file in os.listdir("states"):
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
                banks[eye_trans[led][0]][eye_trans[led][1]] = 1
            eye_frames[name] = banks
    
    
    for frame, values in eye_frames.items():
        frame_uint32 = "0b"
        for v in values[1]:
            frame_uint32 += (str(v))
        frame_uint32 += "0"
        for v in values[2]:
            frame_uint32 += (str(v))
        frame_uint32 += "0"
        eye_frames_uint32[frame] = frame_uint32
        
    print '#include "../../led_display.h"'

    for anim in os.listdir("."):
        if not anim[-3:] == "txt": continue
        left_anims = []
        right_anims = []
        lengths = []
        print
        print "// Frames for %s" % anim[:-4]
        with open(anim) as f:
            for line in f:
                if not line: continue
                line_elements = map(lambda a: a.strip(), line.split(","))
                left_anims.append(eye_frames_uint32[line_elements[0]])
                right_anims.append(eye_frames_uint32[line_elements[1]])
                lengths.append(line_elements[2])
        print "uint32_t %s_l_frames[] = {%s};" % (anim[:-4], ", ".join(left_anims))
        print "uint32_t %s_r_frames[] = {%s};" % (anim[:-4], ", ".join(right_anims))
        print "uint16_t %s_lengths[] = {%s};" % (anim[:-4], ", ".join(lengths))
        print
        print "// Animation struct for %s" % anim[:-4]
        print "face_animation_t %s = {%s_l_frames, %s_r_frames, %s_lengths, %s};" % (anim[:-4],anim[:-4],anim[:-4],anim[:-4],len(lengths))
    
    
if __name__ == "__main__":
    main()
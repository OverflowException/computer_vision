import sys
import re

def extract(filename):
    """pass data filename, return tracks of x coordinates and y coordinates and number of objects"""
    
    try:
        fd = open(sys.argv[1], 'r')
    except IOError:
        print("Cannot open " + filename)
    
    #regular expressions
    header_pattern = re.compile("#Object = (\d+)")
    point_pattern = re.compile("\[(\d+\.?\d*), (\d+\.?\d*)\]")

        
    #Get object count
    line = fd.readline()
    while line[0] != '#':
        line = fd.readline()

    m = header_pattern.search(line)
    obj_count = int(m.group(1))

    #2D arrays, storing track of x and y respectively
    #e.g. structure of obj_x_tracks, m objects, n points each
    # [
    #     [x00, x01, .... x0n]
    #     [x10, x11, .... x1n]
    #     [x20, x21, .... x2n]
    #     [x30, x31, .... x3n]
    #     .
    #     .
    #     [xm0, xm1, .... xmn]
    # ]
    obj_x_tracks = []
    obj_y_tracks = []

    #Allocate 2D Array
    for obj_idx in range(obj_count):
        obj_x_tracks.append([])
        obj_y_tracks.append([])

    #Read file
    obj_idx = 0
    while True:
        line = fd.readline()
        if not line:
            break;

        m = point_pattern.search(line)
    
        obj_x_tracks[obj_idx].append(float(m.group(1)))
        obj_y_tracks[obj_idx].append(float(m.group(2)))
    
        obj_idx = (obj_idx + 1) % obj_count
    
    #Close file
    fd.close()

    return obj_x_tracks, obj_y_tracks, obj_count



#!/usr/bin/python
###########################
#Author: liyao
#Python version: 2.7
#Dependency: matplotlib
###########################
import sys
import re
import matplotlib.pyplot as plt
import numpy as np
import extmovdata as ext
import phshift as ps



if len(sys.argv) != 2:
    print("Usage: python" + sys.argv[0] + " [point log]")
    sys.exit()

tracks_x, tracks_y, obj_count = ext.extract(sys.argv[1])
    
#3 lines to be plotted
line_count = 3

#2D array, storing slopes of lines. Same structure as obj_[xy]_tracks
slopes = []
#1D array, slope value buffer for each line
slope_buf = []
#Allocate 2D array
for line_idx in range(0, line_count):
    #2 points determine a line. The indices of these 2 points in obj_[xy]_tracks arrays
    p1_idx = line_idx * 2
    p2_idx = line_idx * 2 + 1
    #Generate slopes
    for x1, x2, y1, y2 in zip(tracks_x[p1_idx], tracks_x[p2_idx], tracks_y[p1_idx], tracks_y[p2_idx]):
        slope_buf.append((y2 - y1) / (x2 - x1))
    slopes.append(slope_buf)
    slope_buf = []

# #Calculate phase shift
# phshift = phshift(obj_y_tracks[0][:], obj_y_tracks[5][:])
# print("phase shift of y between object 0 and 5: " + str(phshift))

#Time domain figure
time_domain_fig, slope_time_plot = plt.subplots(nrows = 1, ncols = 1)

for line_idx in range(0, line_count):
    slope_time_plot.plot(slopes[line_idx])

lengend_marker = []
for line_idx in range(0, line_count):
    lengend_marker.append("Line " + str(line_idx))
    
slope_time_plot.legend(lengend_marker, loc='upper right')
slope_time_plot.set_title("Slope movement")
slope_time_plot.set_xlabel("Frame number")
slope_time_plot.set_ylabel("Slope value")
slope_time_plot.grid()

time_domain_fig.suptitle("Changing of slopes", fontsize = 20);
time_domain_fig.show()

raw_input("Press [Enter] to continue...")

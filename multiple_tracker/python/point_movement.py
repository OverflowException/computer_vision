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
    
#Calculate phase shift
phshift, _, _ = ps.phshift(tracks_y[0][:], tracks_y[5][:])
print("phase shift of y between object 0 and 5: " + str(phshift))

#Time domain figure
time_domain_fig, (x_time_plot, y_time_plot) = plt.subplots(nrows = 2, ncols = 1)

for obj_idx in range(0, obj_count):
    x_time_plot.plot(tracks_x[obj_idx])
    y_time_plot.plot(tracks_y[obj_idx])

lengend_marker = []
for obj_idx in range(0, obj_count):
    lengend_marker.append("Tracking Point " + str(obj_idx))
    
x_time_plot.legend(lengend_marker, loc='upper right')
x_time_plot.set_title("X movement")
x_time_plot.set_xlabel("Frame number")
x_time_plot.set_ylabel("X coordinate")
x_time_plot.grid()

y_time_plot.legend(lengend_marker, loc='upper right')
y_time_plot.set_title("Y movement")
y_time_plot.set_xlabel("Frame number")
y_time_plot.set_ylabel("Y coordinate")
y_time_plot.grid()

time_domain_fig.suptitle("Tracking point movement", fontsize = 20);
time_domain_fig.show()

# #perform FFT transform
# spect_0 = np.fft.fft(obj_y_tracks[0])
# mag_0 = np.abs(spect_0)
# phase_0 = np.angle(spect_0)

# spect_5 = np.fft.fft(obj_y_tracks[5])
# mag_5 = np.abs(spect_5)
# phase_5 = np.angle(spect_5)

# phase_diff = phase_0 - phase_5

# freq_domain_fig, (x_freq_plot, y_freq_plot)= plt.subplots(nrows = 2, ncols = 1)

# x_freq_plot.plot(mag_0)
# x_freq_plot.plot(mag_5)
# #y_freq_plot.plot(phase_diff)
# y_freq_plot.plot(phase_0)
# y_freq_plot.plot(phase_5)

# freq_domain_fig.show()
        
# #Plot 4 graphs
# fig, ((plt_word_corr, plt_word_accu), (plt_deletion_num, plt_insertion_num)) = plt.subplots(2, 2)
# #plot word correcteness rate
# plt_word_corr.plot(penalty, word_corr)
# plt_word_corr.set_title("Word correctness")
# plt_word_corr.set_xlabel("penalty")
# plt_word_corr.set_ylabel("correct(%)")
# plt_word_corr.grid()
# #plot word accuracy rate
# plt_word_accu.plot(penalty, word_accu)
# plt_word_accu.set_title("Word accuracy")
# plt_word_accu.set_xlabel("penalty")
# plt_word_accu.set_ylabel("Accuracy(%)")
# plt_word_accu.grid()
# #deletion number
# plt_deletion_num.plot(penalty, deletion_num)
# plt_deletion_num.set_title("Number of deletion")
# plt_deletion_num.set_xlabel("penalty")
# plt_deletion_num.set_ylabel("number")
# plt_deletion_num.grid()
# #insertion number
# plt_insertion_num.plot(penalty, insertion_num)
# plt_insertion_num.set_title("Number of insertion")
# plt_insertion_num.set_xlabel("penalty")
# plt_insertion_num.set_ylabel("number")
# plt_insertion_num.grid()

# fig.suptitle(sys.argv[1], fontsize=20);
# fig.show()

raw_input("Press [Enter] to continue...")

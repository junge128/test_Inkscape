import colorsys

print '''GIMP Palette
Name: Inkscape default
Columns: 3
# generated by PaletteGen.py'''


# grays

g_steps = 10
g_step_size = 1.0 / g_steps

for i in range(0, g_steps + 1):
    level = i * g_step_size
    r, g, b = colorsys.hls_to_rgb(0, level, 0)
    
    rval = int(round(r * 255))
    gval = int(round(g * 255))
    bval = int(round(b * 255))
    
    if i == 0:
        line = "%3s %3s %3s  Black" % (rval, gval, bval)
    elif i == g_steps:
        line = "%3s %3s %3s  White" % (rval, gval, bval)
    else:
        line = "%3s %3s %3s  %s%% Gray" % (rval, gval, bval, 100 - int(level * 100))
    print line

    # add three more steps near white
    if i == g_steps - 1:
        level_m = level + 0.25 * g_step_size
        r, g, b = colorsys.hls_to_rgb(0, level_m, 0)
        rval = int(round(r * 255))
        gval = int(round(g * 255))
        bval = int(round(b * 255))
        print "%3s %3s %3s  %s%% Gray" % (rval, gval, bval, 100 - (level_m * 100))

        level_m = level + 0.5 * g_step_size
        r, g, b = colorsys.hls_to_rgb(0, level_m, 0)
        rval = int(round(r * 255))
        gval = int(round(g * 255))
        bval = int(round(b * 255))
        print "%3s %3s %3s  %s%% Gray" % (rval, gval, bval, 100 - int(level_m * 100))

        level_mm = level + 0.75 * g_step_size
        r, g, b = colorsys.hls_to_rgb(0, level_mm, 0)
        rval = int(round(r * 255))
        gval = int(round(g * 255))
        bval = int(round(b * 255))
        print "%3s %3s %3s  %s%% Gray" % (rval, gval, bval, 100 - (level_mm * 100))


# standard HTML colors
print '''128   0   0  Maroon (#800000)
255   0   0  Red (#FF0000)
128 128   0  Olive (#808000)
255 255   0  Yellow (#FFFF00)
  0 128   0  Green (#008000)
  0 255   0  Lime (#00FF00)
  0 128 128  Teal (#008080)
  0 255 255  Aqua (#00FFFF)
  0   0 128  Navy (#000080)
  0   0 255  Blue (#0000FF)
128   0 128  Purple (#800080)
255   0 255  Fuchsia (#FF00FF)'''

# HSL palette
h_steps = 15
s_steps = 3
l_steps = 14
h_step_size = 1.0 / h_steps
s_step_size = 1.3 / s_steps

for h in range(0, h_steps):
    for s in range(0, s_steps):
        l_range = int(round(l_steps - (s*6/s_steps))) - 2
        l_step_size = 1.0 / l_range
        for l in range(1, l_range):
            hval = h * h_step_size
            sval = 1 - (s * s_step_size)
            lval = l * l_step_size
            
            r, g, b = colorsys.hls_to_rgb(hval, lval, sval)
            
            rval = int(round(r * 255))
            gval = int(round(g * 255))
            bval = int(round(b * 255))
            
            line = "%3s %3s %3s  #%02X%02X%02X" % (rval, gval, bval, rval, gval, bval)
            print line
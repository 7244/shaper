this example implements pseudogl and uses shaper with it to draw 2 square with different fragment shaders and positions

after compile run it with

`./a.exe | ffmpeg -y -f rawvideo -video_size 320x240 -framerate 30 -pixel_format rgb24 -i pipe: -filter_complex "split[s0][s1];[s0]palettegen[p];[s1][p]paletteuse" out.gif`

out.gif:

![out.gif](https://raw.githubusercontent.com/7244/rec0/main/sample/out.gif)

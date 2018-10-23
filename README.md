# AV Scissors
AV Scissors is a Qt-based video player whose timeline highlights the parts of the video that contain activity, like movement or noises. It's useful for e.g. reviewing pet surveillance videos.  Useful for pet surveillance, etc. Find out more in the readme file included with the binary distribution of AV Scissors, available from [Tarpeeksi Hyvae Soft's website](http://tarpeeksihyvaesoft.com/soft)

This repo contains the source code of AV Scissors; compilable on Linux (and probably some others). Note that the repo tracks my local one with low granularity only.

![A screenshot of AV Scissors](http://tarpeeksihyvaesoft.com/soft/img/avscissors.png)

## Building
To build on Linux, do ```qmake && make```, or load the .pro file in Qt Creator.

Qt of version 5.5 or higher should suffice.

##### OpenCV
AV Scissors uses the OpenCV library for certain functionality. For proper operation, you'll need to have OpenCV present and properly linked to.

##### Qt Multimedia
Your distro may not come pre-installed with the Qt Multimedia component required for AV Scissors. This lack will manifest in the program working fine expect for there being no video playback. If that's the case, look for and install ```qtmultimedia``` or the like with your package manager.

##### FFMPEG
As a shortcut, AV Scissors exports video audio into a separate WAV file via FFMPEG as an intermediate step. This means that your system must have FFMPEG available.

## A note
The program is currently in beta.

##### Todo
- [ ] ability for the user to alter program settings for audio/video activity detectors
- [ ] add timestamp to playback controls
- [ ] make sound processing not block
- [ ] playback position indicator icon shouldn't be clipped out of the window when on the far left
- [ ] reduce visual artefacts on video player (garbage at the edges, flicker, etc.)
- [ ] playback position is stored as u32, so might overflow for long videos
- [ ] save window size on exit and restore it on load
- [ ] test if qt multimedia is available
- [ ] for the messager, temporarily adjust the number of popups shown to account for the window height not being large enough
- [ ] messager close timer should depend on message length

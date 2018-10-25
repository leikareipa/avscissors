# AV Scissors
AV Scissors is a Qt-based video player whose timeline highlights the parts of the video that contain activity, like movement or noises. Useful for pet surveillance and so on. Find out more in the readme file included with the binary distribution of AV Scissors, available from [Tarpeeksi Hyvae Soft's website](http://tarpeeksihyvaesoft.com/soft)

This repo contains the source code of AV Scissors; it's compilable on Linux, and probabaly Windows, etc. Note that the repo tracks my local one with low granularity only.

![A screenshot of AV Scissors](http://tarpeeksihyvaesoft.com/soft/img/avscissors.png)

## Building
To build on Linux, do ```qmake && make```, or load the .pro file in Qt Creator.

Qt of version 5.5 or higher should suffice.

##### OpenCV
AV Scissors uses the OpenCV library for certain functionality. For proper operation, you'll need to have OpenCV present and properly linked to.

##### Qt Multimedia
Your distro may not come pre-installed with the Qt Multimedia component required for AV Scissors. This lack will manifest in the program working fine expect for there being no video playback. If that's the case, look for and install ```qtmultimedia``` or the like with your package manager.

##### FFMPEG
As a shortcut, AV Scissors exports the video's audio into a separate WAV file via FFMPEG as an intermediate step to processing audio activity. If your system doesn't have FFMPEG globally callable, audio will not be processed; although you can still use the program's other facilities.

## A note
The program is currently in beta. It may lack some usability features, and will make up for that in extra bugs.

##### Todo
- [ ] Ability for the user to alter the settings (thresholds, etc.) of the audio/video activity detectors.
- [ ] At some point, automatic detection of best settings for the activity detectors, based on multi-pass analysis or the like.
- [ ] Add a timestamp under the cursor in the playback controls.
- [ ] Sound processing shouldn't block.
- [ ] Eliminate dependency on FFMPEG.
- [ ] The playback position indicator icon shouldn't get clipped out of the window when it's on the far left.
- [ ] Reduce the video player's visual artefacts (garbage at the edges, flicker, etc.). May need to use something other than QVideoWidget.
- [ ] Video playback position is stored as u32, so might overflow on long videos.
- [ ] Save the window size on exit, and restore it on load.
- [ ] Have some kind of a test for whether Qt Multimedia is available on the user's system - seems it otherwise just fails silently and won't play video, even though the program still runs.
- [ ] For the messager, dynamically adjust the maximum number of popups shown based on window height.
- [ ] The messager's popup close timer should depend on message length.

##### A selection of known bugs
- [ ] If a messager popup comes up and then closes while the mouse cursor is outside of the program window, the playback icon  may freeze until you interact with it.
- [ ] If the video is paused while you move the window, the video player may temporarily disappear or flicker.
- [ ] The video player may display artefacts at the edges. This might be a QVideoWidget issue.
- [ ] For a few seconds after loading in a video that has no sound, the program will block on attempts to import another video.
- [ ] If you try to import two videos separately in quick succession, GUI execution will freeze until the external FFMPEG process has finished extracting the first video's audio.

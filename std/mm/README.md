# mm (Multi-Media library)

`mm` is kscript's multimedia library. It is used for reading, writing, and manipulating audio, images, videos, etc.

It is built off of `ffmpeg`'s libraries, `avformat`, `avcodec`, and `avutil`. This page explains many of the libraries: [https://trac.ffmpeg.org/wiki/Using%20libav*](https://trac.ffmpeg.org/wiki/Using%20libav*). Essentially, these `libav*` libraries help us read many formats using a single API. It also allows for reading containerized multimedia, such as Video+Audio files (such as movies).

## Building `ffmpeg`/`libav*`

You can build using `ffmpeg_build/` folder, with the `./build.sh` script. Otherwise, you can install (on ubuntu) the following:

First, use the updated ffmpeg PPA: `sudo add-apt-repository ppa:jonathonf/ffmpeg-4`

Now, run:

`sudo apt install --reinstall ffmpeg libavcodec-dev libavcodec-extra libavformat-dev libavutil-dev libflac-dev`

Without the newer PPA, there is a good chance the older versions of ffmpeg will not work

This will have: 

  * `.wav` audio files
  * `.mp3` audio files
  * `.ogg` audio files
  * `.flac` audio files
  * `.mp4` video files
  * `.mov` video files
  * ... and much more



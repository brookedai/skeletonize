# Skeletonize
Create topological skeletons from a monochrome png image.
#### Table of Contents
[Build and run](#build-and-run)<br/>
[Project report](#project-report)<br/>
[Examples](#examples)

## Build and run
This program can be run from the command line. <br/>

1. Clone the repository:
  ```
  git clone https://github.com/brookedai/skeletonize.git
  ```
2. Add the images that you would like to be skeletonized in the `./images/in` folder as black and white .png files.
3. Make a folder called `out` that is in the same directory level as `images` and `src`.
4. In `./src/main.cpp:main()`, add the new images in the `filesin` and `filesout`  vectors.
5. cd to the source directory and run make to compile:
  ```
  cd src
  make
  ```
6. Run with:
  ```
  ./skeletonize
  ```
  The output images should appear in the `./out` folder that you created.

## Project report
[Written report](https://github.com/brookedai/skeletonize/blob/master/resources/Brooke%20-%20Topological%20Skeletons.pdf)<br/>
[Presentation](https://learning.video.ubc.ca/media/t/0_1v0lb8rh)<br/>
[Presentation slides](https://github.com/brookedai/skeletonize/blob/master/resources/skeleslides.pdf)<br/>

## Examples
![Discord original](https://github.com/brookedai/skeletonize/blob/master/images/discord.png)
![Discord skeletonized](https://github.com/brookedai/skeletonize/blob/master/resources/images/discord_final.png)
![Discord stylized](https://github.com/brookedai/skeletonize/blob/master/resources/images/discord_stylized.png)

![Hans Solo skeletonized](https://github.com/brookedai/skeletonize/blob/master/resources/images/hansolo_candidate_skeleton.png)
![Hans Solo stylized](https://github.com/brookedai/skeletonize/blob/master/resources/images/hansolo_stylized.png)

![Rose skeletonized](https://github.com/brookedai/skeletonize/blob/master/resources/images/rose_candidate_skeleton.png)
![Rose stylized](https://github.com/brookedai/skeletonize/blob/master/resources/images/rose_stylized.png)

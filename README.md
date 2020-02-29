# The OpenAstroTracker:
![ ](https://i.imgur.com/qGr2bFf.jpg)

A cheap 3D printed tracking, GoTo and autoguiding mount for DSLR astrophotography.

Head over to [r/OpenAstroTech](https://www.reddit.com/r/OpenAstroTech/) for questions and stuff.

I you would be interested in buying the tracker as a printed kit, head over to [this site](https://openastrotech.com/products/)

## What parts to print:
**For latitude specific parts:**
All parts needed once, except:
 - part 05, needed 4 times
 - 20degree has its own version of part 01a, dont use the one from all lat.

**For all latitudes:**
All parts needed once, except:
 -  part 12, twice
 - part 15, twice


The different versions for latitudes work in a +/- 5° degree range. For the southern hemisphere check the first tab of the arduino code and flip the wiring for the RA stepper. Polar alignment will be much harder.
Also check out the first add-on, the [OpenAstroGuider](https://github.com/OpenAstroTech/OpenAstroGuider)

I strongly suggest starting with part 14, the lens ring. If you cant make it fit, you will not be able to use the mount at this point. 

## What to buy
[Shopping list](https://docs.google.com/spreadsheets/d/1L4PQiUul5nzP-nE1Q71Y6VLvNZevVYpP3fgTrcX0LEM/edit?usp=sharing) 

## Guides and how-to
[Arduino wiring](https://imgur.com/hBMxBpg)

[General Guide](https://drive.google.com/file/d/1_YVxujLwpIfMiU5c_vcGPQ0UwRyjkiqJ/view) for older version, needs revision

[Polar alignment](https://drive.google.com/file/d/1WRThR709xvfjo2IPQmQuXUPK6YBmoFnY/view)

Other guides, including a assembly guide will follow shortly.

**How to use the SCAD lens ring generator:**

You will need [OpenSCAD](https://www.openscad.org/downloads.html)

Take a look at an [example for my lens](https://imgur.com/Ql9mAmH).
 1. Start by roughly estimating the center of gravity of your lens with the camera attached.
 2. The coupler is going to be 30mm long, so you need measurements of 15mm in either direction from the center of gravity.
 3. Measure the first diameter. This is 67mm for me, so i put that into Diam1.
 4. The next diameter for my lens is the edge of the focus ring, so i put 65mm into Diam2.
 5. The distance between these points is High1; 9,5mm for me. First section done.
 6. The rest of my lens is flat, so i measure a diameter of 62mm and put it in Diam3 and Diam4.
 7. Because there are no more diameter change, I put High2 as the rest of the length of the coupler, which is 30mm - 9,5mm = 20,5. I leave the other values at 0.

All High values have to add up to 30mm.
If your lens is flat, put its diameter into Diam1 & Diam2 and put High1 to 30. Leave the rest at 0.
I recommend adding 0,5mm or more to the diameters as tolerance and definitely put some isolation tape (or similar) into the coupler to not scratch your lens. Also, **dont screw this too tight!** It will hold fine with loosely tightened screws.

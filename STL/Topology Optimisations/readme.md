## Load optimized infill (smart infill)
How to use:

This guide is using Cura 4.6 but it works for other slicers too. Google smart infill for your slicer.

 1. Select a .stl from the STL folder and its counterpart with the _topo ending from this folder.
 2. Open the non-topo file in Cura
 3. Right click the part and select "center selected model". Dont worry about the part not fitting the buildplate for now. Do not move it.
 4. Open the _topo file for the part. Repeat step 3 for it too. Both parts should perfectly overlap now. If not, repeat the centering, or if that doesnt work, carefully place them manually so that they lay on top of each other perfectly.
 5. On the bottom corner, in the object list, select both models (ctrl + a) ![enter image description here](https://i.imgur.com/eVH6OoU.png)
 6. Now you can use the move tools to move both models at the same time to fit your print bed. Double check that they are still on top of each other after moving.
 7. Select the _topo part only now.
 8. On the left side of Cura, find and select "per model settings". In this menu, select "modify settings for overlaps".
 9. Configure this menu as following:  
 
 ![enter image description here](https://i.imgur.com/nGp14kl.png)
 
 10. Its important to select "infill mesh only". You can edit the visible settings under "select settings". Find and select the 3 settings shown. It is important to use at least 1 wall and a higher infill density than your normal part. Test around with more settings and different infill patterns.

After slicing your part should look something like this:
![enter image description here](https://i.imgur.com/kUpVdgR.png)
The now densely printed areas are where a load simulation determined the highest stresses to be. To save print time and material, you can turn down the normal parts infill density and wall line count (over the usual settings on the right side) as those are not as important for the parts strength anymore.
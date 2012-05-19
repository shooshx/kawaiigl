feature list:
- edit a 3d model via a simple interpreted language
  - declare named points
  - add polygons between these points
  - right click the edit area ("Kawaii Script") for some ready made objects
- rotate the scene by draging the mouse with the left button pressed
- control the zoom using the mouse wheel
- move the points in 3d space by draging a point with right button pressed
- move a group of points by keeping CTRL pressed while right-draging and moving
- change the color of individual points using the "Color" dialog
  - change the model back to its original self by slightly changing the script in the 
    edit window (add a space for instance)
- show or hide points, polygons, lines.
- show or hide numerical coordinates and point label
  - change the font size
- show obscured points and coordinates by pressing ALT while in the 3D view
- show unused points (points that are not part of a polygon)
- show subdivision lines
- use scene lighing
- change light position by checking "Lighting->Side" and draging while right mouse button 
  is pressed.
- change light color using the "Color" dialog while "Side" is checked.
- change light equasion parameters using the "Ambient", "Diffuse", "Specular" sliders.
- use perspective view
- light backfaces - useful with self-intersecting volumes.
- use back face culling
- change the drag-with-left-button-pressed action by changing the combo box
  - translate (move) the object
  - scale it (bigger-smaller)
  - rotate around the Z axis (prepedicular to the screen)
- reset the view to its initial state by pressing the "0" button
- perform subdivision on the object 1 to 5 passes
  - for each pass, control if its a square or round pass
- use per-vertex normals for smoother look, works best with subdivision passes.
- see how many polygons are currently rendered in the blue number in the lower left corner.


Models language
torus(radius-outer, radius-inner, outer-slices, inner-slices)
sphereTri(radius, subdivision)
curveLine
curveRotate
wholeScreenQuad
arrow

sphereQuads - not



author:
Shy Shalom
shooshX@gmail.com
code available upon request.
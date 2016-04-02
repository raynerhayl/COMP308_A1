

---------------------------------------------------------------
Briefly detail what you have done (core, completion, challenge)
and explain how to run your program (arguments, controls, etc).
---------------------------------------------------------------



I set up the program following the unofficial windows guide on my home computer with no erros. I also set it up following the eclipse guid on one of the ecs computers. I ran the program with arguments: work/res/assets/teapot.obj for dragon.obj, sphere.obj ect

CORE: 

I've implemented the display list creation functions. It just adds vertexs and normals at the moment by accessing the corresponding vectors which store all the loaded 'triangles'. 

Not quite sure how the build list varies for the purpose of displaying wire or filled models, I just modified the glPolygonMode before the list is called in the renderGemoetry function. 

COMPLETION:

I think I got the per vertex normals working however the output still looks pretty granular. I wasn't actually able to get the per face normals working on their own as I struggled to implement it since the normals are stored per point. So setting every point on a triangle to the face normal would mean that the old values were getting overriden. Resulted in a weird patch work of dark and light as I would imagine would happen when trying to assign a face normal to each individual vertex.
I did try to modify the display list method so that it was adding normals according to a face normal list instead of reading off of the individual vector lists however this didn't seem to help at all (I thought it should however maybe it's something to do with adding a glNormal3f for every one glVertex3f instead of one for one like the display list is now when reading off of a vertex normal list).

CHALLENGE:

Rotation was tricky. Initially I set it up so that it just rotates according to the world view axis. This was done just with the arrow keys so that pressing left/right would rotate around y, each time the user presses the key it just adds a certain angle to the currently stored angle. Then in the render method I would call glRotatef with that angle for the corresponding axis. So the angle stored was the accumulated angle. 

When I implemented vertical rotation I began to see the limitation of rotating around each axis an amount equal to the accumulated angle stored (from the button pressess). Rotating moves the entire model axis and so after rotating vertically or horizontally and then doing the alternate rotation it would be rotating around the axis which have already themselves been rotated !

To avoid this I used matricies instead. It works by calculating a rotation matrix for the current "infintesimal rotation" due to the mouse movement between render calls. The accumulated rotation itself is stored as a matrix which is multiplied by the current rotation matrix each render cycle. Then this matrix is passed to glMultMatrix to actually transform the image. I would've thought that first I would have to call glLoadIdentity or something so that the matrix transform is reset however this results in odd effects. I guess the matrix transforms are reset after the rendering is done? However this would not be consistant with having strange effects when loading the matrix (surely loading the identity when the matrix transforms are reset would have no effect?). So I'm led to beleive that the accumulated Matrix is actually maintained in the "matrix stack" each render cycle which means that it is being multiplied by the updated accumulated Matrix which isn't consistent with how I designed the implementation ( the accumulated Matrix should represent all rotations to this point) however it still works as intended.

If it is the case that the previous accumulated Matrix which stays in the stack is multiplied by the updated one each render cycle than perhaps my understanding of this implementaion should be as follows: The previous accumulated rotation represents the rotation done to the model axis and the updated one represents how we want to rotate the axis (while maintaining their current rotation). So multiplying the previous by the current is in effect rotating the already rotated model axis but relative to the cameras axis.
# val3dity

Validation of solids according to the international standard ISO 19107.


## How do I use val3dity?

It is a command-line program, which we provide as source code, together with makefiles for Mac and Linux. 

If you use Windows, you can download the following [binary](https://www.dropbox.com/s/aywlwz49ngeh80l/3dValidator1.10.zip). It has the latest val3dity, and [citygml2poly](https://github.com/tudelft-gist/citygml2poly) is used to parse CityGML files. Java and Python must be installed on the system.

To compile val3dity yourself, you first need to install the free libraries [CGAL](http://www.cgal.org), [GEOS](http://trac.osgeo.org/geos/) and [CMake](http://www.cmake.org)--under Mac we suggest using [Homebrew](http://brew.sh/). Afterwards run:

    $ cmake .
    $ make
    
To execute val3dity:

    $ ./val3dity outershell.poly innershell1.poly innershell2.poly
    
Each shell of the solid must be modelled with a [POLY](http://tetgen.berlios.de/fformats.poly.html) file, the first argument is always the outer shell, and the others are inner shells. If you want to read CityGML files as input, have a look at [citygml2poly](https://github.com/tudelft-gist/citygml2poly).

There can be 0 or an infinity of inner shells, their order is not important.
For instance, to validate a solid having only one outer shell (it's a unit cube):

```  
$ ./val3dity data/poly/cube.poly
Reading 1 file(s).
Reading outer shell:	data/poly/cube.poly

Validating surface in 2D with GEOS (their projection)
-----all valid

Triangulating outer shell
-----done

Validating shell #0
-----Planarity
	yes
-----Combinatorial consistency
	yes
-----Geometrical consistency
	yes
-----Orientation of normals
	yes
Shell #0 valid

Valid solid. Hourrraaa!
```

And to validate a solid with one outer shell and 2 inner shells:

```
$ ./val3dity data/poly/cube.poly data/poly/py1.poly data/poly/py7.poly
Reading 3 file(s).
Reading outer shell:	data/poly/cube.poly
Reading inner shell #0:	data/poly/py1.poly
Reading inner shell #1:	data/poly/py7.poly

Validating surface in 2D with GEOS (their projection)
-----all valid

Triangulating outer shell
Triangulating inner shell #0
Triangulating inner shell #1
-----done

Validating shell #0
-----Planarity
	yes
-----Combinatorial consistency
	yes
-----Geometrical consistency
	yes
-----Orientation of normals
	yes
Shell #0 valid

Validating shell #1
-----Planarity
	yes
-----Combinatorial consistency
	yes
-----Geometrical consistency
	yes
-----Orientation of normals
	yes
Shell #1 valid

Validating shell #2
-----Planarity
	yes
-----Combinatorial consistency
	yes
-----Geometrical consistency
	yes
-----Orientation of normals
	yes
Shell #2 valid

Inspecting interactions between the 3 shells
Error 400: shells are face adjacent
	[shell: #1; face: #2]

Invalid solid :(
```


# Error reported 

## Ring level ##

  * REPEATED_POINTS   
  * RING_NOT_CLOSED   
  * RING_SELF_INTERSECT

## Surface level ##

  * SELF_INTERSECTION  
  * NON_PLANAR_SURFACE                     
  * INTERIOR_DISCONNECTED
  * HOLE_OUTSIDE
  * HOLES_ARE_NESTED                      

## Shell level ##

  * NOT_VALID_2_MANIFOLD
    * SURFACE_NOT_CLOSED                     
    * DANGLING_FACES                         
    * FACE_ORIENTATION_INCORRECT_EDGE_USAGE  
    * FREE_FACES                             
    * SURFACE_SELF_INTERSECTS                
  * VERTICES_NOT_USED                      
  * SURFACE_NORMALS_WRONG_ORIENTATION      

## Solid level

  * SHELLS_FACE_ADJACENT                   
  * SHELL_INTERIOR_INTERSECT               
  * INNER_SHELL_OUTSIDE_OUTER              
  * INTERIOR_OF_SHELL_NOT_CONNECTED        
  


## I'm interested in the details of how the validation is performed ##

The validation is performed hierarchically, ie first every surface are validated in 2D (with [GEOS](http://trac.osgeo.org/geos/)), then every shell is validated (must be watertight, no self-intersections, orientation of the normals must be consistent and pointing outwards, etc.), and finally the interactions between the shells are analysed.

Most details about the implementation are available in this [scientific article](http://homepage.tudelft.nl/23t4p/pdfs/_13cacaie.pdf).

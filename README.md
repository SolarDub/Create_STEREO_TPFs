# Create_STEREO_TPFs
 
This application allows a user to create a FITS-format file containing a set of 20x20 pixel images (Target Pixel Files, or TPFs) for a selected star, given its celestial coordinates, from a series of images by the STEREO-A HI-1 (HI-1A) camera as that star passes across the camera's field-of-view. The saved FITS file may be subsequently used to perform stellar photometry for the creation of lightcurve time-series.

This is and ongoing side-project, and this repository will be updated with content such as libraries (namely CFITSIO) required for the execution of the application, links to a database containing the STEREO image FITS files, a link to a script that one may use to acquire these files, and instructions on how to execute the program and produce the TPFs.

A simple application of this project is illustrated in the following video. The movie tracks 12 Sgr, a Cepheid variable star, across a series of 2008 HI-1A images, displayed at the bottom-right of the window. The star is enclosed by a green box. The bottom-left panel shows the 20x20 pixel images of the star produced by this application. Simple Aperture Photometry is performed on each pixel image, which determines the stellar flux at each 40-minute time-step and displayed as a blue point in a photometric time-series, or lightcurve, in the top panel.

By the way - can you guess the three planets that appear in the HI-1A panel at the start of the movie? They are the objects with blooming across the vertical pixels of the CCD image. To the far-right is Saturn, which is lost early on. Near the center and moving right is Mars. The object that is near stationary, as it has nearly the same orbital period as STEREO, is... Earth!

----

https://user-images.githubusercontent.com/81772405/194195006-4beaf50d-90a0-4f55-9e8d-84f1afd0acdf.mp4


#include <stdio.h>
#include "cv.h"
#include "highgui.h"

#define CV_LOAD_IMAGE_COLOR       0
#define CV_LOAD_IMAGE_GRAYSCALE   1
#define CV_LOAD_IMAGE_UNCHANGED  -1


int main( int argc, char** argv )
{
/* data structure for the image */
IplImage *img = 0;

/* check for supplied argument */
if( argc < 2 ) {
    fprintf( stderr, "Usage: loadimg <filename>\n" );
    return 1;
}

/* load the image,
use CV_LOAD_IMAGE_GRAYSCALE to load the image in grayscale */
img = cvLoadImage( argv[1], CV_LOAD_IMAGE_COLOR );

/* always check */
if( img == 0 ) {
    fprintf( stderr, "Cannot load file %s!\n", argv[1] );
    return 1;
}
         
/* create a window */
cvNamedWindow( "image", CV_WINDOW_AUTOSIZE );
         
/* display the image */
cvShowImage( "image", img );
         
/* wait until user press a key */
cvWaitKey(0);
         
/* free memory */
cvDestroyWindow( "image" );
cvReleaseImage( &img );

return 0;
}

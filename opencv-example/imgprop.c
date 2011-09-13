      /* load the image */
      IplImage *img = cvLoadImage( argv[1], CV_LOAD_IMAGE_COLOR );
       
      /* print some properties */
      printf( "Filename:    %s\n",        argv[1] );
      printf( "# channels:  %d\n",        img->nChannels );
      printf( "Pixel depth: %d bits\n",   img->depth );
      printf( "width:       %d pixels\n", img->width );
      printf( "height:      %d pixels\n", img->height );
      printf( "Image size:  %d bytes\n",  img->imageSize );
      printf( "Width step:  %d bytes\n",  img->widthStep );

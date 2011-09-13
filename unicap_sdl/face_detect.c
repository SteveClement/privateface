#include <cv.h>
#include <unicap.h>
#include <ucil.h>
#include <stdio.h>
#include <SDL.h>
#include <SDL_image.h>

//#define DEBUG 
//#define FULLSCREEN 
//#define DELAY
//#define SPEED

// HP Laptop Cam
//#define ID "HP Webcam [2 MP Macro] (/dev/video0)" 

// Logitech
//#define ID "UVC Camera (046d:09a6) (/dev/video1)"

// Logitech
//#define ID "UVC Camera (046d:0990) (/dev/video1)" 

// X200
#define ID "UVC Camera (17ef:480c) (/dev/video0)" 

// X300
//#define ID "UVC Camera (17ef:4807) (/dev/video0)" 

// Define the Offset area for the Face Scanner
#define SCAN_OFFSET 0 

#define HEIGHT 640
#define WIDTH 480

#define CASCADE_NAME "../data/haarcascade_frontalface_alt.xml"

static volatile int quit = 0;

void DrawIMG(SDL_Surface *img, SDL_Surface *dst, int x, int y);

SDL_Surface* movieSurface;
SDL_Surface* screen;
SDL_Surface* logo;
SDL_Event sdl_event;

struct caminfo
{
      unicap_handle_t    handle;
      char              *device_identifier;
      unsigned int       fourcc;
      unicap_rect_t      format_size;
      char              *window;
      unicap_data_buffer_t buffer;
      IplImage          *image;
      int                frame_count;
      unicap_property_t *properties;
      int                property_count;
      CvHaarClassifierCascade *cascade;
      CvMemStorage            *storage;
};

//
// Define arrays of properties which should be set during
// initialisation here. The information could obtained from the
// "device_info" example. 
//
static unicap_property_t camera0_properties[] =
{
/*    { */
/*       identifier:      "Brightness",  */
/*       relations_count: 0, */
/*       { value:         16000.0 },  */
/*       flags:           UNICAP_FLAGS_MANUAL, */
/*       flags_mask:      UNICAP_FLAGS_MANUAL, */
/*    }, */
};

static unicap_property_t camera1_properties[] =
{
   {
      identifier:      "focus", 
      relations_count: 0,
      { value:         600.0 }, 
      flags:           UNICAP_FLAGS_MANUAL,
      flags_mask:      UNICAP_FLAGS_MANUAL,
   },
};

//
// Define the cameras that should be opened here. The information
// could be obtained from the "device_info" example
//
static struct caminfo cameras[] = 
{
   {
	handle:            NULL,
	device_identifier: ID,
	fourcc:            UCIL_FOURCC( 'Y', 'U', 'Y', 'V' ), 
	format_size:       { 0, 0, HEIGHT, WIDTH }, 
	window:            "Camera",
	image:             NULL,
	properties:        camera0_properties,
	property_count:    sizeof( camera0_properties ) / sizeof( unicap_property_t ),

	cascade: NULL,
	storage: NULL,
   },
};
      
//
// Define the FourCC for the target buffer here. 
// For example: 
// Y800 for monochrome 8 bit images
// RGB3 for 24 bit RGB images
//
#define TARGET_FOURCC ( UCIL_FOURCC( 'R', 'G', 'B', '3' ) )

//
// Define the BitsPerPixel for the target buffer here
// For example: 
// 8 for 8 bit monochrome images
// 24 for 24 bit RGB images
//
#define TARGET_BPP ( 24 )


void detect_and_draw( IplImage* img, CvHaarClassifierCascade *cascade, CvMemStorage *storage )
{

    CvPoint pt1, pt2;

    double scale = 2;

#ifdef DELAY
// Allocate framecopy as the same size of the frame
IplImage* temp_img = NULL;
if( !temp_img )
     	temp_img = cvCreateImage( cvSize(img->width,img->height),
                                    8, 1 );
#endif

	// to be removed on switch 2 sdl, opencv uses BGR and we've specified RGB as target_fourcc <- Hmmm I do not think this comment is correct, comment next line to see why
	cvCvtColor(img,img,CV_RGB2BGR);

#ifdef DELAY      
// Check the origin of image. If top left, copy the image frame to frame_copy. 
if( img->origin == IPL_ORIGIN_TL )
	cvCopy( img, temp_img, 0 );
// Else flip and copy the image
else
cvFlip( img, temp_img, 0 );
IplImage* temp_img = cvCreateImage( cvSize( img->width, img->height, 8 ,1 ) );	
#else
IplImage* small_img = cvCreateImage( cvSize( cvRound (img->width/scale),
			 cvRound ((img->height-SCAN_OFFSET)/scale)),
			 8, 3 );
#endif


#ifdef DEBUG
double t = (double)cvGetTickCount();
#endif

    cvResize( img, small_img, CV_INTER_LINEAR );

#ifdef SPEED
cvResize( img, small_img, CV_INTER_LINEAR );
// Commented out due to Crash, see Redmine Ticket 364 (https://source.hacker.lu/issues/364)
//cvEqualizeHist( img, img );

cvPyrDown( img, small_img, CV_GAUSSIAN_5x5 );

// Below Lines are used for the DELAY version
//cvResize( temp_img, small_img, CV_INTER_LINEAR );
//cvEqualizeHist( temp_img, temp_img );
//cvPyrDown( temp_img, small_img, CV_GAUSSIAN_5x5 );
#endif

    cvClearMemStorage( storage );

    if( cascade )
    {
#ifdef DELAY
CvSeq* faces = cvHaarDetectObjects( temp_img, cascade, storage,
#ifdef SPEED
                                            1.2, 2, CV_HAAR_DO_CANNY_PRUNING,
                                            cvSize(40, 40) );
#else
                                            1.1, 3, CV_HAAR_DO_CANNY_PRUNING,
                                            cvSize(20, 20) );
#endif
#else
        CvSeq* faces = cvHaarDetectObjects( small_img, cascade, storage,
#ifdef SPEED
                                            1.2, 2, CV_HAAR_DO_CANNY_PRUNING,
                                            cvSize(40, 40) );
#else
                                            1.1, 3, CV_HAAR_DO_CANNY_PRUNING,
                                            cvSize(20, 20) );
#endif
#endif

#ifdef DEBUG
t = (double)cvGetTickCount() - t;
printf( "detection time = %gms\n", t/((double)cvGetTickFrequency()*1000.) );
#endif

    int i;
        for( i = 0; i < (faces ? faces->total : 0); i++ )
        {
            CvRect* r = (CvRect*)cvGetSeqElem( faces, i );

            // Find the dimensions of the face,and scale it if necessary
            pt1.x = r->x*scale-1;
            pt2.x = (r->x+r->width)*scale-1;
            pt1.y = r->y*scale*1.1;
            pt2.y = (r->y+r->height*0.6)*scale-1;

            cvRectangle( img, pt1, pt2, CV_RGB(0,0,0), -3, 8, 0 );

        }
    }

#ifndef DELAY
cvReleaseImage( &small_img );
#else
cvReleaseImage( &temp_img );
#endif
}

//
// Implement your image processing function in this callback. 
//
// Currently this callback will only display an image through CV. 
// cvWaitKey( 5 ) is required to get the window updated. 
// The program will terminate when the callbacks for all cameras will
// have set their 'quit' condition.
//
static void new_frame_cb( unicap_event_t event, unicap_handle_t handle, unicap_data_buffer_t *buffer, struct caminfo *camera )
{
	ucil_convert_buffer( &camera->buffer, buffer );
	detect_and_draw( camera->image, camera->cascade, camera->storage );

        movieSurface = SDL_CreateRGBSurfaceFrom((void*)camera->image->imageData,
                camera->image->width,
                camera->image->height,
                camera->image->depth*camera->image->nChannels,
                camera->image->widthStep,
                0xff0000, 0x00ff00, 0x0000ff, 0
            );

        DrawIMG(movieSurface, screen, 0, 0);//*/
        SDL_Flip( screen );

        // SDL_PollEvent steps forward in the internal "main" loop of SDL, so we poll it here
        SDL_PollEvent( &sdl_event );
	if (sdl_event.key.keysym.sym == SDLK_ESCAPE) {
                quit++;
	}

#ifdef DELAY
static short init = 0;
static int i = 0;
static IplImage* images[100];

if( i > 99 ) {
  init = 1;
  i = 0;
}

ucil_convert_buffer( &camera->buffer, buffer );

if( !init ) {
  images[i] = cvCreateImage( cvSize( camera->image->width,
                                     camera->image->height-SCAN_OFFSET), IPL_DEPTH_8U, 3 );
}

if(init) {
   detect_and_draw( images[i], camera->cascade, camera->storage );
   cvShowImage( camera->window, images[i] );
}

cvCopy(camera->image,images[i],0);	
	
i++;
#endif

	if( cvWaitKey( 5 ) == 'q' )
	{
	   quit++;
	}
  
}



int main( int argc, char **argv )
{
   int dev_count = sizeof( cameras ) / sizeof( struct caminfo );
   int res = 0;
   int i; 

        if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
        {
                printf("Unable to init SDL: %s\n", SDL_GetError());
                return -1;
        }
        atexit(SDL_Quit);

#ifdef FULLSCREEN
screen = SDL_SetVideoMode( HEIGHT, WIDTH, 32, SDL_DOUBLEBUF|SDL_HWSURFACE|SDL_FULLSCREEN);
#else
screen = SDL_SetVideoMode( HEIGHT, WIDTH, 32, SDL_DOUBLEBUF|SDL_HWSURFACE);
#endif

        if ( screen == NULL )
        {
                printf("Unable to set %dx%d video: %s\n", HEIGHT, WIDTH, SDL_GetError());
                return -1;
        }

	SDL_Surface* temp;
	if(( temp = IMG_Load("logo.png")) == NULL ){
		printf("Logo could not be loaded!: %s\n", SDL_GetError() );
		return -1;
	}
		
	logo = SDL_DisplayFormatAlpha(temp);	

	SDL_FreeSurface(temp);

	movieSurface = SDL_CreateRGBSurface(
                                        SDL_SWSURFACE,
                                        HEIGHT,
                                        WIDTH,
                                        32,
                                        screen->format->Rmask,
                                        screen->format->Gmask,
                                        screen->format->Bmask,
                                        screen->format->Amask );

   for( i = 0; i < dev_count; i++ )
   {
      unicap_device_t device, device_spec;
      unicap_format_t format, format_spec;
      int j;
      
      unicap_void_device( &device_spec );
      strcpy( device_spec.identifier, cameras[i].device_identifier );
      if( !SUCCESS( unicap_enumerate_devices( &device_spec, &device, 0 ) ) )
      {
	 fprintf( stderr, "Could not find device: %s\n", device_spec.identifier );
	 exit( 1 );
      }
      
      if( !SUCCESS( unicap_open( &cameras[i].handle, &device ) ) )
      {
	 fprintf( stderr, "Failed to open device: %s\n", device.identifier );
	 exit( 1 );
      }
      
      unicap_void_format( &format_spec );
      format_spec.fourcc = cameras[i].fourcc;
      format_spec.size.width = cameras[i].format_size.width;
      format_spec.size.height = cameras[i].format_size.height;

      if( !SUCCESS( unicap_enumerate_formats( cameras[i].handle, &format_spec, &format, 0 ) ) )
      {
	 fprintf( stderr, "Could not find format! \n" );
	 exit( 1 );
     }

	unicap_enumerate_formats( cameras[i].handle, &format_spec, &format, 0 );
      
      format.buffer_type = UNICAP_BUFFER_TYPE_SYSTEM;
      format.size.width = cameras[i].format_size.width;
      format.size.height = cameras[i].format_size.height;      

      if( !SUCCESS( unicap_set_format( cameras[i].handle, &format ) ) )
      {
	 fprintf( stderr, "Failed to set format: %s \n", format.identifier );
	 exit( 1 );
      }

      // Read back format
      if( !SUCCESS( unicap_get_format( cameras[i].handle, &format ) ) )
      {
	 fprintf( stderr, "Failed to get format\n" );
	 exit( 1 );
      }

      unicap_copy_format( &cameras[i].buffer.format, &format );
      cameras[i].buffer.format.fourcc = TARGET_FOURCC; 
      cameras[i].buffer.format.bpp = TARGET_BPP;
      cameras[i].buffer.format.buffer_size = cameras[i].buffer.buffer_size = 
	 cameras[i].buffer.format.size.width * cameras[i].buffer.format.size.height * cameras[i].buffer.format.bpp / 8;
      cameras[i].buffer.data = malloc( cameras[i].buffer.format.buffer_size );
      cameras[i].image = cvCreateImage( cvSize( format.size.width, format.size.height ), 8, cameras[i].buffer.format.bpp / 8 );
      cameras[i].image->imageData = cameras[i].buffer.data;
      unicap_register_callback( cameras[i].handle, UNICAP_EVENT_NEW_FRAME, (unicap_callback_t) new_frame_cb, (void*)&cameras[i] );


      for( j = 0; j < cameras[i].property_count; j++ )
      {
	 unicap_property_t property, property_spec;
	 unicap_void_property( &property_spec );
	 strcpy( property_spec.identifier, cameras[i].properties[j].identifier );
	 unicap_enumerate_properties( cameras[i].handle, &property_spec, &property, 0 );

	 property.flags = cameras[i].properties[j].flags;
	 property.flags_mask = cameras[i].properties[j].flags_mask;
	 
	 switch( property.type )
	 {
	    case UNICAP_PROPERTY_TYPE_RANGE:
	    case UNICAP_PROPERTY_TYPE_VALUE_LIST:
	       property.value = cameras[i].properties[j].value;
	       break;
	       
	    case UNICAP_PROPERTY_TYPE_MENU:
	       strcpy( property.menu_item, cameras[i].properties[j].menu_item );
	       break;
	 }

	 unicap_set_property( cameras[i].handle, &property );
      }


      cameras[i].cascade = (CvHaarClassifierCascade*)cvLoad( CASCADE_NAME, 0, 0, 0 );
      if( !cameras[i].cascade )
      {
	 fprintf( stderr, "Failed to load cascade!\n" );
	 exit( 1 );
      }

      cameras[i].storage = cvCreateMemStorage(0);
    
      if( !SUCCESS( unicap_start_capture( cameras[i].handle ) ) )
      {
	 fprintf( stderr, "Failed to start capture!\n" );
	 exit( 1 );
      }
   }

   // While this loop runs, the callbacks for each cameras will be
   // called in their own threads
   while( quit < dev_count )
   {
      usleep( 10000 );
   }

   for( i = 0; i < dev_count; i++ )
   {
      unicap_stop_capture( cameras[i].handle );
      unicap_close( cameras[i].handle );
   }
   
	// clean up after ourselves
	SDL_FreeSurface( movieSurface );
	SDL_FreeSurface( screen );
	SDL_FreeSurface( logo );

	SDL_Quit();

   return( 0 );
}

void DrawIMG(SDL_Surface *img, SDL_Surface *dst, int x, int y)
{
        SDL_Rect dest;
        dest.x = x;
        dest.y = y;
	
	SDL_Rect logodest;
	logodest.x = 0;
	logodest.y= WIDTH - 40;
	SDL_BlitSurface(img, NULL, dst, &dest);
	SDL_BlitSurface(logo,NULL,dst,&logodest);
}

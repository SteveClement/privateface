/*
** cam_display.c
** 
** Made by (Arne Caspari)
** Login   <arne@localhost>
** 
** Started on  Fri Oct 12 11:09:49 2007 Arne Caspari
*/

#include <cv.h>
#include <unicap.h>
#include <ucil.h>
#include <stdio.h>
#include <glib.h>

static volatile int quit = 0;

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


//
// Define the cameras that should be opened here. The information
// could be obtained from the "device_info" example
//
#error "Please define your video capture device here and remove this message!!!!"
static struct caminfo cameras[] = 
{
   {
      handle:            NULL,
      device_identifier: "Built-in iSight (0000:00:1d.7)", 
      fourcc:            UCIL_FOURCC( 'U', 'Y', 'V', 'Y' ), 
      format_size:       { 0, 0, 640, 480 }, 
      window:            "Camera 1",
      image:             NULL,
      frame_count:       90,
      properties:        camera0_properties,
      property_count:    sizeof( camera0_properties ) / sizeof( unicap_property_t ),
   },
};
      
//
// Define the FourCC for the target buffer here. 
// For example: 
// Y800 for monochrome 8 bit images
// RGB3 for 24 bit RGB images
//
#define TARGET_FOURCC ( UCIL_FOURCC( 'Y', '8', '0', '0' ) )

//
// Define the BitsPerPixel for the target buffer here
// For example: 
// 8 for 8 bit monochrome images
// 24 for 24 bit RGB images
//
#define TARGET_BPP ( 8 )


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
   cvNamedWindow( camera->window, 1 );
   cvShowImage( camera->window, camera->image );

   cvWaitKey( 5 );
   
   camera->frame_count--;
   
   if( camera->frame_count == 0 )
   {
      // Caution: This is not a clean solution since 'quit++' is not
      // atomic and the callback for each camera gets called in their
      // own thread 
      quit++;
   }
}

//
//
//
int main( int argc, char **argv )
{
   int dev_count = sizeof( cameras ) / sizeof( struct caminfo );
   int res = 0;
   int i; 

   //
   // Important: You need to call g_thread_init since OpenCVs HighGUI
   // uses gtk/glib and we are calling it from a threaded environment
   //
   g_thread_init( NULL );

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
      
      format.buffer_type = UNICAP_BUFFER_TYPE_SYSTEM;

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
   
   cvDestroyAllWindows();

   return( 0 );
}


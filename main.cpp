#include <QApplication>
#include <QDesktopWidget>
#include "matrixwidget.h"
#include "libfreenect.h"

#include <set>
#include "boost/tuple/tuple.hpp"
#include "boost/tuple/tuple_comparison.hpp"

#include "window.h"
#include <math.h>




//! Entry point for the app
/*!
    Creates the window and shows it.
*/
    
int die = 0;
freenect_context *f_ctx;
freenect_device *f_dev;
uint8_t *depth_mid;
std::set< coord > onCoords;
pthread_t freenect_thread;
pthread_mutex_t depth_mutex = PTHREAD_MUTEX_INITIALIZER;

float rawDepthToMeters(int depthValue)
{
    if (depthValue < 2047)
    {
        return (float)1.0 / (double)depthValue * -0.0030711016 + 3.3309495161;
    }
    return 0.0f;
}

int16_t t_gamma[2048];
    void depth_cb(freenect_device *dev, void *v_depth, uint32_t timestamp)
    {
        int i;
        uint16_t *depth = (uint16_t*)v_depth;

        //pthread_mutex_lock(&gl_backbuf_mutex);
        /*for (i=0; i<640*480; i++) {
            int pval = t_gamma[depth[i]];
            int lb = pval & 0xff;

	        switch (pval>>8) {
	            case 0:
	                depth_mid[3*i+0] = 255;
	                depth_mid[3*i+1] = 255-lb;
	                depth_mid[3*i+2] = 255-lb;
	                break;
	            case 1:
	                depth_mid[3*i+0] = 255;
	                depth_mid[3*i+1] = lb;
	                depth_mid[3*i+2] = 0;
	                break;
	            case 2:
	                depth_mid[3*i+0] = 255-lb;
	                depth_mid[3*i+1] = 255;
	                depth_mid[3*i+2] = 0;
	                break;
	            case 3:
	                depth_mid[3*i+0] = 0;
	                depth_mid[3*i+1] = 255;
	                depth_mid[3*i+2] = lb;
	                break;
	            case 4:
	                depth_mid[3*i+0] = 0;
	                depth_mid[3*i+1] = 255-lb;
	                depth_mid[3*i+2] = 255;
	                break;
	            case 5:
	                depth_mid[3*i+0] = 0;
	                depth_mid[3*i+1] = 0;
	                depth_mid[3*i+2] = 255-lb;
	                break;
	            default:
	                depth_mid[3*i+0] = 0;
	                depth_mid[3*i+1] = 0;
	                depth_mid[3*i+2] = 0;
	                break;
	            }
        }*/
        pthread_mutex_lock(&depth_mutex);
        onCoords.clear();
        int x, y, z, zd;
        x = y = z = 50;

        //printf("===========\n");
        for (int i = 0; i < x; i++){
        	for (int j = 0; j < y; j++) {
        		//printf("i: %d,j: %d.", i, j);
        		//printf("%d\t",j*(480/y) + 640*i*(640/x));
        		//printf("%d\t", depth[j*(480/x) + 640*i] < 900 ? 1 : 0);
                zd = depth[j*(480/y) + 640*i*(640/x)];
        		//printf("%d\t", zd);
                onCoords.insert(boost::make_tuple(j,i,zd*50/3500));

        	}
        	//printf("\n");
        }
        pthread_mutex_unlock(&depth_mutex);
        	
        /*int zd = depth[0];
        if (zd > 3500) zd = 0;
        zd = zd*50/3500;
        onCoords.insert(boost::make_tuple(0,0,zd));*/
        //got_depth++;
        //pthread_cond_signal(&gl_frame_cond);
        //pthread_mutex_unlock(&gl_backbuf_mutex);
    }

static void *freenect_threadfunc(void *arg)
    {
        int accelCount = 0;

        freenect_set_depth_callback(f_dev, depth_cb);
        freenect_set_depth_mode(f_dev, freenect_find_depth_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_DEPTH_MM));

        freenect_start_depth(f_dev);


        while (!die && freenect_process_events(f_ctx) >= 0) {
        }

        printf("\nshutting down streams...\n");

        freenect_stop_depth(f_dev);

        freenect_close_device(f_dev);
        freenect_shutdown(f_ctx);

        printf("-- done!\n");
        return NULL;
    }


    int main(int argc, char *argv[])
    {
    	depth_mid = (uint8_t*)malloc(640*480*3);
        printf("Kinect camera test\n");


		for (int i=0; i<2048; i++) {
			float v = i/2048.0;
			v = powf(v, 3)* 6;
			t_gamma[i] = v*6*256;
		}

        if (freenect_init(&f_ctx, NULL) < 0) {
            printf("freenect_init() failed\n");
            return 1;
        }

        freenect_set_log_level(f_ctx, FREENECT_LOG_DEBUG);
        freenect_select_subdevices(f_ctx, (freenect_device_flags)(FREENECT_DEVICE_CAMERA));

        int nr_devices = freenect_num_devices (f_ctx);
        printf ("Number of devices found: %d\n", nr_devices);

        int user_device_number = 0;
        if (argc > 1)
            user_device_number = atoi(argv[1]);

        printf("user_device_number: %i\n", user_device_number);
        if (nr_devices < 1) {
            f_ctx = NULL;
            f_dev = NULL;
        } else {

	        printf("Device found!\n");

	        if (freenect_open_device(f_ctx, &f_dev, 0) < 0) {
	            printf("Could not open device\n");
	            freenect_shutdown(f_ctx);
	            return 1;
	        }
	        printf("pthread_create\n");
	        int res = pthread_create(&freenect_thread, NULL, freenect_threadfunc, NULL);
	        if (res) {
	            printf("pthread_create failed\n");
	            freenect_shutdown(f_ctx);
	            return 1;
	        }
    	}
        /* else {

        }*/
        //onCoords.insert(boost::make_tuple(1,2,3));
/*
        boost::tuple<int, int, int> c = boost::make_tuple(1,2,3);
        std::set< boost::tuple<int,int,int> > myset;
       	myset.insert(c);
        if (myset.find(boost::make_tuple(1,2,3)) != myset.end()) {
        	printf("yeeeaaaaah!\n");
        } else {
        	printf("nooooaoaaoaoooo\n");
        }
*/
        QApplication app(argc, argv);
        Window window(f_dev, f_ctx);
        window.resize(window.sizeHint());

        int desktopArea = QApplication::desktop()->width() *
        QApplication::desktop()->height();
        int widgetArea = window.width() * window.height();
        if (((float)widgetArea / (float)desktopArea) < 0.75f)
            window.show();
        else
            window.showMaximized();
        return app.exec();
    }

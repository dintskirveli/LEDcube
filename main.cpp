#include <QApplication>
#include <QDesktopWidget>
#include "matrixwidget.h"
#include "libfreenect.h"

#include "window.h"

//! Entry point for the app
/*!
    Creates the window and shows it.
*/
    int main(int argc, char *argv[])
    {
        freenect_context *f_ctx;
        freenect_device *f_dev;

        QApplication app(argc, argv);
        Window window;
        window.resize(window.sizeHint());

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

        if (nr_devices < 1) {
            freenect_shutdown(f_ctx);
            return 1;
        }

        int desktopArea = QApplication::desktop()->width() *
        QApplication::desktop()->height();
        int widgetArea = window.width() * window.height();
        if (((float)widgetArea / (float)desktopArea) < 0.75f)
            window.show();
        else
            window.showMaximized();
        return app.exec();
    }

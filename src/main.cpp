#include <mainwindow.h>

#include <QApplication>
#include <QSurfaceFormat>
#include <QDebug>

#include <QLabel>

#include "scene/scene.h"

void debugFormatVersion()
{
    QSurfaceFormat form = QSurfaceFormat::defaultFormat();
    QSurfaceFormat::OpenGLContextProfile prof = form.profile();

    const char *profile =
        prof == QSurfaceFormat::CoreProfile ? "Core" :
        prof == QSurfaceFormat::CompatibilityProfile ? "Compatibility" :
        "None";

    printf("Requested format:\n");
    printf("  Version: %d.%d\n", form.majorVersion(), form.minorVersion());
    printf("  Profile: %s\n", profile);
}

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    // Set OpenGL 3.2 and, optionally, 4-sample multisampling
    QSurfaceFormat format;
    format.setVersion(3, 2);
    format.setOption(QSurfaceFormat::DeprecatedFunctions, false);
    format.setProfile(QSurfaceFormat::CoreProfile);
    //format.setSamples(4);  // Uncomment for nice antialiasing. Not always supported.

    /*** AUTOMATIC TESTING: DO NOT MODIFY ***/
    /*** Check whether automatic testing is enabled */
    /***/ if (qgetenv("CIS277_AUTOTESTING") != nullptr) format.setSamples(0);

    QSurfaceFormat::setDefaultFormat(format);
    debugFormatVersion();

    MainWindow w;
    w.show();




//-------------------------------------- draw result of perlin noise -------------------------------------------
    QImage myImage(512, 512, QImage::Format_RGB32);
    QRgb value = qRgb(0, 0, 0);
    myImage.fill(value);

    for(int y = 0; y < 500; y++)
    {
        for(int x = 0; x < 500; x++)
        {
            float output = 255 * Scene::perlinNoise2D((float)x, (float)y);
            value = qRgb(output, output, output);
            myImage.setPixel(x, y, value);
        }
    }
    QLabel myLabel;
    myLabel.setPixmap(QPixmap::fromImage(myImage));
    //myLabel.show();       //uncomment to generate perlin noise function texture map

    // hide cursor
    a.setOverrideCursor(QCursor(Qt::BlankCursor));

    return a.exec();
}

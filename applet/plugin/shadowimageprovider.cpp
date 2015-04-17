#include "shadowimageprovider.h"

#include <QtGui/QPainter>
#include <QtGui/QRadialGradient>

#include <QDebug>

ShadowImageProvider::ShadowImageProvider(): QQuickImageProvider(QQuickImageProvider::Image, QQuickImageProvider::ForceAsynchronousImageLoading)
{

}

QImage ShadowImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    Q_UNUSED(requestedSize);

    *size = QSize( 0, 0 );

    // scheme shoud be color/size/strength
    QStringList tokens = id.split('/');

    qDebug() << "Tokens: " << tokens;

    if( tokens.size() != 3 ) {
        return QImage();
    }

    // get shadow color
    auto colorName = tokens.at(0);

    QColor shadowColor( Qt::black );

    if( QColor::isValidColor( colorName ) ) {
        shadowColor.setNamedColor( colorName );
    }

    // get shadow size
    auto isValidInt = false;
    int shadowSize = tokens.at(1).toInt(&isValidInt);

    if(!isValidInt) {
        //TODO
    }

    // get shadow strength
    qreal shadowStrength = tokens.at(2).toDouble(&isValidInt);

    qDebug() << "shadowSize = " << shadowSize << " shadowStrength = " << shadowStrength << " shadowColor = " << shadowColor;

    // This code is copied from Breeze kdecoration / breezedecoration.cpp
    // create image
//    QImage image( 2 * shadowSize, 2 * shadowSize, QImage::Format_ARGB32_Premultiplied);
//    image.fill(Qt::transparent);

//    *size = image.size();

//    // create gradient
//    // gaussian delta function
//    auto alpha = [](qreal x) { return std::exp( -x*x/0.15 ); };

//    // color calculation delta function
//    auto gradientStopColor = [](QColor color, int alpha)
//    {
//        color.setAlpha(alpha);
//        return color;
//    };

//    QRadialGradient radialGradient( shadowSize, shadowSize, shadowSize);
//    for( int i = 0; i < 10; ++i )
//    {
//        const qreal x( qreal( i )/9 );
//        radialGradient.setColorAt(x,  gradientStopColor(shadowColor, alpha(x)*shadowStrength));
//    }

//    radialGradient.setColorAt(1, gradientStopColor( shadowColor, 0 ) );

//    // fill
//    QPainter painter(&image);
//    painter.setCompositionMode(QPainter::CompositionMode_Source);
//    painter.fillRect( image.rect(), radialGradient);

//    // contrast pixel
//    int shadowOffset = 2;
//    painter.setBrush( Qt::NoBrush );
//    painter.setPen( gradientStopColor(shadowColor, shadowStrength) );
//    painter.setRenderHints(QPainter::Antialiasing );
//    painter.drawRoundedRect( QRect( shadowSize - shadowOffset, shadowSize - shadowOffset, shadowOffset, shadowOffset ), 3, 3 );
//    painter.end();




    QImage image( 3 * shadowSize, 3 * shadowSize, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);

    *size = image.size();

    // fill
    QPainter painter(&image);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.setBrush( Qt::NoBrush );
    painter.setRenderHints(QPainter::Antialiasing );

    // create gradient
    // gaussian delta function
    auto alpha = [](qreal x) { return std::exp( -x*x/0.15 ); };

    auto gradientStopColor = [](QColor color, qreal alpha)
    {
        color.setAlphaF(alpha);
        return color;
    };

    QRect rect( QPoint(0,0), image.size() );

    for( int i = 0; i < shadowSize; ++i )
    {
        const qreal x( qreal( i )/ qreal( shadowSize - 1 ) );

        const qreal a = alpha( 1.0 - x) * shadowStrength;
        const QColor c = gradientStopColor( shadowColor, a );

        qDebug() << "Drawing with x = " << x << " a = " << a;

        painter.setPen( c );
        painter.drawRoundedRect( rect, shadowSize - i, shadowSize - i );
        rect.adjust( 1, 1, -1, -1 );
    }

    painter.end();

    return image;
}

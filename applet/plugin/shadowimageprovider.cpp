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

    // scheme shoud be color/size/radius/strength
    QStringList tokens = id.split('/');

    qDebug() << "Tokens: " << tokens;

    if( tokens.size() != 4 ) {
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
        shadowSize = 0;
    }

    // get radius
    int radius = tokens.at(1).toInt(&isValidInt);

    if(!isValidInt) {
        radius = 0;
    }

    // get shadow strength
    qreal shadowStrength = tokens.at(3).toDouble(&isValidInt);

    qDebug() << "shadowSize = " << shadowSize << " shadowStrength = " << shadowStrength << " shadowColor = " << shadowColor;

    int imgSize = qMax( shadowSize, radius ) * 5;

    QImage image( imgSize, imgSize, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);

    *size = image.size();

    // fill
    QPainter painter(&image);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.setBrush( Qt::NoBrush );
    painter.setRenderHints(QPainter::Antialiasing );

    QPen pen = QPen(shadowColor, 2, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin);

    auto alpha = [](qreal x) { return std::exp( -x*x/0.15 ); };

    QRect rect( QPoint(0,0), image.size() );

    for( int i = 0; i < shadowSize; ++i )
    {
        const qreal x( qreal( i )/ qreal( shadowSize ) );

        const qreal a = alpha(1.0 - x) * shadowStrength;

        shadowColor.setAlphaF(a);
        pen.setColor( shadowColor );
        painter.setPen( pen );
        painter.drawRoundedRect( rect, qreal(radius), qreal(radius) );
        rect.adjust( 1, 1, -1, -1 );
    }

    painter.end();

    return image;
}

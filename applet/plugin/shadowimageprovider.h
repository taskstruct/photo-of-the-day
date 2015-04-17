#ifndef SHADOWIMAGEPROVIDER_H
#define SHADOWIMAGEPROVIDER_H

#include <QtQuick/QQuickImageProvider>

class ShadowImageProvider : public QQuickImageProvider
{
public:
    ShadowImageProvider();

    QImage requestImage(const QString &id, QSize *size, const QSize & requestedSize);
};

#endif // SHADOWIMAGEPROVIDER_H

#ifndef PROVIDERSMODEL_H
#define PROVIDERSMODEL_H

#include <QtCore/QAbstractListModel>

#include <KService/KPluginInfo>

class ProvidersModel : public QAbstractListModel
{
    Q_OBJECT
//     Q_PROPERTY(QStringList selectedTimeZones WRITE setSelectedTimeZones MEMBER m_selectedTimeZones NOTIFY selectedTimeZonesChanged)
    
public:
    explicit ProvidersModel(QObject *parent = 0);
    ~ProvidersModel();
    
    enum Roles {
        RealNameRole = Qt::UserRole,
        PluginNameRole
    };
    
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    
    Q_INVOKABLE int getIndex(const QString &pluginName);
    Q_INVOKABLE QString getPluginName(int index);
    
protected:
    QHash<int, QByteArray> roleNames() const;
    
private:
    
    KPluginInfo::List m_providers;
};

#endif // PROVIDERSMODEL_H

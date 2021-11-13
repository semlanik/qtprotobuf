/*
 * MIT License
 *
 * Copyright (c) 2019 Alexey Edelev <semlanik@gmail.com>
 *
 * This file is part of QtProtobuf project https://git.semlanik.org/semlanik/qtprotobuf
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and
 * to permit persons to whom the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies
 * or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
 * FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "qtprotobuflogging.h"
#include "qprotobufserializerregistry_p.h"
#include "qprotobufserializer.h"
#include "qprotobufjsonserializer.h"
#include "qprotobufserializationplugininterface.h"

#include <QDir>
#include <QString>
#include <QHash>
#include <QPluginLoader>
#include <QJsonObject>
#include <QJsonArray>

namespace {
const QLatin1String TypeNames("types");
const QLatin1String MetaData("MetaData");
const QLatin1String Version("version");
const QLatin1String Rating("rating");
const QLatin1String ProtoVersion("protobufVersion");
const QLatin1String PluginName("name");
const QLatin1String ProtobufSerializer("protobuf");
const QLatin1String JsonSerializer("json");
#ifdef _WIN32
const QLatin1String LibExtension(".dll");
const QLatin1String LibPrefix("");
#else
const QLatin1String LibExtension(".so");
const QLatin1String LibPrefix("lib");
#endif

static const char *QtProtobufPluginPath = QT_PROTOBUF_PLUGIN_PATH;
const QString DefaultImpl("Default");
}


namespace QtProtobuf {
//! \private
struct QProtobufSerializerRegistryPrivateRecord final
{
    QProtobufSerializerRegistryPrivateRecord() : loader(nullptr) {}

    ~QProtobufSerializerRegistryPrivateRecord() {
        serializers.clear();
        if (loader && loader->isLoaded()) {
            loader->unload();
        }
        delete loader;
    }

    void createDefaultImpl()
    {
        if (serializers.find(ProtobufSerializer) == serializers.end()) {
            serializers[ProtobufSerializer] = std::shared_ptr<QAbstractProtobufSerializer>(new QProtobufSerializer);
        }
        if (serializers.find(JsonSerializer) == serializers.end()) {
            serializers[JsonSerializer] = std::shared_ptr<QAbstractProtobufSerializer>(new QProtobufJsonSerializer);
        }
    }

    void loadPluginMetadata(const QString &fullFilePath)
    {
        libPath = fullFilePath;
        loader = new QPluginLoader(fullFilePath);
        pluginData = loader->metaData();

        QVariantMap fullJson = pluginData.toVariantMap();
        if (!fullJson.isEmpty()) {
            metaData = fullJson.value(MetaData).toMap();
            pluginLoadedName = metaData.value(PluginName).toString();
            typeArray = metaData.value(TypeNames).toList();
        }
    }

    void loadPlugin()
    {
        QProtobufSerializationPluginInterface *loadedPlugin = qobject_cast<QProtobufSerializationPluginInterface*>(loadPluginImpl());
        if (!pluginData.isEmpty() && loadedPlugin) {
            for (int i = 0; i < typeArray.count(); i++) {
                QString typeName = typeArray.at(i).toString();
                serializers[typeName] = std::shared_ptr<QAbstractProtobufSerializer>(loadedPlugin->serializer(typeName));
            }
        }
    }

    QObject* loadPluginImpl()
    {
        if (loader == nullptr || !loader->load()) {
            qProtoWarning() << "Can't load plugin from" << libPath
                            << "loader error" << (loader != nullptr ? loader->errorString() : "");
            return nullptr;
        }
        return loader->instance();
    }

    std::unordered_map<QString, std::shared_ptr<QAbstractProtobufSerializer>> serializers;
    QJsonObject pluginData;
    QVariantMap metaData;
    QString pluginLoadedName;
    QPluginLoader *loader;
    QString libPath;
    QVariantList typeArray;
};

//! \private
class QProtobufSerializerRegistryPrivate
{

public:
    QProtobufSerializerRegistryPrivate()
    {
        // create default impl
        std::shared_ptr<QProtobufSerializerRegistryPrivateRecord> plugin = std::shared_ptr<QProtobufSerializerRegistryPrivateRecord>(new QProtobufSerializerRegistryPrivateRecord());
        plugin->createDefaultImpl();
        m_plugins[DefaultImpl] = plugin;
        m_pluginPath = QString::fromUtf8(QtProtobufPluginPath);
        QString envPluginPath = QString::fromUtf8(qgetenv("QT_PROTOBUF_PLUGIN_PATH"));
        if (!envPluginPath.isEmpty()) {
            m_pluginPath = envPluginPath;
        }
    }

    QString loadPlugin(const QString &name)
    {
        assert(!name.isEmpty());

        std::shared_ptr<QProtobufSerializerRegistryPrivateRecord> plugin = std::shared_ptr<QProtobufSerializerRegistryPrivateRecord>(new QProtobufSerializerRegistryPrivateRecord());
        QString libPath = m_pluginPath + QDir::separator() + LibPrefix + name + LibExtension;
        plugin->loadPluginMetadata(libPath);

        const QString &pluginName = plugin->pluginLoadedName;
        if (m_plugins.find(pluginName) == m_plugins.end()) {
            plugin->loadPlugin();
            m_plugins[pluginName] = plugin;
        } else {
            plugin->loader = nullptr;
            qProtoInfo() << "Serializer plugin with name" << pluginName << "is already loaded";
        }
        return pluginName;
    }


    std::unordered_map<QString/*pluginName*/, std::shared_ptr<QProtobufSerializerRegistryPrivateRecord>> m_plugins;
    QString m_pluginPath;
};

}

using namespace QtProtobuf;

QProtobufSerializerRegistry::QProtobufSerializerRegistry() :
    dPtr(new QProtobufSerializerRegistryPrivate())
{
}

QProtobufSerializerRegistry::~QProtobufSerializerRegistry() = default;

QString QProtobufSerializerRegistry::loadPlugin(const QString &name)
{
    return dPtr->loadPlugin(name);
}

std::shared_ptr<QAbstractProtobufSerializer> QProtobufSerializerRegistry::getSerializer(const QString &id)
{
    return dPtr->m_plugins[DefaultImpl]->serializers.at(id); //throws
}

std::shared_ptr<QAbstractProtobufSerializer> QProtobufSerializerRegistry::getSerializer(const QString &id, const QString &plugin)
{
    return dPtr->m_plugins[plugin]->serializers.at(id); //throws
}

std::unique_ptr<QAbstractProtobufSerializer> QProtobufSerializerRegistry::acquireSerializer(const QString &/*id*/, const QString &/*plugin*/)
{
    Q_ASSERT_X(false, "QProtobufSerializerRegistry", "acquireSerializer is unimplemented");
    return std::unique_ptr<QAbstractProtobufSerializer>();
}

float QProtobufSerializerRegistry::pluginVersion(const QString &plugin)
{
    if (dPtr->m_plugins.find(plugin) == dPtr->m_plugins.end())
        return 0.0;

    std::shared_ptr<QProtobufSerializerRegistryPrivateRecord> implementation = dPtr->m_plugins[plugin];
    if (implementation->metaData.isEmpty())
        return 0.0;

    return implementation->metaData.value(Version).toFloat();
}

QStringList QProtobufSerializerRegistry::pluginSerializers(const QString &plugin)
{
    QStringList strList;

    if (dPtr->m_plugins.find(plugin) == dPtr->m_plugins.end())
        return strList;

    std::shared_ptr<QProtobufSerializerRegistryPrivateRecord> implementation = dPtr->m_plugins[plugin];

    QVariantList typeArray = implementation->metaData.value(TypeNames).toList();
    foreach(QVariant value, typeArray) {
        if (!value.toString().isEmpty()) {
            strList.append(value.toString());
        }
    }
    return strList;
}

float QProtobufSerializerRegistry::pluginProtobufVersion(const QString &plugin)
{
    if (dPtr->m_plugins.find(plugin) == dPtr->m_plugins.end())
        return 0.0;

    std::shared_ptr<QProtobufSerializerRegistryPrivateRecord> implementation = dPtr->m_plugins[plugin];
    if (implementation.get() && implementation->metaData.isEmpty())
        return 0.0;

    return implementation->metaData.value(ProtoVersion).toFloat();
}

int QProtobufSerializerRegistry::pluginRating(const QString &plugin)
{
    if (dPtr->m_plugins.find(plugin) == dPtr->m_plugins.end())
        return 0;

    std::shared_ptr<QProtobufSerializerRegistryPrivateRecord> implementation = dPtr->m_plugins[plugin];
    if (implementation->metaData.isEmpty())
        return 0;

    return implementation->metaData.value(Rating).toInt();
}

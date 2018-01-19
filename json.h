#ifndef JSON_H
#define JSON_H

#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>


class Json
{
private:
    static bool debugMode; // show error messages?

public:
    static void setDebugMode(bool debugMode);
    static bool modifyJsonValue(QJsonValue &destValue, const QString &path, const QJsonValue &newValue = QJsonValue(QJsonValue::Undefined));
    static bool modifyJsonValue(QJsonDocument& doc, const QString& path, const QJsonValue& newValue = QJsonValue(QJsonValue::Undefined));
};

#endif // JSON_H

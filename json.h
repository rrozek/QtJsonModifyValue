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
    static bool strictMode; // if strict mode is active and any part of path does not exist, the function will not create it and will return false

public:
    static void setDebugMode(bool debugMode);
    static void setStrictMode(bool strictMode);
    static bool modifyJsonValue(QJsonValue &destValue, const QString &path, const QJsonValue &newValue = QJsonValue(QJsonValue::Undefined));
    static bool modifyJsonValue(QJsonDocument& doc, const QString& path, const QJsonValue& newValue = QJsonValue(QJsonValue::Undefined));
};

#endif // JSON_H

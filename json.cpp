#include "json.h"

bool Json::debugMode = false;
bool Json::strictMode = false;

void Json::setDebugMode(bool debugMode) {
    Json::debugMode = debugMode;
}

void Json::setStrictMode(bool strictMode) {
    Json::strictMode = strictMode;
}

bool Json::modifyJsonValue(QJsonValue &destValue, const QString &path, const QJsonValue &newValue) {
    const int indexOfDot = path.indexOf('.');
    const QString dotPropertyName = path.left(indexOfDot); // whole path before first dot
    const QString dotSubPath = indexOfDot > 0 ? path.mid(indexOfDot + 1) : QString(); // whole path after first dot

    const int indexOfSquareBracketOpen = path.indexOf('[');
    const int indexOfSquareBracketClose = path.indexOf(']');

    bool ok;
    const int arrayIndex = path.mid(indexOfSquareBracketOpen + 1, indexOfSquareBracketClose - indexOfSquareBracketOpen - 1).toInt(&ok);
    if (indexOfSquareBracketClose > 0 && !ok) {
        if (debugMode) {
            qDebug() << "Wrong array index:" << path.mid(indexOfSquareBracketOpen, indexOfSquareBracketClose - indexOfSquareBracketOpen + 1);
        }
        return false;
    }

    const QString squareBracketPropertyName = path.left(indexOfSquareBracketOpen); // whole path before first square bracket
    const QString squareBracketSubPath = indexOfSquareBracketOpen > 0 ? path.mid(indexOfSquareBracketOpen) : path.mid(indexOfSquareBracketClose + 1).startsWith('.') ? path.mid(indexOfSquareBracketClose + 2) : path.mid(indexOfSquareBracketClose + 1); // whole path after squareBracketPropertyName

    // determine what is first in path. dot or bracket
    bool useDot = true;
    if (indexOfDot >= 0) { // there is a dot in path
        if (indexOfSquareBracketOpen >= 0) { // there is squarebracket in path
            if (indexOfDot > indexOfSquareBracketOpen)
                useDot = false; // square bracket is earlier, use it
            else
                useDot = true; // dot is earlier, use it
        } else {
            useDot = true; // there is no square bracket in path, use dot
        }
    } else { // there is no dot in path
        if (indexOfSquareBracketOpen >= 0)
            useDot = false; // there is square bracket in path, use it
        else
            useDot = true; // acutally, it doesn't matter, both dot and square bracket don't exist
    }

    QString usedPropertyName = useDot ? dotPropertyName : squareBracketPropertyName; // used property name for current operation, if is empty then the arrayIndex is used
    QString usedSubPath = useDot ? dotSubPath : squareBracketSubPath; // remaining path to be used in next steps

    QJsonValue subValue; // subValue is value of the usedPropertyName or arrayIndex
    if (destValue.isArray()) { // if current QJsonValue is array then usedPropertyName should be empty so the arrayIndex could be used
        if (usedPropertyName.isEmpty()) {
            QJsonArray destArray = destValue.toArray();
            if (destArray.count() <= arrayIndex && !newValue.isUndefined()) { // if arrayIndex is higher than index of last element, fill array with null values to arrayIndex (without arrayIndex)
                for (int i = destArray.count(); i < arrayIndex; ++i) {
                    destArray.append(QJsonValue());
                }
                destValue = destArray;
            }
            subValue = destValue.toArray().at(arrayIndex);
        } else { // if usedPropertyName is not empty, there is an error in requested path
            if (debugMode) {
                qDebug() << "Current QJqueryValue is Array:" << destValue;
                qDebug() << "You requested JSON object with key:" << usedPropertyName;
            }
            return false;
        }
    } else if (destValue.isObject()) { // if current QJsonValue is object then usedPropertyName should not be emty
        if (usedPropertyName.isEmpty()) { // if usedPropertyName is empty, there is an error in requested path
            if (debugMode) {
                qDebug() << "Current QJqueryValue is Object:" << destValue;
                qDebug() << "You requested JSON array with key:" << path.mid(indexOfSquareBracketOpen, indexOfSquareBracketClose - indexOfSquareBracketOpen + 1);
            }
            return false;
        } else {
            subValue = destValue.toObject().value(usedPropertyName);
        }
    } else if (destValue.isUndefined()) { // if the sub value in path do not exist, create it
        if (usedPropertyName.isEmpty()) {
            destValue = QJsonArray();
            QJsonArray destArray = destValue.toArray();
            if (destArray.count() <= arrayIndex && !newValue.isUndefined()) { // if arrayIndex is higher than index of last element, fill array with null values to arrayIndex (without arrayIndex)
                for (int i = destArray.count(); i < arrayIndex; ++i) {
                    destArray.append(QJsonValue());
                }
                destValue = destArray;
            }
            subValue = destValue.toArray().at(arrayIndex);
        } else {
            destValue = QJsonObject();
            subValue = destValue.toObject().value(usedPropertyName);
        }
    } else {
        //this should never happen, it is already handled in previous function call in the if condition written on next few lines as return
        return false; // but just for safety return
    }

    bool returnValue = false;

    if (usedSubPath.isEmpty()) { // i am at the end of path, assign newValue
        subValue = newValue;
        returnValue = true;
    } else {
        if (subValue.isArray() || subValue.isObject()) { // is subValue is array or object continue deeper in path
            returnValue = modifyJsonValue(subValue,usedSubPath,newValue);
        } else if (subValue.isUndefined()) { // if subValue is undefined continue deeper in path only if am i assigning newValue
            if (newValue.isUndefined()) { // if newValue is undefined i do not need to go deeper to remove key at path because the subpath does not exist
                return false; // no need to delete value at requested path, the subpath does not exist
            } else { // continue deeper in path and create all path keys until end
                returnValue = modifyJsonValue(subValue,usedSubPath,newValue);
            }
        } else { // subValue is value and subPath is not empty, i cannot go deeper in requested pat, error
            if (usedPropertyName.isEmpty()) {
                if (debugMode) {
                    qDebug() << "Current QJqueryValue is Value:" << subValue;
                    qDebug() << "You requested JSON array with key:" << path.mid(indexOfSquareBracketOpen, indexOfSquareBracketClose - indexOfSquareBracketOpen + 1);
                }
                return false;
            } else {
                if (debugMode) {
                    qDebug() << "Current QJqueryValue is Value:" << subValue;
                    qDebug() << "You requested JSON object with key:" << usedPropertyName;
                }
                return false;
            }
        }
    }

    // assign subValue back into destValue based on destValue data type
    if (destValue.isArray()) {
        QJsonArray arr = destValue.toArray();
        if (subValue.isUndefined()) { // delete
            if (arr.size() > arrayIndex) {
                arr.removeAt(arrayIndex);
            }
        } else { // assign
            if (arr.size() > arrayIndex) {
                arr.removeAt(arrayIndex);
            }
            arr.insert(arrayIndex, subValue);
        }
        destValue = arr;
    } else if (destValue.isObject()) {
        QJsonObject obj = destValue.toObject();
        if (subValue.isUndefined()) // delete
            obj.remove(usedPropertyName);
        else // assign
            obj.insert(usedPropertyName, subValue);
        destValue = obj;
    } else { // i am not exactly sure if this else statement is needed, i think that this should never happen, but just for safety i leave it here
        destValue = newValue;
    }

    return returnValue;
}

bool Json::modifyJsonValue(QJsonDocument& doc, const QString& path, const QJsonValue& newValue) {
    QJsonValue val;
    if (doc.isArray())
        val = doc.array();
    else
        val = doc.object();

    bool returnValue = modifyJsonValue(val, path, newValue);

    if (val.isArray())
        doc = QJsonDocument(val.toArray());
    else
        doc = QJsonDocument(val.toObject());

    return returnValue;
}

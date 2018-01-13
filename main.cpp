#include <QCoreApplication>
#include <QDebug>

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>

void modifyJsonValue(QJsonValue &destValue, const QString& path, const QJsonValue& newValue = QJsonValue());
void modifyJsonValue(QJsonDocument& doc, const QString& path, const QJsonValue& newValue = QJsonValue());

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    std::string sampleJson = R"({
                          "firstName": "John",
                          "lastName": "Smith",
                          "age": 25,
                          "address":
                          {
                              "streetAddress": "21 2nd Street",
                              "city": "New York",
                              "state": "NY",
                              "postalCode": "10021"
                          },
                          "phoneNumber":
                          [
                              {
                                "type": "home",
                                "number": "212 555-1234"
                              },
                              {
                                "type": "fax",
                                "number": "646 555-4567"
                              }
                          ],
                          "family":
                          [
                              [
                                  "Jeff",
                                  "Marika",
                                  "Tony"
                              ],
                              [
                                  "Steve",
                                  "Sonny",
                                  "Sally"
                              ],
                              {
                                  "father":
                                  {
                                      "name": "Mike",
                                      "age": 55
                                  },
                                  "mother":
                                  {
                                      "name": "Jennifer",
                                      "age": 49
                                  }
                              }
                          ]
                      })";
    QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(sampleJson));

    //modify or add value
    modifyJsonValue(doc, "firstName", QJsonValue("Natalia"));
    modifyJsonValue(doc, "age", 22);
    modifyJsonValue(doc, "address.state", "None");
    modifyJsonValue(doc, "phoneNumber[0].number", "333 543-3210");
    modifyJsonValue(doc, "family[0][2]", "Bill");
    modifyJsonValue(doc, "family[1][1]", "Winston");
    modifyJsonValue(doc, "family[2].father.age", 56);

    //remove value
    modifyJsonValue(doc, "family[1][2]");

    return a.exec();
}

void modifyJsonValue(QJsonValue& destValue, const QString& path, const QJsonValue& newValue)
{
    const int indexOfDot = path.indexOf('.');
    const QString dotPropertyName = path.left(indexOfDot);
    const QString dotSubPath = indexOfDot > 0 ? path.mid(indexOfDot + 1) : QString();

    const int indexOfSquareBracketOpen = path.indexOf('[');
    const int indexOfSquareBracketClose = path.indexOf(']');

    const int arrayIndex = path.mid(indexOfSquareBracketOpen + 1, indexOfSquareBracketClose - indexOfSquareBracketOpen - 1).toInt();

    const QString squareBracketPropertyName = path.left(indexOfSquareBracketOpen);
    const QString squareBracketSubPath = indexOfSquareBracketClose > 0 ? (path.mid(indexOfSquareBracketClose + 1)[0] == '.' ? path.mid(indexOfSquareBracketClose + 2) : path.mid(indexOfSquareBracketClose + 1)) : QString();

    // determine what is first in path. dot or bracket
    bool useDot = true;
    if (indexOfDot >= 0) // there is a dot in path
    {
        if (indexOfSquareBracketOpen >= 0) // there is squarebracket in path
        {
            if (indexOfDot > indexOfSquareBracketOpen)
                useDot = false;
            else
                useDot = true;
        }
        else
            useDot = true;
    }
    else
    {
        if (indexOfSquareBracketOpen >= 0)
            useDot = false;
        else
            useDot = true; // acutally, id doesn't matter, both dot and square bracket don't exist
    }

    QString usedPropertyName = useDot ? dotPropertyName : squareBracketPropertyName;
    QString usedSubPath = useDot ? dotSubPath : squareBracketSubPath;

    QJsonValue subValue;
    if (destValue.isArray())
        subValue = destValue.toArray()[usedPropertyName.toInt()];
    else if (destValue.isObject())
        subValue = destValue.toObject()[usedPropertyName];
    else
        qDebug() << "oh, what should i do now with the following value?! " << destValue;

    if(usedSubPath.isEmpty())
    {
        subValue = newValue;
    }
    else
    {
        if (subValue.isArray())
        {
            QJsonArray arr = subValue.toArray();
            QJsonValue arrEntry = arr[arrayIndex];
            modifyJsonValue(arrEntry,usedSubPath,newValue);
            arr[arrayIndex] = arrEntry;
            subValue = arr;
        }
        else if (subValue.isObject())
            modifyJsonValue(subValue,usedSubPath,newValue);
        else
            subValue = newValue;
    }

    if (destValue.isArray())
    {
        QJsonArray arr = destValue.toArray();
        if (subValue.isNull())
            arr.removeAt(arrayIndex);
        else
            arr[arrayIndex] = subValue;
        destValue = arr;
    }
    else if (destValue.isObject())
    {
        QJsonObject obj = destValue.toObject();
        if (subValue.isNull())
            obj.remove(usedPropertyName);
        else
            obj[usedPropertyName] = subValue;
        destValue = obj;
    }
    else
        destValue = newValue;
}

void modifyJsonValue(QJsonDocument& doc, const QString& path, const QJsonValue& newValue)
{
    QJsonValue val;
    if (doc.isArray())
        val = doc.array();
    else
        val = doc.object();

    modifyJsonValue(val,path,newValue);

    if (val.isArray())
        doc = QJsonDocument(val.toArray());
    else
        doc = QJsonDocument(val.toObject());
}

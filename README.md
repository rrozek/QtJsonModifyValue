# QtJsonModifyValue

## Documentation

For each example I will use this JSON object as my QJsonDocument `doc`.
```
{
    "firstName": "John",
    "lastName": "Smith",
    "age": 25,
    "address": {
        "streetAddress": "21 2nd Street",
        "city": "New York",
        "state": "NY",
        "postalCode": "10021"
    },
    "phoneNumber": [{
            "type": "home",
            "number": "212 555-1234"
        },
        {
            "type": "fax",
            "number": "646 555-4567"
        }
    ],
    "family": [
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
            "father": {
                "name": "Mike",
                "age": 55
            },
            "mother": {
                "name": "Jennifer",
                "age": 49
            }
        }
    ]
}
```

### Basic rules
1. Separate object names with dot: `address.city`.
2. Access array elements just like in c++: `family[3]`.
3. You cannot use dots in object names // I'm working on it

### Adding value
If you want to add value to JSON, use `modifyJsonValue(QJsonValue &destValue, const QString &path, const QJsonValue &newValue)` or `modifyJsonValue(QJsonDocument &doc, const QString &path, const QJsonValue &newValue)`:
```
modifyJsonValue(doc, "phoneNumber[2]", QJsonObject()); // returns true
modifyJsonValue(doc, "phoneNumber[2].type", "work"); // returns true
modifyJsonValue(doc, "phoneNumber[2].number", "123 555-4251"); // returns true
```
The first function will create empty object in `phoneNumber` at array index `2`. And the next two functions will write in the object values for `type` and `number`. You can also skip the first line and the whole sub path will be automatically created for you.
```
// this will produce same result as example above
modifyJsonValue(doc, "phoneNumber[2].type", "work"); // returns true
modifyJsonValue(doc, "phoneNumber[2].number", "123 555-4251"); // returns true
```
There is also other way you can archieve this result. If you already have created `QJsonObject` with the `type` and `number` values, you can do the following.
```
// myObject is the QJsonObject stated above
modifyJsonValue(doc, "phoneNumber[2]", myObject); // returns true
```

If you try to add value to array and the index does not exist it will be automatically created for you and all values between this index and last array element will be filled with `null` value:
```
modifyJsonValue(doc, "family[0][5]", "George"); // returns true
```
This will change the array to this:
```
[
    "Jeff",
    "Marika",
    "Tony",
    null,
    null,
    "George"
]
```

### Changing value
Changing value is the same as adding with the same rules. If the value already exist it will be changed, otherwice added.

### Removing value from array / Removing key-value from object
If you want to remove value from JSON, just use `modifyJsonValue(QJsonValue &destValue, const QString &path)` or `modifyJsonValue(QJsonDocument &doc, const QString &path)` without the `newValue` argument:
```
modifyJsonValue(doc, "family[1][2]"); // returns true
```
This will remove `Sally` from array.

OR

```
modifyJsonValue(doc, "address.state"); // returns true
```
This will remove `state` from object.

If you request to delete array index that is not in array or object key-value that is not in object the function will do nothing and returns `false`.


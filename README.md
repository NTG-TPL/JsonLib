# JsonLib
Реализована небольшая библиотека для парсинга и генерации Json файлов, поддерживающая Fluent интерфейс.

## Пример использования
``` c++
json::Print(
        json::Document{
            json::Builder{}
            .StartDict()
                .Key("ключ 1"s).Value(123)
                .Key("ключ 2"s).Value("значение 2"s)
                .Key("ключ 3"s).StartArray()
                    .Value(456)
                    .Value("значение 2")
                    .Value(nullptr)
                    .StartDict().EndDict()
                    .StartDict()
                        .Key("ключ 1"s).Value("значение 1")
                        .Key("ключ 2"s).Value(nullptr)
                        .Key(""s).Value(nullptr)
                    .EndDict()
                    .Value("Надеюсь, принцип понятен"s)
                .EndArray()
            .EndDict()
            .Build()
        },
        std::cout
    );
```
<details> <summary> результат </summary>

``` json
{
    "ключ 1": 123,
    "ключ 2": "значение 2",
    "ключ 3": [
        456,
        "значение 2",
        null,
        {

        },
        {
            "": null,
            "ключ 1": "значение 1",
            "ключ 2": null
        },
        "Надеюсь, принцип понятен"
    ]
}
```
</details>

## Установка и использование
Просто скопируйте файлы в папку с вашим проектом и подключите через директиву **#include<json_builder.h>**

## Системные требования
Компилятор С++ с поддержкой стандарта C++17 или новее, Cmake версии 3.24 и выше.

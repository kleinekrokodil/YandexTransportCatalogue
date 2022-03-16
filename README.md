# YandexTransportCatalogue 
Проект транспортного справочника (С++17).
Создает базу автобусных остановок и маршрутов, выполняет поиск по ней, а также построение карты маршрутов в формате SVG. 
Реализована поддержка ввода-вывода в формате JSON с помощью самостоятельно разработанного парсера JSON. 
Разработан в IDE CLion, использованы умные указатели unique_ptr и shared_ptr, неупорядоченные ассоциативные контейнеры, исключения.
Поддерживает следующие запросы на ввод:

Stop X: latitude, longitude, D1m to stop1, D2m to stop2, ... - Остановка с названием X и координатами latitude и longitude, DYm to stop Y- расстояние до соседней остановки Y.

Bus X: описание маршрута - Запрос на добавление автобуса номер X. Описание маршрута может задаваться в одном из двух форматов:
        stop1 - stop2 - ... stopN: автобус следует от stop1 до stopN и обратно с указанными промежуточными остановками.
        stop1 > stop2 > ... > stopN > stop1: кольцевой маршрут с конечной stop1.

Запросы на вывод:
Bus X -Вывести информацию об автобусе X в следующем формате: Bus X: R stops on route, U unique stops, L route length, C curvature.
Stop X -Вывести информацию об остановке X в следующем формате: Stop X: buses bus1 bus2 ... busN
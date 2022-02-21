#include "json_builder.h"

#include <utility>

using namespace json;

Builder::Builder() {
    nodes_stack_.push_back(&root_);

    //std::vector<Node*> nodes_stack_; — вектор указателей на те вершины JSON, которые ещё не построены.
    //Т.е.:
    //1. root_ == nullptr_t(по умолчанию для любого нового Node)
    //2. Методы Key, StartDict, StartArray добавляют в вектор соотв. элементы Node(nullptr_t)
    //3. Метод Value присваивает пустому Node значение и заканчивает конструирование Node.
    //4. Методы EndDict и EndArray заканчивают соотв. Node-контейнер
    //5. При вызове Build в векторе незаконченных Node ничего не должно оставаться
}

KeyItemContext Builder::Key(std::string key) {
    if(!IsMap()){
        throw std::logic_error("Calling the Key method outside the dictionary");
    }
    //Метод AsDict константный
    //Создаем элемент словаря {key, node = nullptr_t}
    nodes_stack_.emplace_back(&const_cast<Dict&>(nodes_stack_.back()->AsMap())[key]);
    return *this;
}

Builder &Builder::Value(Node value) {
    //Кидаем исключение, если метод вызван вне конструктора или последний элемент не null и не массив
    if (!IsValue()) {
        throw std::logic_error("Value method call error");
    }
    if (nodes_stack_.back()->IsArray()) {
        const_cast<Array&>(nodes_stack_.back()->AsArray()).push_back(value);
    }
    //Если последний элемент nullptr - присваиваем ему значение и удаляем указатель на законченный элемент из вектора
    else {
        *nodes_stack_.back() = value;
        nodes_stack_.pop_back();
    }
    return *this;
}

DictItemContext Builder::StartDict() {
    if (!IsValue()) {
        throw std::logic_error("StartDict method call error");
    }
    if (nodes_stack_.back()->IsArray()) {
        const_cast<Array &>(nodes_stack_.back()->AsArray()).push_back(Dict());
        Node* node = &const_cast<Array&>(nodes_stack_.back()->AsArray()).back();
        nodes_stack_.push_back(node);
    } else {
        *nodes_stack_.back() = Dict();
    }
    return *this;
}

ArrayItemContext Builder::StartArray() {
    if (!IsValue()) {
        throw std::logic_error("StartArray method call error");
    }
    if (nodes_stack_.back()->IsArray()) {
        const_cast<Array &>(nodes_stack_.back()->AsArray()).push_back(Array());
        nodes_stack_.emplace_back(&const_cast<Array&>(nodes_stack_.back()->AsArray()).back());
    } else {
        *nodes_stack_.back() = Array();
    }
    return *this;
}

Builder &Builder::EndDict() {
    if (!IsMap()) {
        throw std::logic_error("Calling EndDict method outside a dictionary");
    }
    nodes_stack_.pop_back();
    return *this;
}

Builder &Builder::EndArray() {
    if (!IsArray()) {
        throw std::logic_error("Calling EndArray method outside an array");
    }
    nodes_stack_.pop_back();
    return *this;
}

Node Builder::Build() {
    if (!nodes_stack_.empty()) {
        throw std::logic_error("Object is not ready to build");
    }
    return root_;
}

bool Builder::IsMap() {
    return !nodes_stack_.empty() && nodes_stack_.back()->IsMap();
}

bool Builder::IsValue() {
    return !nodes_stack_.empty() || (nodes_stack_.back()->IsNull() && nodes_stack_.back()->IsArray());
}

bool Builder::IsArray() {
    return !nodes_stack_.empty() && nodes_stack_.back()->IsArray();
}


KeyItemContext ItemContext::Key(std::string key) {
    return builder_.Key(key);
}

Builder &ItemContext::Value(Node value) {
    return builder_.Value(std::move(value));
}

DictItemContext ItemContext::StartDict() {
    return builder_.StartDict();
}

ArrayItemContext ItemContext::StartArray() {
    return builder_.StartArray();
}

Builder &ItemContext::EndDict() {
    return builder_.EndDict();
}

Builder &ItemContext::EndArray() {
    return builder_.EndArray();
}

KeyValueContext KeyItemContext::Value(Node value) {
    return ItemContext::Value(std::move(value));
}

ArrayValueContext ArrayItemContext::Value(Node value) {
    return ItemContext::Value(std::move(value));
}

ArrayValueContext ArrayValueContext::Value(Node value) {
    return ItemContext::Value(std::move(value));
}

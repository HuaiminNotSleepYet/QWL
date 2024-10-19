#ifndef QWL_H
#define QWL_H

#include <QBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>

#define QWL_CAT_IMPL(A, B) A##B
#define QWL_CAT(A, B) QWL_CAT_IMPL(A, B)

#define QWL_CHOOSE4(_0, _1, _2, _3, _4, N, ...) N
#define QWL_CHOOSE32(_32,_31,_30,_29,_28,_27,_26,_25,_24,_23,_22,_21,_20,_19,_18,_17,_16,_15,_14,_13,_12,_11,_10,_9,_8,_7,_6,_5,_4,_3,_2,_1,_0, N, ...) N

#define QWL_COUNT(...) QWL_CHOOSE32(0, ##__VA_ARGS__, 32,31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0)

#define QWL_MAP_3_IMPL_0(FUNC, ...)
#define QWL_MAP_3_IMPL_3( FUNC, A, B, C, ...) FUNC(A, B, C)
#define QWL_MAP_3_IMPL_6( FUNC, A, B, C, ...) FUNC(A, B, C) QWL_MAP_3_IMPL_3 (FUNC, __VA_ARGS__)
#define QWL_MAP_3_IMPL_9( FUNC, A, B, C, ...) FUNC(A, B, C) QWL_MAP_3_IMPL_6 (FUNC, __VA_ARGS__)
#define QWL_MAP_3_IMPL_12(FUNC, A, B, C, ...) FUNC(A, B, C) QWL_MAP_3_IMPL_9 (FUNC, __VA_ARGS__)
#define QWL_MAP_3_IMPL_15(FUNC, A, B, C, ...) FUNC(A, B, C) QWL_MAP_3_IMPL_12(FUNC, __VA_ARGS__)
#define QWL_MAP_3_IMPL_18(FUNC, A, B, C, ...) FUNC(A, B, C) QWL_MAP_3_IMPL_15(FUNC, __VA_ARGS__)
#define QWL_MAP_3_IMPL_21(FUNC, A, B, C, ...) FUNC(A, B, C) QWL_MAP_3_IMPL_18(FUNC, __VA_ARGS__)
#define QWL_MAP_3_IMPL_24(FUNC, A, B, C, ...) FUNC(A, B, C) QWL_MAP_3_IMPL_21(FUNC, __VA_ARGS__)
#define QWL_MAP_3_IMPL_27(FUNC, A, B, C, ...) FUNC(A, B, C) QWL_MAP_3_IMPL_24(FUNC, __VA_ARGS__)
#define QWL_MAP_3_IMPL_30(FUNC, A, B, C, ...) FUNC(A, B, C) QWL_MAP_3_IMPL_27(FUNC, __VA_ARGS__)

#define QWL_MAP(ARGC, FUNC, ...) QWL_CAT(QWL_MAP_##ARGC##_IMPL_, QWL_COUNT(__VA_ARGS__))(FUNC, __VA_ARGS__)

namespace qwl {

template<typename Object> class Builder;

/* ----------------------------------- Id ----------------------------------- */

struct Id
{
    template<typename T> struct From { T*  p; };
    template<typename T> struct To   { T*& p; };

    template<typename T> From<T> operator<<(T*  p) const { return {p}; }
    template<typename T> To<T>   operator>>(T*& p) const { return {p}; }

    template<typename T> struct is_from             : std::false_type {};
    template<typename T> struct is_from<Id::From<T>>: std::true_type  {};

    template<typename T> constexpr static bool is_from_v = is_from<T>::value;
};

constexpr Id id = {};

/* -------------------------------- Property -------------------------------- */

template<typename Tag, typename T> struct PropertyValue { T value; };

template<typename Tag>
struct Property
{
    template<typename T>
    auto operator=(T&& v) const
    {
        return operator()(std::forward<T>(v));
    }

    template<typename T>
    auto operator()(T&& v) const
    {
        if constexpr (std::is_lvalue_reference_v<T>)
            return PropertyValue<Tag, T>{v};
        else
            return PropertyValue<Tag, std::decay_t<T>>{v};
    }
};

#define QWL_PROPERTY(NAME, DEFAULT_SETTER)                                      \
    namespace qwl {                                                             \
        struct NAME##_Tag                                                       \
        {                                                                       \
            template<typename T, typename U>                                    \
            static void set(T* obj, const U& val) { obj->DEFAULT_SETTER(val); } \
        };                                                                      \
        constexpr Property<NAME##_Tag> NAME = {};                               \
    }

#define QWL_PROPERTY_SETTER(OBJECT, NAME) \
    template<typename T> static void qwlSetProp(OBJECT* obj, const T& prop, ::qwl::NAME##_Tag)

/* ---------------------------- Attached Property --------------------------- */

template<typename T, typename U>
struct ObjectWithAttachedProperty
{
    T* object;
    U  attached;

    ObjectWithAttachedProperty(T* o, U a) : object(o), attached(a) {}
};

#define QWL_ATTACHED_PROPERTY_IMPL1(TYPE, NAME, DEFAULT) TYPE NAME = DEFAULT;
#define QWL_ATTACHED_PROPERTY_IMPL2(TYPE, NAME, _      ) Properties& NAME(TYPE val) { v.NAME = val; return *this; }
#define QWL_ATTACHED_PROPERTY_IMPL3(TYPE, NAME, _      ) static auto NAME(TYPE val) { return Properties().NAME(val); }

#define QWL_ATTACHED_PROPERTY(...)                                          \
    struct Attached                                                         \
    {                                                                       \
        struct Properties                                                   \
        {                                                                   \
            struct Values                                                   \
            {                                                               \
                QWL_MAP(3, QWL_ATTACHED_PROPERTY_IMPL1, __VA_ARGS__)        \
            };                                                              \
            Values v;                                                       \
                                                                            \
            QWL_MAP(3, QWL_ATTACHED_PROPERTY_IMPL2, __VA_ARGS__)            \
                                                                            \
            template<typename QWL_T>                                        \
            auto operator|(const ::qwl::Builder<QWL_T>& builder)            \
            {                                                               \
                return ObjectWithAttachedProperty(builder.object(), *this); \
            }                                                               \
        };                                                                  \
                                                                            \
        QWL_MAP(3, QWL_ATTACHED_PROPERTY_IMPL3, __VA_ARGS__)                \
    };

/* ---------------------------------- Slot ---------------------------------- */

template<typename Tag, typename R, typename S>
struct ConnectToSlot
{
    R receiver;
    S slot;

    template<typename T>
    void subscribe(T* sender)
    { QObject::connect(sender, Tag::template get<T>(), receiver, slot); }
};

template<typename Tag, typename F>
struct ConnectToFunc
{
    F func;

    template<typename T>
    void subscribe(T* sender)
    { QObject::connect(sender, Tag::template get<T>(), sender, func); }
};

#define QWL_SIGNAL(NAME, SIG)                                               \
    namespace qwl                                                           \
    {                                                                       \
        struct NAME##_Tag                                                   \
        {                                                                   \
            template<typename T> static auto get() { return &T::SIG; }      \
        };                                                                  \
                                                                            \
        template<typename R, typename S>                                    \
        auto NAME(R receiver, S slot)                                       \
        {                                                                   \
            return ConnectToSlot<NAME##_Tag, R, S>{receiver,slot};          \
        }                                                                   \
                                                                            \
        template<typename F>                                                \
        auto NAME(F func)                                                   \
        {                                                                   \
            return ConnectToFunc<NAME##_Tag, F>{func};                      \
        }                                                                   \
    }

/* ---------------------------------- Child --------------------------------- */

#define QWL_ADD_CHILD_IMPL1(OBJECT, CHILD)\
    void static qwlAddChild(OBJECT* obj, CHILD child)

#define QWL_ADD_CHILD_IMPL2(OBJECT, CHILD, BUILDER)\
    void static qwlAddChild(OBJECT* obj, CHILD child, const BUILDER::Attached::Properties::Values& prop = {})

// Usage:
//   Add child                         : QWL_ADD_CHILD(parent type, child type)
//   Add child with attached properties: QWL_ADD_CHILD(parent type, child type, builder type)
#define QWL_ADD_CHILD(...)\
    QWL_CHOOSE4(0, ##__VA_ARGS__, ,QWL_ADD_CHILD_IMPL2, QWL_ADD_CHILD_IMPL1,,)(__VA_ARGS__)

/* --------------------------------- Builder -------------------------------- */

template<typename Object>
class Builder
{
    template<typename Object_> friend class Builder;

public:
    template<typename ...Item>
    Builder(const Item&... item)
    {
        constexpr auto id_from_count = (0 + ... + (Id::is_from_v<Item> ? 1 : 0));
        static_assert(id_from_count <= 1, "A control can have only one id.");

        if constexpr (id_from_count == 0)
            o = new Object;
        else
            (applyId(item), ...);

        (apply(item), ...);
    }

    operator Object*() const { return o; }

    Object* object() const { return o; }

private:
    Object* o;

    template<typename T>
    void applyId(T&) {}

    template<typename T>
    void applyId(Id::From<T> from) { o = from.p; }

    template<typename T> void apply(Id::From<T> ) {}
    template<typename T> void apply(Id::To<T> to) { to.p = o; }

    template<typename T, typename V, typename Tag, typename = std::void_t<>>
    struct has_custom_setter : std::false_type {};

    template<typename T, typename V, typename Tag>
    struct has_custom_setter<T, V, Tag,
            std::void_t<decltype(qwlSetProp(std::declval<T*>(),
                                            std::declval<V>(),
                                            std::declval<Tag>()))>>
        : std::true_type {};

    template<typename Tag, typename T>
    void apply(const PropertyValue<Tag, T>& prop)
    {
        if constexpr (has_custom_setter<Object, T, Tag>::value)
            qwlSetProp(o, prop.value, Tag());
        else
            Tag::set(o, prop.value);
    }

    template<typename T>
    void apply(const Builder<T>& child)
    { qwlAddChild(o, child.object()); }

    template<typename T, typename U>
    void apply(const ObjectWithAttachedProperty<T, U>& child)
    { qwlAddChild(o, child.object, child.attached.v); }

    template<typename T>
    void apply(const T& child)
    { qwlAddChild(o, child); }

    template<typename Signal, typename R, typename S>
    void apply(ConnectToSlot<Signal, R, S> conn) { conn.subscribe(o); }

    template<typename Signal, typename F>
    void apply(ConnectToFunc<Signal, F> conn) { conn.subscribe(o); }
};

/* -------------------------------- Controls -------------------------------- */

using Widget        = Builder<QWidget>;
using CheckBox      = Builder<QCheckBox>;
using ComboBox      = Builder<QComboBox>;
using DoubleSpinBox = Builder<QDoubleSpinBox>;
using GroupBox      = Builder<QGroupBox>;
using Label         = Builder<QLabel>;
using LineEdit      = Builder<QLineEdit>;
using PlainTextEdit = Builder<QPlainTextEdit>;
using PushButton    = Builder<QPushButton>;
using Slider        = Builder<QSlider>;
using RadioButton   = Builder<QRadioButton>;
using SpinBox       = Builder<QSpinBox>;

template<typename Object>
class GridLayoutBuilder : public Builder<Object>
{
public:
    using Builder<Object>::Builder;

    QWL_ATTACHED_PROPERTY
    (
        int          , row       , 0,
        int          , rowSpan   , 1,
        int          , col       , 0,
        int          , colSpan   , 1,
        Qt::Alignment, align     , {}
    )

    struct rowStretch { int row; int stretch; rowStretch(int r, int s) : row(r), stretch(s) {} };
    struct colStretch { int col; int stretch; colStretch(int c, int s) : col(c), stretch(s) {} };
};

using GridLayout = GridLayoutBuilder<QGridLayout>;

template<typename Object>
class BoxLayoutBuilder : public Builder<Object>
{
public:
    using Builder<Object>::Builder;

    QWL_ATTACHED_PROPERTY
    (
        int          , stretch, 0,
        Qt::Alignment, align  , {}
    );

    struct Stretch
    {
        int value;
        Stretch(int v) : value(v) {}
    };
};

using BoxLayout  = BoxLayoutBuilder<QBoxLayout>;
using HBoxLayout = BoxLayoutBuilder<QHBoxLayout>;
using VBoxLayout = BoxLayoutBuilder<QVBoxLayout>;

} // namespace qwl

QWL_PROPERTY(align      , setAlignment  )
QWL_PROPERTY(checkable  , setCheckable  )
QWL_PROPERTY(checked    , setChecked    )
QWL_PROPERTY(checkState , setCheckState )
QWL_PROPERTY(default_   , setDefault    )
QWL_PROPERTY(flat       , setFlat       )
QWL_PROPERTY(maximum    , setMaximum    )
QWL_PROPERTY(minimum    , setMinimum    )
QWL_PROPERTY(orientation, setOrientation)
QWL_PROPERTY(title      , setTitle      )
QWL_PROPERTY(text       , setText       )
QWL_PROPERTY(tristate   , setTristate   )
QWL_PROPERTY(value      , setValue      )

QWL_PROPERTY_SETTER(QPlainTextEdit, text) { obj->setPlainText(prop); }

QWL_SIGNAL(onClicked     , clicked     )
QWL_SIGNAL(onToggled     , toggled     )
QWL_SIGNAL(onValueChanged, valueChanged)

QWL_ADD_CHILD(QWidget, QLayout*) { obj->setLayout(child); }

QWL_ADD_CHILD(QGridLayout, QLayout*, qwl::GridLayout) { obj->addLayout(child, prop.row, prop.col, prop.rowSpan, prop.colSpan, prop.align); }
QWL_ADD_CHILD(QGridLayout, QWidget*, qwl::GridLayout) { obj->addWidget(child, prop.row, prop.col, prop.rowSpan, prop.colSpan, prop.align); }
QWL_ADD_CHILD(QGridLayout, qwl::GridLayout::rowStretch) { obj->setRowStretch(child.row, child.stretch); }
QWL_ADD_CHILD(QGridLayout, qwl::GridLayout::colStretch) { obj->setColumnStretch(child.col, child.stretch); }

QWL_ADD_CHILD(QBoxLayout, QLayout*               , qwl::BoxLayout) { obj->addLayout(child, prop.stretch); }
QWL_ADD_CHILD(QBoxLayout, QWidget*               , qwl::BoxLayout) { obj->addWidget(child, prop.stretch, prop.align); }
QWL_ADD_CHILD(QBoxLayout, qwl::BoxLayout::Stretch, qwl::BoxLayout) { obj->addStretch(child.value); }

QWL_ADD_CHILD(QComboBox, const QString&) { obj->addItem(child); }

#endif // QWL_H

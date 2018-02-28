/** @file
 *****************************************************************************
 Macros for quick construction of interface and factory classes for non field
 agnostic gadgets.
 *****************************************************************************
 * @author     This file is part of libsnark, developed by SCIPR Lab
 *             and contributors (see AUTHORS).
 * @copyright  MIT license (see LICENSE file)
 *****************************************************************************/

#ifndef LIBSNARK_GADGETLIB3_INCLUDE_GADGETLIB3_GADGETMACROS_HPP_
#define LIBSNARK_GADGETLIB3_INCLUDE_GADGETLIB3_GADGETMACROS_HPP_

// The macro below counts the number of arguments sent with __VA_ARGS__
// it has not been tested yet. Due to a current MSVC bug it is not in use yet.

/** 
    Macro which creates Base classes for function specific gadgets. For instance
    CREATE_GADGET_BASE_CLASS(AND_GadgetBase) will create a base class which should be inherited by
    R1P_AND_Gadget and ANOTHER_FIELD_AND_Gadget. The Factory class which makes a field agnostic
    gadget will be created by the CREATE_GADGET_FACTORY_CLASS(AND_Gadget, ...) macro
*/
#define CREATE_GADGET_BASE_CLASS(GadgetBase)     \
class GadgetBase : virtual public Gadget {       \
protected:                                       \
    GadgetBase(ProtoboardPtr pb) : Gadget(pb) {} \
public:                                          \
    virtual ~GadgetBase() = 0;                   \
private:                                         \
    virtual void init() = 0;                     \
    DISALLOW_COPY_AND_ASSIGN(GadgetBase);        \
}; // class GadgetBase



/** 
    Macro for creating gadget factory classes. For instance
    CREATE_GADGET_FACTORY_CLASS(AND_Gadget, 2, VariableArray, input, Variable, result); creates a
    class AND_Gadget with the method:
    GadgetPtr AND_Gadget::create(ProtoboardPtr pb,
                                          const VariableArray& input,
                                          const Variable& result)
    which will instantiate a field specific gadget depending on the Protoboard type.
    This macro implements the factory design pattern.
*/
#define ADD_ELLIPSES_1(Type1, name1)                                                              \
    const Type1 & name1

#define ADD_ELLIPSES_2(Type1, name1, Type2, name2)                                                \
    const Type1 & name1, const Type2 & name2

#define ADD_ELLIPSES_3(Type1, name1, Type2, name2, Type3, name3)                                  \
    const Type1 & name1, const Type2 & name2, const Type3 & name3

#define ADD_ELLIPSES_4(Type1, name1, Type2, name2, Type3, name3, Type4, name4)                    \
    const Type1 & name1, const Type2 & name2, const Type3 & name3, const Type4 & name4

#define ADD_ELLIPSES_5(Type1, name1, Type2, name2, Type3, name3, Type4, name4, Type5, name5)      \
    const Type1 & name1, const Type2 & name2, const Type3 & name3, const Type4 & name4,           \
    const Type5 & name5

#define ADD_ELLIPSES_7(Type1, name1, Type2, name2, Type3, name3, Type4, name4, Type5, name5,      \
                       Type6, name6, Type7, name7, Type8, name8, Type9, name9)                    \
    const Type1 & name1, const Type2 & name2, const Type3 & name3, const Type4 & name4,           \
    const Type5 & name5, const Type6 & name6, const Type7 & name7

#define ADD_ELLIPSES_8(Type1, name1, Type2, name2, Type3, name3, Type4, name4, Type5, name5,      \
                       Type6, name6, Type7, name7, Type8, name8)                                  \
    const Type1 & name1, const Type2 & name2, const Type3 & name3, const Type4 & name4,           \
    const Type5 & name5, const Type6 & name6, const Type7 & name7, const Type8 & name8

#define ADD_ELLIPSES_9(Type1, name1, Type2, name2, Type3, name3, Type4, name4, Type5, name5,      \
                       Type6, name6, Type7, name7, Type8, name8, Type9, name9)                    \
    const Type1 & name1, const Type2 & name2, const Type3 & name3, const Type4 & name4,           \
    const Type5 & name5, const Type6 & name6, const Type7 & name7, const Type8 & name8,           \
    const Type9 & name9

/*  
    This was supposed to be a variadic macro CREATE_GADGET_FACTORY_CLASS(...) which invokes the
    correct number of arguments. Due to an MSVC bug and lack of time it will currently be copied
    with different names.
    Hopefully some day I will have time to find a workaround / use Boost preprocessor instead.
    The MSVC bug (feature...) is that __VA_ARGS__ passes to sub macros as 1 argument, so defining
    the following:
        #define CREATE_GADGET_FACTORY_CLASS(__VA_ARGS__) \
            CREATE_GADGET_FACTORY_CLASS_ ## PP_NARG(__VA_ARGS__)(__VA_ARGS__)
    will always create CREATE_GADGET_FACTORY_CLASS_1(__VA_ARGS__)
    Moreover, this macro considers __VA_ARGS__ to be only 1 argument!
*/
#define CREATE_GADGET_FACTORY_CLASS_1(GadgetType, Type1, name1)                                   \
class GadgetType {                                                                                \
public:                                                                                           \
    static GadgetPtr create(ProtoboardPtr pb, ADD_ELLIPSES_1(Type1, name1)) {                     \
		addGadgetToTraceback(#GadgetType);														  \
        GadgetPtr pGadget;                                                                        \
        if (pb->fieldType() == FieldType::R1P) {                                                  \
            pGadget.reset(new R1P_ ## GadgetType(pb, name1));                                     \
/* #ifndef REMOVE_LD2_CODE \\ Do not remove this comment, it is for an external deploy script */  \
        } else if (pb->fieldType() == FieldType::LD2) {                                           \
            pGadget.reset(new LD2_ ## GadgetType(pb, name1));                                     \
/* #endif \\ REMOVE_LD2_CODE, Do not remove this comment, it is for an external deploy script */  \
        } else {                                                                                  \
            GADGETLIB_FATAL("Attempted to create gadget of undefined Protoboard type.");          \
        }                                                                                         \
        pGadget->init();                                                                          \
		removeGadgetFromTraceback(#GadgetType);													  \
        return pGadget;                                                                           \
    }                                                                                             \
private:                                                                                          \
    DISALLOW_CONSTRUCTION(GadgetType);                                                            \
    DISALLOW_COPY_AND_ASSIGN(GadgetType);                                                         \
}; // class GadgetType

#define CREATE_GADGET_FACTORY_CLASS_2(GadgetType, Type1, name1, Type2, name2)                     \
class GadgetType {                                                                                \
public:                                                                                           \
    static GadgetPtr create(ProtoboardPtr pb,                                                     \
                                ADD_ELLIPSES_2(Type1, name1, Type2, name2)) {                     \
		addGadgetToTraceback(#GadgetType);														  \
        GadgetPtr pGadget;                                                                        \
        if (pb->fieldType() == FieldType::R1P) {                                                  \
            pGadget.reset(new R1P_ ## GadgetType(pb, name1, name2));                              \
/* #ifndef REMOVE_LD2_CODE \\ Do not remove this comment, it is for an external deploy script */  \
        } else if (pb->fieldType() == FieldType::LD2) {                                           \
            pGadget.reset(new LD2_ ## GadgetType(pb, name1, name2));                              \
/* #endif \\ REMOVE_LD2_CODE, Do not remove this comment, it is for an external deploy script */  \
        } else {                                                                                  \
            GADGETLIB_FATAL("Attempted to create gadget of undefined Protoboard type.");          \
        }                                                                                         \
        pGadget->init();                                                                          \
		removeGadgetFromTraceback(#GadgetType);													  \
        return pGadget;                                                                           \
    }                                                                                             \
private:                                                                                          \
    DISALLOW_CONSTRUCTION(GadgetType);                                                            \
    DISALLOW_COPY_AND_ASSIGN(GadgetType);                                                         \
}; // class GadgetType

#define CREATE_GADGET_FACTORY_CLASS_3(GadgetType, Type1, name1, Type2, name2, Type3, name3)       \
class GadgetType {                                                                                \
public:                                                                                           \
    static GadgetPtr create(ProtoboardPtr pb,													  \
                                ADD_ELLIPSES_3(Type1, name1, Type2, name2, Type3, name3)) {       \
		addGadgetToTraceback(#GadgetType);														  \
        GadgetPtr pGadget;                                                                        \
        if (pb->fieldType() == FieldType::R1P) {                                                  \
            pGadget.reset(new R1P_ ## GadgetType(pb, name1, name2, name3));                       \
/* #ifndef REMOVE_LD2_CODE \\ Do not remove this comment, it is for an external deploy script */  \
        } else if (pb->fieldType() == FieldType::LD2) {                                           \
            pGadget.reset(new LD2_ ## GadgetType(pb, name1, name2, name3));                       \
/* #endif \\ REMOVE_LD2_CODE, Do not remove this comment, it is for an external deploy script */  \
        } else {                                                                                  \
            GADGETLIB_FATAL("Attempted to create gadget of undefined Protoboard type.");          \
        }                                                                                         \
        pGadget->init();                                                                          \
		removeGadgetFromTraceback(#GadgetType);													  \
        return pGadget;                                                                           \
    }                                                                                             \
private:                                                                                          \
    DISALLOW_CONSTRUCTION(GadgetType);                                                            \
    DISALLOW_COPY_AND_ASSIGN(GadgetType);                                                         \
}; // class GadgetType

#define CREATE_GADGET_FACTORY_CLASS_4(GadgetType, Type1, name1, Type2, name2, Type3, name3,       \
                                      Type4, name4)                                               \
class GadgetType {                                                                                \
public:                                                                                           \
    static GadgetPtr create(ProtoboardPtr pb,													  \
                  ADD_ELLIPSES_4(Type1, name1, Type2, name2, Type3, name3, Type4, name4)) {       \
		addGadgetToTraceback(#GadgetType);														  \
        GadgetPtr pGadget;																		  \
        if (pb->fieldType() == FieldType::R1P) {                                                  \
            pGadget.reset(new R1P_ ## GadgetType(pb, name1, name2, name3, name4));                \
/* #ifndef REMOVE_LD2_CODE \\ Do not remove this comment, it is for an external deploy script */  \
        } else if (pb->fieldType() == FieldType::LD2) {                                           \
            pGadget.reset(new LD2_ ## GadgetType(pb, name1, name2, name3, name4));                \
/* #endif \\ REMOVE_LD2_CODE, Do not remove this comment, it is for an external deploy script */  \
        } else {                                                                                  \
            GADGETLIB_FATAL("Attempted to create gadget of undefined Protoboard type.");          \
        }                                                                                         \
        pGadget->init();                                                                          \
		removeGadgetFromTraceback(#GadgetType);													  \
        return pGadget;                                                                           \
    }                                                                                             \
private:                                                                                          \
    DISALLOW_CONSTRUCTION(GadgetType);                                                            \
    DISALLOW_COPY_AND_ASSIGN(GadgetType);                                                         \
}; // class GadgetType

#define CREATE_GADGET_FACTORY_CLASS_5(GadgetType, Type1, name1, Type2, name2, Type3, name3,       \
                                      Type4, name4, Type5, name5)                                 \
class GadgetType {                                                                                \
public:                                                                                           \
    static GadgetPtr create(ProtoboardPtr pb,                                                     \
                  ADD_ELLIPSES_5(Type1, name1, Type2, name2, Type3, name3, Type4, name4,          \
                                 Type5, name5)) {                                                 \
		addGadgetToTraceback(#GadgetType);														  \
        GadgetPtr pGadget;                                                                        \
        if (pb->fieldType() == FieldType::R1P) {                                                  \
            pGadget.reset(new R1P_ ## GadgetType(pb, name1, name2, name3, name4, name5));         \
/* #ifndef REMOVE_LD2_CODE \\ Do not remove this comment, it is for an external deploy script */  \
        } else if (pb->fieldType() == FieldType::LD2) {                                           \
            pGadget.reset(new LD2_ ## GadgetType(pb, name1, name2, name3, name4, name5));         \
/* #endif \\ REMOVE_LD2_CODE, Do not remove this comment, it is for an external deploy script */  \
        } else {                                                                                  \
            GADGETLIB_FATAL("Attempted to create gadget of undefined Protoboard type.");          \
        }                                                                                         \
        pGadget->init();                                                                          \
		removeGadgetFromTraceback(#GadgetType);													  \
        return pGadget;                                                                           \
    }                                                                                             \
private:                                                                                          \
    DISALLOW_CONSTRUCTION(GadgetType);                                                            \
    DISALLOW_COPY_AND_ASSIGN(GadgetType);                                                         \
}; // class GadgetType

#define CREATE_GADGET_FACTORY_CLASS_7(GadgetType, Type1, name1, Type2, name2, Type3, name3,       \
                                      Type4, name4, Type5, name5, Type6, name6, Type7, name7)     \
class GadgetType {                                                                                \
public:                                                                                           \
    static GadgetPtr create(ProtoboardPtr pb,                                                     \
                  ADD_ELLIPSES_7(Type1, name1, Type2, name2, Type3, name3, Type4, name4,          \
                                 Type5, name5, Type6, name6, Type7, name7)) {                     \
		addGadgetToTraceback(#GadgetType);														  \
        GadgetPtr pGadget;                                                                        \
        if (pb->fieldType() == FieldType::R1P) {                                                  \
            pGadget.reset(new R1P_ ## GadgetType(pb, name1, name2, name3, name4, name5, name6,    \
                                                 name7));                                         \
/* #ifndef REMOVE_LD2_CODE \\ Do not remove this comment, it is for an external deploy script */  \
        } else if (pb->fieldType() == FieldType::LD2) {                                           \
            pGadget.reset(new LD2_ ## GadgetType(pb, name1, name2, name3, name4, name5, name6,    \
                                                 name7));                                         \
/* #endif \\ REMOVE_LD2_CODE, Do not remove this comment, it is for an external deploy script */  \
        } else {                                                                                  \
            GADGETLIB_FATAL("Attempted to create gadget of undefined Protoboard type.");          \
        }                                                                                         \
        pGadget->init();                                                                          \
		removeGadgetFromTraceback(#GadgetType);													  \
        return pGadget;                                                                           \
    }                                                                                             \
private:                                                                                          \
    DISALLOW_CONSTRUCTION(GadgetType);                                                            \
    DISALLOW_COPY_AND_ASSIGN(GadgetType);                                                         \
}; // class GadgetType


#define CREATE_GADGET_FACTORY_CLASS_8(GadgetType, Type1, name1, Type2, name2, Type3, name3,       \
                                      Type4, name4, Type5, name5, Type6, name6, Type7, name7,     \
                                      Type8, name8)                                               \
class GadgetType {                                                                                \
public:                                                                                           \
    static GadgetPtr create(ProtoboardPtr pb,                                                     \
                  ADD_ELLIPSES_8(Type1, name1, Type2, name2, Type3, name3, Type4, name4,          \
                                 Type5, name5, Type6, name6, Type7, name7, Type8, name8)) {       \
		addGadgetToTraceback(#GadgetType);														  \
        GadgetPtr pGadget;                                                                        \
        if (pb->fieldType() == FieldType::R1P) {                                                  \
            pGadget.reset(new R1P_ ## GadgetType(pb, name1, name2, name3, name4, name5, name6,    \
                                                 name7, name8));                                  \
/* #ifndef REMOVE_LD2_CODE \\ Do not remove this comment, it is for an external deploy script */  \
        } else if (pb->fieldType() == FieldType::LD2) {                                           \
            pGadget.reset(new LD2_ ## GadgetType(pb, name1, name2, name3, name4, name5, name6,    \
                                                 name7, name8));                                  \
/* #endif \\ REMOVE_LD2_CODE, Do not remove this comment, it is for an external deploy script */  \
        } else {                                                                                  \
            GADGETLIB_FATAL("Attempted to create gadget of undefined Protoboard type.");          \
        }                                                                                         \
        pGadget->init();                                                                          \
		removeGadgetFromTraceback(#GadgetType);													  \
        return pGadget;                                                                           \
    }                                                                                             \
private:                                                                                          \
    DISALLOW_CONSTRUCTION(GadgetType);                                                            \
    DISALLOW_COPY_AND_ASSIGN(GadgetType);                                                         \
}; // class GadgetType

#define CREATE_GADGET_FACTORY_CLASS_9(GadgetType, Type1, name1, Type2, name2, Type3, name3,       \
                                      Type4, name4, Type5, name5, Type6, name6, Type7, name7,     \
                                      Type8, name8, Type9, name9)                                 \
class GadgetType {                                                                                \
public:                                                                                           \
    static GadgetPtr create(ProtoboardPtr pb,                                                     \
                  ADD_ELLIPSES_9(Type1, name1, Type2, name2, Type3, name3, Type4, name4,          \
                                 Type5, name5, Type6, name6, Type7, name7, Type8, name8,          \
                                 Type9, name9)) {                                                 \
		addGadgetToTraceback(#GadgetType);														  \
        GadgetPtr pGadget;                                                                        \
        if (pb->fieldType() == FieldType::R1P) {                                                  \
            pGadget.reset(new R1P_ ## GadgetType(pb, name1, name2, name3, name4, name5, name6,    \
                                                 name7, name8, name9));                           \
/* #ifndef REMOVE_LD2_CODE \\ Do not remove this comment, it is for an external deploy script */  \
        } else if (pb->fieldType() == FieldType::LD2) {                                           \
            pGadget.reset(new LD2_ ## GadgetType(pb, name1, name2, name3, name4, name5, name6,    \
                                                 name7, name8, name9));                           \
/* #endif \\ REMOVE_LD2_CODE, Do not remove this comment, it is for an external deploy script */  \
        } else {                                                                                  \
            GADGETLIB_FATAL("Attempted to create gadget of undefined Protoboard type.");          \
        }                                                                                         \
        pGadget->init();                                                                          \
		removeGadgetFromTraceback(#GadgetType);													  \
        return pGadget;                                                                           \
    }                                                                                             \
private:                                                                                          \
    DISALLOW_CONSTRUCTION(GadgetType);                                                            \
    DISALLOW_COPY_AND_ASSIGN(GadgetType);                                                         \
}; // class GadgetType

#endif // LIBSNARK_GADGETLIB3_INCLUDE_GADGETLIB3_GADGETMACROS_HPP_

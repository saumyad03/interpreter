#include "val.h"
// add op to this
Value Value::operator+(const Value& op) const {
    // two integers
    if (T == VINT && op.GetType() == VINT) return Value(Itemp + op.GetInt());
    // two reals
    if (T == VREAL && op.GetType() == VREAL) return Value(Rtemp + op.GetReal());
    // integer and real
    if (T == VINT && op.GetType() == VREAL) return Value(Itemp + op.GetReal());
    // real and integer
    if (T == VREAL && op.GetType() == VINT) return Value(Rtemp + op.GetInt());
    // none of the above
    return Value();
}

// subtract op from this
Value Value::operator-(const Value& op) const {
    // two integers
    if (T == VINT && op.GetType() == VINT) return Value(Itemp - op.GetInt());
    // two reals
    if (T == VREAL && op.GetType() == VREAL) return Value(Rtemp - op.GetReal());
    // integer and real
    if (T == VINT && op.GetType() == VREAL) return Value(Itemp - op.GetReal());
    // real and integer
    if (T == VREAL && op.GetType() == VINT) return Value(Rtemp - op.GetInt());
    // none of the above
    return Value();
}

// multiply this by op
Value Value::operator*(const Value& op) const {
    // two integers
    if (T == VINT && op.GetType() == VINT) return Value(Itemp * op.GetInt());
    // two reals
    if (T == VREAL && op.GetType() == VREAL) return Value(Rtemp * op.GetReal());
    // integer and real
    if (T == VINT && op.GetType() == VREAL) return Value(Itemp * op.GetReal());
    // real and integer
    if (T == VREAL && op.GetType() == VINT) return Value(Rtemp * op.GetInt());
    // none of the above
    return Value();
}

// divide this by op
Value Value::operator/(const Value& op) const {
    // two integers
    if (T == VINT && op.GetType() == VINT) return Value(Itemp / op.GetInt());
    // two reals
    if (T == VREAL && op.GetType() == VREAL) return Value(Rtemp / op.GetReal());
    // integer and real
    if (T == VINT && op.GetType() == VREAL) return Value(Itemp / op.GetReal());
    // real and integer
    if (T == VREAL && op.GetType() == VINT) return Value(Rtemp / op.GetInt());
    // none of the above
    return Value();
}

Value Value::operator==(const Value& op) const {
    // two integers
    if (T == VINT && op.GetType() == VINT) return Value(Itemp == op.GetInt());
    //two floats
    if (T == VREAL && op.GetType() == VREAL) return Value(Rtemp == op.GetReal());
    //int and float
    if (T == VINT && op.GetType() == VREAL) return Value(Itemp == op.GetReal());
    //float and int
    if (T == VREAL && op.GetType() == VINT) return Value(Rtemp == op.GetInt());
    //two booleans
    if (T == VBOOL && op.GetType() == VBOOL) return Value(Btemp == op.GetBool());
    // none of the above
    return Value();
}

Value Value::operator>(const Value& op) const {
    // two integers
    if (T == VINT && op.GetType() == VINT) return Value(Itemp > op.GetInt());
    //two floats
    if (T == VREAL && op.GetType() == VREAL) return Value(Rtemp > op.GetReal());
    //int and float
    if (T == VINT && op.GetType() == VREAL) return Value(Itemp > op.GetReal());
    //float and int
    if (T == VREAL && op.GetType() == VINT) return Value(Rtemp > op.GetInt());
    //two booleans
    if (T == VBOOL && op.GetType() == VBOOL) return Value(Btemp > op.GetBool());
    // none of the above
    return Value();
}

Value Value::operator<(const Value& op) const {
    // two integers
    if (T == VINT && op.GetType() == VINT) return Value(Itemp < op.GetInt());
    //two floats
    if (T == VREAL && op.GetType() == VREAL) return Value(Rtemp < op.GetReal());
    //int and float
    if (T == VINT && op.GetType() == VREAL) return Value(Itemp < op.GetReal());
    //float and int
    if (T == VREAL && op.GetType() == VINT) return Value(Rtemp < op.GetInt());
    //two booleans
    if (T == VBOOL && op.GetType() == VBOOL) return Value(Btemp < op.GetBool());
    // none of the above
    return Value();
}

Value Value::operator&&(const Value& op) const {
    // boolean and boolean
    if (T == VBOOL && op.GetType() == VBOOL) return Value(Btemp && op.GetBool());
    // none of the above
    return Value();
}

Value Value::operator||(const Value& op) const {
    // boolean and boolean
    if (T == VBOOL && op.GetType() == VBOOL) return Value(Btemp || op.GetBool());
    // none of the above
    return Value();
}

Value Value::operator!() const { //NOT Unary operator
    // boolean
    if (T == VBOOL) return Value(!Btemp);
    // none of the above
    return Value();
}
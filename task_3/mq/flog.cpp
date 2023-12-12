#pragma GCC diagnostic ignored "-Wcast-qual"
#pragma GCC diagnostic ignored "-Wformat="
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wformat-extra-args"

#include "flog.hpp"

FILE* logOutf = NULL;

// i - int
// x - long long
// c - char
// f - float
// d - double
// e - long double
// Pc - char*
// P + varType = varType*
//unsigned types
// j - int
// y - long long
// h - char

const unsigned int HashMult = 107u;

unsigned int countHash (void* from, void* to) {

    assert (from != NULL);
    assert (to != NULL);
    assert (from <= to);

    unsigned int hash = 0;

    for (; from < to; from = (char*) from + 1) {

        hash *= HashMult;
        hash += *(unsigned char*)from;
    }

    return hash;
}

void flogIntern (const void* val, const char* varType, const char* varName, size_t varSize, const char* fileName, const char* funcName, size_t line) {

    assert (val != NULL);
    assert (varType != NULL);
    assert (varName != NULL);
    assert (varSize > 0);
    assert (fileName != NULL);
    assert (line > 0);

    fprintf (logOutf, "<pre>In file %s, function %s, line %u : %s = ", fileName, funcName, line, varName);

    if (!strcmp (varType, "Pc") || !strcmp (varType, "PKc")) fprintf (logOutf, "<%s>", *((const char**) val));
    else if (varType[0] == 'P') fprintf (logOutf, "0x%p", *(void**)val);

    else if (!strcmp (varType, "i") || !strcmp (varType, "Ki")) fprintf (logOutf, "%d"   , *( ( const int*)                val));
    else if (!strcmp (varType, "x") || !strcmp (varType, "Kx")) fprintf (logOutf, "%lld" , *( ( const long long*)          val));
    else if (!strcmp (varType, "c") || !strcmp (varType, "Kc")) fprintf (logOutf, "%hhd" , *( ( const char*)               val));
    else if (!strcmp (varType, "f") || !strcmp (varType, "Kf")) fprintf (logOutf, "%g"   , *( ( const float*)              val));
    else if (!strcmp (varType, "d") || !strcmp (varType, "Kd")) fprintf (logOutf, "%lg"  , *( ( const double*)             val));
    else if (!strcmp (varType, "j") || !strcmp (varType, "Kj")) fprintf (logOutf, "%u"   , *( ( const unsigned int*)       val));
    else if (!strcmp (varType, "y") || !strcmp (varType, "Ky")) fprintf (logOutf, "%llu" , *( ( const unsigned long long*) val));
    else if (!strcmp (varType, "h") || !strcmp (varType, "Kh")) fprintf (logOutf, "%hhu" , *( ( const unsigned char*)      val));
    else {
        for (int i = 0; i < varSize; i++) fprintf (logOutf, "%hhX", *((char*) val + i));
        fprintf (logOutf, " :: UNDEFINED_FORMAT : sizeof() = %u", varSize);
    }
    fprintf (logOutf, ";<br></pre>\n");
}

const char* getFormatIntern (const char* varType) {

    assert (varType != NULL);

    if (!strcmp (varType, "Pc") || !strcmp (varType, "PKc"))    return "%s";
    else if (varType[0] == 'P') return "%p";

    else if (!strcmp (varType, "i") || !strcmp (varType, "Ki")) return "%d"  ;
    else if (!strcmp (varType, "x") || !strcmp (varType, "Kx")) return "%lld";
    else if (!strcmp (varType, "c") || !strcmp (varType, "Kc")) return "%hhd";
    else if (!strcmp (varType, "f") || !strcmp (varType, "Kf")) return "%g"  ;
    else if (!strcmp (varType, "d") || !strcmp (varType, "Kd")) return "%lg" ;
    else if (!strcmp (varType, "j") || !strcmp (varType, "Kj")) return "%u"  ;
    else if (!strcmp (varType, "y") || !strcmp (varType, "Ky")) return "%llu";
    else if (!strcmp (varType, "h") || !strcmp (varType, "Kh")) return "%hhu";
    else return "%p";
}
 
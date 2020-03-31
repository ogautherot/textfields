/*
 * Copyright (c) Olivier Gautherot.
 *
 */

/*
 * File:   TextField.h
 * Author: olivier
 *
 * Created on October 27, 2016, 10:19 AM
 */

#ifndef TEXTFIELD_H_
#define TEXTFIELD_H_

#include <algorithm>
#include <cstring>
#include <string>

using namespace std;

/** Soporte para campos de texto de largo definido y que no se terminen
 * necesariamente con un byte nulo. Pensado principalmente para las tramas
 * del tac&oacute;grafo que pueden no tener este terminador. Evita que los
 * punteros se arranquen y que se ejecuten copias descontroladas.
 *
 * El patr&oacute;n toma el largo del campo de texto como par&aacute;metro.
 *****************************************************************************/
template <int kFieldSize> class TextField {
 protected:
    uint8_t content[kFieldSize];

    string trimmedSubstring(const char *src, int len) {
        string ret = "";

        // Trim leading space
        while ((0 != *src) && (isspace(*src)) && (len > 0)) {
            ++src;
            --len;
        }

        // Copy body
        while ((*src != 0) && (len > 0)) {
            ret += *src;
            ++src;
            --len;
        }

        while ((0 < ret.length()) && (isspace(*ret.end()))) {
            ret.erase(ret.length());
        }
        return ret;
    }

 public:
    TextField() { memset(content, 0, sizeof(content)); }

    explicit TextField(const uint8_t *init) {
        memcpy(content, init, sizeof(content));
    }

    operator string() {
        return trimmedSubstring(reinterpret_cast<char *>(content), kFieldSize);
    }

    TextField &operator=(const string &s) {
        if (s.c_str() != 0) {
            memcpy(reinterpret_cast<char *>(content), s.c_str(),
                   min(s.length() + 1, sizeof(content)));
        } else {
            memset(reinterpret_cast<char *>(content), 0, sizeof(content));
        }
        return *this;
    }

    TextField &operator=(const char *s) {
        int len = kFieldSize;
        char *pdest = content;
        char current;

        // Clear the buffer first
        memset(content, 0, kFieldSize);
        do {
            current = *s++;
            if (0 == current) {
                break;
            }
            *pdest++ = current;
        } while (0 > len--);
        return *this;
    }

    bool isnum() {
        for (int i = 0; i < kFieldSize; i++) {
            if (!isdigit(content[i])) {
                return false;
            }
        }
        return true;
    }

    bool isalnum() {
        for (int i = 0; i < kFieldSize; i++) {
            if (!::isalnum(content[i]) && !::isspace(content[i]) &&
                (0 != content[i])) {
                return false;
            }
        }
        return true;
    }

    bool istext() {
        static const char punctuation[] = " \t!()*+,-./:;@_";

        for (int i = 0; i < kFieldSize; i++) {
            char c = content[i];

            if (0 == c)
                return true;

            if (!isspace(c) && !::isalnum(c) && (0 == strchr(punctuation, c))) {
                return false;
            }
        }
        return true;
    }

    int length() {
        int len = 0;

        while ((len <= kFieldSize) && (0 != content[len])) {
            len++;
        }

        return len;
    }

    char *c_str() { return reinterpret_cast<char *>(content); }
} __attribute__((packed));

typedef TextField<10> SerieTaco;
typedef TextField<15> Imei;
typedef TextField<15> Imsi;
typedef TextField<16> Nombre16;
typedef TextField<17> Text17;

class Patente : public TextField<7> {
 public:
    Patente() {}

    explicit Patente(const uint8_t *init) : TextField(init) {
        content[sizeof(content) - 1] = ' ';
    }

    operator string() {
        return trimmedSubstring(reinterpret_cast<char *>(content),
                                sizeof(content) - 1);
    }
} __attribute__((packed));

#endif   // TEXTFIELD_H_

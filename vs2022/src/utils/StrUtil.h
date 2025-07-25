#include <stdlib.h>
#include <string.h>

struct ByteSlice {
    uint8_t* d = nullptr;
    size_t sz = 0;
    bool owned = false; // tracks if we own the memory

    ByteSlice() = default;
    ~ByteSlice() { 
        if (owned) {
            free(d);
        }
    }

    // Non-owning constructors
    ByteSlice(const char* str) {
        d = (uint8_t*)str;
        sz = strlen(str);
        owned = false;
    }
    ByteSlice(char* str) {
        d = (uint8_t*)str;
        sz = strlen(str);
        owned = false;
    }
    ByteSlice(const uint8_t* data, size_t size) {
        d = (uint8_t*)data;
        sz = size;
        owned = false;
    }

    // Copy constructor - creates a new copy
    ByteSlice(const ByteSlice& other) {
        if (other.empty()) {
            d = nullptr;
            sz = 0;
            owned = false;
            return;
        }
        d = (uint8_t*)memdup(other.d, other.sz);
        sz = other.sz;
        owned = true;
    }

    // Copy assignment - creates a new copy
    ByteSlice& operator=(const ByteSlice& other) {
        if (this != &other) {
            if (owned) {
                free(d);
            }
            if (other.empty()) {
                d = nullptr;
                sz = 0;
                owned = false;
                return *this;
            }
            d = (uint8_t*)memdup(other.d, other.sz);
            sz = other.sz;
            owned = true;
        }
        return *this;
    }

    // Move constructor
    ByteSlice(ByteSlice&& other) noexcept {
        d = other.d;
        sz = other.sz;
        owned = other.owned;
        other.d = nullptr;
        other.sz = 0;
        other.owned = false;
    }

    // Move assignment
    ByteSlice& operator=(ByteSlice&& other) noexcept {
        if (this != &other) {
            if (owned) {
                free(d);
            }
            d = other.d;
            sz = other.sz;
            owned = other.owned;
            other.d = nullptr;
            other.sz = 0;
            other.owned = false;
        }
        return *this;
    }

    void Set(uint8_t* data, size_t size) {
        if (owned) {
            free(d);
        }
        d = data;
        sz = size;
        owned = true;  // Taking ownership
    }

    void Set(char* data, size_t size) {
        Set((uint8_t*)data, size);
    }

    uint8_t* data() const {
        return d;
    }

    uint8_t* Get() const {
        return d;
    }

    size_t size() const {
        return sz;
    }

    int Size() const {
        return (int)sz;
    }

    bool empty() const {
        return !d;
    }

    bool IsEmpty() const {
        return !d;
    }

    // Creates an owned copy
    ByteSlice Clone() const {
        if (empty()) {
            return {};
        }
        ByteSlice result;
        result.d = (uint8_t*)memdup(d, sz);
        result.sz = sz;
        result.owned = true;
        return result;
    }

    void Free() {
        if (owned) {
            free(d);
        }
        d = nullptr;
        sz = 0;
        owned = false;
    }

    operator const char*() {
        return (const char*)d;
    }
};

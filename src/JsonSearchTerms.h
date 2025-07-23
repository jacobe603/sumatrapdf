/* Copyright 2022 the SumatraPDF project authors (see AUTHORS file).
   License: GPLv3 */

// Search terms functionality interface

struct KeySearchTerm {
    char* text;        // Simple pointer - no dynamic management for now
    COLORREF color;
    
    KeySearchTerm() : text(nullptr), color(0) {}
    // No destructor - avoiding dynamic memory management for debugging
};

// Access functions for configurable search terms
const KeySearchTerm* GetKeySearchTerms();
int GetKeySearchTermsCount();
void ReloadSearchTermsFromFile();

void ShowLoadSearchTermsDialog(void* tab);
void ClearKeyTermHighlights(void* win);
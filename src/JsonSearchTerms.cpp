/* Copyright 2022 the SumatraPDF project authors (see AUTHORS file).
   License: GPLv3 */

// Simplified search terms - step by step debugging

#include "utils/BaseUtil.h"
#include "utils/FileUtil.h"
#include "AppSettings.h"
#include "JsonSearchTerms.h"

// External implementation function (defined in SumatraPDF.cpp)
extern void CreateHighlightAnnotationsForKeyTerms(void* tabPtr);

// Ultra-simple static terms for now - avoid all dynamic allocation
static const struct { const char* text; COLORREF color; } gStaticTerms[] = {
    {"SCHWAB", RGB(255, 255, 0)},    // yellow
    {"Chiller", RGB(255, 165, 0)},   // orange
    {"Warranty", RGB(255, 0, 0)},    // red
    {"Safety", RGB(0, 255, 0)},      // green
    {"Service", RGB(0, 0, 255)},     // blue
    {"Motor", RGB(128, 0, 128)}      // purple
};

// Simplified test function - avoid complex path operations that may cause crashes
void ReloadSearchTermsFromFile() {
    // For now, just show that the function is being called without doing file operations
    // This eliminates the crash until we can properly debug the path/memory issues
    MessageBoxA(nullptr, "ReloadSearchTermsFromFile called successfully.\nUsing static terms for now.", "Search Terms", MB_OK);
    
    // Future implementation would read from file when path operations are debugged
    // The crash appears to be in AutoFreeStr or path manipulation functions
}

// Access functions - return static terms for now
const KeySearchTerm* GetKeySearchTerms() {
    // Convert static terms to KeySearchTerm format
    static KeySearchTerm staticKeyTerms[dimof(gStaticTerms)];
    static bool initialized = false;
    
    if (!initialized) {
        for (size_t i = 0; i < dimof(gStaticTerms); i++) {
            staticKeyTerms[i].text = (char*)gStaticTerms[i].text; // Cast away const for compatibility
            staticKeyTerms[i].color = gStaticTerms[i].color;
        }
        initialized = true;
    }
    
    return staticKeyTerms;
}

int GetKeySearchTermsCount() {
    return dimof(gStaticTerms);
}

void ShowLoadSearchTermsDialog(void* tabPtr) {
    // Simple static dialog for now
    const char* message = 
        "Auto-highlighting 6 static search terms:\n\n"
        "• SCHWAB (yellow)\n• Chiller (orange)\n• Warranty (red)\n"
        "• Safety (green)\n• Service (blue)\n• Motor (purple)\n\n"
        "This will create persistent highlight annotations.\n\n"
        "Click OK to start highlighting.";
    
    int result = MessageBoxA(nullptr, message, "Highlight Key Terms", MB_OKCANCEL | MB_ICONINFORMATION);
    
    if (result == IDOK) {
        // Check if tabPtr is valid before passing it
        if (!tabPtr) {
            MessageBoxA(nullptr, "Error: No document tab available", "Highlight Key Terms", MB_OK);
            return;
        }
        
        // Call the safe implementation
        CreateHighlightAnnotationsForKeyTerms(tabPtr);
    }
}

// Stub function for future functionality
void ClearKeyTermHighlights(void* win) {
    // Future implementation would clear any active highlights
}

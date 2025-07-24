/* Copyright 2022 the SumatraPDF project authors (see AUTHORS file).
   License: GPLv3 */

// Simplified search terms - step by step debugging

#include "utils/BaseUtil.h"
#include "utils/FileUtil.h"
#include "utils/JsonParser.h"
#include "utils/WinUtil.h"
#include "AppSettings.h"
#include "JsonSearchTerms.h"

#include <algorithm> // for std::min

// External implementation function (defined in SumatraPDF.cpp)
extern void CreateHighlightAnnotationsForKeyTerms(void* tabPtr);

// Dynamic storage for loaded JSON data
static Vec<KeySearchTerm> gLoadedTerms;
static bool gJsonLoaded = false;

// Helper function to parse hex color string like "#FF0000" to COLORREF
static COLORREF ParseHexColor(const char* hexStr) {
    if (!hexStr || hexStr[0] != '#' || strlen(hexStr) != 7) {
        return RGB(255, 0, 0); // Default to red on parse error
    }
    
    int r, g, b;
    if (sscanf_s(hexStr + 1, "%02x%02x%02x", &r, &g, &b) == 3) {
        return RGB(r, g, b);
    }
    return RGB(255, 0, 0); // Default to red on parse error
}

// JSON visitor to extract array of search terms
class MultiTermVisitor : public json::ValueVisitor {
private:
    KeySearchTerm currentTerm{};
    int currentIndex = -1;
    
public:
    bool Visit(const char* path, const char* value, json::Type type) override {
        // Parse paths like "/searchTerms[0]/term" and "/searchTerms[0]/color"
        if (str::StartsWith(path, "/searchTerms[") && type == json::Type::String) {
            // Extract index from path like "/searchTerms[0]/term"
            const char* indexStart = path + 13; // Skip "/searchTerms["
            const char* indexEnd = str::Find(indexStart, "]");
            if (indexEnd) {
                int index = atoi(str::DupTemp(indexStart, indexEnd - indexStart));
                
                // Check if this is a new term (new index)
                if (index != currentIndex) {
                    // Save previous term if valid
                    if (currentIndex >= 0 && currentTerm.text) {
                        gLoadedTerms.Append(currentTerm);
                    }
                    // Start new term
                    currentIndex = index;
                    currentTerm = KeySearchTerm{}; // Reset
                }
                
                // Check if this is term or color field
                const char* field = indexEnd + 2; // Skip "]/"
                if (str::Eq(field, "term")) {
                    free(currentTerm.text); // Free any existing
                    currentTerm.text = str::Dup(value);
                }
                else if (str::Eq(field, "color")) {
                    currentTerm.color = ParseHexColor(value);
                }
            }
        }
        return true; // Continue parsing
    }
    
    // Call this after parsing is complete to save the last term
    void Finalize() {
        if (currentIndex >= 0 && currentTerm.text) {
            gLoadedTerms.Append(currentTerm);
        }
    }
};

// Ultra-simple static terms for now - avoid all dynamic allocation
static const struct { const char* text; COLORREF color; } gStaticTerms[] = {
    {"SCHWAB", RGB(255, 255, 0)},    // yellow
    {"Chiller", RGB(255, 165, 0)},   // orange
    {"Warranty", RGB(255, 0, 0)},    // red
    {"Safety", RGB(0, 255, 0)},      // green
    {"Service", RGB(0, 0, 255)},     // blue
    {"Motor", RGB(128, 0, 128)}      // purple
};

// Load search terms from JSON file
void ReloadSearchTermsFromFile() {
    // Clear any existing loaded terms
    for (KeySearchTerm& term : gLoadedTerms) {
        free(term.text);
    }
    gLoadedTerms.Reset();
    gJsonLoaded = false;
    
    // Get path to JSON file in same directory as executable
    TempStr exeDir = GetSelfExeDirTemp();
    TempStr jsonPath = path::JoinTemp(exeDir, "search-terms.json");
    
    // Try to read the JSON file
    ByteSlice data = file::ReadFile(jsonPath);
    if (!data.data()) {
        MessageBoxA(nullptr, "JSON file 'search-terms.json' not found in exe directory.\nUsing default static terms.", "Info", MB_OK);
        return;
    }
    
    // Parse JSON with our visitor
    MultiTermVisitor visitor;
    bool parseSuccess = json::Parse((const char*)data.data(), &visitor);
    visitor.Finalize(); // Save the last term
    data.Free();
    
    if (parseSuccess && gLoadedTerms.Size() > 0) {
        gJsonLoaded = true;
        
        // Show debug info about what was loaded
        char debugMsg[512];
        sprintf_s(debugMsg, sizeof(debugMsg), 
            "JSON loaded successfully!\n\nLoaded %d search terms:\n",
            (int)gLoadedTerms.Size());
        
        // Add first few terms to debug message
        int maxShow = std::min(3, (int)gLoadedTerms.Size());
        for (int i = 0; i < maxShow; i++) {
            const KeySearchTerm& term = gLoadedTerms[i];
            int r = GetRValue(term.color);
            int g = GetGValue(term.color);  
            int b = GetBValue(term.color);
            char termInfo[128];
            sprintf_s(termInfo, sizeof(termInfo), "\n- %s (RGB %d,%d,%d)", term.text, r, g, b);
            strcat_s(debugMsg, sizeof(debugMsg), termInfo);
        }
        if (gLoadedTerms.Size() > maxShow) {
            strcat_s(debugMsg, sizeof(debugMsg), "\n...");
        }
        
        MessageBoxA(nullptr, debugMsg, "JSON Loaded", MB_OK);
    } else {
        MessageBoxA(nullptr, "Failed to parse JSON file or no terms found.\nUsing default static terms.", "JSON Parse Error", MB_OK);
        gJsonLoaded = false;
    }
}

// Access functions - return loaded JSON data if available, otherwise static terms
const KeySearchTerm* GetKeySearchTerms() {
    // If JSON was loaded successfully, return the loaded terms
    if (gJsonLoaded && gLoadedTerms.Size() > 0) {
        return gLoadedTerms.els;
    }
    
    // Fallback to static terms
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
    // If JSON was loaded, return actual count
    if (gJsonLoaded && gLoadedTerms.Size() > 0) {
        return (int)gLoadedTerms.Size();
    }
    
    // Fallback to static terms count
    return dimof(gStaticTerms);
}

void ShowLoadSearchTermsDialog(void* tabPtr) {
    char message[512];
    
    if (gJsonLoaded && gLoadedTerms.Size() > 0) {
        // Show loaded JSON terms
        sprintf_s(message, sizeof(message),
            "Auto-highlighting %d loaded JSON search terms:\n\n",
            (int)gLoadedTerms.Size());
            
        // Add first few terms to message
        int maxShow = std::min(4, (int)gLoadedTerms.Size());
        for (int i = 0; i < maxShow; i++) {
            const KeySearchTerm& term = gLoadedTerms[i];
            int r = GetRValue(term.color);
            int g = GetGValue(term.color);  
            int b = GetBValue(term.color);
            char termInfo[128];
            sprintf_s(termInfo, sizeof(termInfo), "- %s (RGB %d,%d,%d)\n", term.text, r, g, b);
            strcat_s(message, sizeof(message), termInfo);
        }
        if (gLoadedTerms.Size() > maxShow) {
            strcat_s(message, sizeof(message), "...\n");
        }
        
        strcat_s(message, sizeof(message), 
            "\nThis will create persistent highlight annotations.\n\n"
            "Click OK to start highlighting.");
    } else {
        // Fallback to static terms message
        strcpy_s(message, sizeof(message),
            "Auto-highlighting 6 static search terms:\n\n"
            "- SCHWAB (yellow)\n- Chiller (orange)\n- Warranty (red)\n"
            "- Safety (green)\n- Service (blue)\n- Motor (purple)\n\n"
            "This will create persistent highlight annotations.\n\n"
            "Click OK to start highlighting.");
    }
    
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

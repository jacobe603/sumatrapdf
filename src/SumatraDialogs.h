/* Copyright 2022 the SumatraPDF project authors (see AUTHORS file).
   License: GPLv3 */

struct GlobalPrefs;

char* Dialog_GoToPage(HWND hwnd, const char* currentPageLabel, int pageCount, bool onlyNumeric = true);
char* Dialog_Find(HWND hwnd, const char* previousSearch, bool* matchCase);
char* Dialog_GetPassword(HWND hwnd, const char* fileName, bool* rememberPassword);
INT_PTR Dialog_PdfAssociate(HWND hwnd, bool* dontAskAgainOut);
const char* Dialog_ChangeLanguge(HWND hwnd, const char* currLangCode);
bool Dialog_CustomZoom(HWND hwnd, bool forChm, float* currZoomInOut);
INT_PTR Dialog_Settings(HWND hwnd, GlobalPrefs* prefs);
bool Dialog_AddFavorite(HWND hwnd, const char* pageNo, AutoFreeStr& favName);

// Memory-safe page range data structure (JSON pattern - stack allocated)
struct PageRangeData {
    int pages[1000];        // Fixed array - no dynamic allocation
    int count;              // Simple count tracking  
    char inputText[256];    // Copy of user input for debugging
    bool isValid;           // Simple validation flag
};

// Page extraction dialog and utilities
int ParseSinglePage(const char* input, int totalPages);
char* GetPageNumberFromUser(HWND hwnd, int pageCount, int currentPage);
char* GetPageRangeFromUser(HWND hwnd, int pageCount, int currentPage);
bool ParsePageRangesSafe(const char* input, int totalPages, PageRangeData* data);
Vec<int>* ParsePageRangeString(const char* input, int totalPages); // DEPRECATED: Use ParsePageRangesSafe() instead
Vec<int>* ParsePageRanges(const char* input, int totalPages); // DEPRECATED
char* Dialog_ExtractPages(HWND hwnd, int pageCount, int currentPage);

enum class PrintRangeAdv { All = 0, Even, Odd };
enum class PrintScaleAdv { None = 0, Shrink, Fit };
enum class PrintRotationAdv { Auto = 0, Portrait, Landscape };

struct Print_Advanced_Data {
    PrintRangeAdv range;
    PrintScaleAdv scale;
    PrintRotationAdv rotation;
    bool autoRotate;

    explicit Print_Advanced_Data(PrintRangeAdv range = PrintRangeAdv::All, PrintScaleAdv scale = PrintScaleAdv::Shrink,
                                 PrintRotationAdv rotation = PrintRotationAdv::Auto, bool autoRotate = true)
        : range(range), scale(scale), rotation(rotation), autoRotate(autoRotate) {
    }
};

HPROPSHEETPAGE CreatePrintAdvancedPropSheet(Print_Advanced_Data* data, ScopedMem<DLGTEMPLATE>& dlgTemplate);

TempStr ZoomLevelStr(float zoom);

# SumatraPDF Automatic Key Terms Highlighting System

## Overview

This guide documents the implementation of an automatic highlighting system for SumatraPDF that searches for predefined key terms throughout PDF documents and creates persistent color-coded highlight annotations.

## Features

✅ **Multi-term search and highlighting** - 6 configurable search terms with unique colors  
✅ **Document-wide coverage** - Searches all pages in the document  
✅ **Persistent annotations** - Creates real PDF annotations that save with the file  
✅ **Color-coded system** - Each term gets a distinct highlight color  
✅ **Crash-safe implementation** - Avoids memory corruption issues  
✅ **Performance limits** - Smart limits to prevent system overload  
✅ **Easy access** - Integrated into File menu with simple dialog  

## User Interface

### Access Method
- **Menu Location**: File → "Highlight Key Terms"
- **Keyboard Shortcut**: None (can be added via accelerator table)

### Process Flow
1. User opens a PDF document in SumatraPDF
2. User selects "Highlight Key Terms" from File menu
3. Dialog shows the 6 terms to be highlighted with their colors
4. User clicks OK to confirm
5. System searches entire document and creates color-coded highlights
6. Results dialog shows number of annotations created
7. Highlights are immediately visible and saved with the PDF

## Technical Implementation

### Core Architecture

The implementation uses a **direct annotation creation approach** that bypasses problematic selection manipulation functions, ensuring stability and performance.

### Key Files Modified

#### 1. `src/JsonSearchTerms.h` - Interface Definition
```cpp
struct KeySearchTerm {
    char* text;        // Search term text
    COLORREF color;    // Windows color value
};

// Access functions
const KeySearchTerm* GetKeySearchTerms();
int GetKeySearchTermsCount();
void ShowLoadSearchTermsDialog(void* tab);
```

#### 2. `src/JsonSearchTerms.cpp` - Search Terms and Dialog
```cpp
// Static search terms with color definitions
static const struct { const char* text; COLORREF color; } gStaticTerms[] = {
    {"SCHWAB", RGB(255, 255, 0)},    // Yellow
    {"Chiller", RGB(255, 165, 0)},   // Orange
    {"Warranty", RGB(255, 0, 0)},    // Red
    {"Safety", RGB(0, 255, 0)},      // Green
    {"Service", RGB(0, 0, 255)},     // Blue
    {"Motor", RGB(128, 0, 128)}      // Purple
};
```

#### 3. `src/SumatraPDF.cpp` - Main Implementation
- **Function**: `CreateHighlightAnnotationsForKeyTerms(void* tabPtr)`
- **Location**: Lines 4413-4535
- **Purpose**: Core highlighting logic using safe direct approach

#### 4. Integration Files
- **`src/Commands.h`**: Added `CmdHighlightKeyTerms` command (line 194)
- **`src/Menu.cpp`**: Added menu item (lines 123-126)
- **`premake5.files.lua`**: Added JsonSearchTerms files to build (line 652)

### Core Algorithm

```cpp
// High-level algorithm
for each page in document:
    for each search term:
        search for term on current page
        for each match found:
            extract text bounds from search result
            create highlight annotation with term's color
            position annotation over found text
            increment counter
        cleanup search resources
update UI and show results
```

### Critical Technical Decisions

#### 1. **Direct Engine API Usage**
**Problem**: `MakeAnnotationsFromSelection()` caused heap corruption  
**Solution**: Use `EngineMupdfCreateAnnotation()` directly
```cpp
// SAFE - Direct engine call (same as Menu.cpp uses)
Annotation* annot = EngineMupdfCreateAnnotation(engine, pageNo, PointF{}, &args);
```

#### 2. **Manual Coordinate Conversion**
**Problem**: `ToRectF()` function caused type definition conflicts  
**Solution**: Manual conversion from `Rect` to `RectF`
```cpp
// Convert search result coordinates manually
Rect r = result->rects[i];
RectF rf{(float)r.x, (float)r.y, (float)r.dx, (float)r.dy};
```

#### 3. **Proper Color Structure Setup**
**Problem**: Colors all appeared yellow due to incomplete `ParsedColor` setup  
**Solution**: Set all required `ParsedColor` fields
```cpp
args.col.wasParsed = true;
args.col.parsedOk = true;
args.col.col = term.color;  // COLORREF
args.col.pdfCol = MkPdfColor(r, g, b, 255);  // PdfColor
```

#### 4. **Memory Safety and Performance**
**Problem**: Risk of memory exhaustion with large documents  
**Solution**: Conservative limits and immediate cleanup
```cpp
const int MAX_ANNOTATIONS = 50;  // Prevent memory issues
// Immediate cleanup after each search
free(wideSearchTerm);
dm->textSearch->Reset();
```

### Key Functions Used

#### Text Search Functions
- **`TextSearch::Reset()`** - Clear search state
- **`TextSearch::SetSensitive(false)`** - Case-insensitive search
- **`TextSearch::FindFirst(pageNo, term)`** - Find first occurrence on page
- **`TextSearch::FindNext()`** - Find subsequent occurrences

#### Annotation Creation Functions
- **`EngineMupdfCreateAnnotation(engine, pageNo, pos, args)`** - Create annotation
- **`SetQuadPointsAsRect(annot, rects)`** - Position highlight over text
- **`GetBounds(annot)`** - Calculate annotation bounds
- **`EngineSupportsAnnotations(engine)`** - Validate annotation support

#### UI Update Functions
- **`MainWindowRerender(win)`** - Refresh document display
- **`ToolbarUpdateStateForWindow(win, true)`** - Update toolbar state
- **`FindMainWindowByTab(tab)`** - Get window from tab

#### Utility Functions
- **`ToWStr(text)`** - Convert to wide string for search
- **`GetRValue/GetGValue/GetBValue(color)`** - Extract RGB components
- **`MkPdfColor(r, g, b, a)`** - Create PDF color value

### Error Handling and Safety

#### Validation Checks
```cpp
// Comprehensive validation
if (!tabPtr) return;  // Valid tab pointer
if (!tab || !tab->win) return;  // Valid tab and window
if (!dm || !dm->GetEngine()) return;  // Valid document model
if (!EngineSupportsAnnotations(engine)) return;  // Annotation support
if (!dm->textSearch) return;  // Text search availability
```

#### Memory Management
- **Immediate cleanup**: `free(wideSearchTerm)` after each search
- **State reset**: `dm->textSearch->Reset()` prevents corruption
- **Bounds checking**: Annotation limit prevents memory exhaustion
- **No complex allocations**: Avoid problematic `SelectionOnPage` creation

#### Performance Safeguards
- **50 annotation limit**: Prevents system overload
- **Page-by-page processing**: Reduces memory pressure  
- **Early termination**: Stop when limit reached with proper cleanup

## Build Integration

### Premake5 Configuration
```lua
-- In premake5.files.lua
files_in_dir("src", {
    -- ...existing files...
    "JsonSearchTerms.*",  -- Added for highlighting system
    -- ...more files...
})
```

### Visual Studio Build Process
1. **Generate projects**: `premake5 vs2022`
2. **Open solution**: `vs2022/SumatraPDF.sln`
3. **Build configuration**: Release x64 (or Debug for testing)
4. **Output**: Executable with highlighting functionality

## Testing and Validation

### Test Cases
1. **Basic functionality**: Open PDF, highlight terms, verify colors
2. **Multi-page documents**: Ensure all pages are searched
3. **Large documents**: Verify 50-annotation limit works
4. **No matches**: Handle documents without search terms gracefully
5. **Multiple instances**: Ensure same term highlighted multiple times
6. **Persistence**: Save PDF and verify annotations remain

### Known Limitations
- **50 annotation limit**: Prevents highlighting all matches in very large documents
- **Static terms**: Currently hardcoded, future versions could load from file
- **PDF only**: Only works with PDF documents that support annotations
- **No undo**: Annotations are created immediately without undo option

## Future Enhancements

### Planned Improvements
1. **Dynamic term loading**: Read search terms from JSON/text file
2. **Higher annotation limits**: Optimize for larger documents
3. **Undo functionality**: Ability to remove created highlights
4. **Custom colors**: User-configurable color assignments
5. **Search options**: Case sensitivity, whole word matching
6. **Progress indication**: Progress bar for large document processing

### Configuration File Support
The system is designed to eventually support loading terms from `search-terms.txt`:
```json
[
    {"term": "SCHWAB", "color": "#FFFF00"},
    {"term": "Chiller", "color": "#FFA500"},
    {"term": "Warranty", "color": "#FF0000"}
]
```

## Troubleshooting

### Common Issues

#### No Highlights Created
- **Check document type**: Only PDFs with annotation support work
- **Verify terms exist**: Ensure search terms appear in document text
- **Case sensitivity**: System searches case-insensitively

#### All Highlights Same Color
- **Check ParsedColor setup**: Ensure all fields are set correctly
- **Verify color extraction**: Debug RGB values from COLORREF

#### Application Crashes
- **Use direct approach**: Avoid `MakeAnnotationsFromSelection()`
- **Check memory limits**: Ensure annotation limit is respected
- **Validate pointers**: Check all tab/window/engine pointers

### Debug Build Information
For debugging, add temporary MessageBox calls to trace execution:
```cpp
char debug[100];
sprintf_s(debug, sizeof(debug), "Found %d matches for '%s'", matchCount, term.text);
MessageBoxA(nullptr, debug, "Debug", MB_OK);
```

## Summary

This automatic highlighting system provides a robust, crash-free solution for highlighting key terms in PDF documents. The implementation prioritizes stability and performance while delivering a professional user experience. The direct annotation approach and careful memory management ensure reliable operation across various document types and sizes.

The system demonstrates successful integration with SumatraPDF's existing architecture while adding significant new functionality that enhances document review workflows.
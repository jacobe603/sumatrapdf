# SumatraPDF Codebase Documentation

This document provides an overview of the SumatraPDF codebase architecture and key components for development purposes.

## Project Overview

**SumatraPDF** is a multi-format document reader for Windows supporting PDF, EPUB, MOBI, CBZ/CBR, FB2, CHM, XPS, and DjVu formats. It's written in C++ and uses a modular architecture with pluggable document engines.

## Directory Structure

```
sumatrapdf/
├── src/                    # Main application source code
├── mupdf/                  # MuPDF library for PDF/XPS rendering
├── ext/                    # Third-party libraries (freetype, libjpeg, zlib, etc.)
├── do/                     # Go automation scripts for building
├── vs2022/                 # Visual Studio project files
├── tools/                  # Development tools
├── docs/                   # Documentation
├── translations/           # Localization files
└── gfx/                    # Icons and graphics
```

## Core Architecture

### Document Engine System

The application uses a pluggable engine architecture where each document format has its own engine:

#### Engine Base Classes
- **`src/EngineBase.h`** - Abstract base class defining the engine interface
- **`src/EngineCreate.cpp`** - Factory for creating appropriate engines based on file type

#### Format-Specific Engines
- **`src/EngineMupdf.cpp`** - PDF/XPS engine using MuPDF library
- **`src/EngineDjVu.cpp`** - DjVu document support  
- **`src/EngineEbook.cpp`** - EPUB/MOBI/FB2 ebook formats
- **`src/EngineImages.cpp`** - Image format support (PNG, JPEG, etc.)
- **`src/EnginePs.cpp`** - PostScript support

### Main Application Components

#### Core Application Files
- **`src/SumatraPDF.cpp`** - Main entry point and application initialization
- **`src/MainWindow.cpp/h`** - Primary application window and UI management
- **`src/WindowTab.cpp/h`** - Tab management for multiple documents
- **`src/Canvas.cpp/h`** - Document rendering canvas

#### Document Management
- **`src/DisplayModel.cpp/h`** - Document display logic, zoom, rotation, page layout
- **`src/DocController.h`** - Interface for controlling document operations
- **`src/RenderCache.cpp/h`** - Page rendering cache for performance optimization
- **`src/FileHistory.cpp/h`** - Recently opened files tracking

#### User Interface Components
- **`src/Toolbar.cpp/h`** - Application toolbar with navigation/zoom controls
- **`src/Menu.cpp/h`** - Menu system and context menus
- **`src/Tabs.cpp/h`** - Document tab management
- **`src/TableOfContents.cpp/h`** - TOC/bookmarks sidebar panel
- **`src/CommandPalette.cpp/h`** - Command palette feature (Ctrl+K)

#### Text Handling & Search
- **`src/TextSearch.cpp/h`** - Document text search functionality
- **`src/TextSelection.cpp/h`** - Text selection and copying
- **`src/Selection.cpp/h`** - General selection management

### Utility Systems

#### Core Utilities (`src/utils/`)
- **`BaseUtil.cpp/h`** - Core utility functions and data structures
- **`WinUtil.cpp/h`** - Windows-specific utilities and Win32 helpers
- **`FileUtil.cpp/h`** - File I/O operations and path handling
- **`StrUtil.cpp/h`** - String manipulation and conversion utilities
- **`Vec.h`** - Template-based dynamic array implementation

#### Specialized Utilities
- **`Archive.cpp/h`** - Archive file handling (ZIP, RAR, etc.)
- **`HttpUtil.cpp/h`** - HTTP operations for update checking
- **`CryptoUtil.cpp/h`** - Cryptographic functions
- **`JsonParser.cpp/h`** - JSON parsing for settings and configuration

### Configuration & Settings

- **`src/GlobalPrefs.cpp/h`** - Global application preferences
- **`src/AppSettings.cpp/h`** - Settings management and persistence
- **`src/Settings.h`** - Settings structure definitions
- **`src/Theme.cpp/h`** - UI theming and color management

### Specialized Features

#### Annotations
- **`src/Annotation.cpp/h`** - PDF annotation support
- **`src/EditAnnotations.cpp/h`** - Annotation editing UI

#### External Integration
- **`src/ExternalViewers.cpp/h`** - Integration with external applications
- **`src/Print.cpp/h`** - Printing functionality
- **`src/PdfSync.cpp/h`** - SyncTeX support for LaTeX integration

#### Accessibility
- **`src/uia/`** - UI Automation support for accessibility
  - **`Provider.cpp/h`** - Main UIA provider
  - **`DocumentProvider.cpp/h`** - Document-specific UIA implementation

## Build System

### Build Tools
- **`premake5.lua`** - Premake5 configuration for generating Visual Studio projects
- **`doit.bat`** - Entry point for Go-based automation scripts
- **`do/main.go`** - Main Go automation script for building, signing, uploading

### Build Process
1. Run `doit.bat` to execute Go automation scripts
2. Scripts generate Visual Studio projects using premake5
3. Compile using Visual Studio or MSBuild
4. Supports Debug/Release configurations for x32/x64/ARM64

### Dependencies
- MuPDF library (included in `mupdf/`)
- Various third-party libraries in `ext/` (freetype, libjpeg, zlib, etc.)
- WebView2 for modern web content rendering

## Key Features Implementation

### Multi-format Support
Each document format is handled by its corresponding engine, allowing easy addition of new formats by implementing the `EngineBase` interface.

### Performance Optimizations
- **Render caching** (`RenderCache.cpp`) - Caches rendered pages
- **Lazy loading** - Pages rendered on-demand
- **Background rendering** - Pre-renders nearby pages

### User Experience
- **Tabbed interface** - Multiple documents in one window
- **Full-screen presentation mode** - For presentations
- **Customizable UI** - Themes, toolbar customization
- **Keyboard shortcuts** - Comprehensive keyboard navigation

### Platform Integration
- **Windows shell integration** - File associations, thumbnails
- **Print system integration** - Native Windows printing
- **Accessibility support** - Screen reader compatibility
- **Touch/gesture support** - For touch-enabled devices

## Development Guidelines

### Code Style
- C++ with Win32 API
- Consistent naming conventions
- Header/implementation file separation
- Extensive use of forward declarations

### Testing
- Unit tests in `src/utils/tests/`
- Regression tests in `src/regress/`
- Stress testing framework in `src/StressTesting.cpp`

### Debugging
- Crash handler (`src/CrashHandler.cpp`) for error reporting
- Logging system (`src/utils/Log.cpp`)
- Debug builds with extensive assertions

## New Features

### Automatic Key Terms Highlighting System

A comprehensive annotation-based highlighting system has been implemented to automatically find and highlight predefined key terms in PDF documents with persistent annotations.

#### Implementation Overview

**Problem Solved**: Automatically highlight key terms ("Chiller", "Warranty", "Reheat", "Filters") across entire PDF documents with persistent, color-coded annotations that save with the file.

**Solution Architecture**: Leveraged existing SumatraPDF systems (TextSearch, Annotation, Selection) rather than creating parallel implementations.

#### Files Modified/Added:

**Core Implementation:**
- **`src/JsonSearchTerms.h/cpp`** - Interface and search term definitions with access functions
- **`src/SumatraPDF.cpp`** - `CreateHighlightAnnotationsForKeyTerms()` function (lines 4413-4527)
- **`src/Commands.h`** - Added `CmdHighlightKeyTerms` command (line 194)
- **`src/Menu.cpp`** - Added "Highlight Key Terms" menu item (lines 123-126)
- **`src/Accelerators.cpp`** - Added Ctrl+Shift+E for "Show in Folder" (line 145)
- **`premake5.files.lua`** - Added JsonSearchTerms files to build system (line 652)

#### Technical Implementation Details

**Text Search Integration:**
```cpp
// Uses existing TextSearch API for robust text finding
dm->textSearch->Reset();
dm->textSearch->SetSensitive(false); // Case insensitive
TextSel* result = dm->textSearch->FindFirst(pageNo, wideSearchTerm);
```

**Annotation Creation:**
```cpp
// Creates real PDF annotations, not just visual overlays
AnnotCreateArgs args{AnnotationType::Highlight};
args.col.pdfCol = MkPdfColor(r, g, b, 255); // Term-specific colors
Annotation* annot = EngineMupdfCreateAnnotation(engine, pageNo, PointF{}, &args);
SetQuadPointsAsRect(annot, rects); // Set highlight regions
```

**Color-Coded System (FINAL WORKING VERSION):**
- 🟡 "SCHWAB" - Yellow (RGB 255,255,0)
- 🟠 "Chiller" - Orange (RGB 255,165,0)  
- 🔴 "Warranty" - Red (RGB 255,0,0)
- 🟢 "Safety" - Green (RGB 0,255,0)
- 🔵 "Service" - Blue (RGB 0,0,255)
- 🟣 "Motor" - Purple (RGB 128,0,128)

#### Key Architectural Decisions

**1. UPDATED - Safe Direct Approach (Final Implementation):**
- Used `TextSearch::FindFirst()/FindNext()` for robust text finding
- Used `EngineMupdfCreateAnnotation()` DIRECTLY for persistent PDF annotations
- Manual coordinate conversion (`Rect` to `RectF`) to avoid type conflicts
- **AVOIDED** `MakeAnnotationsFromSelection()` due to heap corruption issues

**2. Clean Module Separation:**
- JsonSearchTerms.cpp: Simple interface, access functions, dialog
- SumatraPDF.cpp: Core implementation with full header access
- Avoided complex header dependencies and circular includes

**3. Multi-Page Handling:**
```cpp
// Proper page boundary handling in search loop
while (result && result->len > 0) {
    // Process results...
    result = dm->textSearch->FindNext();
    // Break if moved to different page
    if (result && result->len > 0 && result->pages[0] != pageNo) {
        break;
    }
}
```

#### User Interface Integration

**Menu Access:** File → "Highlight Key Terms"
**Process Flow:**
1. Shows dialog with terms and colors to be highlighted
2. User clicks OK to confirm
3. Searches entire document for all terms
4. Creates color-coded highlight annotations
5. Updates UI with annotation count
6. Annotations automatically save with PDF

**Error Handling:**
- Validates document is open and supports annotations
- Provides clear feedback for no results found
- Handles memory management for TextSel objects

#### Build System Integration

**Premake5 Configuration:**
```lua
files_in_dir("src", {
    -- ...existing files...
    "JsonSearchTerms.*",  -- Added to build system
    -- ...more files...
})
```

**Build Process:**
1. `premake5 vs2022` - Regenerate project files
2. MSBuild compilation with proper symbol linking
3. Resolved linker issues using access functions vs extern globals

#### Usage Instructions

1. **Open PDF**: Load any PDF document in SumatraPDF
2. **Activate Feature**: File menu → "Highlight Key Terms" 
3. **Confirm Terms**: Dialog shows 6 terms with assigned colors
4. **Execute**: Click OK to search and highlight entire document
5. **Results**: Shows count of annotations created (max 50)
6. **Persistence**: Annotations save automatically with PDF file

**📋 COMPLETE DOCUMENTATION**: See `AUTOMATIC_HIGHLIGHTING_GUIDE.md` for comprehensive implementation details, troubleshooting, and technical specifications.

#### Integration with Existing Systems

**TextSearch System:**
- Multi-page search with proper page boundary handling
- Case-insensitive matching
- Unicode text conversion (ToWStr)
- Coordinate extraction for precise highlighting

**Annotation System:**
- Creates `AnnotationType::Highlight` PDF annotations
- Uses `SetQuadPointsAsRect()` for precise text coverage
- Integrates with annotation list UI (`UpdateAnnotationsList`)
- Follows annotation color system (`MkPdfColor`)

**UI System:**
- Updates main window rendering (`MainWindowRerender`)
- Updates toolbar state (`ToolbarUpdateStateForWindow`) 
- Proper memory cleanup for selection objects

#### Development Lessons Learned

**Key Insights from Implementation:**

1. **Leverage Existing Systems**: Rather than creating parallel implementations, the most effective approach was to use existing, battle-tested systems (TextSearch, Annotation, Selection)

2. **Header Dependency Management**: Large C++ codebases require careful attention to circular dependencies. Solution: Split complex implementations across multiple files with different header requirements

3. **Build System Integration**: Understanding the build pipeline (Premake5 → MSBuild) before making changes prevented many compilation issues

4. **Incremental Development**: Building complexity gradually with frequent compilation catches issues early

5. **Follow Established Patterns**: The `MakeAnnotationsFromSelection()` pattern provided a perfect template for implementing similar annotation creation logic

**Technical Challenges Solved:**
- **Linker Symbol Issues**: Resolved by using access functions instead of extern globals
- **Memory Management**: Proper cleanup of TextSel and SelectionOnPage objects
- **Multi-Page Search**: Handling page boundary detection in search loops
- **Color System Integration**: Using existing MkPdfColor() functions for consistency

**Architecture Success Factors:**
- Clean separation between interface (JsonSearchTerms.cpp) and implementation (SumatraPDF.cpp)
- Integration with existing command/menu system for UI consistency
- Use of existing annotation APIs for persistence and PDF compliance

## Contributing

1. Fork the repository on GitHub
2. Use the build system (`doit.bat`) for compilation
3. Follow existing code patterns and conventions
4. Test thoroughly before submitting pull requests
5. Discuss significant changes in the issue tracker first

## Development Notes

### Build Workflow

- **User is to use VS 2022 to manually build the code**

For more detailed information, see the official documentation at:
https://www.sumatrapdfreader.org/docs/Contribute-to-SumatraPDF
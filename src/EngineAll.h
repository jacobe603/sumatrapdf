/* Copyright 2022 the SumatraPDF project authors (see AUTHORS file).
   License: GPLv3 */

struct Annotation;
enum class AnnotationType;
struct PasswordUI;
struct FileArgs;
struct AnnotCreateArgs;

/* EngineDjVu.cpp */
void CleanupEngineDjVu();
bool IsEngineDjVuSupportedFileType(Kind kind);
EngineBase* CreateEngineDjVuFromFile(const char* path);
EngineBase* CreateEngineDjVuFromStream(IStream* stream);

/* EngineEbook.cpp */
EngineBase* CreateEngineEpubFromFile(const char* fileName);
EngineBase* CreateEngineEpubFromStream(IStream* stream);
EngineBase* CreateEngineFb2FromFile(const char* fileName);
EngineBase* CreateEngineFb2FromStream(IStream* stream);
EngineBase* CreateEngineMobiFromFile(const char* fileName);
EngineBase* CreateEngineMobiFromStream(IStream* stream);
EngineBase* CreateEnginePdbFromFile(const char* fileName);
EngineBase* CreateEngineChmFromFile(const char* fileName);
EngineBase* CreateEngineHtmlFromFile(const char* fileName);
EngineBase* CreateEngineTxtFromFile(const char* fileName);

void SetDefaultEbookFont(const char* name, float size);
void EngineEbookCleanup();

/* EngineImages.cpp */

bool IsEngineImageSupportedFileType(Kind);
EngineBase* CreateEngineImageFromFile(const char* fileName);
EngineBase* CreateEngineImageFromStream(IStream* stream);

bool IsEngineImageDirSupportedFile(const char* fileName, bool sniff = false);
EngineBase* CreateEngineImageDirFromFile(const char* fileName);

bool IsEngineCbxSupportedFileType(Kind kind);
EngineBase* CreateEngineCbxFromFile(const char* path);
EngineBase* CreateEngineCbxFromStream(IStream* stream);

/* EngineMupdf.cpp */

using ShowErrorCb = Func1<const char*>;

bool IsEngineMupdfSupportedFileType(Kind);
EngineBase* CreateEngineMupdfFromFile(const char* path, Kind kind, int displayDPI, PasswordUI* pwdUI = nullptr);
EngineBase* CreateEngineMupdfFromStream(IStream* stream, const char* nameHint, PasswordUI* pwdUI = nullptr);
EngineBase* CreateEngineMupdfFromData(const ByteSlice& data, const char* nameHint, PasswordUI* pwdUI);
ByteSlice LoadEmbeddedPDFFile(const char* path);
const char* ParseEmbeddedStreamNumber(const char* path, int* streamNoOut);
Annotation* EngineMupdfCreateAnnotation(EngineBase*, int pageNo, PointF pos, AnnotCreateArgs* args);
void EngineMupdfGetAnnotations(EngineBase*, Vec<Annotation*>&);
bool EngineMupdfHasUnsavedAnnotations(EngineBase*);
bool EngineMupdfSupportsAnnotations(EngineBase*);
bool EngineMupdfSaveUpdated(EngineBase* engine, const char* path, const ShowErrorCb& showErrorFunc);
Annotation* EngineMupdfGetAnnotationAtPos(EngineBase*, int pageNo, PointF pos, Annotation*);
ByteSlice EngineMupdfLoadAttachment(EngineBase*, int attachmentNo);
bool AddSearchTermBookmark(EngineBase* engine, int pageNo, const char* searchTerm);

// Structure for hierarchical bookmark creation
struct TermPageData {
    char* termName;
    Vec<int> pages;
};

bool CreateHierarchicalSearchBookmarks(EngineBase* engine, Vec<TermPageData>& termData);
bool DeleteAllBookmarks(EngineBase* engine);
bool DeleteAllHighlights(EngineBase* engine);
void RefreshTocForEngine(EngineBase* engine);
bool ExtractSinglePageToNewPDF(EngineBase* engine, int pageNumber, const char* outputPath);
bool ExtractMultiplePagesToNewPDF(EngineBase* engine, int startPage, int endPage, const char* outputPath);
bool ExtractPagesToNewPDF(EngineBase* engine, Vec<int>& pageNumbers, const char* outputPath);

// Forward declaration for memory-safe page range structure
struct PageRangeData;
bool ExtractPageRangeToNewPDF(EngineBase* engine, const PageRangeData* data, const char* outputPath);

/* EnginePs.cpp */

bool IsEnginePsAvailable();
bool IsEnginePsSupportedFileType(Kind);
EngineBase* CreateEnginePsFromFile(const char* fileName);

/* EngineCreate.cpp */

bool IsSupportedFileType(Kind kind, bool enableEngineEbooks);

EngineBase* CreateEngineFromFile(const char* filePath, PasswordUI* pwdUI, bool enableChmEngine);

bool EngineSupportsAnnotations(EngineBase*);
bool EngineGetAnnotations(EngineBase*, Vec<Annotation*>&);
bool EngineHasUnsavedAnnotations(EngineBase*);
Annotation* EngineGetAnnotationAtPos(EngineBase*, int pageNo, PointF pos, Annotation*);

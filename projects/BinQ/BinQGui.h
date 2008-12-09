#ifndef COMPASS_GUI_H
#define COMPASS_GUI_H
//#include "rose.h"

#include "BinQAbstract.h"


#include <QTextBrowser>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QRProgress.h>

#include <QList>
#include <QListWidget>
#include <QListWidgetItem>

#include <QDesktopWidget>
#include <QApplication>

//#include "Item.h"
//#include "BinAnalyses.h"

class Slide;
class BinQSupport;


class BinQGUI : public BinQAbstract
{
  public:
  BinQGUI(std::string, std::string,     
	  std::vector<std::string> dllA,     
	  std::vector<std::string> dllB, 
	  bool test);
    ~BinQGUI();

    // public functions ---- GUI
    void run( ) ;
    void open();
    void reset();
    // handling of interaction
    void highlightFunctionRow(int, qrs::QRTable* widget);
    void unhighlightFunctionRow(int, qrs::QRTable* widget);
    void highlightInstructionRow(int, qrs::QRTable* widget);
    void unhighlightInstructionRow(int, qrs::QRTable* widget);
    // GUI
    void showFile(int row,qrs::QRTable *currentWidget, 
		  std::vector<SgNode*>& funcsFile, std::vector<Item*>& itemsFile);

#if 0
    // internal data that stores information of each table entry
    std::vector<Item*> itemsFileA;
    std::vector<Item*> itemsFileB;
    std::map<SgNode*,int> itemsNodeA;
    std::map<int,Item*> byteItemFileA;
    std::map<int,Item*> byteItemFileB;
    std::map<int,Item*> rowItemFileA;
    std::map<int,Item*> rowItemFileB;
    // the two main files
    SgNode* fileA;
    SgNode* fileB;
    SgNode* currentSelectedFile;
    // the dll files
    std::vector<SgNode*> dllFilesA;
    std::vector<SgNode*> dllFilesB;
    int screenWidth;
    std::vector<BinAnalyses*> analyses;
    BinAnalyses* currentAnalysis;
    // filenames for both files
    std::string fileNameA,fileNameB;
#endif
    // GUI elements that need to be accesses within this object but also from 
    // analyses implemented for BinQ
    QTextEdit *console;
    QTextEdit *analysisResult;
    QTextEdit *fileInfo;
    QTextEdit *sectionInfo;
    QListWidget *listWidget;
    QTextEdit *analysisInfo;


    // elements in the GUI used for scrolling and painting
    Slide *slide;
    int maxrows;
    QScrollBar* bar;
    QScrollBar* bar2;
    QScrollBar* bar3;


    // main GUI elements
    qrs::QRWindow *window;
    qrs::QRTable *tableWidget;
    qrs::QRTable *codeTableWidget;
    qrs::QRTable *codeTableWidgetDLL;
    qrs::QRTable *currentTableWidget;
    std::vector<qrs::QRTable*> codeTableWidgetADLLlist;
    std::vector<qrs::QRTable*> codeTableWidgetBDLLlist;
    QTabWidget *analysisTab;
    QTabWidget *codeTabA;
    QTabWidget *codeTabB;
    qrs::QRTable *tableWidget2;
    qrs::QRTable *codeTableWidget2;
    //qrs::QRTable *currentWidget;
    std::map< qrs::QRTable*, SgNode*> getFileForWidget;

    
    // functions that implement GUI element functionality
    void updateByteItemList();
    void insertSectionInformation(SgAsmElfSection* sec,std::string filename);
    void insertFileInformation();
  protected:

  private:
    // used for testing

    void testAnalyses();
#if 0
    bool test;
    // holds function information that is displayed in function tables
    std::vector<SgNode*> funcsFileA;
    std::vector<SgNode*> funcsFileB;
    // support class
    BinQSupport* binqsupport;
    void init();
    bool sourceFile;
    int screenHeight;
    std::vector<std::string> dllA;
    std::vector<std::string> dllB;
    void createItem(SgNode* file, std::vector<Item*>& itemsFile,std::vector<SgNode*>& funcsFile, bool dll);
    void createFunction(SgNode* file,std::vector<SgNode*>& funcsFile, bool dll);
#endif

    // support functions
    void showFileTab();
    void createGUI();


}; //class BinQGUI

#endif

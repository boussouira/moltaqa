#ifndef KTAB_H
#define KTAB_H

#include <QTabWidget>
#include <QWebView>
#include <QVBoxLayout>
#include <QTabBar>
#include "pageinfo.h"

/**
  @brief This class handle displaying Quran pages in tabs.

  KTab class handle the adding, removing and navigation between tabs.
  */
class KTab : public QTabWidget
{
    Q_OBJECT
public:
    KTab(QWidget *parent = 0);

public slots:

    /// @brief Add a new tab.
    void addNewOnglet(Page::Type pPageType = Page::QuranPage);

    /**
      @brief Close a specific tab.
      @param tabIndex   The tab index.
      */
    void closeTab(int tabIndex);

    /**
      @brief Create a new widget in order to add it to the QTabWidget(\em this).

      By calling this method, a new PageInfo well be added to the m_sowarInfo.
      @return A QWebView contained in a QWidget.
      */
    QWidget *newOnglet(Page::Type pPageType);

    /**
      @brief Get the a pointer to the current displayed QWebView.
      @return A pointer to the current QWebView.
      */
    QWebView *currentPage();

    /**
      @brief Get the a pointer to the current PageInfo.
      @return A pointer to the current PageInfo.
      */
    PageInfo *currentPageInfo();

    /**
      @brief Capture the signale when the tab is moved from an index position to an other one.

      When the tab has moved from an index postion to an other one, we need to do the same thing
      with the PageInfo contiand in m_sowarInfo.
      @param from   The previous position index.
      @param to     The new position index.
      */
    void tabIsMoved(int from, int to);

signals:
    /**
      @brief Force the MainWindow class to reload the current PageInfo.
      */
    void reloadCurrentPageInfo();

    /**
      @brief this signal does the same thing as QTabBar::tabMoved().
      */
    void tabMoved(int from, int to);

private:
    QList<PageInfo* > m_sowarInfo;
    QTabBar *m_tab;

};

#endif // KTAB_H

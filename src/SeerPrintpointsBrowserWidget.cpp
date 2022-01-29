#include "SeerPrintpointsBrowserWidget.h"
#include "SeerPrintpointCreateDialog.h"
#include "SeerUtl.h"
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QTreeWidgetItemIterator>
#include <QtWidgets/QApplication>
#include <QtCore/QDebug>

SeerPrintpointsBrowserWidget::SeerPrintpointsBrowserWidget (QWidget* parent) : QWidget(parent) {

    // Construct the UI.
    setupUi(this);

    // Setup the widgets
    printpointsTreeWidget->clear();

    printpointsTreeWidget->setSortingEnabled(false);
    printpointsTreeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    printpointsTreeWidget->resizeColumnToContents(0); // number
    printpointsTreeWidget->resizeColumnToContents(1); // type
    printpointsTreeWidget->resizeColumnToContents(2); // disp
    printpointsTreeWidget->resizeColumnToContents(3); // enabled
    printpointsTreeWidget->resizeColumnToContents(4); // addr
  //printpointsTreeWidget->resizeColumnToContents(5); // func      Too long to show
    printpointsTreeWidget->resizeColumnToContents(6); // file
  //printpointsTreeWidget->resizeColumnToContents(7); // fullname  Too long to show
    printpointsTreeWidget->resizeColumnToContents(8); // line
    printpointsTreeWidget->resizeColumnToContents(9); // dprintf
    printpointsTreeWidget->resizeColumnToContents(10); // thread-groups
    printpointsTreeWidget->resizeColumnToContents(11); // times
    printpointsTreeWidget->resizeColumnToContents(12); // original-location

    /*
    printpointsTreeWidget->setColumnHidden(1, true); // ??? Hide or have a config to hide/show columns.
    printpointsTreeWidget->setColumnHidden(6, true);
    */

    // Connect things.
    QObject::connect(printpointsTreeWidget,         &QTreeWidget::itemDoubleClicked,    this,  &SeerPrintpointsBrowserWidget::handleItemDoubleClicked);
    QObject::connect(refreshPrintpointsToolButton,  &QToolButton::clicked,              this,  &SeerPrintpointsBrowserWidget::handleRefreshToolButton);
    QObject::connect(addPrintpointToolButton,       &QToolButton::clicked,              this,  &SeerPrintpointsBrowserWidget::handleAddToolButton);
    QObject::connect(deletePrintpointsToolButton,   &QToolButton::clicked,              this,  &SeerPrintpointsBrowserWidget::handleDeleteToolButton);
    QObject::connect(enablePrintpointsToolButton,   &QToolButton::clicked,              this,  &SeerPrintpointsBrowserWidget::handleEnableToolButton);
    QObject::connect(disablePrintpointsToolButton,  &QToolButton::clicked,              this,  &SeerPrintpointsBrowserWidget::handleDisableToolButton);
}

SeerPrintpointsBrowserWidget::~SeerPrintpointsBrowserWidget () {
}

QStringList SeerPrintpointsBrowserWidget::printpointsText () const {

    QStringList printpointList;

    QTreeWidgetItemIterator it(printpointsTreeWidget);

    while (*it) {

        // Build a printpoint specification.
        QString printpointParameters;

        // Handle all printpoints as pending.
        // Some printpoints may point to source files that have not been
        // loaded (by programs that use runtime shared libaries).
        printpointParameters += " -f " + (*it)->text(11);

        printpointParameters.replace(" -source ", " --source ");
        printpointParameters.replace(" -line ",   " --line ");

        printpointList.append(printpointParameters);

        //qDebug() << printpointParameters;

        ++it;
    }

    return printpointList;
}

void SeerPrintpointsBrowserWidget::handleText (const QString& text) {

    // Don't do any work if the widget is hidden.
    if (isHidden()) {
        return;
    }

    QApplication::setOverrideCursor(Qt::BusyCursor);

    if (text.startsWith("^done,BreakpointTable={") && text.endsWith("}")) {

        printpointsTreeWidget->clear();

        //
        // ^done,BreakpointTable={
        //     nr_rows="1",nr_cols="6",
        //
        //     hdr=[
        //             {width="7",alignment="-1",col_name="number",colhdr="Num"},
        //             {width="14",alignment="-1",col_name="type",colhdr="Type"},
        //             {width="4",alignment="-1",col_name="disp",colhdr="Disp"},
        //             {width="3",alignment="-1",col_name="enabled",colhdr="Enb"},
        //             {width="18",alignment="-1",col_name="addr",colhdr="Address"},
        //             {width="40",alignment="2",col_name="what",colhdr="What"}
        //         ],
        //
        //     body=[
        //             bkpt={number="3",
        //                   type="dprintf",
        //                   disp="keep",
        //                   enabled="y",
        //                   addr="0x0000000000403a78",
        //                   func="main(int, char**)",
        //                   file="explorer.cpp",
        //                   fullname="/home/erniep/Development/Peak/src/Apps/Explorer/explorer.cpp",
        //                   line="84",
        //                   thread-groups=["i1"],
        //                   times="0",
        //                   script={"printf "i=%d  argc=%d\n",i,argc"},
        //                   original-location="-source /home/erniep/Development/Peak/src/Apps/Explorer/explorer.cpp -line 84"}
        //          ]
        // }


        // Don't filter characters. It messes up \n in the the dprintf_text.
        // QString newtext = Seer::filterEscapes(text); // Filter escaped characters.

        QString body_text = Seer::parseFirst(text, "body=", '[', ']', false);

        if (body_text != "") {

            QStringList bkpt_list = Seer::parse(text, "bkpt=", '{', '}', false);

            for ( const auto& bkpt_text : bkpt_list  ) {
                QString number_text            = Seer::parseFirst(bkpt_text, "number=",            '"', '"', false);
                QString type_text              = Seer::parseFirst(bkpt_text, "type=",              '"', '"', false);
                QString disp_text              = Seer::parseFirst(bkpt_text, "disp=",              '"', '"', false);
                QString enabled_text           = Seer::parseFirst(bkpt_text, "enabled=",           '"', '"', false);
                QString addr_text              = Seer::parseFirst(bkpt_text, "addr=",              '"', '"', false);
                QString func_text              = Seer::parseFirst(bkpt_text, "func=",              '"', '"', false);
                QString file_text              = Seer::parseFirst(bkpt_text, "file=",              '"', '"', false);
                QString fullname_text          = Seer::parseFirst(bkpt_text, "fullname=",          '"', '"', false);
                QString line_text              = Seer::parseFirst(bkpt_text, "line=",              '"', '"', false);
                QString dprintf_text           = Seer::parseFirst(bkpt_text, "script=",            '{', '}', false);
                QString thread_groups_text     = Seer::parseFirst(bkpt_text, "thread-groups=",     '[', ']', false);
                QString times_text             = Seer::parseFirst(bkpt_text, "times=",             '"', '"', false);
                QString original_location_text = Seer::parseFirst(bkpt_text, "original-location=", '"', '"', false);

                // Only look for 'breakpoint' type break points.
                if (type_text != "dprintf") {
                    continue;
                }

                // Add the level to the tree.
                QTreeWidgetItem* topItem = new QTreeWidgetItem;
                topItem->setText(0, number_text);
                topItem->setText(1, type_text);
                topItem->setText(2, disp_text);
                topItem->setText(3, enabled_text);
                topItem->setText(4, addr_text);
                topItem->setText(5, func_text);
                topItem->setText(6, file_text);
                topItem->setText(7, fullname_text);
                topItem->setText(8, line_text);
                topItem->setText(9, dprintf_text);
                topItem->setText(10, thread_groups_text);
                topItem->setText(11, times_text);
                topItem->setText(12, original_location_text);

                printpointsTreeWidget->addTopLevelItem(topItem);
            }
        }

    }else if (text.startsWith("^error,msg=\"No registers.\"")) {
        // Ignore.

    }else{
        // Ignore others.
    }

    printpointsTreeWidget->resizeColumnToContents(0);
    printpointsTreeWidget->resizeColumnToContents(1);
    printpointsTreeWidget->resizeColumnToContents(2);
    printpointsTreeWidget->resizeColumnToContents(3);
    printpointsTreeWidget->resizeColumnToContents(4);
  //printpointsTreeWidget->resizeColumnToContents(5);
    printpointsTreeWidget->resizeColumnToContents(6);
  //printpointsTreeWidget->resizeColumnToContents(7);
    printpointsTreeWidget->resizeColumnToContents(8);
    printpointsTreeWidget->resizeColumnToContents(9);
    printpointsTreeWidget->resizeColumnToContents(10);
    printpointsTreeWidget->resizeColumnToContents(11);
    printpointsTreeWidget->resizeColumnToContents(12);

    QApplication::restoreOverrideCursor();
}

void SeerPrintpointsBrowserWidget::handleStoppingPointReached () {

    // Don't do any work if the widget is hidden.
    if (isHidden()) {
        return;
    }

    emit refreshPrintpointsList();
}

void SeerPrintpointsBrowserWidget::handleItemDoubleClicked (QTreeWidgetItem* item, int column) {

    Q_UNUSED(column);

    int lineno = item->text(8).toInt();

    emit selectedFile(item->text(6), item->text(7), lineno);
}

void SeerPrintpointsBrowserWidget::handleRefreshToolButton () {

    emit refreshPrintpointsList();
}

void SeerPrintpointsBrowserWidget::handleAddToolButton () {

    SeerPrintpointCreateDialog dlg(this);

    int ret = dlg.exec();

    if (ret == 0) {
        return;
    }

    // Build a printpoint specification.
    QString printpointParameters = dlg.printpointText();

    // If nothing, just return.
    if (printpointParameters == "") {
        return;
    }

    // Otherwise send the command to create the printpoint.
    emit insertPrintpoint(printpointParameters);
}

void SeerPrintpointsBrowserWidget::handleDeleteToolButton () {

    // Get selected tree items.
    QList<QTreeWidgetItem*> items = printpointsTreeWidget->selectedItems();

    // Build a string that is a list of printpoints.
    QString printpoints;

    QList<QTreeWidgetItem*>::iterator i;
    for (i = items.begin(); i != items.end(); ++i) {
        if (i != items.begin()) {
            printpoints += " ";
        }
        printpoints += (*i)->text(0);
    }

    // Don't do anything if the list of printpoints is empty.
    if (printpoints == "") {
        return;
    }

    // Send the signal.
    emit deletePrintpoints(printpoints);
}

void SeerPrintpointsBrowserWidget::handleEnableToolButton () {

    // Get selected tree items.
    QList<QTreeWidgetItem*> items = printpointsTreeWidget->selectedItems();

    // Build a string that is a list of printpoints.
    QString printpoints;

    QList<QTreeWidgetItem*>::iterator i;
    for (i = items.begin(); i != items.end(); ++i) {

        if (i != items.begin()) {
            printpoints += " ";
        }

        printpoints += (*i)->text(0);
    }

    // Don't do anything if the list of printpoints is empty.
    if (printpoints == "") {
        return;
    }

    // Send the signal.
    emit enablePrintpoints(printpoints);
}

void SeerPrintpointsBrowserWidget::handleDisableToolButton () {

    // Get selected tree items.
    QList<QTreeWidgetItem*> items = printpointsTreeWidget->selectedItems();

    // Build a string that is a list of printpoints.
    QString printpoints;

    QList<QTreeWidgetItem*>::iterator i;
    for (i = items.begin(); i != items.end(); ++i) {

        if (i != items.begin()) {
            printpoints += " ";
        }

        printpoints += (*i)->text(0);
    }

    // Don't do anything if the list of printpoints is empty.
    if (printpoints == "") {
        return;
    }

    // Send the signal.
    emit disablePrintpoints(printpoints);
}

void SeerPrintpointsBrowserWidget::showEvent (QShowEvent* event) {

    QWidget::showEvent(event);

    emit refreshPrintpointsList();
}


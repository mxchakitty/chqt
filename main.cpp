#include "imports.h"
#include "menu.h"

// used for top bar at the moment
// will be exported to a separate file
// and be used for the entire application
// to allow for easy customization
const QString stylesheet = " \
QWidget { \
    background-color: #f2f2f2; \
} \
QWidget[border] { \
    border: 1px solid #e0e0e0; \
} \
QWidget[borderWithNoBrim] { \
    border-top: none; \
} \
QWidget[borderError] { \
    border: 1px solid #ff0000; \
} \
QLineEdit, QLabel { \
    background-color: #ffffff; \
}";

void setHeaderWidth(QLineEdit *header) {
    int textWidth = header -> fontMetrics().horizontalAdvance(header -> text());

    if (textWidth < 240) {
        header -> setFixedWidth(textWidth + 10);
    }
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QMainWindow *window = new QMainWindow();
    QWidget content;

    window -> setCentralWidget(&content);
    content.setWindowTitle("chqt");
    content.setMinimumSize(680, 360);
    content.resize(1280, 720); 
    window -> resize(1280, 720);

    QMenuBar *menuBar = new QMenuBar(window);
    window -> setMenuBar(menuBar);
    menuBar -> setNativeMenuBar(false);
    menuBar -> setStyleSheet("QMenuBar { background-color: #f2f2f2; border-bottom: 1px solid #e0e0e0; }");

    populate(menuBar, window);

    QWidget sidebarL, main, sidebarR;
    QHBoxLayout layout;

    // LEFT SIDEBAR
    QTreeView *treeView = new QTreeView();
    QStandardItemModel *model = new QStandardItemModel();

    // typically the tree would be populated from a database
    QStandardItem *server = new QStandardItem("Default Server");
    QStandardItem *channel = new QStandardItem("Default Channel");
    server -> appendRow(channel);
    model -> appendRow(server);

    treeView -> setModel(model);
    treeView -> setHeaderHidden(true);
    treeView -> expandAll();
    treeView -> setEditTriggers(QAbstractItemView::NoEditTriggers);
    treeView -> setCurrentIndex(model -> indexFromItem(channel));

    sidebarL.setFixedWidth(content.width() / 6);

    QVBoxLayout *sidebarL_layout = new QVBoxLayout();
    QLabel *headerL = new QLabel("Connections");
    headerL -> setAlignment(Qt::AlignCenter);
    sidebarL_layout -> addWidget(headerL);
    sidebarL_layout -> addWidget(treeView);
    sidebarL.setLayout(sidebarL_layout);

    layout.addWidget(&sidebarL);

    // MAIN
    QVBoxLayout *main_layout = new QVBoxLayout();
    QWidget topBar, chatArea, inputArea;
    main_layout -> setContentsMargins(0, 0, 0, 0);
    main_layout -> setSpacing(0);
    topBar.setProperty("border", true);
    topBar.setProperty("borderWithNoBrim", true);

    // top bar
    QHBoxLayout *topBar_layout = new QHBoxLayout();

    QLineEdit *header = new QLineEdit("Default Channel");
    header -> setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    header -> setProperty("border", true);
    setHeaderWidth(header);

    QObject::connect(header, &QLineEdit::editingFinished, [=]() {
        // check permissions before changing header
        // for now there is no permission system, default to reject
        header -> setText("Default Channel");
        setHeaderWidth(header);
        // send permission denied message
    });

    // check for non alpha-numeric characters
    QObject::connect(header, &QLineEdit::textEdited, [=]() {
        QString text = header -> text();
        for (int i = 0; i < text.length(); i++) {
            bool isAllowed = text[i].isLetterOrNumber() || text[i] == '_' || text[i] == '-' || text[i] == ' ';
            // if not allowed or isnt alphanumeric
            if (!(isAllowed)) {
                // find you and kill you >:3c
                header -> setText(text.remove(i, 1));
                QPoint pos = header -> mapToGlobal(header -> pos());
                pos.setY(pos.y() + 12);

                QToolTip::showText(pos, "Invalid character");
                header -> setStyleSheet("border: 1px solid red");

                QTimer::singleShot(3000, [=]() {
                    QToolTip::hideText();
                    header -> setStyleSheet("");
                }); 
            }
        }
        
        // constantly resize the header to fit text, but cap it at a certain size
        setHeaderWidth(header);
    });

    // convert to a QLineEdit next
    QLineEdit *topic = new QLineEdit("Default Topic");
    topic -> setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    topic -> setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    topic -> setProperty("border", true);

    QObject::connect(topic, &QLineEdit::editingFinished, [=]() {
        // check permissions before changing header
        // for now there is no permission system, default to reject
        topic -> setText("Default Topic");
        // send permission denied message
    });

    QPushButton *button = new QPushButton("Button");
    button -> setFixedWidth(100);
    button -> setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    topBar_layout -> addWidget(header);
    topBar_layout -> addWidget(topic);
    topBar_layout -> addWidget(button);
    topBar_layout -> setAlignment(Qt::AlignVCenter);

    topBar.setStyleSheet(stylesheet);

    topBar_layout -> setSpacing(2);
    topBar.setFixedHeight(40);

    topBar.setLayout(topBar_layout);

    // chat area

    // bottom bar
    QHBoxLayout *inputArea_layout = new QHBoxLayout();

    // toolbar, then input area below it 
    // input area is a hbox with a textbox and a button
    QTextEdit *input = new QTextEdit();
    // set minimum and maximum height
    input -> setMinimumHeight(28);
    input -> setMaximumHeight(28);

    //input -> setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

    input -> setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    input -> setProperty("border", true);

    // button, aligned to top right, using icon
    QPushButton *send = new QPushButton();
    // set icon from linux icon theme
    send -> setIcon(QIcon::fromTheme("mail-send"));
    send -> setIconSize(QSize(14, 14));
    send -> setFixedSize(28, 28);

    inputArea_layout -> addWidget(input);
    inputArea_layout -> addWidget(send);
    inputArea_layout -> setAlignment(Qt::AlignRight | Qt::AlignBottom);


    inputArea.setLayout(inputArea_layout);

    main_layout -> addWidget(&topBar);
    main_layout -> addWidget(&chatArea);
    main_layout -> addWidget(&inputArea);

    main.setLayout(main_layout);
    main.setProperty("main", true);

    main.setStyleSheet("background: #fdfdfd; border-left: 1px solid #e0e0e0; border-right: 1px solid #e0e0e0; padding: 2px;");
    content.setStyleSheet("background: #f2f2f2;");

    layout.addWidget(&main);

    // RIGHT SIDEBAR
    QList<QStandardItem *> items;

    // types of possible members
    QStandardItem *op = new QStandardItem("@Operator");
    QStandardItem *moderator = new QStandardItem("+Moderator");
    QStandardItem *member = new QStandardItem("Member");
    QStandardItem *buddy = new QStandardItem("Buddy");
    QStandardItem *muted = new QStandardItem("Muted");
    QStandardItem *blocked = new QStandardItem("Blocked");
    QStandardItem *self = new QStandardItem("You");

    // give each member type a font style
    buddy -> setForeground(QBrush(QColor(2, 200, 0)));
    muted -> setForeground(QBrush(QColor(200, 200, 200)));
    blocked -> setForeground(QBrush(QColor(255, 0, 0)));

    QFont font;
    font.setBold(true);

    self -> setFont(font);

    std::array <QStandardItem *, 7> member_types = {op, moderator, member, buddy, muted, blocked, self};

    for (auto &item : member_types) {
        items.append(item);
    }

    QStandardItemModel *member_model = new QStandardItemModel();
    member_model -> appendColumn(items);

    // create a tree view from the model
    QTreeView *member_tree = new QTreeView();
    member_tree -> setModel(member_model);
    member_tree -> setHeaderHidden(true);
    member_tree -> expandAll();
    member_tree -> setEditTriggers(QAbstractItemView::NoEditTriggers);
 
    sidebarR.setFixedWidth(content.width() / 6);

    QVBoxLayout *sidebarR_layout = new QVBoxLayout();
    QLabel *headerR = new QLabel("Members");
    headerR -> setAlignment(Qt::AlignCenter);
    sidebarR_layout -> addWidget(headerR);
    sidebarR_layout -> addWidget(member_tree);
    sidebarR.setLayout(sidebarR_layout);
    
    layout.addWidget(&sidebarR);

    // finishing up
    content.setLayout(&layout);

    content.layout() -> setContentsMargins(0, 0, 0, 0);
    window -> layout() -> setContentsMargins(0, 0, 0, 0);

    window -> show();
    return app.exec();
}

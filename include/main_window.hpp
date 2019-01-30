#include <QMainWindow>
#include <QListWidgetItem>

#include <memory>

class QAction;
class QActionGroup;
class QLabel;
class QMenu;
class QListWidget;
class QHBoxLayout;

class RenderArea;

#include "render_area.hpp"

class MainWindow : public QMainWindow
{
  Q_OBJECT

  public:
    MainWindow();
    void createMenus();

    void loadOption();
    void exitOption();
    void aboutOption();

    bool loadModelFile(QString fileName);
 
    std::unique_ptr<RenderArea> area;

  private:
};

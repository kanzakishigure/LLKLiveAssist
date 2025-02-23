#pragma once

#include <ElaScrollPage.h>
#include <qchar.h>
class QVBoxLayout;
namespace NAssist {

class BasePage : public ElaScrollPage {
  Q_OBJECT
public:
  explicit BasePage(QWidget *parent = nullptr);
  ~BasePage();

protected:
  virtual void createCustomWidget(QString desText);
};
} // namespace NAssist

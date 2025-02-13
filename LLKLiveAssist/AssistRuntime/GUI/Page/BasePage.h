#pragma once

#include <ElaScrollPage.h>
class QVBoxLayout;
namespace NAssist {

class BasePage : public ElaScrollPage {
  Q_OBJECT
public:
  explicit BasePage(QWidget *parent = nullptr);
  ~BasePage();

protected:
  void createCustomWidget(QString desText);
};
} // namespace NAssist

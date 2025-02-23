#include "ModelPage.h"
#include <ElaMenu.h>
#include <ElaScrollArea.h>
#include <ElaTableView.h>
#include <ElaToolButton.h>
#include <ElaContentDialog.h>
#include <ElaText.h>

#include <QHBoxLayout>
#include <QHeaderView>
#include <QVBoxLayout>
#include <qaction.h>
#include <qobjectdefs.h>
#include <qwidget.h>

#include "Data/GSoVITSModel.h"
#include "GSoVITSAssist.h"
#include "GUI/ModelView/GsovitsTableViewModel.h"
#include "GUI/Widgets/ModelConfigContainer.h"
#include "ModuleManager.h"
namespace NAssist {
ModelPage::ModelPage(QWidget *parent) : BasePage(parent) {
  // 预览窗口标题
  setWindowTitle("GPT-Sovits Model Importer");
  // 顶部元素
  createCustomWidget("管理GPT-Sovits模型, 一站式导入GPT-Sovist模型");


    //ElaTableView
    ElaText* table_text = new ElaText("Model Library", this);
    table_text->setTextPixelSize(18);
    m_tableView = new ElaTableView(this);
    // ElaScrollBar* tableViewFloatScrollBar = new ElaScrollBar(m_tableView->verticalScrollBar(), m_tableView);
    // tableViewFloatScrollBar->setIsAnimation(true);
    m_gsovits_table_view_model =  new GsovitsTableViewModel(this);
    m_gsovits_table_view_model->appendData(NAssist::ModuleManager::getInstance().getModule<GSoVITSAssist>()->getGSoVITSModels());
    QFont table_header_font = m_tableView->horizontalHeader()->font();
    table_header_font.setPixelSize(16);
    m_tableView->horizontalHeader()->setFont(table_header_font);
    m_tableView->setModel(m_gsovits_table_view_model);
    m_tableView->setAlternatingRowColors(true);
    m_tableView->setIconSize(QSize(38, 38));
    m_tableView->verticalHeader()->setHidden(true);
    m_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->horizontalHeader()->setMinimumSectionSize(60);
    m_tableView->verticalHeader()->setMinimumSectionSize(46);
    m_tableView->setFixedHeight(450);
    connect(m_tableView, &ElaTableView::tableViewShow, this, [=]() {
      m_tableView->resizeColumnsToContents();
    });
    QHBoxLayout* tableViewLayout = new QHBoxLayout();
    tableViewLayout->setContentsMargins(0, 0, 10, 0);
    tableViewLayout->addWidget(m_tableView);

   

    QWidget* centralWidget = new QWidget(this);
    centralWidget->setWindowTitle("Model Library");
    QVBoxLayout* centerLayout = new QVBoxLayout(centralWidget);
    centerLayout->addWidget(table_text);
    centerLayout->addSpacing(10);
    centerLayout->addLayout(tableViewLayout);
    addCentralWidget(centralWidget,true,true,0);
}
ModelPage::~ModelPage() {}


void ModelPage::createCustomWidget(QString desText) {
  // 顶部元素
  QWidget *customWidget = new QWidget(this);
  ElaText *subTitleText = new ElaText(this);
  subTitleText->setText("内置模型 import from: https://www.ai-hobbyist.com/");
  subTitleText->setTextInteractionFlags(Qt::TextSelectableByMouse);
  subTitleText->setTextPixelSize(11);

  ElaToolButton *create_model_button = new ElaToolButton(this);
  create_model_button->setFixedHeight(35);
  create_model_button->setIsTransparent(false);
  create_model_button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  //_toolButton->setPopupMode(QToolButton::MenuButtonPopup);
  create_model_button->setText("Add Model");
  create_model_button->setElaIcon(ElaIconType::FileImport);
  ElaMenu *import_model_menu = new ElaMenu(this);

  //add create model
  {
    
    auto* model_import_action = import_model_menu->addElaIconAction(ElaIconType::File, "Create New Model");
    m_model_config_dilog = new ElaContentDialog(window());
    m_model_config_dilog->setLeftButtonText(u8"取消");
    m_model_config_dilog->setMiddleButtonText(u8"重置");
    m_model_config_dilog->setRightButtonText(u8"创建");
   
    ModelConfigContainer* new_model_config_container = new ModelConfigContainer(this);
    connect(m_model_config_dilog, &ElaContentDialog::middleButtonClicked, this,
            [=]() {
              new_model_config_container->setmodel_name("");
              new_model_config_container->setmodel_img_path("");
              new_model_config_container->setmodel_author("");
              new_model_config_container->setmodel_category("");
              new_model_config_container->setsovits_weights("");
              new_model_config_container->setgpt_weights("");
              new_model_config_container->setref_audio_path("");
              new_model_config_container->setprompt_text("");
              new_model_config_container->setprompt_lang("");
              new_model_config_container->setmodel_description("");

            });
    connect(m_model_config_dilog, &ElaContentDialog::rightButtonClicked, this,
            [=]() {
              auto models =  NAssist::ModuleManager::getInstance().getModule<GSoVITSAssist>()->getGSoVITSModels();
              GSoVITSModel model;
              model.model_name = new_model_config_container->getmodel_name().toStdString();
              model.model_img = new_model_config_container->getmodel_img_path().toStdString();
              model.model_author = new_model_config_container->getmodel_author().toStdString();
              model.model_category = new_model_config_container->getmodel_category().toStdString();
              model.sovits_weights = new_model_config_container->getsovits_weights().toStdString();
              model.gpt_weights = new_model_config_container->getgpt_weights().toStdString();
              model.ref_audio_path = new_model_config_container->getref_audio_path().toStdString();
              model.prompt_text = new_model_config_container->getprompt_text().toStdString();
              model.prompt_lang = new_model_config_container->getprompt_lang().toStdString();
              model.model_description = new_model_config_container->getmodel_description().toStdString();
              models.push_back(model);
              NAssist::ModuleManager::getInstance().getModule<GSoVITSAssist>()->setGSoVITSModels(models);
              Q_EMIT gSovitsmodelChanged();
            });
    
    connect( this,&ModelPage::gSovitsmodelChanged,[this](){
      m_gsovits_table_view_model->appendData(NAssist::ModuleManager::getInstance().getModule<GSoVITSAssist>()->getGSoVITSModels());
    });
    m_model_config_dilog->setCentralWidget(new_model_config_container);
    m_model_config_dilog->hide();
    connect(model_import_action,&QAction::triggered,this,[=](){
      m_model_config_dilog->show();
    });
    
    
  }

  {
    auto* model_import_action = import_model_menu->addElaIconAction(ElaIconType::EarthAmericas,"Import Exist Model");
    
  }

  create_model_button->setMenu(import_model_menu);

  

  


  QHBoxLayout *buttonLayout = new QHBoxLayout();
  buttonLayout->addWidget(create_model_button);
  buttonLayout->setAlignment(Qt::AlignLeft);

  ElaText *descText = new ElaText(this);
  descText->setText(desText);
  descText->setTextPixelSize(13);

  QVBoxLayout *topLayout = new QVBoxLayout(customWidget);
  topLayout->setContentsMargins(0, 0, 0, 0);
  topLayout->addWidget(subTitleText);
  topLayout->addSpacing(5);
  topLayout->addLayout(buttonLayout);
  topLayout->addSpacing(5);
  topLayout->addWidget(descText);
  setCustomWidget(customWidget);
}
} // namespace NAssist
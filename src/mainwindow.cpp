#include "glwidget.h"
#include "glycogenanalysismanager.h"
#include "glycogencluster.h"
#include "object.h"
#include "openglmanager.h"



#include "mainwindow.h"
#include <chrono>
#include <map>
#include <QPixmap>
#include "ui_mainwindow.h"

namespace fs = std::filesystem;

//------------------------------------------------------
//
MainWindow::MainWindow(QWidget* parent, InputForm* input_form) :
  QMainWindow(parent),
  mainwindow_ui(new Ui::MainWindow)
{
  mainwindow_ui->setupUi(this);
  //getGLWidget()->init(input_form);

  m_data_container = new DataContainer(input_form);

  m_main_widget = new MainWidget(m_data_container, input_form, mainwindow_ui->centralwidget);
  //m_mainWidget->initializeGL();
  mainwindow_ui->gridLayout_5->addWidget(m_main_widget, 0, 0);
 
  m_currentSelectedCluster = 0;
  m_clusters = 0;

  initializeSlicePositionSlider();
  initializeSynapseThresholdSlider();
  initializeOpacitySlider();
 
  m_treemodel = new TreeModel(mainwindow_ui->groupBox_16, m_data_container, m_main_widget);
  mainwindow_ui->verticalLayout_15->addWidget(m_treemodel);

  mainwindow_ui->pushButton->setStyleSheet("border: none; background-color: rgba(255, 255, 255, 100);");
  mainwindow_ui->pushButton->setIconSize(QSize(60, 60));
  mainwindow_ui->pushButton->setMinimumSize(60, 60);
  mainwindow_ui->pushButton->setMaximumSize(60, 60);

  mainwindow_ui->pushButton_2->setStyleSheet("border: none; background-color: rgba(255, 255, 255, 100);");
  mainwindow_ui->pushButton_2->setIconSize(QSize(60, 60));
  mainwindow_ui->pushButton_2->setMinimumSize(60, 60);
  mainwindow_ui->pushButton_2->setMaximumSize(60, 60);

  mainwindow_ui->pushButton_3->setStyleSheet("border: none; background-color: rgba(255, 255, 255, 100);");
  mainwindow_ui->pushButton_3->setIconSize(QSize(60, 60));
  mainwindow_ui->pushButton_3->setMinimumSize(60, 60);
  mainwindow_ui->pushButton_3->setMaximumSize(60, 60);

  setupSignalsNSlots();
  initializeVisualizationPresets();
 
}

void MainWindow::initializeSlicePositionSlider()
{
  QSlider* slice_position_slider = mainwindow_ui->horizontalSlider;
  int initial_tick_position = 100.0 / (MESH_MAX_Z / 2.0);
  slice_position_slider->setValue(initial_tick_position);
  double distance_value = ((double)initial_tick_position / 100.0) * sqrt(3) * MESH_MAX_X;
  mainwindow_ui->lineEdit_5->setText(QString::number(std::round(distance_value * 100.0) / 100.0));

  connect(slice_position_slider, SIGNAL(valueChanged(int)), this, SLOT(on_slice_position_slider_changed(int)));
  m_main_widget->set_slice_position(initial_tick_position);
}

void MainWindow::initializeSynapseThresholdSlider()
{
  QSlider* synapse_threshold_slider = mainwindow_ui->horizontalSlider_3;
  int initial_tick_position = 100.0 / (sqrt(3.0) * MESH_MAX_X);
  synapse_threshold_slider->setValue(initial_tick_position);
  double distance_value = ((double)initial_tick_position / 100.0) * sqrt(3) * MESH_MAX_X;
  mainwindow_ui->lineEdit->setText(QString::number(std::round(distance_value * 100.0) / 100.0));

  connect(synapse_threshold_slider, SIGNAL(valueChanged(int)), this, SLOT(on_synapse_distance_slider_changed(int)));
}

void MainWindow::initializeOpacitySlider()
{
  QSlider* opacity_slider = mainwindow_ui->horizontalSlider_4;
  int initial_tick_position = 30;
  opacity_slider->setValue(initial_tick_position);
  double opacity_value = (double)initial_tick_position / 100.0;

  connect(opacity_slider, SIGNAL(valueChanged(int)), this, SLOT(on_opacity_slider_changed(int)));

  on_opacity_slider_changed(initial_tick_position);

  
}

void MainWindow::initializeVisualizationPresets()
{
  mainwindow_ui->checkBox_2->setChecked(true);
  mainwindow_ui->checkBox_3->setChecked(true);
  mainwindow_ui->checkBox_5->setChecked(true);

  on_structure_selection_changed(-1);
  on_high_detail_vis_clicked();
}

//------------------------------------------------------
//
MainWindow::~MainWindow()
{
  delete mainwindow_ui;
  delete tableView;
  delete m_currentSelectedCluster;
  delete m_clusters;
}

//------------------------------------------------------
//
void MainWindow::setupSignalsNSlots()
{
  QObject::connect(this, SIGNAL(update_glycogen_clustering_timing(QString)),
    mainwindow_ui->glycogen_clustering_ms, SLOT(setText(QString)));

  QObject::connect(this, SIGNAL(update_glycogen_cluter_mapping_timing(QString)),
    mainwindow_ui->glycogen_cluster_mapping_ms, SLOT(setText(QString)));

  QObject::connect(this, SIGNAL(update_glycogen_granules_mapping_timing(QString)),
    mainwindow_ui->glycogen_granules_mapping_ms, SLOT(setText(QString)));

  QObject::connect(m_treemodel, &QTreeView::doubleClicked, m_treemodel, &TreeModel::selectItem);

  QObject::connect(mainwindow_ui->checkBox, SIGNAL(stateChanged(int)), this, SLOT(on_structure_selection_changed(int)));
  QObject::connect(mainwindow_ui->checkBox_2, SIGNAL(stateChanged(int)), this, SLOT(on_structure_selection_changed(int)));
  QObject::connect(mainwindow_ui->checkBox_3, SIGNAL(stateChanged(int)), this, SLOT(on_structure_selection_changed(int)));
  QObject::connect(mainwindow_ui->checkBox_5, SIGNAL(stateChanged(int)), this, SLOT(on_structure_selection_changed(int)));
  QObject::connect(mainwindow_ui->checkBox_12, SIGNAL(stateChanged(int)), this, SLOT(on_structure_selection_changed(int)));

  QObject::connect(mainwindow_ui->pushButton, SIGNAL(released()), this, SLOT(on_high_detail_vis_clicked()));
  QObject::connect(mainwindow_ui->pushButton_2, SIGNAL(released()), this, SLOT(on_medium_detail_vis_clicked()));
  QObject::connect(mainwindow_ui->pushButton_3, SIGNAL(released()), this, SLOT(on_low_detail_vis_clicked()));

}

void MainWindow::onAddWebWidget()
{
}

//------------------------------------------------------
//
void MainWindow::on_object_clicked(QList<QStandardItem*> items)
{
  tableView->appendRow(items);
}

//------------------------------------------------------
//
void MainWindow::clearTable()
{
  tableView->removeRows(0, tableView->rowCount(), QModelIndex());

}

//------------------------------------------------------
//
void MainWindow::RemoveRowAt(QModelIndex selectedIndex)
{
  QModelIndex hvgxID_index = tableView->index(selectedIndex.row(), 0);
  QVariant hvgxID_item = tableView->data(hvgxID_index);
  int hvgxID = hvgxID_item.toInt();
  getDeletedData(hvgxID);
  tableView->removeRows(selectedIndex.row(), 1, QModelIndex());
}

void MainWindow::getIDAt(QModelIndex selectedIndex)
{
  qDebug() << "getIDAt";
  QModelIndex hvgxID_index = tableView->index(selectedIndex.row(), 0);
  QVariant hvgxID_item = tableView->data(hvgxID_index);
  int hvgxID = hvgxID_item.toInt();

  getIDAtSelectedRow(hvgxID);
}

//------------------------------------------------------
//
void MainWindow::on_mitoRadioButton_toggled(bool checked)
{
  mainwindow_ui->glycogenMapToBoutonsCheckBox->setEnabled(!checked);
  mainwindow_ui->glycogenMapToSpinesCheckBox->setEnabled(!checked);
}

//------------------------------------------------------
//
void MainWindow::on_boutonsSpinesRadioButton_toggled(bool checked)
{
  mainwindow_ui->glycogenMapToBoutonsCheckBox->setEnabled(checked);
  mainwindow_ui->glycogenMapToSpinesCheckBox->setEnabled(checked);
}

//------------------------------------------------------
//
void MainWindow::getPath(QString pathLabel)
{
  mainwindow_ui->listPaths->addItem(pathLabel);
}

//------------------------------------------------------
//
void MainWindow::getSelectedPathIndex(QModelIndex index)
{
  QString pathLabel = mainwindow_ui->listPaths->item(index.row())->text();
  signalPathLabel(pathLabel);
}

//------------------------------------------------------
//
void MainWindow::deleteSelectedPath(QModelIndex index)
{
  // return label to mousepad to remove it from paths list
  QListWidgetItem* item = mainwindow_ui->listPaths->item(index.row());
  QString pathLabel = item->text();
  delete mainwindow_ui->listPaths->takeItem(index.row());
  signalDeletedPathLabel(pathLabel);
}



//------------------------------------------------------
//
void  MainWindow::on_glycogenMappingTreeWidget_itemChanged(QTreeWidgetItem* item, int column)
{
  bool state = true;
  if (item->checkState(0) == Qt::Unchecked)
    state = false;

  QString ID_str = item->text(0); // highlighted immediately
  glycogenMappedSelectedState(ID_str, state);
}

//------------------------------------------------------
//
void MainWindow::on_filterByProximityListWidget_itemChanged(QListWidgetItem* item)
{
  bool flag = true;
  if (item->checkState() == Qt::Unchecked)
    flag = false;

  signalProximityTypeState(item->text(), flag);
}

//------------------------------------------------------
//
void MainWindow::on_listWidget_itemChanged(QListWidgetItem*)
{
  signalMappingTreeWidget(mainwindow_ui->glycogenMappingTreeWidget);
}

void MainWindow::on_structure_selection_changed(int state)
{
  qDebug() << "seting image";
  VisConfiguration config;
  config.axons = mainwindow_ui->checkBox->isChecked();
  config.dends = mainwindow_ui->checkBox_2->isChecked();
  config.mitos = mainwindow_ui->checkBox_3->isChecked();
  config.syn = mainwindow_ui->checkBox_5->isChecked();
  config.sliceView = mainwindow_ui->checkBox_12->isChecked();

  mainwindow_ui->horizontalSlider->setEnabled(config.sliceView);


  SelectedVisMethods visMethods = m_main_widget->setThumbnailIcons(config);
  m_main_widget->showSlice(config.sliceView);

  // low detail
  mainwindow_ui->pushButton_3->setIcon(QIcon(visMethods.high_icon));
   
  // medium detail
  mainwindow_ui->pushButton_2->setIcon(QIcon(visMethods.medium_icon));

  // high detail
  mainwindow_ui->pushButton->setIcon(QIcon(visMethods.low_icon));
}

void MainWindow::on_high_detail_vis_clicked()
{
  mainwindow_ui->groupBox_22->setStyleSheet("border: 1px solid gray");
  mainwindow_ui->groupBox_23->setStyleSheet("border: 1px solid gray");

  mainwindow_ui->groupBox_26->setStyleSheet("border: 1px solid orange");

  m_main_widget->setNumberOfEntities(NumberOfEntities::LOW);
}

void MainWindow::on_medium_detail_vis_clicked()
{
  mainwindow_ui->groupBox_22->setStyleSheet("border: 1px solid gray");
  mainwindow_ui->groupBox_26->setStyleSheet("border: 1px solid gray");

  mainwindow_ui->groupBox_23->setStyleSheet("border: 1px solid orange");

  m_main_widget->setNumberOfEntities(NumberOfEntities::MEDIUM);
}

void MainWindow::on_low_detail_vis_clicked()
{
  mainwindow_ui->groupBox_26->setStyleSheet("border: 1px solid gray");
  mainwindow_ui->groupBox_23->setStyleSheet("border: 1px solid gray");

  mainwindow_ui->groupBox_22->setStyleSheet("border: 1px solid orange");

  m_main_widget->setNumberOfEntities(NumberOfEntities::HIGH);
}

void MainWindow::on_synapse_distance_slider_changed(int value)
{
  double slider_value = m_main_widget->on_synapse_distance_slider_changed(value);
  mainwindow_ui->lineEdit->setText(QString::number(std::round(slider_value * 100.0) / 100.0));
}

void MainWindow::on_slice_position_slider_changed(int value)
{
  m_main_widget->on_slice_position_slider_changed(value);
  mainwindow_ui->lineEdit_5->setText(QString::number(std::round(value) / (100.0 / MESH_MAX_Z)));
}

void MainWindow::on_opacity_slider_changed(int value)
{
  m_main_widget->on_opacity_slider_changed(value);
  mainwindow_ui->lineEdit_4->setText(QString::number(std::round(value) / 100.0));
}

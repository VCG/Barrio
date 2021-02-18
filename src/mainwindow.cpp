#include "glwidget.h"
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
  QMainWindow(parent), mainwindow_ui(new Ui::MainWindow)
{
  mainwindow_ui->setupUi(this);

  m_data_container = new DataContainer(input_form);

  m_main_widget = new MainWidget(m_data_container, input_form, &m_vis_settings, mainwindow_ui->centralwidget);

  mainwindow_ui->verticalLayout_17->addWidget(m_main_widget);
 
  m_currentSelectedCluster = 0;
  m_clusters = 0;

  initializeSlicePositionSlider();
  //initializeSynapseThresholdSlider();
  initializeOpacitySlider();
  initializeColormapComboBox();
 
  m_treemodel = new TreeModel(mainwindow_ui->groupBox_16, m_data_container, m_main_widget);
  mainwindow_ui->verticalLayout_5->addWidget(m_treemodel);

  mainwindow_ui->groupBox_29->hide();
  mainwindow_ui->numberOfBins->setMinimum(5);
  mainwindow_ui->numberOfBins->setMaximum(100);
  mainwindow_ui->numberOfBins->setValue(20);

  QString val;
  QFile file;
  file.setFileName(PATH + QString("spec.json"));
  file.open(QIODevice::ReadOnly | QIODevice::Text);
  val = file.readAll();
  file.close();

  QJsonDocument spec_doc = QJsonDocument::fromJson(val.toUtf8());
  QJsonArray tasks = spec_doc.array();

  addTask(mainwindow_ui->groupBox_24, tasks[0].toObject());

  initVisMethods();
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

//void MainWindow::initializeSynapseThresholdSlider()
//{
//  QSlider* synapse_threshold_slider = mainwindow_ui->horizontalSlider_3;
//  int initial_tick_position = 100.0 / (sqrt(3.0) * MESH_MAX_X);
//  synapse_threshold_slider->setValue(initial_tick_position);
//  double distance_value = ((double)initial_tick_position / 100.0) * sqrt(3) * MESH_MAX_X;
//  mainwindow_ui->lineEdit->setText(QString::number(std::round(distance_value * 100.0) / 100.0));
//
//  connect(synapse_threshold_slider, SIGNAL(valueChanged(int)), this, SLOT(on_synapse_distance_slider_changed(int)));
//}

void MainWindow::initializeOpacitySlider()
{
  QSlider* opacity_slider = mainwindow_ui->horizontalSlider_4;
  int initial_tick_position = 30;
  opacity_slider->setValue(initial_tick_position);
  double opacity_value = (double)initial_tick_position / 100.0;

  connect(opacity_slider, SIGNAL(valueChanged(int)), this, SLOT(on_opacity_slider_changed(int)));

  on_opacity_slider_changed(initial_tick_position);

  
}

void MainWindow::initializeColormapComboBox()
{
  QDir dir =  QDir::currentPath();
  dir.cdUp();
  dir.cd("src/colormaps");
  
  QDirIterator iterator(dir, QDirIterator::Subdirectories);
  
  while (iterator.hasNext())
  {
    QFile file(iterator.next());
    QFileInfo fileInfo(file.fileName());
    QString name = fileInfo.fileName();

    if (QString::compare(name, ".", Qt::CaseInsensitive) != 0 && QString::compare(name, "..", Qt::CaseInsensitive) != 0)
    {
      mainwindow_ui->comboBox_2->addItem(name);
    }
  } 
}

void MainWindow::initializeVisualizationPresets()
{
  on_structure_selection_changed(-1);
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

  mainwindow_ui->horizontalSlider->setEnabled(false);
  QObject::connect(mainwindow_ui->sliceEnabled, SIGNAL(stateChanged(int)), this, SLOT(on_structure_selection_changed(int)));

  QObject::connect(mainwindow_ui->numberOfBins, SIGNAL(valueChanged(int)), this, SLOT(on_number_of_bins_input_changed(int)));
  QObject::connect(mainwindow_ui->comboBox_2, SIGNAL(currentTextChanged(QString)), this, SLOT(on_colormap_changed(QString)));

}

void MainWindow::initVisMethods()
{
  // set all boxes to not initialized
  foreach(const auto & id, m_selection_boxes.keys())
  {
    m_selection_boxes[id]->setStyleSheet("border: 1px solid gray");
  }

  // take first low scale vis as initialization
  foreach(const auto& id, m_visualizations.keys())
  {
    if (m_visualizations[id].scale == NumberOfEntities::LOW)
    {
      m_selection_boxes[id]->setStyleSheet("border: 1px solid orange");
      m_main_widget->setVisMethod(m_visualizations[id]);
      
      break; // exit loop once initialized
    }
  }
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
  bool sliderEnabled = mainwindow_ui->sliceEnabled->isChecked();
  mainwindow_ui->horizontalSlider->setEnabled(sliderEnabled);
  m_main_widget->showSlice(sliderEnabled);
}

void MainWindow::on_vis_clicked()
{
  QPushButton* button = qobject_cast<QPushButton*>(sender());
  QGroupBox* sender = qobject_cast<QGroupBox*>(button->parent());

  // reset style of other boxes
  foreach(const auto & id, m_selection_boxes.keys())
  {
    m_selection_boxes[id]->setStyleSheet("border: 1px solid gray");
    if (m_selection_boxes[id] == sender)
    {
      Vis vis = m_visualizations[id];
      QJsonObject settings = m_vis_settings[id];
      m_main_widget->setVisMethod(vis);
    }
  }
 
  sender->setStyleSheet("border: 1px solid orange");
}

//void MainWindow::on_synapse_distance_slider_changed(int value)
//{
//  double slider_value = m_main_widget->on_synapse_distance_slider_changed(value);
//  mainwindow_ui->lineEdit->setText(QString::number(std::round(slider_value * 100.0) / 100.0));
//}

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

void MainWindow::on_colormap_changed(QString text)
{
  m_main_widget->setColormap(text);
}

void MainWindow::on_number_of_bins_input_changed(int value)
{
  m_main_widget->setNumberOfBinsForHistogram(value);
}

void MainWindow::addTask(QGroupBox* ui_element, QJsonObject task)
{
  QString task_name = task.value("name").toString();
  ui_element->setTitle(task_name);

  QJsonArray sub_tasks = task.value("subtasks").toArray();
  for (auto sub_task : sub_tasks)
  {
    QGroupBox* gb_sub_task = new QGroupBox();
    ui_element->layout()->addWidget(gb_sub_task);
    addSubTask(gb_sub_task, sub_task.toObject());
  }

  // add Subtasks
}

void MainWindow::addSubTask(QGroupBox* ui_element, QJsonObject subtask)
{
  QString name = subtask.value("name").toString();
  ui_element->setTitle(name);

  QJsonObject lsv_settings = subtask.value("low_scale_vis").toObject();
  QJsonObject msv_settings = subtask.value("medium_scale_vis").toObject();
  QJsonObject hsv_settings = subtask.value("high_scale_vis").toObject();

  int lsv = lsv_settings["id"].toInt();
  int msv = msv_settings["id"].toInt();
  int hsv = hsv_settings["id"].toInt();

  m_vis_settings.insert(lsv, lsv_settings);
  m_vis_settings.insert(msv, msv_settings);
  m_vis_settings.insert(hsv, hsv_settings);

  ui_element->setLayout(new QHBoxLayout);

  addVisualizationSelection(ui_element->layout(), "Low Scale", lsv);
  addVisualizationSelection(ui_element->layout(), "Medium Scale", msv);
  addVisualizationSelection(ui_element->layout(), "High Scale", hsv);
}

void MainWindow::addVisualizationSelection(QLayout* layout, QString scale, int vis_id)
{
  QPushButton* btn = new QPushButton();
  btn->setStyleSheet("border: none; background-color: rgba(255, 255, 255, 100);");
  btn->setIconSize(QSize(60, 60));
  btn->setMinimumSize(60, 60);
  btn->setMaximumSize(60, 60);

  Vis vis_info = m_main_widget->getVisInfo(vis_id);
  btn->setIcon(QIcon(vis_info.icon_path));
  
  QGroupBox* ls_box = new QGroupBox(scale);
  ls_box->setLayout(new QGridLayout);
  ls_box->layout()->addWidget(btn);
  layout->addWidget(ls_box);

  m_selection_boxes[vis_info.id] = ls_box;
  m_visualizations[vis_info.id] = vis_info;

  QObject::connect(btn, SIGNAL(released()), this, SLOT(on_vis_clicked()));
}
